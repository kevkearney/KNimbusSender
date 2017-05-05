/*
  KnimbusLightning.h - Library for using Mod1016 Lightning Sensor with knimbus weather station.
  Created by Kevin D. Kearney, May 03, 2017.
  Released into the public domain.
*/

#ifndef KnimbusLightning_h
#define KnimbusLightning_h

#include <Arduino.h>
#include <AS3935mod.h>

class KnimbusLightning
{
  public:
    void InitializeLightningSensor(int irqPin, bool indoorMode, int noiseFloor, int tuneNum);
    void TranslateIRQ(String &eventType, int &distance);
    void DisableDisturbers();
    void EnableDisturbers();    
  private:
    void printDistance();
	
};
#endif
