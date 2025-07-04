//Simply photoresistor test program. The higher light is, the more leds are on. 
const int redLed = 7;     
const int yellowLed = 6;
const int greenLed = 5;
const int blueLed = 4;
const int ldr = A0;

int sensor;

void setup() {
  pinMode(redLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  //the value of light 
  sensor = analogRead(ldr);
  Serial.println(sensor);

  if(sensor >= 100 && sensor < 200)
    changeLeds(0, 0, 0, 1);
  else if(sensor >= 200 && sensor < 300)
    changeLeds(0, 0, 1, 1);
  else if(sensor >= 300 && sensor < 400)
    changeLeds(0, 1, 1, 1);
  else if(sensor >= 400)
    changeLeds(1, 1, 1, 1);
  else
    changeLeds(0, 0, 0, 0);
}

//function to fast turn on/off leds
void changeLeds(int redState, int yellowState, int greenState, int blueState){
    digitalWrite(redLed, redState);
    digitalWrite(yellowLed, yellowState);
    digitalWrite(greenLed, greenState);
    digitalWrite(blueLed, blueState);
}
