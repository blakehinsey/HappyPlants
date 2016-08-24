#include <Arduino.h>


// Sensor Input Definition
  const int sensSoilPin = A0; //moisture sensor pin
  int sensSoilValue; // ** What sensor is this? There will be multiple sensors
  const int sensWaterPin = A1; //water level sensor pin
  int sensWaterValue;
  //** tAMB and rHUM
  const int potPumpDurPin = A2;
  int potPumpDurValue;

// Initialise timer and state variables
  unsigned long tCurrent; //all times are defined in seconds

  //Soil and Water Level Sensors
  int tMeasInterval = 10; //take a measurement every tMeasInterval seconds
    //** target interval of 10 minutes (600s)
  int measState = LOW;
  unsigned long tMeasStart = 0;
  unsigned long tMeasCount = 0;
  int tMeasDuration = 5; // duration to take moisture level readings
    //target duration 5s
  int threshWaterValue; // pump turn on water level sensor reading threshold
  int threshSoilValue; // soil moisture level limit ** is this min or max?

  //Pump control
  int pumpState = LOW; // pump demand state, low: OFF
  unsigned long tPumpStart = 0;
  unsigned long tPumpCount = 0;
  int tPumpInterval = 20; //how often should the pump be turned on? suggest 6 hours (2160s)
  int tPumpDuration = 3; //** can bre removed, fixed duration for pump to stay on
  const int tPumpDurMin = 2; //seconds
  const int tPumpDurMax = 7; //seconds
    //** suggest using tPump from 10 to 120 seconds

void setup() {
  Serial.begin(9600);
  pinMode(13,OUTPUT); //signal to turn on the moisture sensor
  digitalWrite(13,LOW); //initialise
  //** setup pin to turn on water level sensor
  //** pinMode(2,OUTPUT); //signal to turn on the pump
  Serial.print("tCurr, tMeas, tPump, tPumpDur, Soil Sensor \n");
}

void loop() {
  tCurrent = millis()/1000; //time in seconds

  tPumpDuration = map(potPumpDurValue,10,1020,tPumpDurMin,tPumpDurMax);

  //Time interval between taking measurements
    if((tCurrent - tMeasStart) > tMeasInterval){ //check if the required time interval has passed
      measState = HIGH;
      tMeasStart = tCurrent;
    }

  //Enable the sensor and take reading
  if(measState == HIGH && (tCurrent - tMeasStart) <= tMeasDuration){
    tMeasCount = tCurrent;
    digitalWrite(13,HIGH); //signal to turn on soil measurement
    sensSoilValue = analogRead(sensSoilPin);

    //** if can run both sensors in parallell, use the same trigger
  }
  else {
    measState = LOW;
    tMeasCount = 0;
    sensSoilValue = 0;
    digitalWrite(13,LOW);
  }

//Pump Control
/* The pump timer will turn on every tPumpInterval unless:
 * Water tank is low OR
 * Soil moisture is too high
 * If either condition is true wait another tPumpInterval
 */

  // Pump Timer
    if((tCurrent - tPumpStart) > tPumpInterval){ //check if the required time interval has passed
      pumpState = HIGH;
      tPumpStart = tCurrent;
    }
  // Check the status of the water level sensor
    //** add thresholds declared in header threshWaterValue and threshSoilValue to set cond below
  if(sensWaterValue > 700){ //** pick a sensible value for sensor out of water contact
    pumpState = LOW;
    //** turn on low water level LED
  }

  //** check moisture level sensor, if too high skip watering cycle
  if(pumpState == HIGH && (tCurrent - tPumpStart) <= tPumpDuration){
    tPumpCount = tCurrent;
  }
  else{
    pumpState = LOW;
    tPumpCount = 0;
  }

  Serial.print(tCurrent);
  Serial.print(", ");
  Serial.print(tMeasCount);
  Serial.print(", ");
  Serial.print(tPumpCount);
  Serial.print(", ");
  Serial.print(tPumpDuration);
  Serial.print(", ");
  if(measState == HIGH) Serial.print(sensSoilValue);
  Serial.print("\n");
  delay(1000);
}
