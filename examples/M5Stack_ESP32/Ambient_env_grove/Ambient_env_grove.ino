#include <M5Stack.h>
#include "DHT12.h"
#include <Wire.h>
#include "Adafruit_Sensor.h"
#include <Adafruit_BMP280.h>
#include "Ambient.h"

DHT12 dht12;
Adafruit_BMP280 bme;

#define TIME_TO_SLEEP  60  // Ambientにデータを送る周期

WiFiClient client;
Ambient ambient;

const char* ssid = "ssid";
const char* password = "password";

unsigned int channelId = 100; // AmbientのチャネルID  shimojima+ambient@gmail.com
const char* writeKey = "writeKey"; // ライトキー

void setup() {
    M5.begin();
    M5.Lcd.setTextSize(2);

    Wire.begin();               // I2Cを初期化する
    while (!bme.begin(0x76)) {  // BMP280を初期化する
        M5.Lcd.println("BMP280 init fail");
    }
    WiFi.begin(ssid, password);  //  Wi-Fi APに接続
    while (WiFi.status() != WL_CONNECTED) {  //  Wi-Fi AP接続待ち
        delay(500);
        Serial.print(".");
    }
    Serial.print("WiFi connected\r\nIP address: ");
    Serial.println(WiFi.localIP());

    ambient.begin(channelId, writeKey, &client); // チャネルIDとライトキーを指定してAmbientの初期化
}

void loop() {
    float tmp = dht12.readTemperature();
    float humid = dht12.readHumidity();
    float press = bme.readPressure() / 100.0;

    M5.Lcd.setCursor(40, 40);
    M5.Lcd.printf("temp: %4.1f'C\r\n", tmp);
    M5.Lcd.setCursor(40, 80);
    M5.Lcd.printf("humid:%4.1f%%\r\n", humid);
    M5.Lcd.setCursor(40, 120);
    M5.Lcd.printf("press:%4.0fhPa\r\n", press);

    // 温度、湿度、気圧、バッテリー電圧の値をAmbientに送信する
    ambient.set(1, tmp);
    ambient.set(2, humid);
    ambient.set(3, press);

    ambient.send();

    delay(TIME_TO_SLEEP * 1000);
}
