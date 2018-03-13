#include <ESP8266WiFi.h>
#include <Wire.h>
#include "Ambient.h"
#include "HDC1000.h"

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
#define RDY 12

#define PERIOD 30

const char* ssid = "・・・ssid・・・";
const char* password = "・・・パスワード・・・";

unsigned int channelId = 100;
const char* writeKey = "ライトキー";

WiFiClient client;
Ambient ambient;

HDC1000 hdc1000;

void setup()
{
    wifi_set_sleep_type(LIGHT_SLEEP_T);

#ifdef _DEBUG
    Serial.begin(115200);
    delay(20);
#endif
    DBG("Start");
    pinMode(LED, OUTPUT);

    WiFi.begin(ssid, password);

    int i = 0;
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);

        DBGLED(LED, i++ % 2);
        DBG(".");
    }

    DBGLED(LED, LOW);
    DBG("WiFi connected\r\n");
    DBG("IP address: ");
    DBG(WiFi.localIP());
    DBG("\r\n");

    ambient.begin(channelId, writeKey, &client);
    hdc1000.begin(SDA, SCL, RDY);

    DBG("Manufacture ID: ");
    DBG(hdc1000.readId(), HEX);
    DBG("\r\n");
}

void loop()
{
    float temp, humid;
    char humidbuf[12];

    temp = hdc1000.readTemperature();
    humid = hdc1000.readHumidity();

    DBG("temp: ");
    DBG(temp);
    DBG(" DegC,  humid: ");
    DBG(humid);
    DBG(" %\r\n");

    ambient.set(1, temp);                // データーがint型かfloat型であれば、直接セットすることができます。
    dtostrf(humid, 3, 1, humidbuf);      // データーの桁数などを制御したければ自分で文字列形式に変換し、
    ambient.set(2, humidbuf);            // セットします。

    DBGLED(LED, HIGH);

    ambient.send();

    DBGLED(LED, LOW);

    delay(PERIOD * 1000);
}
