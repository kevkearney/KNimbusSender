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
    
    /* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
    RF24 radio(7,8);
    const uint64_t pipes[2] = { 0xF0F0F0F0E1, 0xF0F0F0F0D2 };
};
#endif