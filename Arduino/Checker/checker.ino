#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "resources.h"

// --- KONFIGURACJA LOGIKI BUZZERA 3-PINOWEGO ---
// Jeśli buzzer milczy po naciśnięciu lub piszczy sam z siebie, zmień true na false:
const bool BUZZER_ACTIVE_LOW = true;

const int PIN_PWMA = 5;
const int PIN_AI2  = 4;
const int PIN_AI1  = 3;

const int PIN_PWMB = 8;
const int PIN_BI2  = 9;
const int PIN_BI1  = 10;

const int PIN_BUZZER = 11;
const int PIN_LIGHTS = 12;

const int MOTOR_SPEED = 210;

const char* ssid = "CheckerJF";
const char* password = "checker0505";

WebServer server(80);
Adafruit_BME280 bme;
bool bmeFound = false;
bool lightsOn = false;

unsigned long lastSerialPrint = 0;

void moveForward() {
  digitalWrite(PIN_AI1, HIGH);
  digitalWrite(PIN_AI2, LOW);
  analogWrite(PIN_PWMA, MOTOR_SPEED);

  digitalWrite(PIN_BI1, HIGH);
  digitalWrite(PIN_BI2, LOW);
  analogWrite(PIN_PWMB, MOTOR_SPEED);
}

void moveBackward() {
  digitalWrite(PIN_AI1, LOW);
  digitalWrite(PIN_AI2, HIGH);
  analogWrite(PIN_PWMA, MOTOR_SPEED);

  digitalWrite(PIN_BI1, LOW);
  digitalWrite(PIN_BI2, HIGH);
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

void toggleLights() {
  lightsOn = !lightsOn;
  digitalWrite(PIN_LIGHTS, lightsOn ? HIGH : LOW);
}

void hornOn() {
  digitalWrite(PIN_BUZZER, BUZZER_ACTIVE_LOW ? LOW : HIGH);
}

void hornOff() {
  digitalWrite(PIN_BUZZER, BUZZER_ACTIVE_LOW ? HIGH : LOW);
}

void handleCommand() {
  if (server.hasArg("val")) {
    char cmd = server.arg("val")[0];
    switch (cmd) {
      case 'F': moveForward();  break;
      case 'B': moveBackward(); break;
      case 'L': turnLeft();     break;
      case 'R': turnRight();    break;
      case 'S': stopMotors();   break;
      case 'H': toggleLights(); break;
      case 'K': hornOn();       break;
      case 'k': hornOff();      break;
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

  pinMode(PIN_PWMA, OUTPUT);
  pinMode(PIN_AI1, OUTPUT);
  pinMode(PIN_AI2, OUTPUT);
  pinMode(PIN_PWMB, OUTPUT);
  pinMode(PIN_BI1, OUTPUT);
  pinMode(PIN_BI2, OUTPUT);
  pinMode(PIN_LIGHTS, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  stopMotors();
  digitalWrite(PIN_LIGHTS, LOW);
  hornOff(); // Ustawienie stanu spoczynkowego buzzera

  if (bme.begin(0x76, &Wire)) {
    bmeFound = true;
    Serial.println("[OK] BME280 polaczony pod 0x76.");
  } else {
    Serial.println("[UWAGA] BME280 nieodnaleziony.");
  }

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("Adres: http://");
  Serial.println(WiFi.softAPIP());

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