#include "PID.h"
#include <cmath>
PID::PID(float Kp_, float Ki_, float Kd_, float minOutput, float maxOutput) {
    Kp = Kp_;
    Ki = Ki_;
    Kd = Kd_;
    outputMin = minOutput;
    outputMax = maxOutput;
    integral = 0;
    lastError = 0;
    setpoint = 0;
}

void PID::setSetpoint(float sp) {
    setpoint = sp;
}

// PID.cpp
float PID::compute(float input, float dt) {
    float error = setpoint - input;
    integral += error * dt;
    float derivative = (error - lastError) / dt;
    float output = Kp * error + Ki * integral + Kd * derivative;

    // Giới hạn đầu ra
    if (output > outputMax) output = outputMax;
    else if (output < outputMin) output = outputMin;

    // Deadzone
    if (fabs(output) < deadzone) {
        output = 0;
    }

    lastError = error;
    return output;
}


void PID::reset() {
    integral = 0;
    lastError = 0;
}
float PID::get_set() {
    return setpoint;
}