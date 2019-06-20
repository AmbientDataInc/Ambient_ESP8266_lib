/*
 * Modified from Switchscience BME280 sample code.
 * https://github.com/SWITCHSCIENCE/BME280/blob/master/Arduino/BME280_I2C/BME280_I2C.ino
 */
#ifndef MBED_BME280_H
#define MBED_BME280_H

#include <Wire.h>

#define BME280_ADDRESS 0x76

class BME280
{
public:
    BME280(void);

    virtual ~BME280();
  
    void begin(void);
  
    double readTemperature(void);
  
    double readPressure(void);
  
    double readHumidity(void);

private:
    signed long int t_fine;
  
    uint16_t dig_T1;
     int16_t dig_T2;
     int16_t dig_T3;
    uint16_t dig_P1;
     int16_t dig_P2;
     int16_t dig_P3;
     int16_t dig_P4;
     int16_t dig_P5;
     int16_t dig_P6;
     int16_t dig_P7;
     int16_t dig_P8;
     int16_t dig_P9;
     uint8_t  dig_H1;
     int16_t dig_H2;
     uint8_t  dig_H3;
     int16_t dig_H4;
     int16_t dig_H5;
     int8_t  dig_H6;
  
    void writeReg(uint8_t reg_address, uint8_t data);
    void readTrim(void);
};

#endif // MBED_BME280_H


