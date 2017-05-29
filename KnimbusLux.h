/*
  KnimbusLux.h - Library for using TS2561 Lux Sensor with knimbus weather station.
  Created by Kevin D. Kearney, May 03, 2017.
  Released into the public domain.
*/

#ifndef KnimbusLux_h
#define KnimbusLux_h

#include <Adafruit_TSL2561_U.h>

class KnimbusLux
{
  public:
    bool InitializeLightSensor();
    bool GetLightValue(unsigned int &lux);
  private:
    void displaySensorDetails();
    void configureSensor();
};
#endif

