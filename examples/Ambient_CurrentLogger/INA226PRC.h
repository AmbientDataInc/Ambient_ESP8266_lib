#ifndef INA226PRC_H
#define INA226PRC_H

#include <Wire.h>

class INA226PRC
{
public:
    INA226PRC(void);

    virtual ~INA226PRC();
  
    void begin(int sda, int sck);

    int readId(void);

    float readCurrent(void);
  
    float readVoltage(void);

    short readCurrentReg(void);
  
    short readVoltageReg(void);

private:
};

#endif // INA226PRC_H

