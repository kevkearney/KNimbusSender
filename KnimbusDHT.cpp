#include "KnimbusDHT.h"

DHT dht(DHTPIN, DHTTYPE);

void KnimbusDHT::InitializeThermometer() {
  Serial.println("DHT22 initialize!");
  dht.begin();
}
bool KnimbusDHT::GetThermometerValue(float &temperature, float &humidity) {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C ");
  return true;
}