#include <ESP8266WiFi.h>

#include <Ticker.h>
#include <SPI.h>
#include "MCP3002.h"
#include "Ambient.h"

#define _DEBUG 1

#if _DEBUG
#define DBG(...) { Serial.print(__VA_ARGS__); }
#define ERR(...) { Serial.print(__VA_ARGS__); }
#else
#define DBG(...)
#define ERR(...)
#endif /* _DBG */

extern double squareRoot(double);

const char* ssid = "yourSSID";
const char* password = "yourPassword";

WiFiClient client;

unsigned int channelId = 100;
const char* writeKey = "writeKey";
Ambient ambient;

Ticker t1, t2;
volatile int t1flag, t2flag;

#define LED 4

#define PERIOD 30            // 送信間隔(秒)
#define SAMPLE_PERIOD 1     // サンプリング間隔(ミリ秒)
#define SAMPLE_SIZE 100     // 1ms x 100 = 100ms

MCP3002 mcp3002;

void t1callback() {
  t1flag = 1;
}

void t2callback() {
  t2flag = 1;
}


float ampRead(int ch) {
  int vt;
  float amp, ampsum;
  t2flag = 0;
  ampsum = 0;
  
  t2.attach_ms(SAMPLE_PERIOD, t2callback);
  
  for (int i = 0; i < SAMPLE_SIZE; i++) {
    while (t2flag == 0) {        // 測定開始の割込み待ち
      yield();
    }
    t2flag = 0;
    
    vt = mcp3002.readData(ch) - 512;
    amp = (float)vt * 3.3 / 1023.0 * 3000.0 / 100.0;
    ampsum += amp * amp;
  }

  t2.detach();
  
  return ((float)squareRoot((double)(ampsum / SAMPLE_SIZE)));
}

void setup() {
#if _DEBUG
  Serial.begin(115200);
#endif
  delay(80);
  DBG("\nPower Monitor\n");

  pinMode(LED, OUTPUT);

  while (WiFi.status() != WL_CONNECTED) {  //  Wi-Fi AP接続待ち
    delay(0);
  }

  DBG("WiFi connected\r\nIP address: ");
  DBG(WiFi.localIP());
  DBG("\r\n");
    
  ambient.begin(channelId, writeKey, &client);

  mcp3002.begin(15);

  t1flag = 0;
  t1.attach(PERIOD, t1callback);
}

void loop() {
  float amp0, amp1;
  char buf0[12], buf1[12];
  
  while (t1flag == 0) {
    yield();
  }
  t1flag = 0;

  amp0 = ampRead(0);
  amp1 = ampRead(1);

  dtostrf(amp0, 3, 1, buf0);
  ambient.set(1, buf0);
  DBG("amp0: ");  DBG(buf0);

  dtostrf(amp1, 3, 1, buf1);
  ambient.set(2, buf1);
  DBG(", amp1: ");  DBG(buf1);  DBG("\r\n");

  digitalWrite(LED, HIGH);

  ambient.send();

  digitalWrite(LED, LOW);
}
