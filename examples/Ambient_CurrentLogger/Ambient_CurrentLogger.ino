#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <Wire.h>
#include "INA226PRC.h"
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
#define SDA 14
#define SCL 13

INA226PRC ina226prc;

const char* ssid = "...ssid...";
const char* password = "...password...";
WiFiClient client;

unsigned int channelId = 100;
const char* writeKey = "...writeKey...";
Ambient ambient;

#define SAMPLING 5     // サンプリング間隔(ミリ秒)
#define NSAMPLES 2500     // 5ms x 2500 = 12.5秒
#define BUFSIZE 15000

bool start;

Ticker t2;
volatile int done;
short ampbuf[NSAMPLES];
short voltbuf[NSAMPLES];
int sampleIndex;
char buffer[BUFSIZE];


void sampling() {
    short amp = ina226prc.readCurrentReg();
    short volt = ina226prc.readVoltageReg();

    if (!start) {
        if (amp > -3 && amp < 3) {
            return;
        } 
        start = true;
        DBGLED(LED, HIGH);
    }
    if (!done) {
        ampbuf[sampleIndex] = amp;
        voltbuf[sampleIndex] = volt;
        if (++sampleIndex >= NSAMPLES) {
            done = true;
            DBGLED(LED, LOW);
        }
    }
}

#define min(a,b) (((a) < (b)) ? (a) : (b))

void setup()
{
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

    ina226prc.begin(SDA, SCL);

    Serial.print("Manufacture ID: ");
    Serial.println(ina226prc.readId(), HEX);

    start = false;
    done = false;
    sampleIndex = 0;

    t2.attach_ms(SAMPLING, sampling);
    while (!done) {
        delay(0);
    }
    t2.detach();

    int sent = 0;
    int datapersend = (BUFSIZE - 44) / 41;

    while (sent < NSAMPLES) {
        sprintf(buffer, "{\"writeKey\":\"%s\",\"data\":[", writeKey);
        datapersend = min(datapersend, NSAMPLES - sent);
        DBG("data/send: ");DBG(datapersend);DBG(", sent: ");DBG(sent);DBG("\r\n");

        for (int i = 0; i < datapersend; i++) {
            char fampbuf[8], fvoltbuf[8];

            float a, v;
            a = (float)(ampbuf[sent + i]);
            a = (a < 0) ? 0 : a;
            dtostrf(a * 0.1, 3, 1, fampbuf);

            v = (float)(voltbuf[sent + i]);
            v = (v < 0) ? 0 : v;
            dtostrf(v * 1.25, 3, 1, fvoltbuf);
            
            sprintf(&buffer[strlen(buffer)], "{\"created\":%d,\"d1\":%s,\"d2\":%s},", SAMPLING * (sent + i), fampbuf, fvoltbuf);
        }
        buffer[strlen(buffer)-1] = '\0';
        sprintf(&buffer[strlen(buffer)], "]}\r\n");
        
        DBG("buf: ");DBG(strlen(buffer));DBG(" bytes\r\n");
        
        int n = ambient.bulk_send(buffer);
        DBG("sent: ");DBG(n);DBG("\r\n");

        sent += datapersend;

        delay(5000);
    }
}

void loop()
{
    while (true) {
        delay(0);
    }
}

