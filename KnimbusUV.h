/*
  KnimbusUV.h - Library for using SI1145 UV Sensor with knimbus weather station.
  Created by Kevin D. Kearney, June 18, 2017.
  Released into the public domain.
*/

#ifndef KnimbusUV_h
#define KnimbusUV_h

#include <Adafruit_SI1145.h>

class KnimbusUV
{
  public:
    bool InitializeUVSensor();
    bool GetUVValue(unsigned int &uvValue); 
};
#endif
