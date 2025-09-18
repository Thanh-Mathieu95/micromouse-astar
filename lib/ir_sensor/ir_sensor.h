#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include "hardware/adc.h"
#include "variables.h"

extern volatile int16_t sensorValues[4];
extern SemaphoreHandle_t sensorMutex;

// Hàm khởi tạo ADC và IR
void initSensors();

// Task đọc cảm biến
void taskReadSensors(void *pvParameters);

#endif // SENSORS_H
