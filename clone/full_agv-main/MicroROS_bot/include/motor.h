#ifndef _MOTOR_H
#define _MOTOR_H
#include <Arduino.h>
#define R_MOTOR   0
#define L_MOTOR   1

void Motor_Init(void);
void Motor_Translation_Controller(void);

void Motor_ResetEncoder(void);
void Motor_Translation_Set(float velocity);
void Motor_Rotation_Set(float omega);
void Motor_Set(void);
void Motor_Set_Velocity(float linear);
void Motor_Set_Angular(float angle);

float Motor_Velocity_Get(void);
float Motor_Omega_Get(void);
#endif