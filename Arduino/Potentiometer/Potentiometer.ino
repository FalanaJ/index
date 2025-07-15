const int potPin = A0;
const int ledPin = 5;

int sensorValue = 0;
int brightness = 0;

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  sensorValue = analogRead(potPin);
  float voltage = sensorValue * (5.0 / 1023.0);
  brightness = map(sensorValue, 0, 1023, 0, 255);
  analogWrite(ledPin, brightness); 

  Serial.print("sensorValue: ");
  Serial.print(sensorValue);
  Serial.print(" voltage: ");
  Serial.print(voltage);
  Serial.print(" brightness: ");
  Serial.print(brightness);
  Serial.println();
  delay(1);
}
