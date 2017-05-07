/*
  KnimbusLightning.h - Library for matching and comparing control values with knimbus weather station.
  Created by Kevin D. Kearney, May 07, 2017.
  Released into the public domain.
*/

#ifndef KnimbusControlValueHelper_h
#define KnimbusControlValueHelper_h

#include <Arduino.h>
#include "KnimbusRadioContracts.h"

class KnimbusControlValueHelper
{
	public:
	  bool SetNewControlValues(WeatherControlMsg &weatherControl,WeatherControlMsg &newWeatherControlValues);
};
#endif
