#include "KnimbusBarometer.h"

Adafruit_BME280 bmp; // I2C
int _stationaltitude;

KnimbusBarometer::KnimbusBarometer(int stationAltitude){
  _stationaltitude = stationAltitude;
}

void KnimbusBarometer::InitializeBarometer() {
  Serial.println("Pressure Sensor Test");
  Serial.println("");
  if (!bmp.begin())
  {
    /* There was a problem detecting the BMP280 ... check your connections */
    Serial.print("Ooops, no BMP280 detected ... Check your wiring or I2C ADDR!");
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

bool KnimbusBarometer::GetBarometerValue(float &barometerValue, float &temperature) {

  temperature = bmp.readTemperature();
  barometerValue = bmp.readPressure() / 100;

  Serial.print(F("Baro Temperature = "));
  Serial.print(bmp.readTemperature());
  Serial.println(" *C");

  float SeaLevelbarometerValue = seaLevelForAltitude(_stationaltitude, barometerValue );

  Serial.print("Raw Pressure: ");
  Serial.println(barometerValue);
  Serial.print("Sea Level Pressure: ");
  Serial.println(SeaLevelbarometerValue);
}


