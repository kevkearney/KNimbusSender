
#include "KnimbusBarometer.h"

Adafruit_BME280 bme; // I2C

void KnimbusBarometer::InitializeBarometer() {
  Serial.println(F("Pressure Sensor Test"));
  Serial.println("");
  if (!bme.begin())
  {
    /* There was a problem detecting the bme280 ... check your connections */
    Serial.print(F("Ooops, no bme280 detected"));
    while (1);
  }
   Serial.println(F("-- Weather Station Scenario --"));
   Serial.println(F("forced mode, 1x temperature / 1x humidity / 1x pressure oversampling,"));
   Serial.println(F("filter off"));
   bme.setSampling(Adafruit_BME280::MODE_FORCED,
                    Adafruit_BME280::SAMPLING_X1, // temperature
                    Adafruit_BME280::SAMPLING_X1, // pressure
                    Adafruit_BME280::SAMPLING_X1, // humidity
                    Adafruit_BME280::FILTER_OFF   );
  delay(500);
}

bool KnimbusBarometer::GetBarometerValue(int &barometerValue, int &temperature, int &humidity) {
  bme.takeForcedMeasurement(); 
  
  float fltTemperature = bme.readTemperature();
  float fltBarometerValue = bme.readPressure() / 10;
  float fltHumidityValue = bme.readHumidity();

  barometerValue = (int)fltBarometerValue;
  temperature = (int)(fltTemperature * 100); 
  humidity = (int) (fltHumidityValue * 100);
  
  Serial.print(F("Baro Temperature = "));
  Serial.print(temperature);
  Serial.println(F(" *C"));
  Serial.print(F("Raw Pressure: "));
  Serial.println(barometerValue);  
   Serial.print(F("Humidity = "));
    Serial.print(bme.readHumidity());
    Serial.println(F(" %"));
  
}


