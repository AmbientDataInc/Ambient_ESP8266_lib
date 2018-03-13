/*
 * M5StackとBME680をI2C接続し、温度、湿度、気圧を測定しプリントアプトする
 */
#include <M5Stack.h>
#include <Wire.h>
#include "Adafruit_BME680.h"
#include "Ambient.h"

#define PERIOD 60

Adafruit_BME680 bme680; // I2C

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

    WiFi.begin(ssid, password);  //  Wi-Fi APに接続
    while (WiFi.status() != WL_CONNECTED) {  //  Wi-Fi AP接続待ち
        delay(100);
    }

    Serial.print("WiFi connected\r\nIP address: ");
    Serial.println(WiFi.localIP());

    if (!bme680.begin()) {
        Serial.println("Could not find a valid BME680 sensor, check wiring!");
        while (1) {
            delay(0);
        }
    }
    // Set up oversampling and filter initialization
    bme680.setTemperatureOversampling(BME680_OS_8X);
    bme680.setHumidityOversampling(BME680_OS_2X);
    bme680.setPressureOversampling(BME680_OS_4X);
    bme680.setIIRFilterSize(BME680_FILTER_SIZE_3);
    bme680.setGasHeater(320, 150); // 320*C for 150 ms

    ambient.begin(channelId, writeKey, &client); // チャネルIDとライトキーを指定してAmbientの初期化

    // LCD display
    M5.Lcd.printf("BME680 test\r\n");
}

void loop() {
    int t = millis();
    float temp, humid, pressure, gas;

    // BME680で温度、湿度、気圧を測定する
    if (! bme680.performReading()) {
        Serial.println("Failed to perform reading :(");
        return;
    }
    temp = (float)bme680.temperature;
    humid = (float)bme680.humidity;
    pressure = (float)bme680.pressure / 100.0;
    gas = (float)bme680.gas_resistance / 1000.0;

    M5.Lcd.printf("temp: %2.2f", temp);
    M5.Lcd.printf(" humid: %0.2f", humid);
    M5.Lcd.printf(" pressure: %f", pressure);
    M5.Lcd.printf(" gas: %f\r\n", gas);

    // 温度、湿度、気圧、CO2、TVOCの値をAmbientに送信する
    ambient.set(1, String(temp).c_str());
    ambient.set(2, String(humid).c_str());
    ambient.set(3, String(pressure).c_str());
    ambient.set(4, String(gas).c_str());

    ambient.send();

    t = millis() - t;
    t = (t < PERIOD * 1000) ? (PERIOD * 1000 - t) : 1;
    delay(t);
}
