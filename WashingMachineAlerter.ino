#include <Wire.h>
#include <Adafruit_TCS34725.h>
#include <arduino.h>

#define speakerPinA 3
#define speakerPinB 4
#define resetAlarmButton 5
#define toneA 7
#define toneB 9
#define timePerToneInMillis 750     //(3/4ths second)
#define alarmThreshholdBrightness 200

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

volatile unsigned long speakerTimer;
volatile unsigned long TimeSinceLastVoltageReversal;
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
  delay(60);  // takes 50ms to read
  tcs.getRawData(&red, &green, &blue, &clear);
  Serial.println("Green level: " + String(green));
}

void alarmLoop() {
  while (!alarmReset) {
    if (digitalRead(resetAlarmButton)) {
      Serial.println(F("Alarm Button Pushed"));
      alarmReset = true;
      digitalWrite(speakerPinA, LOW);
      digitalWrite(speakerPinB, LOW);
      return;
    }

    if (speakerTimer + timePerToneInMillis < millis()) {
      speakerTimer = millis();
      toneState = !toneState;
    }

    //lazy copy-paste
    if (toneState) {
      if (TimeSinceLastVoltageReversal + toneA < millis()) {
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

void swapPolarity(){
  //Serial.println(F("Swapped polarity"));
  speakerPinState = !speakerPinState;
  digitalWrite (speakerPinA, speakerPinState);
  digitalWrite (speakerPinB, !speakerPinState);
}

