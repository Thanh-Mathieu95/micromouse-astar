#include "robot_control.h"
#include "encoder_motor.h" 
#include "PID.h"
#include "Pin_and_init/variables.h" // Sử dụng file variables.h của bạn

// --- CÁC HẰNG SỐ CẦN HIỆU CHỈNH ---
#define WHEEL_BASE_CM 12.0f // Khoảng cách giữa hai bánh xe -> CẦN ĐO VÀ HIỆU CHỈNH CHÍNH XÁC

// Tốc độ cơ bản cho các chuyển động (0-255)
#define BASE_MOVE_SPEED 80
#define BASE_TURN_SPEED 70
#define CURVE_BASE_SPEED 70 // Tốc độ cơ sở cho bánh xe bên trong khi rẽ cong

// Hằng số PID cho việc đi thẳng
#define STRAIGHT_KP 0.5
#define STRAIGHT_KI 0.1
#define STRAIGHT_KD 0.2

// --- CÁC HẰNG SỐ TÍNH TOÁN ---
const float CELL_DISTANCE_MM = CELL_DISTANCE_CM * 10.0f;

// Tính toán số ticks dựa trên hằng số của bạn
const long TICKS_PER_CELL_LEFT = (long)(CELL_DISTANCE_MM / MM_PER_COUNT_LEFT);
const long TICKS_PER_CELL_RIGHT = (long)(CELL_DISTANCE_MM / MM_PER_COUNT_RIGHT);

// Tính toán cho rẽ cong 90 độ
// Bán kính của vòng cung mà tâm robot di chuyển
const float TURN_RADIUS_MM = (2.0f * CELL_DISTANCE_MM / PI) - (WHEEL_BASE_CM * 10.0f / 2.0f);
// Quãng đường bánh xe bên trong (inner) và ngoài (outer) cần đi
const float INNER_ARC_LENGTH_MM = (2.0f * PI * TURN_RADIUS_MM) / 4.0f;
const float OUTER_ARC_LENGTH_MM = (2.0f * PI * (TURN_RADIUS_MM + WHEEL_BASE_CM * 10.0f)) / 4.0f;

// Số ticks tương ứng cho mỗi bánh
const long INNER_TICKS_90_DEG = (long)(INNER_ARC_LENGTH_MM / MM_PER_COUNT_LEFT); // Giả sử MM_PER_COUNT xấp xỉ bằng nhau
const long OUTER_TICKS_90_DEG = (long)(OUTER_ARC_LENGTH_MM / MM_PER_COUNT_RIGHT);

// Tỉ lệ tốc độ giữa hai bánh khi rẽ cong
const float CURVE_SPEED_RATIO = OUTER_ARC_LENGTH_MM / INNER_ARC_LENGTH_MM;


// Khởi tạo bộ điều khiển PID
PIDController straightPID(STRAIGHT_KP, STRAIGHT_KI, STRAIGHT_KD);

void setup_robot_control() {
    initEncoderMotor(); 
}

// --- CÁC HÀM DI CHUYỂN CƠ BẢN (Dừng-Xoay-Đi) ---

void move_forward_one_cell() {
    resetEncoders();
    straightPID.reset();

    long left_ticks = 0;
    long right_ticks = 0;

    // Sử dụng giá trị trung bình của 2 hằng số ticks
    long target_ticks = (TICKS_PER_CELL_LEFT + TICKS_PER_CELL_RIGHT) / 2;

    while ((left_ticks + right_ticks) / 2 < target_ticks) {
        left_ticks = getLeftEncoderCount() * ENCODER_LEFT_POLARITY;
        right_ticks = getRightEncoderCount() * ENCODER_RIGHT_POLARITY;

        int error = left_ticks - right_ticks;
        int correction = straightPID.calculate(error);

        controlMotor(BASE_MOVE_SPEED - correction, BASE_MOVE_SPEED + correction);
        delay(10);
    }
    controlMotor(0, 0);
}

void turn_left_90() {
    resetEncoders();
    long ticks_target = (long)(((WHEEL_BASE_CM * 10.0f * PI / 4.0f) / MM_PER_COUNT_LEFT));
    while ((abs(getLeftEncoderCount()) + abs(getRightEncoderCount())) / 2 < ticks_target) {
        controlMotor(-BASE_TURN_SPEED, BASE_TURN_SPEED);
        delay(10);
    }
    controlMotor(0, 0);
}

