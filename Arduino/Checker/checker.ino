#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "resources.h"

// --- MOTOR PIN CONFIGURATION (TB6612 -> Arduino Nano ESP32) ---
const int PIN_PWMA = 5;
const int PIN_AI2  = 4;
const int PIN_AI1  = 3;

const int PIN_PWMB = 8;
const int PIN_BI2  = 9;
const int PIN_BI1  = 10;

const int MOTOR_SPEED = 210; // PWM: 0 - 255

// --- WI-FI ACCESS POINT ---
const char* ssid = "CheckerJF";
const char* password = "checker0505";

WebServer server(80);
Adafruit_BME280 bme;
bool bmeFound = false;

unsigned long lastSerialPrint = 0;

// --- MOVE FUNCTIONS ---
void moveForward() {
  digitalWrite(PIN_AI1, LOW);
  digitalWrite(PIN_AI2, HIGH);
  analogWrite(PIN_PWMA, MOTOR_SPEED);

  digitalWrite(PIN_BI1, LOW);
  digitalWrite(PIN_BI2, HIGH);
  analogWrite(PIN_PWMB, MOTOR_SPEED);
}

void moveBackward() {
  digitalWrite(PIN_AI1, HIGH);
  digitalWrite(PIN_AI2, LOW);
  analogWrite(PIN_PWMA, MOTOR_SPEED);

  digitalWrite(PIN_BI1, HIGH);
  digitalWrite(PIN_BI2, LOW);
  analogWrite(PIN_PWMB, MOTOR_SPEED);
}

void turnLeft() {
  digitalWrite(PIN_AI1, LOW);
  digitalWrite(PIN_AI2, HIGH);
  analogWrite(PIN_PWMA, MOTOR_SPEED);

  digitalWrite(PIN_BI1, HIGH);
  digitalWrite(PIN_BI2, LOW);
  analogWrite(PIN_PWMB, MOTOR_SPEED);
}

void turnRight() {
  digitalWrite(PIN_AI1, HIGH);
  digitalWrite(PIN_AI2, LOW);
  analogWrite(PIN_PWMA, MOTOR_SPEED);

  digitalWrite(PIN_BI1, LOW);
  digitalWrite(PIN_BI2, HIGH);
  analogWrite(PIN_PWMB, MOTOR_SPEED);
}

void stopMotors() {
  analogWrite(PIN_PWMA, 0);
  analogWrite(PIN_PWMB, 0);
}

// --- HTTP ENDPOINT HANDLERS ---
void handleCommand() {
  if (server.hasArg("val")) {
    char cmd = server.arg("val")[0];
    switch (cmd) {
      case 'F': moveForward();  break;
      case 'B': moveBackward(); break;
      case 'L': turnLeft();     break;
      case 'R': turnRight();    break;
      case 'S':
      default:  stopMotors();   break;
    }
  }
  server.send(200, "text/plain", "OK");
}

void handleTelemetry() {
  float temp = 0.0, hum = 0.0, pres = 0.0;
  if (bmeFound) {
    temp = bme.readTemperature();
    hum  = bme.readHumidity();
    pres = bme.readPressure() / 100.0F;
  }
  char json[120];
  snprintf(json, sizeof(json), "{\"temp\":%.1f,\"hum\":%.1f,\"pres\":%.2f}", temp, hum, pres);
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);

  unsigned long start = millis();
  while (!Serial && (millis() - start < 3000));

  Serial.println("\n--- CHECKER 1.1 START ---");

  // MOTOR PINS INITIALIZATION
  pinMode(PIN_PWMA, OUTPUT);
  pinMode(PIN_AI1, OUTPUT);
  pinMode(PIN_AI2, OUTPUT);
  pinMode(PIN_PWMB, OUTPUT);
  pinMode(PIN_BI1, OUTPUT);
  pinMode(PIN_BI2, OUTPUT);

  stopMotors();

  // BME280 (I2C with 0x76) INITIALIZATION
  if (bme.begin(0x76, &Wire)) {
    bmeFound = true;
    Serial.println("[OK] BME280 polaczony pod 0x76.");
  } else {
    Serial.println("[UWAGA] BME280 nieodnaleziony.");
  }

  // WI-FI
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("Adres: http://");
  Serial.println(WiFi.softAPIP());

  //ROUTES
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", PAGE_HTML);
  });
  server.on("/cmd", HTTP_GET, handleCommand);
  server.on("/data", HTTP_GET, handleTelemetry);

  server.begin();
  Serial.println("Serwer HTTP wystartowal.");
  Serial.println("----------------------------------------");
}

void loop() {
  server.handleClient();

  if (bmeFound && (millis() - lastSerialPrint >= 2000)) {
    lastSerialPrint = millis();

    float t = bme.readTemperature();
    float h = bme.readHumidity();
    float p = bme.readPressure() / 100.0F;

  }
}