#ifndef ENCODER_MOTOR_H
#define ENCODER_MOTOR_H

#include <Arduino.h>
#include "variables.h"
#include "PID.h"
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

void initEncoderMotor();
// Encoder counts
extern volatile long encoderCountLeft;
extern volatile long encoderCountRight;

// Motor
extern float motorSpeed_left;
extern float motorSpeed_right;
extern float tagetSpeed_left;
extern float tagetSpeed_right;

// Mutex
extern SemaphoreHandle_t encoderMutex;

// PID instances

extern PID pidLeft;
extern PID pidRight;

// Encoder ISR
    void onEncoderLeft();
    void onEncoderRight();

// Motor control functions
void controlMotor_left(int speed);
void controlMotor_right(int speed);
void controlMotor(int speed_left, int speed_right);
void stopMotor();
void controlMotor_speed(float targetSpeedLeft, float targetSpeedRight);

// Tasks
void taskEncodertoSpeed(void *pvParameters);
void taskMotorControl(void *pvParameters);

#endif // ENCODER_MOTOR_H
