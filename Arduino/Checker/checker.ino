#include <WiFi.h>
#include <WebServer.h>
#include "resources.h"

// --- MOTOR PIN CONFIGURATION ---
const int PIN_PWMA = 5;
const int PIN_AI2  = 4;
const int PIN_AI1  = 3;

const int PIN_PWMB = 8;
const int PIN_BI2  = 9;
const int PIN_BI1  = 10;

const int MOTOR_SPEED = 210; // PWM: 0 - 255

// --- WI-FI ---
const char* ssid = "CheckerJF";
const char* password = "checker0505";

WebServer server(80);

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

// --- API COMMAND HANDLER ---
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

void setup() {
  Serial.begin(115200);

  // Waiting for connection
  unsigned long start = millis();
  while (!Serial && (millis() - start < 3000));

  Serial.println("\n--- CHECKER ---");

  // INITIALIZATION TB6612 PINS
  pinMode(PIN_PWMA, OUTPUT);
  pinMode(PIN_AI1, OUTPUT);
  pinMode(PIN_AI2, OUTPUT);
  pinMode(PIN_PWMB, OUTPUT);
  pinMode(PIN_BI1, OUTPUT);
  pinMode(PIN_BI2, OUTPUT);

  stopMotors();

  // Wi-Fi Configuration and start
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("Controller address: http://");
  Serial.println(WiFi.softAPIP());

  // HTTP
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", PAGE_HTML);
  });
  server.on("/cmd", HTTP_GET, handleCommand);

  server.begin();
  Serial.println("HTTP Server work correctly");
  Serial.println("----------------------------------------");
}

void loop() {
  server.handleClient();
}