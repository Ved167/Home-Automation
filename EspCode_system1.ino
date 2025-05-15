// === MODIFIED CODE 1 ===

#define BLYNK_TEMPLATE_ID "TMPL3cG2rQqt6"
#define BLYNK_TEMPLATE_NAME "FINALRELAY"
#define BLYNK_AUTH_TOKEN "7jRg_uPB-krdLDDCkx9un70j-cV67qwW"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SinricPro.h>
#include <SinricProSwitch.h>
#include <SoftwareSerial.h>

// WiFi credentials
char ssid[] = "MumbaiGalaxy";
char pass[] = "12345678";

// Moisture relay pins
#define RELAY3 14  // D5
#define RELAY4 12  // D6

// Ultrasonic sensor pins
#define TRIG_PIN 5  // D1
#define ECHO_PIN 4  // D2

// SinricPro credentials
#define APP_KEY     "4b02bdf6-a926-405f-a4f7-b72cb52370a6"
#define APP_SECRET  "dd2bdc67-ff66-41c2-a0ef-89cea0c899d9-8b8443c4-c678-439c-a8ab-35a2dd38c684"
#define DEVICE_ID_3 "67f2c6b2dc4a25d5c3a30d57"  // Relay3

// Thresholds
#define MOISTURE_ON_THRESHOLD 45
#define MOISTURE_OFF_THRESHOLD 60
#define TANK_HEIGHT_CM 27
#define WATER_LEVEL_THRESHOLD 10

// Global variables
int m3 = 0, m4 = 0;
bool autoRelay3 = false, autoRelay4 = false;
bool waterNotified = false;

SoftwareSerial mySerial(13, 15); // RX = D7, TX = D8
String inputString = "";

void setupRelays() {
  pinMode(RELAY3, OUTPUT);
  pinMode(RELAY4, OUTPUT);
  digitalWrite(RELAY3, HIGH);
  digitalWrite(RELAY4, HIGH);
}

BLYNK_WRITE(V0) { digitalWrite(RELAY3, param.asInt() ? LOW : HIGH); }
BLYNK_WRITE(V1) { digitalWrite(RELAY4, param.asInt() ? LOW : HIGH); }

bool onPowerState3(const String &deviceId, bool state) {
  if (deviceId == DEVICE_ID_3) digitalWrite(RELAY3, state ? LOW : HIGH);
  return true;
}

void setupSinric() {
  SinricProSwitch &sw3 = SinricPro[DEVICE_ID_3];
  sw3.onPowerState(onPowerState3);
  SinricPro.begin(APP_KEY, APP_SECRET);
  SinricPro.restoreDeviceStates(true);
}

void setupUltrasonic() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  setupRelays();
  setupUltrasonic();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  setupSinric();
}

void loop() {
  Blynk.run();
  SinricPro.handle();
  readMoistureData();
  measureWaterLevel();
}

void readMoistureData() {
  while (mySerial.available()) {
    char c = mySerial.read();
    if (c == '\n' || c == '\r') {
      if (inputString.length() > 0) {
        sscanf(inputString.c_str(), "%d,%d", &m3, &m4);
        inputString = "";
        Blynk.virtualWrite(V4, m3);
        Blynk.virtualWrite(V5, m4);
        autoControl();
      }
    } else {
      inputString += c;
    }
  }
}

void autoControl() {
  if (m3 < MOISTURE_ON_THRESHOLD && !autoRelay3) {
    digitalWrite(RELAY3, LOW); autoRelay3 = true;
  } else if (m3 > MOISTURE_OFF_THRESHOLD && autoRelay3) {
    digitalWrite(RELAY3, HIGH); autoRelay3 = false;
  }

  if (m4 < MOISTURE_ON_THRESHOLD && !autoRelay4) {
    digitalWrite(RELAY4, LOW); autoRelay4 = true;
  } else if (m4 > MOISTURE_OFF_THRESHOLD && autoRelay4) {
    digitalWrite(RELAY4, HIGH); autoRelay4 = false;
  }
}

void measureWaterLevel() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  int distance = duration * 0.0344 / 2;
  int water_level = TANK_HEIGHT_CM - distance;

  if (water_level < 0) water_level = 0;
  if (water_level > TANK_HEIGHT_CM) water_level = TANK_HEIGHT_CM;

  Blynk.virtualWrite(V10, water_level);

  if (water_level < WATER_LEVEL_THRESHOLD && !waterNotified) {
    Blynk.logEvent("low_water", "Water level is low, fill it quickly!!");
    waterNotified = true;
  } else if (water_level >= WATER_LEVEL_THRESHOLD && waterNotified) {
    waterNotified = false;
  }
}
