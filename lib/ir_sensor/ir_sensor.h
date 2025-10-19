#ifndef SENSORS_H
#define SENSORS_H
#pragma once
#include <Arduino.h>
#include "variables.h"

extern int16_t sensorValues[4];

// Hàm khởi tạo ADC và IR
void initSensors();

// Task đọc cảm biến
void update_ir();
#endif // SENSORS_H
