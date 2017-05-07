/*
  KnimbusResponseParser.h - Library for using parsing response values for the knimbus weather station.
  Created by Kevin D. Kearney, May 04, 2017.
  Released into the public domain.
*/

#ifndef KnimbusResponseParser_h
#define KnimbusResponseParser_h

#include <Arduino.h>

typedef struct {
    int sleepTime;
    bool lightningIndoors;
    int lightningTune;
    int lightningNoiseFloor;
    int radioPower;    
} WeatherControl;

class KnimbusParser
{
  public:    
    WeatherControl ParseRadioResponse(String response);
};
#endif
