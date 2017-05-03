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
    //altitude is in meters
    KnimbusBarometer(int stationAltitude);
    void InitializeBarometer();
    bool GetBarometerValue(float &barometerValue, float &temperature);
  private:
    float seaLevelForAltitude(float altitude, float atmospheric);
    int _stationAltitude;
};
#endif

