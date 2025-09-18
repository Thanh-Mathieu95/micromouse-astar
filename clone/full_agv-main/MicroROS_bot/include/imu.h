#ifndef _IMU_H
#define _IMU_H

void IMU_Init(void);
void IMU_Read(void);
void IMU_Angle_Velocity_Set(float speed);
float IMU_Angle_Velocity_Get(void);
#endif // _IMU_H