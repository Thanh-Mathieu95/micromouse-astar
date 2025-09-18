#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "motor.h"
#include "imu.h"

#define ROTATION_SPEED 0.5

Adafruit_MPU6050 mpu;
float angular_velocity = 0; 

void IMU_Init()
{
    Wire.begin();
    while (!mpu.begin()) {
        Serial.println("Failed to find MPU6050 chip");
        delay(1000);
    }
    Serial.println("MPU6050 Found!");
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
}

void IMU_Read(){
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    angular_velocity = g.gyro.z;
    if(abs(angular_velocity) < 0.02)
        angular_velocity = 0;
    
}

float IMU_Angle_Velocity_Get(void){
    // Serial.print("Angle Velocity:");
    // Serial.println(angular_velocity);
    return angular_velocity;
}

void IMU_Angle_Velocity_Set(float speed){

    Serial.println(speed);

    // if(speed >= 0){
    //     Motor_Velocity_Set(R_MOTOR, speed);
    //     Motor_Velocity_Set(L_MOTOR, -speed);
    // }
    // else{
    //     Motor_Velocity_Set(R_MOTOR, -speed);
    //     Motor_Velocity_Set(L_MOTOR, speed);
    // }
}

// void IMU_Angle_Check(void){

//     if(!flag_angle_set)
//         return;

//     if( angle_set - 0.02 < anglez && anglez < angle_set + 0.02 ){
//         flag_angle_set = false;
//         Motor_Velocity_Set(R_MOTOR, 0);
//         Motor_Velocity_Set(L_MOTOR, 0);
//     }
    
// }