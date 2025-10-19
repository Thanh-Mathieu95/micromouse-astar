#ifndef VARIABLES_H
#define VARIABLES_H

// ADC pins
#define PIN_ADC0 26
#define PIN_ADC1 27
#define PIN_ADC2 28
#define PIN_ADC3 29

// Motor control
#define PIN_PWMB 0
#define PIN_BIN1 1
#define PIN_BIN2 2
#define PIN_STBY 3
#define PIN_AIN1 4
#define PIN_AIN2 5
#define PIN_PWMA 6

// Encoder
#define PIN_EA1 18
#define PIN_EA2 19
#define PIN_EB1 22
#define PIN_EB2 23
#define ENCODER_LEFT_POLARITY   +1     // +1 nếu lắp đúng chiều
#define ENCODER_RIGHT_POLARITY  -1     // -1 nếu lắp ngược chiều
#define MM_PER_COUNT_LEFT       0.071f // mm di chuyển trên mỗi xung đếm (trái)
#define MM_PER_COUNT_RIGHT      0.072f // mm di chuyển trên mỗi xung đếm (phải)
#define DEG_PER_MM_DIFFERENCE   0.65f // độ quay / mm chênh lệch

// IR + Buzzer + LED
#define PIN_IR      16
#define PIN_BUZZER  7
#define PIN_LED1    8

// Buttons
#define PIN_BTN1 24
#define PIN_BTN2 25

// I2C
#define PIN_I2C_SDA 20
#define PIN_I2C_SCL 21

#define MID 13
#define BASE1 110
#define BASE2 105
#define MAX1 325
#define MIN1 55
#define MAX2 305
#define MIN2 55

#define MIN0 30
#define MIN3 30
#define BASE0 205
#define BASE3 175

// ==== Biến toàn cục ====

// ==== ISR (ngắt encoder) ====
// void IRAM_ATTR onEncoderLeft();
// void IRAM_ATTR onEncoderRight();

#endif
