#include "MCP3002.h"

MCP3002::MCP3002(void) {
}

MCP3002::~MCP3002() {
}

void MCP3002::begin(void) {
  pinMode(15, OUTPUT);
  SPI.begin();
  SPI.setBitOrder (MSBFIRST);
}

int MCP3002::readData(int ch) {
  byte cmd = (ch == 0) ? 0x60 : 0x70;
  digitalWrite(15, LOW);
  byte MSB = SPI.transfer(0x60); // read from ch0
  byte LSB = SPI.transfer(0x00);
  digitalWrite(15, HIGH);

  return ((MSB << 8) + LSB) & 0x3ff;
}

