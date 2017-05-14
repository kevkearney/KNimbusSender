/*
  KnimbusBarometer.h - Library for using BME280 Barometer with knimbus weather station.
  Created by Kevin D. Kearney, May 03, 2017.
  Released into the public domain.
*/

#ifndef KnimbusBarometer_h
#define KnimbusBarometer_h

#include <Adafruit_BME280.h>
class KnimbusBarometer
{
  public:
    void InitializeBarometer();
    bool GetBarometerValue(int &barometerValue, int &temperature, int &humidity);
  private:
    float seaLevelForAltitude(float altitude, float atmospheric);
    int _stationAltitude;
};
#endif

