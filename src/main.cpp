#include <Arduino.h>
#include "encoder_motor.h"
#include "variables.h"
#include "ir_sensor.h"

void taskTest() {
  Serial.print("Stack còn lại: ");
  Serial.println(uxTaskGetStackHighWaterMark(NULL)); // in ra số word còn lại
}

void buzzerBeep(int freq, int durationMs) {
  tone(PIN_BUZZER, freq);      // phát âm với tần số freq (Hz)
  delay(durationMs*1.2);           // kêu trong durationMs ms
  noTone(PIN_BUZZER);          // tắt buzzer
}

void buzzerAlarm() {
  for(int i = 0; i < 2; i++) {
    buzzerBeep(3000,200);
    delay(50);
  }
}
void levelUp(){
    buzzerBeep(329*3, 100);
    buzzerBeep(392*3, 100);
    buzzerBeep(659*3, 100);
    buzzerBeep(523*3, 100);
    buzzerBeep(587*3, 100);
    buzzerBeep(790*3, 100);
}

void setup() {
  Serial.begin(115200);
  initEncoderMotor();
  initSensors();
  pinMode(PIN_BUZZER,OUTPUT);
  digitalWrite(PIN_STBY, LOW);
  xTaskCreate(taskMotorControl, "motor", 1024, NULL, 1, NULL);
  xTaskCreate(taskReadSensors, "ir_sensor", 4000, NULL,1,NULL);
  delay(3000);
  controlMotor_speed(-10,0);
}

void loop() {
    // không làm gì
    //levelUp();
    Serial.print(sensorValues[0]);
    Serial.print(" ");
    Serial.print(sensorValues[1]);
    Serial.print(" ");
    Serial.print(sensorValues[2]);
    Serial.print(" ");
    Serial.println(sensorValues[3]);
}
