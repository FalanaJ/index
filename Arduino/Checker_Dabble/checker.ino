#include <ArduinoBLE.h>

// Motor driver (TB6612) -> Arduino Nano ESP32 pin definitions
const int PIN_PWMA = 5; // Left motor speed (PWM)
const int PIN_AI2  = 4; // Left motor direction 2
const int PIN_AI1  = 3; // Left motor direction 1

const int PIN_PWMB = 8; // Right motor speed (PWM)
const int PIN_BI2  = 9; // Right motor direction 2
const int PIN_BI1  = 10; // Right motor direction 1

const int MOTOR_SPEED = 200; // Speed range: 0 - 255

// BLE service and characteristic used by the Dabble app
BLEService dabbleService("0000ffe0-0000-1000-8000-00805f9b34fb");
BLECharacteristic dabbleChar("0000ffe1-0000-1000-8000-00805f9b34fb", BLENotify | BLEWriteWithoutResponse, 20);

// --- MOTION CONTROL FUNCTIONS ---

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
  // Rotate counterclockwise in place
  digitalWrite(PIN_AI1, LOW);
  digitalWrite(PIN_AI2, HIGH);
  analogWrite(PIN_PWMA, MOTOR_SPEED);

  digitalWrite(PIN_BI1, HIGH);
  digitalWrite(PIN_BI2, LOW);
  analogWrite(PIN_PWMB, MOTOR_SPEED);
}

void turnRight() {
  // Rotate clockwise in place
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

// --- SETUP AND MAIN LOOP ---

void setup() {
  // Configure motor driver control pins as outputs
  pinMode(PIN_PWMA, OUTPUT);
  pinMode(PIN_AI1, OUTPUT);
  pinMode(PIN_AI2, OUTPUT);
  pinMode(PIN_PWMB, OUTPUT);
  pinMode(PIN_BI1, OUTPUT);
  pinMode(PIN_BI2, OUTPUT);

  stopMotors();

  // Initialize the onboard BLE hardware
  if (!BLE.begin()) {
    while (1); // Halt on initialization failure
  }

  // Configure BLE advertising parameters
  BLE.setLocalName("Dabble-ESP32");
  BLE.setAdvertisedService(dabbleService);
  dabbleService.addCharacteristic(dabbleChar);
  BLE.addService(dabbleService);

  BLE.advertise();
}

void loop() {
  // Listen for incoming BLE central connections
  BLEDevice central = BLE.central();

  if (central) {
    while (central.connected()) {
      if (dabbleChar.written()) {
        const uint8_t* rawData = dabbleChar.value();
        int dataLength = dabbleChar.valueLength();

        // Dabble packet structure: [0xFF, 0x01, Module_ID, Length, Data...]
        // Module ID 0x01 corresponds to the Gamepad module.
        if (dataLength >= 6 && rawData[0] == 0xFF && rawData[2] == 0x01) {
          uint8_t dpadState = rawData[6]; // Directional buttons bitmask

          // Bitmask evaluation for directional pad inputs:
          // 0x01 = Up, 0x02 = Down, 0x04 = Left, 0x08 = Right, 0x00 = Released
          if (dpadState & 0x01) {
            moveBackward();
          } else if (dpadState & 0x02) {
            moveForward();
          } else if (dpadState & 0x04) {
            turnLeft();
          } else if (dpadState & 0x08) {
            turnRight();
          } else {
            stopMotors(); // Halt vehicle when no directional input is active
          }
        }
      }
    }
    // Safety fail-safe: stop all motors upon disconnection
    stopMotors();
  }
}