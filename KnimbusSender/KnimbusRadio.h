/*
  KnimbusRadio.h - Library for using RF24 radio with knimbus weather station.
  Created by Kevin D. Kearney, April 30, 2017.
  Released into the public domain.
*/

#ifndef KnimbusRadio_h
#define KnimbusRadio_h

#include <RF24.h>
#include <Arduino.h>

//Weather Struct
typedef struct{
    float Temperature;
    unsigned int Humidity;
    float BaroPressure;
    float BaroTemperature;
    float Lux;
}
Weather_t;

class KnimbusRadio
{
  public: 
    void SetupRadio();
    bool XMitWeather(Weather_t weatherData, const String &responseMsg);
    bool XMitLightning(String lightningType,int lightningDistance);
  private:
    bool PowerOnRadioAndXMit(void* buf,int size,const String &responseMsg);    
};
#endif
