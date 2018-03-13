/*
 * M5StackとBME280をI2C接続し、温度、湿度、気圧を測定しプリントアプトする
 */
#include <M5Stack.h>
#include <Wire.h>
#include "BME280.h"
#include "Ambient.h"

#define PERIOD 60

BME280 bme280;

WiFiClient client;
Ambient ambient;

const char* ssid = "your ssid";
const char* password = "your password";

unsigned int channelId = 100; // AmbientのチャネルID
const char* writeKey = "writeKey"; // ライトキー

void setup(){
    M5.begin();
    Wire.begin(); // I2Cの初期化
    Serial.begin(74880);
    delay(100);
    Serial.println("\r\nM5Stack+BME280->Ambient test");

    WiFi.begin(ssid, password);  //  Wi-Fi APに接続
    while (WiFi.status() != WL_CONNECTED) {  //  Wi-Fi AP接続待ち
        delay(100);
    }

    Serial.print("WiFi connected\r\nIP address: ");
    Serial.println(WiFi.localIP());

    pinMode(21, INPUT_PULLUP); //デファルトのSDAピン21　のプルアップの指定
    pinMode(22, INPUT_PULLUP); //デファルトのSDAピン22　のプルアップの指定

    bme280.begin(); // BME280の初期化

    ambient.begin(channelId, writeKey, &client); // チャネルIDとライトキーを指定してAmbientの初期化
}

void loop() {
    int t = millis();
    float temp, humid, pressure;

    // BME280で温度、湿度、気圧を測定する
    temp = (float)bme280.readTemperature();
    humid = (float)bme280.readHumidity();
    pressure = (float)bme280.readPressure();

    if (pressure > 800.0) { // 電源投入直後に異常値が読めたら捨てる
        M5.Lcd.printf("temp: %2.2f", temp);
        M5.Lcd.printf(" humid: %0.2f", humid);
        M5.Lcd.printf(" pressure: %f\r\n", pressure);

        // 温度、湿度、気圧、CO2、TVOCの値をAmbientに送信する
        ambient.set(1, String(temp).c_str());
        ambient.set(2, String(humid).c_str());
        ambient.set(3, String(pressure).c_str());

        ambient.send();
    }

    t = millis() - t;
    t = (t < PERIOD * 1000) ? (PERIOD * 1000 - t) : 1;
    delay(t);
}
