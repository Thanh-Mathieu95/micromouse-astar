#include "ir_sensor.h"

volatile int16_t sensorValues[4] = {0, 0, 0, 0};
SemaphoreHandle_t sensorMutex;

void initSensors() {
    adc_init();
    adc_gpio_init(PIN_ADC0);
    adc_gpio_init(PIN_ADC1);
    adc_gpio_init(PIN_ADC2);
    adc_gpio_init(PIN_ADC3);

    pinMode(PIN_IR, OUTPUT);
    digitalWrite(PIN_IR, LOW);

    sensorMutex = xSemaphoreCreateMutex();
    if (!sensorMutex) {
        Serial.println("Failed to create sensor mutex!");
    }
}

void taskReadSensors(void *pvParameters) {
    const TickType_t xDelay = pdMS_TO_TICKS(50); // đọc mỗi 50ms

    for (;;) {
        if (xSemaphoreTake(sensorMutex, portMAX_DELAY) == pdTRUE) {
            uint16_t rawValues[4];
            uint16_t irValues[4];

            // Đọc khi IR tắt
            for (int i = 0; i < 4; i++) {
                adc_select_input(PIN_ADC0 + i - 26);
                rawValues[i] = adc_read();
            }

            // Bật IR
            digitalWrite(PIN_IR, HIGH);
            vTaskDelay(pdMS_TO_TICKS(5));

            // Đọc khi IR bật
            for (int i = 0; i < 4; i++) {
                adc_select_input(PIN_ADC0 + i - 26);
                irValues[i] = adc_read();
            }

            // Tắt IR
            digitalWrite(PIN_IR, LOW);

            // Lưu chênh lệch vào biến toàn cục
            for (int i = 0; i < 4; i++) {
                sensorValues[i] = (int16_t)(irValues[i] ) //- rawValues[i]
                ;
            }

            xSemaphoreGive(sensorMutex);
        }
        vTaskDelay(xDelay);
    }
}

