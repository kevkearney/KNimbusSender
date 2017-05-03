/*
  KnimbusLightning.h - Library for using Mod1016 Lightning Sensor with knimbus weather station.
  Created by Kevin D. Kearney, May 03, 2017.
  Released into the public domain.
*/

#ifndef KnimbusLightning_h
#define KnimbusLightning_h

#include <Arduino.h>
#include <AS3935mod.h>
#include "KnimbusRadio.h"

class KnimbusLightning
{
  public:
    void InitializeLightningSensor(int irqPin);
    void translateIRQ(uns8 irq);
  private:
    void printDistance();
	
};
#endif
