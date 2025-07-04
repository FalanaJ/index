const int leds[] = {2, 3, 4, 5};
const int numLeds = 4;
int mainCounter = 0;

void setup() {
  for (int i = 0; i < numLeds; i++) {
    pinMode(leds[i], OUTPUT);
  }
  Serial.begin(9600);
}

void loop() {
  Serial.println(mainCounter);
  showBinary(mainCounter);
  delay(2000);

  mainCounter++;
  if (mainCounter > 15) {
    mainCounter = 0;
  }
}

void showBinary(int value) {
  for (int i = 0; i < numLeds; i++) {
    digitalWrite(leds[i], (value >> i) & 1); 
  }
}

