/*
 * Modified from Switchscience BME280 sample code.
 * http://trac.switch-science.com/wiki/BME280
 */
#include <ESP8266WiFi.h>
#include <SPI.h>
#include "BME280_SPI.h"

BME280::BME280() {
}

BME280::~BME280() {
}

void BME280::begin(int cs) {
    uint8_t osrs_t = 1;             //Temperature oversampling x 1
    uint8_t osrs_p = 1;             //Pressure oversampling x 1
    uint8_t osrs_h = 1;             //Humidity oversampling x 1
    uint8_t mode = 3;               //Normal mode
    uint8_t t_sb = 5;               //Tstandby 1000ms
    uint8_t filter = 0;             //Filter off 
    uint8_t spi3w_en = 0;           //3-wire SPI Disable
    
    uint8_t ctrl_meas_reg = (osrs_t << 5) | (osrs_p << 2) | mode;
    uint8_t config_reg    = (t_sb << 5) | (filter << 2) | spi3w_en;
    uint8_t ctrl_hum_reg  = osrs_h;

    bme280_cs = cs;
    pinMode(cs, OUTPUT);
    SPI.begin();
    SPI.setBitOrder (MSBFIRST);

    writeReg(0xF2,ctrl_hum_reg);
    writeReg(0xF4,ctrl_meas_reg);
    writeReg(0xF5,config_reg);
    readTrim();    
}

double BME280::readTemperature(void) {
    signed long int temp_raw;
    signed long int var1, var2;
  
    uint8_t data[8];

    readReg(0xfa, 3, data);

    temp_raw = ((uint32_t)data[0] << 12) | ((uint32_t)data[1] << 4) | ((uint32_t)data[2] >> 4);
  
    var1 = ((((temp_raw >> 3) - ((signed long int)dig_T1<<1))) * ((signed long int)dig_T2)) >> 11;
    var2 = (((((temp_raw >> 4) - ((signed long int)dig_T1)) * ((temp_raw>>4) - ((signed long int)dig_T1))) >> 12) * ((signed long int)dig_T3)) >> 14;
    
    t_fine = var1 + var2;
  
    return ((t_fine * 5 + 128) >> 8) / 100.0;
}

double BME280::readPressure(void) {
    unsigned long int pres_raw;
    signed long int var1, var2;
    unsigned long int P;
  
    uint8_t data[8];

    readReg(0xf7, 3, data);

    pres_raw = ((uint32_t)data[0] << 12) | ((uint32_t)data[1] << 4) | ((uint32_t)data[2] >> 4);
  
    var1 = (((signed long int)t_fine)>>1) - (signed long int)64000;
    var2 = (((var1>>2) * (var1>>2)) >> 11) * ((signed long int)dig_P6);
    var2 = var2 + ((var1*((signed long int)dig_P5))<<1);
    var2 = (var2>>2)+(((signed long int)dig_P4)<<16);
    var1 = (((dig_P3 * (((var1>>2)*(var1>>2)) >> 13)) >>3) + ((((signed long int)dig_P2) * var1)>>1))>>18;
    var1 = ((((32768+var1))*((signed long int)dig_P1))>>15);
    if (var1 == 0) {
        return 0;
    }    
    P = (((unsigned long int)(((signed long int)1048576)-pres_raw)-(var2>>12)))*3125;
    if(P<0x80000000) {
        P = (P << 1) / ((unsigned long int) var1);   
    } else {
        P = (P / (unsigned long int)var1) * 2;    
    }
    var1 = (((signed long int)dig_P9) * ((signed long int)(((P>>3) * (P>>3))>>13)))>>12;
    var2 = (((signed long int)(P>>2)) * ((signed long int)dig_P8))>>13;
    P = (unsigned long int)((signed long int)P + ((var1 + var2 + dig_P7) >> 4));
    return P / 100.0;
}

