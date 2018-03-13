#ifndef MCP3002_H
#define MCP3002_H

#include <SPI.h>

class MCP3002
{
public:
  MCP3002(void);

  virtual ~MCP3002();

  void begin(int cs);

  int readData(int ch);
private:
  int mcp3002_cs;
};

#endif // MCP3002_H


