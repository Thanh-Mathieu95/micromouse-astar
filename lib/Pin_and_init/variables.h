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

// ==== Biến toàn cục ====

extern volatile bool is_new_cell;


// ==== ISR (ngắt encoder) ====
// void IRAM_ATTR onEncoderLeft();
// void IRAM_ATTR onEncoderRight();

#endif
