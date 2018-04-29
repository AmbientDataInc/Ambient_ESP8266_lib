# ESP8266/ESP32用Arduino・Ambientライブラリー

[Ambient](https://ambidata.io)はIoTデーターの可視化サービスです。 マイコンから送られたデーターを受信し、蓄積し、可視化(グラフ化)します。

![Ambient structure](https://ambidata.io/wp/wp-content/uploads/2016/09/AmbientStructure.jpg)

Ambientにユーザー登録(無料)し、マイコンからデーターを送ると、こんな感じでグラフ表示させることができます。

![Ambient chart](https://ambidata.io/wp/wp-content/uploads/2016/09/fig3-1024x651.jpg)

ここではArduino ESP8266/ESP32用のライブラリーとサンプルを示しています。サンプルには次のプログラムがあります。

* 環境モニター(examples/Ambient_ESP8266): ESP8266と温度・湿度センサーのHDC1000を使い、温度、湿度を測定し、Ambientに送信してモニターします。解説を[「Arduino ESP8266で温度・湿度を測定し、Ambientに送ってグラフ化する」](https://ambidata.io/docs/esp8266/)に書きました。
* 心拍モニター(examples/Ambient_HeartBeat): ESP8266と心拍センサーを使い、心拍波形をグラフ化します。解説を[「ESP8266と心拍センサーとAmbientで心拍波形をモニターする」](https://ambidata.io/examples/heartbeat/)に書きました。
* 消費電流モニター(examples/Ambient_CurrentLogger): ESP8266と電流値モジュールINA226PRCを使い、マイコンの消費電流をモニターします。解説を[「AmbientとArduino ESP8266でマイコンの消費電流モニターを作る」](https://ambidata.io/examples/current-logger/)に書きました。
* 電力モニター(examples/Ambient_powermonitor): ESP8266と電流クランプで家庭などの電力使用状況をモニターします。解説を[「Arduino ESP8266とAmbientで家の電力使用をモニターする」](https://ambidata.io/examples/powermonitor/)に書きました。
* ESPr DeveloperとBME280の動作確認(examples/Ambient_ESP_BME280_test/): ESPr DeveloperとBME280の動作確認プログラムです。
* ESPr DeveloperとBME280で温度、湿度、気圧を測定(examples/Ambient_ESP_BME280/): ESPr DeveloperとBME280で温度、湿度、気圧を測定し、Ambientに送信します。測定と測定の間はdelay()で待ちます。
* ESPr DeveloperとBME280で温度、湿度、気圧を測定(examples/Ambient_ESP_BME280_ds/): ESPr DeveloperとBME280で温度、湿度、気圧を測定し、Ambientに送信します。測定と測定の間はDeep Sleepで待ちます。


## 材料

* Ambient.h: Ambient用のヘッダーファイル
* Ambient.cpp: Ambientライブラリーのソースファイル
* examples/Ambient_ESP8266: 環境モニターのソースファイル
* examples/Ambient_HeartBeat: 心拍モニターのソースファイル
* examples/Ambient_CurrentLogger: 消費電流モニターのソースファイル
* examples/Ambient_powermonitor: 電力モニターのソースファイル
* examples/Ambient_ESP_BME280_test/: ESPr DeveloperとBME280の動作確認プログラム
* examples/Ambient_ESP_BME280/: ESPr DeveloperとBME280で温度、湿度、気圧を測定
* examples/Ambient_ESP_BME280_ds/: ESPr DeveloperとBME280で温度、湿度、気圧を測定 (Deep Sleep版)

ライブラリーのインポート方法、サンプルプログラムの動かし方は[Ambientサイト](https://ambidata.io/docs/esp8266/)に書きましたので、ご参照ください。
