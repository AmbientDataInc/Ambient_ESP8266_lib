/*
 * BME280で5分毎に温度、湿度、気圧を測定し、Ambientに送信する
 * 測定と測定の間はDeep Sleepで待つ
 */
#include <ESP8266WiFi.h>
#include <SPI.h>
#include "BME280_SPI.h"
#include "Ambient.h"

extern "C" {
#include "user_interface.h"
}

#define BME_CS 15

#define PERIOD 300

BME280 bme280;
WiFiClient client;
Ambient ambient;

const char* ssid = "...ssid...";
const char* password = "...password...";

unsigned int channelId = 100; // AmbientのチャネルID
const char* writeKey = "...writeKey..."; // ライトキー

void setup()
{
    int t = millis();
    wifi_set_sleep_type(LIGHT_SLEEP_T);

    Serial.begin(115200);
    delay(10);

    Serial.println("Start");

    WiFi.begin(ssid, password);  //  Wi-Fi APに接続
    while (WiFi.status() != WL_CONNECTED) {  //  Wi-Fi AP接続待ち
        delay(100);
    }

    Serial.print("WiFi connected\r\nIP address: ");
    Serial.println(WiFi.localIP());

    bme280.begin(BME_CS);

    ambient.begin(channelId, writeKey, &client); // チャネルIDとライトキーを指定してAmbientの初期化

    double temp, humid, pressure;

    temp = bme280.readTemperature();
    humid = bme280.readHumidity();
    pressure = bme280.readPressure();

    Serial.print("temp: ");
    Serial.print(temp);
    Serial.print(", humid: ");
    Serial.print(humid);
    Serial.print(", pressure: ");
    Serial.println(pressure);

    ambient.set(1, temp); // 温度をデータ1にセット
    ambient.set(2, humid); // 湿度をデータ2にセット
    ambient.set(3, pressure); // 気圧をデータ3にセット

    ambient.send(); // データをAmbientに送信

    t = millis() - t;
    t = (t < PERIOD * 1000) ? (PERIOD * 1000 - t) : 1;
    ESP.deepSleep(t * 1000, RF_DEFAULT);
    delay(1000);
}

void loop()
{
}

