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

#include "thingProperties.h"

void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500);

  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  
  pumpOneOff();
  pumpTwoOff();

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

const unsigned long twentyMinutes = 1000 * 60 * 20;

unsigned long currentMillis = 0; 
unsigned long stopPumpOneMillis = 0;
unsigned long stopPumpTwoMillis = 0;
unsigned long startPumpTwoMillis = 0;
bool inWateringCycle = false;

void loop() {
  ArduinoCloud.update();

  currentMillis = millis();

  if (digitalRead(1) == LOW && currentMillis > stopPumpOneMillis) {
    pumpOneOff();
  }

  if (digitalRead(2) == LOW && currentMillis > stopPumpTwoMillis) {
    pumpTwoOff();
  }

  if (inWateringCycle && currentMillis > startPumpTwoMillis && digitalRead(2) == HIGH) {
    drainTank();
    inWateringCycle = false;
  }
}

void pumpOneOn() {
  digitalWrite(1, LOW);
}

void pumpOneOff() {
  digitalWrite(1, HIGH);
}

void pumpTwoOn() {
  digitalWrite(2, LOW);
}

void pumpTwoOff() {
  digitalWrite(2, HIGH);
}

void fillTank(int timeInSeconds) {
  pumpOneOn();
  stopPumpOneMillis = currentMillis + timeInSeconds * 1000;
}

void drainTank() {
  pumpTwoOn();
  stopPumpTwoMillis = currentMillis + 5000;
}

/*
  Since FillByTime is READ_WRITE variable, onFillByTimeChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onFillByTimeChange()  {
  if (!fillByTime) return;
  fillTank(fillTimeInSeconds);
}

/*
  Since FullWateringCycle is READ_WRITE variable, onFullWateringCycleChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onFullWateringCycleChange()  {
  if (!fullWateringCycle) return;
  fillTank(110); // approx seconds to fill the tank to perfect height
  startPumpTwoMillis = currentMillis + twentyMinutes;
  inWateringCycle = true;
}

/*
  Since Drain is READ_WRITE variable, onDrainChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onDrainChange()  {
  if (!drain) return;
  drainTank();
}

/*
  Since FillTimeInSeconds is READ_WRITE variable, onFillTimeInSecondsChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onFillTimeInSecondsChange()  {
  // Add your code here to act upon FillTimeInSeconds change
}
