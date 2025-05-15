// === MODIFIED CODE 2 ===

#define BLYNK_TEMPLATE_ID "TMPL3cG2rQqt6"
#define BLYNK_TEMPLATE_NAME "FINALRELAY"
#define BLYNK_AUTH_TOKEN "7jRg_uPB-krdLDDCkx9un70j-cV67qwW"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <SinricPro.h>
#include <SinricProSwitch.h>

#define DHTPIN 5
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define relay1Pin 14
#define relay2Pin 12

#define DEVICE_ID_1 "67f2c5fd8ed485694c0ba0b1"
#define DEVICE_ID_2 "67f2c683bddfc53e33cae0f8"
#define APP_KEY     "4b02bdf6-a926-405f-a4f7-b72cb52370a6"
#define APP_SECRET  "dd2bdc67-ff66-41c2-a0ef-89cea0c899d9-8b8443c4-c678-439c-a8ab-35a2dd38c684"

char ssid[] = "MumbaiGalaxy";
char pass[] = "12345678";

BlynkTimer timer;

bool onPowerState(const String &deviceId, bool state) {
  if (deviceId == DEVICE_ID_1) digitalWrite(relay1Pin, state ? LOW : HIGH);
  else if (deviceId == DEVICE_ID_2) digitalWrite(relay2Pin, state ? LOW : HIGH);
  return true;
}

void setupSinric() {
  SinricProSwitch &sw1 = SinricPro[DEVICE_ID_1];
  SinricProSwitch &sw2 = SinricPro[DEVICE_ID_2];
  sw1.onPowerState(onPowerState);
  sw2.onPowerState(onPowerState);
  SinricPro.begin(APP_KEY, APP_SECRET);
  SinricPro.restoreDeviceStates(true);
}

void setup() {
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  pinMode(relay1Pin, OUTPUT);
  pinMode(relay2Pin, OUTPUT);
  digitalWrite(relay1Pin, LOW);
  digitalWrite(relay2Pin, LOW);
  dht.begin();
  setupSinric();
  timer.setInterval(1000L, sendDataToBlynk);
}

void loop() {
  Blynk.run();
  timer.run();
  SinricPro.handle();
}

void sendSensorData() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int gasLevel = analogRead(A0);

  Blynk.virtualWrite(V2, h);
  Blynk.virtualWrite(V3, t);
  Blynk.virtualWrite(V7, gasLevel);
}

BLYNK_WRITE(V8) {
  int relay1State = param.asInt();
  digitalWrite(relay1Pin, relay1State == 0 ? HIGH : LOW);
}

BLYNK_WRITE(V9) {
  int relay2State = param.asInt();
  digitalWrite(relay2Pin, relay2State == 0 ? HIGH : LOW);
}

void sendDataToBlynk() {
  sendSensorData();
}
