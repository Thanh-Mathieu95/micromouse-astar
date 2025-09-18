#ifndef PID_H
#define PID_H

class PID {
private:
    float Kp, Ki, Kd;      // Hệ số PID
    float setpoint;         // Giá trị mong muốn
    float integral;         // Tích phân
    float lastError;        // Lỗi lần trước
    float outputMin, outputMax; // Giới hạn đầu ra
    float deadzone = 50;

public:
    PID(float Kp, float Ki, float Kd, float minOutput = -255, float maxOutput = 255);
    void setDeadzone(float dz) { deadzone = dz; }
    float get_set();
    void setSetpoint(float sp);
    float compute(float input, float dt); // dt tính bằng giây
    void reset(); // Reset integral và lastError
};

#endif
