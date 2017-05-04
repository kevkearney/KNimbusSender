/*
  KnimbusDHT.h - Library for using DHT22 TempHumid Sensor with knimbus weather station.
  Created by Kevin D. Kearney, May 03, 2017.
  Released into the public domain.
*/

#ifndef KnimbusDHT_h
#define KnimbusDHT_h

#include <DHT.h>

#define DHTPIN            4        // Pin which is connected to the DHT sensor.
#define DHTTYPE           DHT22     // DHT 22 (AM2302)


class KnimbusDHT
{
  public:
    void InitializeThermometer();
    bool GetThermometerValue(float &temperature, float &humidity);    
  private:
    
};
#endif