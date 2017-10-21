/*
 * 心拍センサーを10ビットADコンバーターmcp3002につなぎ、
 * 5秒ごとに、5m秒周期で600回、3秒分サンプリングし、Ambientに送る
 */
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Ticker.h>
#include <SPI.h>
#include "MCP3002.h"
#include "Ambient.h"

extern "C" {
#include "user_interface.h"
}

#define _DEBUG 1
#if _DEBUG
#define DBG(...) { Serial.print(__VA_ARGS__); }
#define DBGLED(...) { digitalWrite(__VA_ARGS__); }
#else
#define DBG(...)
#define DBGLED(...)
#endif /* _DBG */

#define LED 4

#define PERIOD 5000 // milliceconds

MCP3002 mcp3002;

const char* ssid = "ssid";
const char* password = "password";
WiFiClient client;

unsigned int channelId = 100;
const char* writeKey = "...writeKey...";
const char* userKey = "...userKey...";
Ambient ambient;

#define SAMPLING 5  //  Sampling period in milliseconds
#define NSAMPLES 600  //  Number of Samples
#define BUFSIZE  17000

bool found;
unsigned int thresh = 512;
unsigned int P = 512;

Ticker t2;
volatile int done;
unsigned int signal[NSAMPLES+50];
int sampleIndex;
char buffer[BUFSIZE];

void sampling() {
    unsigned int Signal = mcp3002.readData(0);

    if (!found) {
        if (Signal > thresh && Signal > P) { // thresh(ほぼ振幅の1/2)以上の最大値を見つける => P
            P = Signal;
        }
        if (P > thresh && Signal < thresh) { // Pが見つかって、信号がthresh未満になったら
            found = true;                    // 測定開始
            DBGLED(LED, HIGH);
        }
    }
    if (!found) {
        return;
    }
    if (!done) {
        signal[sampleIndex++] = Signal;
        if (sampleIndex >= (NSAMPLES + 50)) {
            done = true;
            DBGLED(LED, LOW);
        }
    }
}

void setup()
{
    wifi_set_sleep_type(LIGHT_SLEEP_T);

#ifdef _DEBUG
    Serial.begin(115200);
    delay(20);
#endif
    DBG("Start\r\n");
    pinMode(LED, OUTPUT);

    WiFi.begin(ssid, password);

    DBGLED(LED, HIGH);
    while (WiFi.status() != WL_CONNECTED) {
        delay(0);
    }
    DBGLED(LED, LOW);

    DBG("WiFi connected\r\nIP address: ");
    DBG(WiFi.localIP());
    DBG("\r\n");

    ambient.begin(channelId, writeKey, &client);

    mcp3002.begin();
}

void loop()
{
    unsigned long stime = millis(); // 開始時刻を記録
    found = false;
    done = false;
    sampleIndex = 0;

    t2.attach_ms(SAMPLING, sampling);
    while (!done) {
        delay(0);
    }
    t2.detach();

    ambient.delete_data(userKey);

    unsigned int T = 512;
    int start = 0;
    for (int i = 0; i < 50; i++) { // 測定開始から50サンプル以内の最小値 => T
        if (signal[i] < T) {
            T = signal[i];
            start = i;              // 最小値のインデックス => start
        }
    }

    sprintf(buffer, "{\"writeKey\":\"%s\",\"data\":[", writeKey);
    for (int i = start; i < start + NSAMPLES; i++) {
        sprintf(&buffer[strlen(buffer)], "{\"created\":%d,\"d%d\":%d},", SAMPLING * i, 1, signal[i]);
    }
    buffer[strlen(buffer)-1] = '\0';
    sprintf(&buffer[strlen(buffer)], "]}\r\n");
    
    DBG("buf: ");DBG(strlen(buffer));DBG(" bytes\r\n");
    
    int sent = ambient.bulk_send(buffer);
    DBG("sent: ");DBG(sent);DBG("\r\n");

    unsigned long elapse = millis() - stime; // 経過時間を計算
    if (elapse < PERIOD) {
        delay(PERIOD - elapse);
    }
}

