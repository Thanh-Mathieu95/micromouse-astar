#include "encoder_motor.h"

PID pidLeft(200, 0, 0.4, -255, 255);
PID pidRight(200, 0, 0.4, -255, 255);


// Khởi tạo các biến encoder
volatile long encoderCountLeft = 0;
volatile long encoderCountRight = 0;

float motorSpeed_left = 0;
float motorSpeed_right = 0;
float tagetSpeed_left = 0;
float tagetSpeed_right = 0;

float left_distance = 0;
float right_distance = 0;
SemaphoreHandle_t encoderMutex;   // hoặc mutex kiểu khác nếu không dùng FreeRTOS
void initEncoderMotor() {
    pinMode(PIN_PWMA, OUTPUT); pinMode(PIN_AIN1, OUTPUT); pinMode(PIN_AIN2, OUTPUT); pinMode(PIN_STBY, OUTPUT);
    digitalWrite(PIN_STBY, LOW);
    // Tạo mutex nếu chưa có
    if (encoderMutex == NULL) {
        encoderMutex = xSemaphoreCreateMutex();
        if (encoderMutex == NULL) {
            Serial.println("Failed to create encoder mutex!");
        }
    }

    // Gắn interrupt cho encoder
    attachInterrupt(digitalPinToInterrupt(PIN_EA1), onEncoderLeft, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_EB1), onEncoderRight, CHANGE);
}

// encoder_functions++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void onEncoderLeft() {
        if (digitalRead(PIN_EA1) == digitalRead(PIN_EA2)) {
            encoderCountLeft--;
        } else {
            encoderCountLeft++;
        }
    }
void onEncoderRight() {

        if (digitalRead(PIN_EB1) == digitalRead(PIN_EB2)) {
            encoderCountRight--;
        } else {
            encoderCountRight++;
        }
    }
void resetencoder(){
    encoderCountLeft = 0;
    encoderCountRight = 0;
}
void reset_distance(){
    left_distance = 0;
    right_distance = 0;
}
// motor_functions++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void controlMotor_left(int speed) {
    if (speed > 0) {
        digitalWrite(PIN_AIN1, HIGH);
        digitalWrite(PIN_AIN2, LOW);
        analogWrite(PIN_PWMA, speed);
    } else if (speed < 0) {
        digitalWrite(PIN_AIN1, LOW);
        digitalWrite(PIN_AIN2, HIGH);
        analogWrite(PIN_PWMA, -speed);
    } else {
        digitalWrite(PIN_AIN1, LOW);
        digitalWrite(PIN_AIN2, LOW);
        analogWrite(PIN_PWMA, 0);
    }
}
void controlMotor_right(int speed) {
    if (speed > 0) {
        digitalWrite(PIN_BIN1, HIGH);
        digitalWrite(PIN_BIN2, LOW);
        analogWrite(PIN_PWMB, speed);
    } else if (speed < 0) {
        digitalWrite(PIN_BIN1, LOW);
        digitalWrite(PIN_BIN2, HIGH);
        analogWrite(PIN_PWMB, -speed);
    } else {
        digitalWrite(PIN_BIN1, LOW);
        digitalWrite(PIN_BIN2, LOW);
        analogWrite(PIN_PWMB, 0);
    }
}
void controlMotor(int speed_left, int speed_right) {
    const int DEADZONE = 50;  // ngưỡng dưới tắt motor cho ddowx oonf
    if (abs(speed_left) < DEADZONE) speed_left = 0;
    if (abs(speed_right) < DEADZONE) speed_right = 0;
    controlMotor_left(speed_left);
    controlMotor_right(speed_right);
}

void stopMotor() {
    controlMotor(0, 0);
}
void controlMotor_speed(float targetSpeedLeft, float targetSpeedRight) {
  tagetSpeed_left = targetSpeedLeft;
  tagetSpeed_right = targetSpeedRight;
  pidLeft.setSetpoint(tagetSpeed_left);
  pidRight.setSetpoint(tagetSpeed_right);
}
void printAligned(int val, int width) {
    int len = String(val).length();
    for (int i = 0; i < width - len; i++) Serial.print(" ");
    Serial.print(val);
}

void taskMotorControl(void *pvParameters) {
    const TickType_t xDelay = pdMS_TO_TICKS(5); // 100ms
    TickType_t lastTick = xTaskGetTickCount();
    long lastCountLeft = 0;
    long lastCountRight = 0;

    for (;;) {
        // Lấy tốc độ động cơ với mutex
        xSemaphoreTake(encoderMutex, portMAX_DELAY);
        float speedLeftMeasured = encoderCountLeft;
        float speedRightMeasured = encoderCountRight;
        xSemaphoreGive(encoderMutex);
                // Tính tốc độ
        float speedLeft  = (speedLeftMeasured - lastCountLeft);  // nhân tạm để thấy rõ
        float speedRight = (speedRightMeasured - lastCountRight);

        // Serial.print("Speed L: "); Serial.print(speedLeft);
        // Serial.print("  R: "); Serial.println(speedRight);

        lastCountLeft  = speedLeftMeasured;
        lastCountRight = speedRightMeasured;
        // Tính dt thực tế
        TickType_t now = xTaskGetTickCount();
        float dt = (now - lastTick) * portTICK_PERIOD_MS / 1000.0f;
        lastTick = now;

        // PID
        int controlSignalLeft  = static_cast<int>(pidLeft.compute(speedLeftMeasured, dt));
        int controlSignalRight = static_cast<int>(pidRight.compute(speedRightMeasured, dt));

        controlMotor(controlSignalLeft, controlSignalRight);
        resetencoder();
        //Serial.print("Stack còn lại: ");
        //Serial.println(uxTaskGetStackHighWaterMark(NULL));
        // printAligned(controlSignalLeft, 5);
        // Serial.print("   ");
        // printAligned(speedLeftMeasured, 7);
        // Serial.print("   ");
        // printAligned(pidLeft.get_set(), 7);
        // Serial.println();

        vTaskDelay(xDelay);
    }
}

