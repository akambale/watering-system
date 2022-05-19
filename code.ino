/*
  The following variables are automatically generated and updated when changes are made to the Thing

  int fillTimeInSeconds;
  bool drain;
  bool fillByTime;
  bool fullWateringCycle;

  Variables which are marked as READ/WRITE in the Cloud Thing will also have functions
  which are called when their values are changed from the Dashboard.
  These functions are generated with the Thing and added at the end of this sketch.
*/

/*********** GLOBALS & CONSTANTS ***********/

#include "thingProperties.h"

// approx seconds to fill the tank to perfect height
const int fullFillTime = 130;
const int oneSecond = 1000;
const int oneMinute = oneSecond * 60;
const unsigned long twentyMinutes = oneMinute * 20;

const bool fillPump = true;
const bool drainPump = false;

unsigned long currentMillis = 0; 
unsigned long stopFillPumpMillis = 0;
unsigned long stopDrainPumpMillis = 0;
unsigned long startPumpTwoMillis = 0;

bool inWateringCycle = false;

/****************** SETUP ******************/

void setup() {
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  
  turnOff(fillPump);
  turnOff(drainPump);

  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500);

  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  /*
     The following function allows you to obtain more information
     related to the state of network and IoT Cloud connection and errors
     the higher number the more granular information you’ll get.
     The default is 0 (only errors).
     Maximum is 4
  */
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
}


/****************** LOOP ******************/

void loop() {
  ArduinoCloud.update();

  currentMillis = millis();

  if (isOn(fillPump) && currentMillis > stopFillPumpMillis) {
    turnOff(fillPump);
  }

  if (isOn(drainPump)  && currentMillis > stopDrainPumpMillis) {
    turnOff(drainPump);
  }

  // this odd order was chosen to figure out what is
  // most likely to trigger short circuit evaluation
  if (inWateringCycle && currentMillis > startPumpTwoMillis && isOff(drainPump)) {
    drainTank();
    inWateringCycle = false;
  }
}

/*************** PUMP CONTROL ***************/

void turnOn(bool pump) {
  const int pumpNum = pump ? 1 : 2;
  digitalWrite(pumpNum, LOW);
}

void turnOff(bool pump) {
  const int pumpNum = pump ? 1 : 2;
  digitalWrite(pumpNum, HIGH);
}

void stopAllPumps() {
  turnOff(fillPump);
  turnOff(drainPump);
}

/*************** PUMP STATUS ***************/

bool isOn(bool pump) {
  const int pumpNum = pump ? 1 : 2;
  return digitalRead(pumpNum) == LOW;
}

bool isOff(bool pump) {
  const int pumpNum = pump ? 1 : 2;
  return digitalRead(pumpNum) == HIGH;
}

/*************** OPERATIONS ***************/

void fillTank(int timeInSeconds) {
  turnOn(fillPump);
  stopFillPumpMillis = currentMillis + timeInSeconds * oneSecond;
}

void drainTank() {
  turnOn(drainPump);
  stopDrainPumpMillis = currentMillis + oneSecond * 60;
}

/*************** EVENT HANDLERS ***************/

void onFillByTimeChange()  {
  if (!fillByTime) return;
  fillTank(fillTimeInSeconds);
}

void onFullWateringCycleChange()  {
  if (!fullWateringCycle) return;
  fillTank(fullFillTime); 
  startPumpTwoMillis = currentMillis + twentyMinutes + fullFillTime;
  inWateringCycle = true;
}

void onDrainChange()  {
  if (!drain) return;
  drainTank();
}

void onKillSwitchChange()  {
  stopAllPumps();
}

void onFillTimeInSecondsChange()  {
  // Add your code here to act upon FillTimeInSeconds change
}
