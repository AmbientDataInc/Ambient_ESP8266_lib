/*
 * BME280で5秒毎に温度、湿度、気圧を測定し、シリアルモニタに出力する
 */

#include <ESP8266WiFi.h>
#include <SPI.h>
#include "BME280_SPI.h"

#define BME_CS 15

#define PERIOD 5

BME280 bme280;

void setup()
{
    Serial.begin(115200);
    delay(10);

    Serial.println("Start");

    bme280.begin(BME_CS);
}

void loop()
{
    double temp, humid, pressure;

    temp = bme280.readTemperature();
    humid = bme280.readHumidity();
    pressure = bme280.readPressure();

    Serial.print("temp: ");
    Serial.print(temp);
    Serial.print(", humid: ");
    Serial.print(humid);
    Serial.print(", pressure: ");
    Serial.println(pressure);

    delay(PERIOD * 1000);
}

