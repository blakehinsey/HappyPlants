#include <Arduino.h>

// Initialise timer and state variables
  unsigned long tCurrent; //all times are defined in seconds

//Soil and Water Level Sensors
  //Sensors and values
  const int sensSoilPin = A0; //moisture sensor pin
  int sensSoilValue;
  const int sensWaterPin = A1; //water level sensor pin
  int sensWaterValue;
  //Timers and states
  int tMeasInterval = 10; //take a measurement every tMeasInterval seconds
    //** target interval of 10 minutes (600s)
  int measState = LOW;
  unsigned long tMeasStart = 0; //time relative to tCurrent when pump state goes HIGH
  unsigned long tMeasCount = 0; //tCurrent while measState is HIGH
  int tMeasDuration = 5; // duration to take moisture level readings
    //target duration 5s
  //Thresholds
  int threshWaterValue = 700; // pump turn on water level sensor reading threshold
  int threshSoilValue; // soil moisture level limit ** is this min or max?
    //** this still isn't used in the code

//Pump control
  //Sensors
  const int potPumpDurPin = A2;
  int potPumpDurValue;
  //Timers and states
  int pumpState = LOW; // pump demand state, low: OFF
  unsigned long tPumpStart = 0; //time relative to tCurrent when pump state goes HIGH
  unsigned long tPumpCount = 0; //tCurrent while measState is HIGH
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
  Serial.print("tCurr, tMeas, tPump, tPumpDur, Soil Sensor \n"); //header for serial output
}

void loop() {
  tCurrent = millis()/1000; //time in second

  //Read the potentiometer which sets the pump on duration, defined by tPumpDurMin/tPumpDurMax
  //** potPumpDurValue = analogRead(potPumpDurPin);
  tPumpDuration = map(potPumpDurValue,10,1020,tPumpDurMin,tPumpDurMax);


  /*
   * Sensors are only enabled for short periods of times as the contacts can corrode
   */

  //Soil and water level checks
  if((tCurrent - tMeasStart) > tMeasInterval){ //check if the required time interval has passed
    measState = HIGH;
    tMeasStart = tCurrent;
  }

  //Enable the sensor and take reading for tMeasDuration seconds
  if(measState == HIGH && (tCurrent - tMeasStart) <= tMeasDuration){
    tMeasCount = tCurrent;
    digitalWrite(13,HIGH); //signal to turn on soil measurement
    sensSoilValue = analogRead(sensSoilPin);
    //** if can run both sensors in parallell, use the same trigger ie: measState should turn on moisture and water level sensor
  }
  else {
    //** add a loop of logic to set the 'sensSoilValueLast' and sensWaterValueLast on the falling edge of
    //** measState for the pump state machine to use
    measState = LOW;
    tMeasCount = 0;
    sensSoilValue = 0;
    digitalWrite(13,LOW); //turn off the sensors
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
  if(sensWaterValue > threshWaterValue){ //** pick a sensible value for sensor out of water contact
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
