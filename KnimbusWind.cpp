#include "KnimbusWind.h"



volatile unsigned long Rotations;  // cup rotation counter used in interrupt routine
volatile unsigned long ContactBounceTime;  // Timer to avoid contact bounce in interrupt routine

float WindSpeed;        // speed miles per hour

bool KnimbusWind::InitializeWindSensor() {
  pinMode(AnemometerPin, INPUT);
  Serial.println(F("Davis Wind Speed Test"));
  Serial.println(F("Rotations\tMPH"));
  return true;
}

int KnimbusWind::GetWindSpeedValue() {
  Rotations = 0;   //Set NbTops to 0 ready for calculations
  delay (3000);  //Wait 3 seconds to average
  // convert to mp/h using the formula V=P(2.25/T)
  // V = P(2.25/3) = P * 0.75

  WindSpeed = (int)((Rotations * 0.75) * 100);

  Serial.print(Rotations);   Serial.print(F("\t\t"));
  Serial.println(WindSpeed);

  return WindSpeed;
}

int KnimbusWind::GetWindDirection() {
  int CalDirection;// converted value with offset applied 
  int VaneValue;
  int Direction;
  
  VaneValue = analogRead(WindVanePin);
  Direction = map(VaneValue, 0, 1023, 0, 360);
  
  CalDirection = Direction + Offset; 
  
  if(CalDirection > 360) 
  CalDirection = CalDirection - 360; 
  
  if(CalDirection < 0) 
  CalDirection = CalDirection + 360; 

  Serial.print(F("VaneValue: ")); Serial.println(VaneValue);
  Serial.print(F("Direction: ")); Serial.println(Direction);
  Serial.print(F("Calibrated Direction: ")); Serial.println(CalDirection);
  
  return CalDirection;
}

void KnimbusWind::handleInterrupt() {
  if ((millis() - ContactBounceTime) > 15 ) {  // debounce the switch contact.
    Rotations++;
    ContactBounceTime = millis();
  }
}
