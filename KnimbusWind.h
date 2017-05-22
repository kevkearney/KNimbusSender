/*
  KnimbusWind.h - Library for using Davis Vantage Pro Wind Sensor with knimbus weather station.
  Created by Kevin D. Kearney, May 20, 2017.
  Released into the public domain.
*/

#ifndef KnimbusWind_h
#define KnimbusWind_h
#include "Arduino.h"
#include <math.h>
#define AnemometerPin (A1)      //The pin location of the anemometer sensor
#define WindVanePin (A0)      //The pin location of the anemometer sensor
#define Offset 152; 

class KnimbusWind
{
  public:
    bool InitializeWindSensor();
    int GetWindSpeedValue();
    int GetWindDirection();
    void handleInterrupt();
  private:
    int ConvertWindDirection(int Direction);
};
#endif
