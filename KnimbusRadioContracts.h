/*
  KnimbusRadioContracts.h - Library for using parsing response values for the knimbus weather station.
  Created by Kevin D. Kearney, May 04, 2017.
  Released into the public domain.
*/

#ifndef KnimbusRadioContracts_h
#define KnimbusRadioContracts_h

#include <Arduino.h>

//Weather Control Struct
typedef struct {
    int SleepTime;
    bool LightningIndoors;
    int LightningTune;
    int LightningNoiseFloor;
    int RadioPower;
    bool SystemReset;    
} WeatherControlMsg;

//Weather Message Struct
typedef struct {
  byte MessageType = 1;
  int Temperature;
  int Humidity;
  int BaroPressure;
  int BaroTemperature;
  int BaroHumidity;
  unsigned int Lux;
}
WeatherDataMsg;

//Lightning Message Struct
typedef struct {
  byte MessageType = 2;
  int EventType;
  int Distance;
  int Intensity;  
}
LightningMsg;
#endif
