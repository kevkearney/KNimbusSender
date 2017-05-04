#include "KnimbusDHT.h"

DHT dht(DHTPIN, DHTTYPE);

void KnimbusDHT::InitializeThermometer() {
  Serial.println(F("DHT22 initialize!"));
  dht.begin();
}
bool KnimbusDHT::GetThermometerValue(float &temperature, float &humidity) {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  Serial.print(F("Humidity: "));
  Serial.print(humidity);
  Serial.println(F(" %\t"));
  Serial.print(F("Temperature: "));
  Serial.print(temperature);
  Serial.println(F(" *C "));
  return true;
}