double BME280::readHumidity(void) {
    unsigned long int hum_raw;
    signed long int v_x1;
  
    uint8_t data[8];

    readReg(0xfd, 3, data);

    hum_raw  = ((uint32_t)data[0] << 8) | (uint32_t)data[1];
  
    v_x1 = (t_fine - ((signed long int)76800));
    v_x1 = (((((hum_raw << 14) -(((signed long int)dig_H4) << 20) - (((signed long int)dig_H5) * v_x1)) + 
              ((signed long int)16384)) >> 15) * (((((((v_x1 * ((signed long int)dig_H6)) >> 10) * 
              (((v_x1 * ((signed long int)dig_H3)) >> 11) + ((signed long int) 32768))) >> 10) + (( signed long int)2097152)) * 
              ((signed long int) dig_H2) + 8192) >> 14));
    v_x1 = (v_x1 - (((((v_x1 >> 15) * (v_x1 >> 15)) >> 7) * ((signed long int)dig_H1)) >> 4));
    v_x1 = (v_x1 < 0 ? 0 : v_x1);
    v_x1 = (v_x1 > 419430400 ? 419430400 : v_x1);
    return (unsigned long int)(v_x1 >> 12) / 1024.0;   
}

void BME280::writeReg(uint8_t reg_address, uint8_t data) {
    digitalWrite(bme280_cs, LOW);
    SPI.transfer(reg_address & B01111111);
    SPI.transfer(data);
    digitalWrite(bme280_cs, HIGH);
}

void BME280::readReg(uint8_t reg_address, int numBytes, uint8_t * data) {
    uint8_t addr = reg_address | B10000000;
    digitalWrite(bme280_cs, LOW);
    SPI.transfer(addr);
    for (int i = 0; i < numBytes; i++) {
        data[i] = SPI.transfer(0x00);
    }
    digitalWrite(bme280_cs, HIGH);
}

void BME280::readTrim() {
    uint8_t data[32],i=0;

    readReg(0x88, 24, data);
    readReg(0xA1,  1, &data[24]);
    readReg(0xE1,  7, &data[25]);

    dig_T1 = (data[1] << 8) | data[0];
    dig_T2 = (data[3] << 8) | data[2];
    dig_T3 = (data[5] << 8) | data[4];
    dig_P1 = (data[7] << 8) | data[6];
    dig_P2 = (data[9] << 8) | data[8];
    dig_P3 = (data[11]<< 8) | data[10];
    dig_P4 = (data[13]<< 8) | data[12];
    dig_P5 = (data[15]<< 8) | data[14];
    dig_P6 = (data[17]<< 8) | data[16];
    dig_P7 = (data[19]<< 8) | data[18];
    dig_P8 = (data[21]<< 8) | data[20];
    dig_P9 = (data[23]<< 8) | data[22];
    dig_H1 = data[24];
    dig_H2 = (data[26]<< 8) | data[25];
    dig_H3 = data[27];
    dig_H4 = (data[28]<< 4) | (0x0F & data[29]);
    dig_H5 = (data[30] << 4) | ((data[29] >> 4) & 0x0F);
    dig_H6 = data[31];   

#if 0
    Serial.print("T1:");Serial.print(dig_T1);
    Serial.print(", T2:");Serial.print(dig_T2);
    Serial.print(", T3:");Serial.print(dig_T3);
    Serial.print("\r\n");
    Serial.print("P1:");Serial.print(dig_P1);
    Serial.print(", P2:");Serial.print(dig_P2);
    Serial.print(", P3:");Serial.print(dig_P3);
    Serial.print(", P4:");Serial.print(dig_P4);
    Serial.print(", P5:");Serial.print(dig_P5);
    Serial.print(", P6:");Serial.print(dig_P6);
    Serial.print(", P7:");Serial.print(dig_P7);
    Serial.print(", P8:");Serial.print(dig_P8);
    Serial.print(", P9:");Serial.print(dig_P9);
    Serial.print("\r\n");
    Serial.print("H1:");Serial.print(dig_H1);
    Serial.print(", H2:");Serial.print(dig_H2);
    Serial.print(", H3:");Serial.print(dig_H3);
    Serial.print(", H4:");Serial.print(dig_H4);
    Serial.print(", H5:");Serial.print(dig_H5);
    Serial.print(", H6:");Serial.print(dig_H6);
    Serial.print("\r\n");
#endif
}

