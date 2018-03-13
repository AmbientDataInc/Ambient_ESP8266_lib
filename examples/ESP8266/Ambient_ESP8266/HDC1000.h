#ifndef HDC1000_H
#define HDC1000_H

#include <Wire.h>

#define HDC1000_ADDRESS 0x40
#define HDC1000_TEMP_REG 0x00
#define HDC1000_HUMID_REG 0x01
#define HDC1000_CONF_REG 0x02

class HDC1000
{
public:
    HDC1000(void);

    virtual ~HDC1000();
  
    void begin(int sda, int sck, int rdy);

    int readId(void);
  
    float readTemperature(void);
  
    float readHumidity(void);

private:
    int _rdy;
};

#endif // HDC1000_H


