#include "KnimbusDHT.h"

DHT dht(DHTPIN, DHTTYPE);

void KnimbusDHT::InitializeThermometer() {
  Serial.println(F("DHT22 initialize!"));
  dht.begin();
}
bool KnimbusDHT::GetThermometerValue(int &temperature, int &humidity) {


  float fltHumidity = dht.readHumidity();
  float fltTemperature = dht.readTemperature();

  humidity = (int)(fltHumidity * 100);
  temperature = (int)(fltTemperature * 100);

  Serial.print(F("Humidity: "));
  Serial.print(humidity);
  Serial.println(F(" %\t"));
  Serial.print(F("Temperature: "));
  Serial.print(temperature);
  Serial.println(F(" *C "));

  Serial.print(F("Humidity: "));
  Serial.print(fltHumidity);
  Serial.println(F(" %\t"));
  Serial.print(F("Temperature: "));
  Serial.print(fltTemperature);
  Serial.println(F(" *C "));
  return true;
}