void turn_right_90() {
    resetEncoders();
    long ticks_target = (long)(((WHEEL_BASE_CM * 10.0f * PI / 4.0f) / MM_PER_COUNT_RIGHT));
    while ((abs(getLeftEncoderCount()) + abs(getRightEncoderCount())) / 2 < ticks_target) {
        controlMotor(BASE_TURN_SPEED, -BASE_TURN_SPEED);
        delay(10);
    }
    controlMotor(0, 0);
}

void turn_around_180() {
    turn_right_90();
    turn_right_90();
}


// --- CÁC HÀM RẼ CONG ---

void curve_turn_right_90() {
    resetEncoders();
    // Khi rẽ phải, bánh trái là bánh ngoài, bánh phải là bánh trong
    long left_ticks = 0;
    long right_ticks = 0;

    while (left_ticks < OUTER_TICKS_90_DEG || right_ticks < INNER_TICKS_90_DEG) {
        left_ticks = getLeftEncoderCount() * ENCODER_LEFT_POLARITY;
        right_ticks = getRightEncoderCount() * ENCODER_RIGHT_POLARITY;

        float current_ratio = (left_ticks > 0) ? (float)right_ticks / left_ticks : 0;
        float target_ratio = (float)INNER_TICKS_90_DEG / OUTER_TICKS_90_DEG;

        // Điều khiển tốc độ để giữ đúng tỉ lệ quãng đường
        if (current_ratio < target_ratio) {
            // Bánh phải cần chạy nhanh hơn
            controlMotor(CURVE_BASE_SPEED * CURVE_SPEED_RATIO, CURVE_BASE_SPEED + 5);
        } else {
            // Bánh trái cần chạy nhanh hơn
            controlMotor(CURVE_BASE_SPEED * CURVE_SPEED_RATIO + 5, CURVE_BASE_SPEED);
        }
        delay(10);
    }
    controlMotor(0, 0);
}

void curve_turn_left_90() {
    resetEncoders();
    // Khi rẽ trái, bánh phải là bánh ngoài, bánh trái là bánh trong
    long left_ticks = 0;
    long right_ticks = 0;

    while (right_ticks < OUTER_TICKS_90_DEG || left_ticks < INNER_TICKS_90_DEG) {
        left_ticks = getLeftEncoderCount() * ENCODER_LEFT_POLARITY;
        right_ticks = getRightEncoderCount() * ENCODER_RIGHT_POLARITY;

        float current_ratio = (right_ticks > 0) ? (float)left_ticks / right_ticks : 0;
        float target_ratio = (float)INNER_TICKS_90_DEG / OUTER_TICKS_90_DEG;

        if (current_ratio < target_ratio) {
            // Bánh trái cần chạy nhanh hơn
            controlMotor(CURVE_BASE_SPEED + 5, CURVE_BASE_SPEED * CURVE_SPEED_RATIO);
        } else {
            // Bánh phải cần chạy nhanh hơn
            controlMotor(CURVE_BASE_SPEED, CURVE_BASE_SPEED * CURVE_SPEED_RATIO + 5);
        }
        delay(10);
    }
    controlMotor(0, 0);
}


// --- HÀM "BỘ NÃO" ---

void execute_move(Point next_cell, Point current_pos, RobotDirection* current_dir) {
    RobotDirection target_dir;
    if (next_cell.y < current_pos.y) target_dir = NORTH_DIR;
    else if (next_cell.x > current_pos.x) target_dir = EAST_DIR;
    else if (next_cell.y > current_pos.y) target_dir = SOUTH_DIR;
    else target_dir = WEST_DIR;

    int direction_diff = (target_dir - *current_dir + 4) % 4;

    // Cập nhật hướng mới của robot TRƯỚC KHI di chuyển
    *current_dir = target_dir;

    switch (direction_diff) {
        case 0: // Đi thẳng
            move_forward_one_cell();
            break;
        case 1: // Rẽ phải
            curve_turn_right_90();
            break;
        case 2: // Quay lại
            turn_around_180(); // Quay 180 độ vẫn cần dừng tại chỗ
            move_forward_one_cell();
            break;
        case 3: // Rẽ trái
            curve_turn_left_90();
            break;
    }
}