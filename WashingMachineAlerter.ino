#include <Wire.h>
#include <Adafruit_TCS34725.h>
#include <arduino.h>

#define speakerPinA 3
#define speakerPinB 4
#define toneA 1
#define toneB 4
#define timePerToneInMillis 750
#define alarmThreshholdBrightness 200


Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_4X);

volatile unsigned long speakerTimer;
volatile unsigned long TimeSinceLastVoltageReversal;
volatile unsigned long TimeSinceLastStatusCheck;
volatile boolean       toneState;
volatile boolean       alarmReset = false;
volatile boolean       speakerPinState;
uint16_t clear, red, green, blue;


void setup() {
  Serial.begin(115200);
  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No sensor found");
    while (1); //halt
  }
  pinMode(speakerPinA, OUTPUT);
  pinMode(speakerPinB, OUTPUT);
  tcs.setInterrupt(true);
}


void loop() {
  updateBrightness();
  if (green > alarmThreshholdBrightness) {
    Serial.println(F("Entered alarm loop"));
    speakerTimer = millis();
    alarmReset = false;
    alarmLoop();
  }
}


void updateBrightness() {
  tcs.getRawData(&red, &green, &blue, &clear);
  Serial.println("Green level: " + String(green));
}


void alarmLoop() {
  TimeSinceLastStatusCheck = millis();
  while (!alarmReset){
    
    if (speakerTimer + timePerToneInMillis < millis()){
      speakerTimer = millis();
      toneState = !toneState;
      updateBrightness();
      if (green < alarmThreshholdBrightness){
        digitalWrite(speakerPinA, LOW);
        digitalWrite(speakerPinB, LOW);
        alarmReset = true;
        Serial.println(F("Alarm Reset"));
        return;
      }
    }
 
    if (toneState){
      
      if (TimeSinceLastVoltageReversal + toneA < millis()){
        swapPolarity();
        TimeSinceLastVoltageReversal = millis();
      }
      
    } else {
      
      if (TimeSinceLastVoltageReversal + toneB < millis()) {
        swapPolarity();
        TimeSinceLastVoltageReversal = millis();
      }
    }
  }
}

void swapPolarity() {
  //Serial.println(F("Swapped polarity"));
  speakerPinState = !speakerPinState;
  digitalWrite (speakerPinA, speakerPinState);
  digitalWrite (speakerPinB, !speakerPinState);
}

