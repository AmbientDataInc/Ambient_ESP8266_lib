#include <ESP8266WiFi.h>
#include <Wire.h>
#include "HDC1000.h"

HDC1000::HDC1000() {
}

HDC1000::~HDC1000() {
}

void HDC1000::begin(int sda, int sck, int rdy) {
    Wire.begin(sda, sck);

    Wire.beginTransmission(HDC1000_ADDRESS);
    Wire.write(HDC1000_CONF_REG);
    Wire.write(0x00);
    Wire.write(0x00);
    Wire.endTransmission();

    _rdy = rdy;
    pinMode(rdy, INPUT);
}

int HDC1000::readId(void) {
    int id;
    
    Wire.beginTransmission(HDC1000_ADDRESS);
    Wire.write(0xfe);
    Wire.endTransmission();
    Wire.requestFrom(HDC1000_ADDRESS, 2);
    while (Wire.available() < 2) {
      ;
    }
    id = Wire.read() << 8;
    id |= Wire.read();

    return (id);
}

float HDC1000::readTemperature(void) {
    unsigned int temp_raw;
    
    Wire.beginTransmission(HDC1000_ADDRESS);
    Wire.write(HDC1000_TEMP_REG);
    Wire.endTransmission();
    while (digitalRead(_rdy) != 0) {
        delay(1);
    }
    Wire.requestFrom(HDC1000_ADDRESS, 2);
    while (Wire.available() < 2) {
      ;
    }
    temp_raw = Wire.read() << 8;
    temp_raw |= Wire.read();

    return ((float)temp_raw / 65536.0 * 165.0 - 40.0);
}

float HDC1000::readHumidity(void) {
    unsigned int humid_raw;
    
    Wire.beginTransmission(HDC1000_ADDRESS);
    Wire.write(HDC1000_HUMID_REG);
    Wire.endTransmission();
    while (digitalRead(_rdy) != 0) {
        delay(1);
    }
    Wire.requestFrom(HDC1000_ADDRESS, 2);
    while (Wire.available() < 2) {
      ;
    }
    humid_raw = Wire.read() << 8;
    humid_raw |= Wire.read();

    return ((float)humid_raw / 65536.0 * 100.0);
}

