
#include "KnimbusBarometer.h"

Adafruit_BME280 bmp; // I2C
int _stationaltitude;

KnimbusBarometer::KnimbusBarometer(int stationAltitude){
  _stationaltitude = stationAltitude;
}

void KnimbusBarometer::InitializeBarometer() {
  Serial.println(F("Pressure Sensor Test"));
  Serial.println("");
  if (!bmp.begin())
  {
    /* There was a problem detecting the BMP280 ... check your connections */
    Serial.print(F("Ooops, no BMP280 detected"));
    while (1);
  }
  delay(500);
}

float KnimbusBarometer::seaLevelForAltitude(float altitude, float atmospheric)
{
  // Equation taken from BMP180 datasheet (page 17):
  //  http://www.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf

  // Note that using the equation from wikipedia can give bad results
  // at high altitude.  See this thread for more information:
  //  http://forums.adafruit.com/viewtopic.php?f=22&t=58064

  return atmospheric / pow(1.0 - (altitude / 44330.0), 5.255);
}

bool KnimbusBarometer::GetBarometerValue(int &barometerValue, int &temperature) {

  float fltTemperature = bmp.readTemperature();
  float fltBarometerValue = bmp.readPressure() / 10;

  barometerValue = (int)fltBarometerValue;
  temperature = (int)(fltTemperature * 100);
  
  
  Serial.print(F("Baro Temperature = "));
  Serial.print(temperature);
  Serial.println(F(" *C"));
  Serial.print(F("Raw Pressure: "));
  Serial.println(barometerValue);

    Serial.print(F("Baro Temperature = "));
  Serial.print(fltTemperature);
  Serial.println(F(" *C"));
  Serial.print(F("Raw Pressure: "));
  Serial.println(fltBarometerValue);
  
   float SeaLevelbarometerValue = seaLevelForAltitude(_stationaltitude, fltBarometerValue );
  //Serial.print(F("Sea Level Pressure: "));
  //Serial.println(SeaLevelbarometerValue);
}


