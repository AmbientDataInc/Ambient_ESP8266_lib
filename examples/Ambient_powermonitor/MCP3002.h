#ifndef MCP3002_H
#define MCP3002_H

#include <SPI.h>

class MCP3002
{
public:
  MCP3002(void);

  virtual ~MCP3002();

  void begin(void);

  int readData(int ch);
};

#endif // MCP3002_H


