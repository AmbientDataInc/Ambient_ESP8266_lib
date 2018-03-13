#include "MCP3002.h"

MCP3002::MCP3002(void) {
}

MCP3002::~MCP3002() {
}

void MCP3002::begin(int cs) {
  mcp3002_cs = cs;
  pinMode(cs, OUTPUT);
  digitalWrite(mcp3002_cs, HIGH);
  SPI.begin();
  SPI.setBitOrder (MSBFIRST);
}

int MCP3002::readData(int ch) {
  byte cmd = (ch == 0) ? 0x60 : 0x70;
  digitalWrite(mcp3002_cs, LOW);
  byte MSB = SPI.transfer(cmd); // read from ch
  byte LSB = SPI.transfer(0x00);
  digitalWrite(mcp3002_cs, HIGH);

  return ((MSB << 8) + LSB) & 0x3ff;
}

