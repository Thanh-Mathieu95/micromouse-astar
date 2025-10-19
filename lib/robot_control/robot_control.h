
#ifndef ROBOT_CONTROL_H
#define ROBOT_CONTROL_H

#include <Arduino.h>
#include "pathfinding.h" // Để sử dụng struct Point và các định nghĩa hướng

// Khai báo các hằng số cho việc di chuyển.
// BẠN CẦN HIỆU CHỈNH CÁC GIÁ TRỊ NÀY CHO ROBOT CỦA MÌNH.
#define CELL_DISTANCE_CM 18.0f      // Khoảng cách một ô (ví dụ: 18cm)
#define WHEEL_DIAMETER_CM 3.4f      // Đường kính bánh xe
#define ENCODER_PULSES_PER_REV 360  // Số xung encoder cho một vòng quay bánh xe

// Hướng của robot
enum RobotDirection {
    NORTH_DIR = 0,
    EAST_DIR = 1,
    SOUTH_DIR = 2,
    WEST_DIR = 3
};

/**
 * @brief Khởi tạo các chân và biến cần thiết cho việc điều khiển robot.
 */
void setup_robot_control();

/**
 * @brief Đi thẳng về phía trước một khoảng cách bằng một ô.
 * Sử dụng PID và encoder để đảm bảo đi thẳng và đúng khoảng cách.
 */
void move_forward_one_cell();

/**
 * @brief Quay 90 độ sang trái tại chỗ.
 * Sử dụng PID và encoder để quay chính xác.
 */
void turn_left_90();

/**
 * @brief Quay 90 độ sang phải tại chỗ.
 * Sử dụng PID và encoder để quay chính xác.
 */
void turn_right_90();

/**
 * @brief Quay 180 độ tại chỗ.
 */
void turn_around_180();

/**
 * @brief Hàm "bộ não" điều khiển robot di chuyển đến ô tiếp theo.
 * 
 * @param next_cell Ô tiếp theo cần đến.
 * @param current_pos Vị trí hiện tại của robot.
 * @param current_dir Con trỏ đến hướng hiện tại của robot (sẽ được cập nhật).
 */
/**
 * @brief Rẽ cong 90 độ sang phải, di chuyển sang ô bên cạnh.
 */
void curve_turn_right_90();

/**
 * @brief Rẽ cong 90 độ sang trái, di chuyển sang ô bên cạnh.
 */
void curve_turn_left_90();

/**
 * @brief Hàm "bộ não" điều khiển robot di chuyển đến ô tiếp theo.
 * 
 * @param next_cell Ô tiếp theo cần đến.
 * @param current_pos Vị trí hiện tại của robot.
 * @param current_dir Con trỏ đến hướng hiện tại của robot (sẽ được cập nhật).
 */
void execute_move(Point next_cell, Point current_pos, RobotDirection* current_dir);


#endif // ROBOT_CONTROL_H
