
#include "KnimbusBarometer.h"

Adafruit_BME280 bmp; // I2C

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
  
}


