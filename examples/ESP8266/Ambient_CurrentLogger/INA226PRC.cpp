#include <ESP8266WiFi.h>
#include <Wire.h>
#include "INA226PRC.h"

#define INA226PRC_ADDRESS 0x40
#define INA226PRC_CONFIG_REG 0x00
#define INA226PRC_SHUNTV_REG 0x01
#define INA226PRC_BUSV_REG 0x02
#define INA226PRC_POWER_REG 0x03
#define INA226PRC_CURRENT_REG 0x04
#define INA226PRC_CALIB_REG 0x05
#define INA226PRC_MASK_REG 0x06

// AVG Bit [11:9]
#define INA226_AVG1    0b000
#define INA226_AVG4    0b001
#define INA226_AVG16   0b010
#define INA226_AVG64   0b011
#define INA226_AVG128  0b100
#define INA226_AVG256  0b101
#define INA226_AVG512  0b110
#define INA226_AVG1024 0b111

// VBUSCT Bit [8:6]
#define INA226_VBUSCT140u   0b000
#define INA226_VBUSCT204u   0b001
#define INA226_VBUSCT332u   0b010
#define INA226_VBUSCT588u   0b011
#define INA226_VBUSCT1100u  0b100
#define INA226_VBUSCT2116u  0b101
#define INA226_VBUSCT4156u  0b110
#define INA226_VBUSCT8244u  0b111

// VSHCT Bit [5:3]
#define INA226_VSHCT140u   0b000
#define INA226_VSHCT204u   0b001
#define INA226_VSHCT332u   0b010
#define INA226_VSHCT588u   0b011
#define INA226_VSHCT1100u  0b100
#define INA226_VSHCT2116u  0b101
#define INA226_VSHCT4156u  0b110
#define INA226_VSHCT8244u  0b111

INA226PRC::INA226PRC() {
}

INA226PRC::~INA226PRC() {
}

void INA226PRC::begin(int sda, int sck) {
    int config = (INA226_AVG4 << 9) | (INA226_VBUSCT588u << 6) | (INA226_VSHCT588u << 3)
                 | 0b111;
    Wire.begin(sda, sck);

    Wire.beginTransmission(INA226PRC_ADDRESS);
    Wire.write(INA226PRC_CONFIG_REG);
    Wire.write((config >> 8) & 0x00ff);
    Wire.write(config & 0x00ff);
    Wire.endTransmission();
}

int INA226PRC::readId(void) {
    int id;
    
    Wire.beginTransmission(INA226PRC_ADDRESS);
    Wire.write(0xfe);
    Wire.endTransmission();
    Wire.requestFrom(INA226PRC_ADDRESS, 2);
    id = Wire.read() << 8;
    id |= Wire.read();

    return (id);
}

short INA226PRC::readCurrentReg(void) {
    short highbyte;
    short current_raw;
    
    Wire.beginTransmission(INA226PRC_ADDRESS);
    Wire.write(INA226PRC_SHUNTV_REG);
    Wire.endTransmission();

    Wire.requestFrom(INA226PRC_ADDRESS, 2);
    highbyte = Wire.read();
    current_raw = (highbyte << 8) | Wire.read();

    return (current_raw);
}

float INA226PRC::readCurrent(void) {
    float current = (float)readCurrentReg();

    // 1LSBが0.1mAなので、0.1倍してmAに

    return (current * 0.1);
}

short INA226PRC::readVoltageReg(void) {
    short highbyte;
    short volt_raw;
    
    Wire.beginTransmission(INA226PRC_ADDRESS);
    Wire.write(INA226PRC_BUSV_REG);
    Wire.endTransmission();
    Wire.requestFrom(INA226PRC_ADDRESS, 2);
    highbyte = Wire.read();
    volt_raw = (highbyte << 8) | Wire.read();

    return (volt_raw);
}

float INA226PRC::readVoltage(void) {
    float volt = (float)readVoltageReg();

    // １LSBが1.25mVなので、1.25倍してmVに

    return (volt * 1.25);
}

