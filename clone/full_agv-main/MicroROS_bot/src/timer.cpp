#include <Arduino.h>
#include <esp32-hal-timer.h>            // Include the ESP32 timer library
#include "timer.h"
//#include "mqtt.h"
#include "motor.h"
#include "imu.h"
//#include "transform.h"


#define TIMER_0_PERIOD_MS  20       // Timer 0 period in milliseconds = 10 seconds
#define TIMER_1_PERIOD_MS   1000      // Timer 1 period in milliseconds = 30 seconds

hw_timer_t *timer_0 = NULL;
hw_timer_t *timer_1 = NULL;

uint32_t cp0_regs[24];
uint32_t cp1_regs[24];

/**
 * @brief 
 * 
 * @return * Timer 0
 */
void IRAM_ATTR Timer0InterruptHandler() {
    // enable FPU
    xthal_set_cpenable(1);
    // Save FPU registers
    xthal_save_cp1(cp1_regs);

    Motor_Translation_Controller();

    // Restore FPU
    xthal_restore_cp1(cp1_regs);
    // and turn it back off
    xthal_set_cpenable(0);
} 

/**
 * @brief 
 * 
 * @return * Timer 1
 */
void IRAM_ATTR Timer1InterruptHandler() {
    // enable FPU
    xthal_set_cpenable(1);
    // Save FPU registers
    xthal_save_cp0(cp0_regs);

    // Serial.println("Timer 1 interrupt");
    Motor_ResetEncoder();
    //Translation_Caculate(Motor_Velocity_Get(), (float)TIMER_1_PERIOD_MS / 1000);
    //Rotation_Caculate(IMU_Angle_Velocity_Get(), (float)TIMER_1_PERIOD_MS / 1000);

    // Restore FPU
    xthal_restore_cp0(cp0_regs);
    // and turn it back off
    xthal_set_cpenable(0);
}


/**
 * @brief 
 * 
 */
void Timer_Init(){
    timer_0 = timerBegin(0, 80, true);                               // Timer 0, prescaler 80, clock source 0
    timerAttachInterrupt(timer_0, &Timer0InterruptHandler, true);     // Attach interrupt handler
    timerAlarmWrite(timer_0, TIMER_0_PERIOD_MS * 1000, true);          // Set alarm value (in microseconds)
    timerAlarmEnable(timer_0);                                       // Enable alarm

    timer_1 = timerBegin(1, 80, true);                               // Timer 1, prescaler 80, clock source 0
    timerAttachInterrupt(timer_1, &Timer1InterruptHandler, true);     // Attach interrupt handler
    timerAlarmWrite(timer_1, TIMER_1_PERIOD_MS * 1000, true);          // Set alarm value (in microseconds)
    timerAlarmEnable(timer_1);        
}

/**
 * @brief 
 * 
 */
void Timer_Start(){
    timerAttachInterrupt(timer_0, &Timer0InterruptHandler, true); // Attach interrupt handler
    timerAlarmEnable(timer_0);

    timerAttachInterrupt(timer_1, &Timer1InterruptHandler, true); // Attach interrupt handler
    timerAlarmEnable(timer_1);
}

/**
 * @brief 
 * 
 */
void Timer_Stop(){
    timerDetachInterrupt(timer_0);
    timerDetachInterrupt(timer_1);
    timerAlarmDisable(timer_0);
    timerAlarmDisable(timer_1);    
}