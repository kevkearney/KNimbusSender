/*
  KnimbusRadio.h - Library for using RF24 radio with knimbus weather station.
  Created by Kevin D. Kearney, April 30, 2017.
  Released into the public domain.
*/

#ifndef KnimbusRadio_h
#define KnimbusRadio_h

#include <RF24.h>
#include <Arduino.h>
#include "KnimbusRadioContracts.h"



class KnimbusRadio
{
  public:
    void SetupRadio(int powerlevel);
    bool XMitWeather(WeatherDataMsg weatherData, WeatherControlMsg& responseMsg);
    void XMitLightning(LightningMsg lightningData);
  private:
    bool PowerOnRadioAndXMit(void* buf, int payloadSize, WeatherControlMsg& responseMsg);
    void SetPowerLevel(int pwr);
};
#endif




