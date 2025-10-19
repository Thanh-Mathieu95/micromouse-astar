#include "ir_sensor.h"
int16_t sensorValues[4] = {0, 0, 0, 0};

void initSensors() {

    analogReadResolution(12); // 12-bit (0–4095)
    pinMode(PIN_IR, OUTPUT);
    digitalWrite(PIN_IR, LOW);
    delay(1000);

}

void update_ir(){
  digitalWrite(9,HIGH);
  delay(10);
  sensorValues[0] = analogRead(PIN_ADC0);

  sensorValues[1] = analogRead(PIN_ADC1);

  sensorValues[2] = analogRead(PIN_ADC2);

  sensorValues[3] = analogRead(PIN_ADC3);
  digitalWrite(9,LOW);
  delay(10);
    sensorValues[0] -= analogRead(PIN_ADC0);
      sensorValues[1] -= analogRead(PIN_ADC1);
        sensorValues[2] -= analogRead(PIN_ADC2);
  sensorValues[3] -= analogRead(PIN_ADC3);
}
            