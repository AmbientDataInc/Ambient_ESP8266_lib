/*
 * M5StackとBME280をI2C接続し、温度、湿度、気圧を測定しプリントアプトする
 */
#include <M5Stack.h>
#include <Wire.h>
#include "BME280.h"

BME280 bme280;

void setup(){
    M5.begin();
    Wire.begin(); // I2Cの初期化
    Serial.begin(74880);

    pinMode(21, INPUT_PULLUP); //デファルトのSDAピン21　のプルアップの指定
    pinMode(22, INPUT_PULLUP); //デファルトのSDAピン22　のプルアップの指定

    bme280.begin(); // BME280の初期化

    M5.Lcd.printf("BME280 test\r\n"); // LCD display
}

void loop() {
    float temp, humid, pressure;

    // BME280で温度、湿度、気圧を測定する
    temp = (float)bme280.readTemperature();
    humid = (float)bme280.readHumidity();
    pressure = (float)bme280.readPressure();

    M5.Lcd.printf("temp: %2.2f", temp);
    M5.Lcd.printf(" humid: %0.2f", humid);
    M5.Lcd.printf(" pressure: %f\r\n", pressure);
    delay(5000);
}
