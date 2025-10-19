#include "encoder_motor.h"

PID pidLeft(0.24, 0, 0.00005, -255, 255);
PID pidRight(0.24, 0, 0.00005, -255, 255);
PID motion(00.1,000 ,.002,12,-12);

// Khởi tạo các biến encoder
volatile long encoderCountLeft = 0;
volatile long encoderCountRight = 0;
volatile bool lastLeftA = 0;
volatile bool lastRightA = 0;

float motorSpeed_left = 0;
float motorSpeed_right = 0;
float tagetSpeed_left = 0;
float tagetSpeed_right = 0;

float left_distance = 0;
float right_distance = 0;

float robot_speed= 0;
float robot_w_speed= 0;


float robot_distance = 0;
float dic_rot= 0;
float w_rot= 0;

SemaphoreHandle_t encoderMutex;   // hoặc mutex kiểu khác nếu không dùng FreeRTOS
void initEncoderMotor() {
    pinMode(PIN_PWMA, OUTPUT); pinMode(PIN_AIN1, OUTPUT); pinMode(PIN_AIN2, OUTPUT);
    pinMode(PIN_PWMB, OUTPUT); pinMode(PIN_BIN1, OUTPUT); pinMode(PIN_BIN2, OUTPUT);
    pinMode(PIN_STBY, OUTPUT);
    pinMode(PIN_EA1, INPUT_PULLUP);
    pinMode(PIN_EA2, INPUT_PULLUP);
    pinMode(PIN_EB1, INPUT_PULLUP);
    pinMode(PIN_EB2, INPUT_PULLUP);

    digitalWrite(PIN_STBY, LOW);

    // Gắn interrupt cho encoder
    attachInterrupt(digitalPinToInterrupt(PIN_EA2), onEncoderLeft, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_EB1), onEncoderRight, CHANGE);
}

// encoder_functions++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void onEncoderLeft() {

    bool a = digitalRead(PIN_EA1);
    bool b = digitalRead(PIN_EA2);

    if (a != lastLeftA) {
        if (a == b) encoderCountLeft++;
        else encoderCountLeft--;
    }
    lastLeftA = a;
    }

void onEncoderRight() {
    bool a = digitalRead(PIN_EB1);
    bool b = digitalRead(PIN_EB2);

    if (a != lastRightA) {
        if (a == b) encoderCountRight--;
        else encoderCountRight++;
    }
    lastRightA = a;
    }
void resetencoder(){
    encoderCountLeft = 0;
    encoderCountRight = 0;
}
void reset_distance(){
    left_distance = 0;
    right_distance = 0;
}
void update_encoder_task(unsigned long lastTick) {
    noInterrupts();
    float speedLeftMeasured = encoderCountLeft;
    float speedRightMeasured = encoderCountRight;
    encoderCountLeft = 0;
    encoderCountRight = 0;
    interrupts();
    unsigned long now = millis();
    float dt = (now - lastTick) / 1000.0f; // chuyển ms -> giây

    motorSpeed_left  = ((speedLeftMeasured )* MM_PER_COUNT_LEFT)/ dt;  // nhân tạm để thấy rõ
    motorSpeed_right = ((speedRightMeasured)*MM_PER_COUNT_RIGHT)/dt;
    robot_speed = (motorSpeed_left + motorSpeed_right)/2;
    robot_w_speed = ((speedLeftMeasured - speedRightMeasured)*DEG_PER_MM_DIFFERENCE)/dt;
    robot_distance += (speedLeftMeasured)* MM_PER_COUNT_LEFT;
    w_rot += (speedLeftMeasured - speedRightMeasured)*DEG_PER_MM_DIFFERENCE;
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
        digitalWrite(PIN_AIN1, HIGH);
        digitalWrite(PIN_AIN2, HIGH);
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
        digitalWrite(PIN_BIN1, HIGH);
        digitalWrite(PIN_BIN2, HIGH);
        analogWrite(PIN_PWMB, 0);
    }
}
void controlMotor(int speed_left, int speed_right) {
    const int DEADZONE = 30; 
    if (abs(speed_left) < DEADZONE) speed_left = 0;
    if (abs(speed_right) < DEADZONE) speed_right = 0;
    controlMotor_left(speed_left);
    controlMotor_right(speed_right);
}

void stopMotor() {
    controlMotor(0, 0);
}
void controlMotor_speed(float targetSpeedLeft, float targetSpeedRight, unsigned long lastTime) {
    unsigned long now = millis();
    float dt = (now - lastTime) / 1000.0;  // lấy giá trị lastTime
    //*lastTime = now;                        // cập nhật giá trị qua con trỏ

    pidLeft.setSetpoint(targetSpeedLeft);
    pidRight.setSetpoint(targetSpeedRight);
    int baseleft  = (targetSpeedLeft  - 42.88) / 10.5;
    int baseright = (targetSpeedRight - 42.17) / 10.5;

    float leftOutput  = baseleft + pidLeft.compute(motorSpeed_left, dt);
    float rightOutput = baseright + pidRight.compute(motorSpeed_right, dt);
    Serial.printf("L:%7.2f   R:%7.2f    baseleft:%7d      baseright:%7d    SpeedLeft:%7.2f      SpeedRight:%7.2f\n", leftOutput, rightOutput,baseleft,baseright,motorSpeed_left,motorSpeed_right);
    controlMotor(leftOutput, rightOutput);
}


void printAligned(int val, int width) {
    int len = String(val).length();
    for (int i = 0; i < width - len; i++) Serial.print(" ");
    Serial.print(val);
}

unsigned long wait_loop(uint16_t time){
    unsigned long now = millis();
    delay(time);
    return now;
}

void forward(float dictance, float speed , float end_speed){
    unsigned long lastTime = millis();
    motion.setSetpoint(0);
    while (robot_distance < dictance){
        update_encoder_task(lastTime);
        float base_speed = 0;
        if (robot_distance < dictance / 2){base_speed = speed;}
        else{base_speed = end_speed;}
        float left_speed = base_speed + motion.compute(robot_w_speed,lastTime);
        float right_speed = base_speed - motion.compute(robot_w_speed,lastTime);
        controlMotor_speed(left_speed,right_speed,lastTime);

    }
}
void go_one_cell(){
    forward( 1620, 2.2 , 2.2);
}
void check_encoder(){
    controlMotor_right(50);
    while(encoderCountRight < 1000){
        Serial.println(encoderCountRight);
        delay(50);
    }
    stopMotor();
    delay(2000);
    Serial.println(encoderCountRight);
}