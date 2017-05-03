


#include <DHT.h>
#include "KnimbusRadio.h"
#include "KnimbusBarometer.h"
#include "KnimbusLux.h"
#include "KnimbusLightning.h"

#include <LowPower.h>

#define DHTPIN            4        // Pin which is connected to the DHT sensor.
#define DHTTYPE           DHT22     // DHT 22 (AM2302)


const short stationAltitude = 342;
short sleepTime = 10;

#define MOD1016IRQ_PIN 2  //Lightning Sensor IRQ

DHT dht(DHTPIN, DHTTYPE);
KnimbusRadio kRadio;
KnimbusLux kLux;
KnimbusBarometer kBaro(stationAltitude);
KnimbusLightning kLightning;

volatile bool lightningDetected = false;

void alert() {
  lightningDetected = true;
}


int ParseSleepTimeResponse(String response) {
  int sleepTimeBeginIndex = response.indexOf(":") + 1;
  int sleepTimeEndIndex = response.indexOf("}");
  return response.substring(sleepTimeBeginIndex, sleepTimeEndIndex).toInt();;
}



void InitializeThermometer() {
  Serial.println("DHT22 initialize!");
  dht.begin();
}


bool GetThermometerValue(float &temperature, float &humidity) {
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


void setup() {
  delay(3000);
  Serial.begin(9600);
  kRadio.SetupRadio();
  kBaro.InitializeBarometer();
  InitializeThermometer();

  pinMode(MOD1016IRQ_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(MOD1016IRQ_PIN), alert, RISING);
  
  kLux.InitializeLightSensor();
  kLightning.InitializeLightningSensor(MOD1016IRQ_PIN);
}

void loop() {
  Weather_t weatherData;

  float fltHumidity;
  bool tempSuccess = GetThermometerValue(weatherData.Temperature, fltHumidity);
  bool baroSuccess = kBaro.GetBarometerValue(weatherData.BaroPressure, weatherData.BaroTemperature);
  bool lightSuccess = kLux.GetLightValue(weatherData.Lux);

  weatherData.Humidity = (int)fltHumidity;

  String radioResponse;

  Serial.println("Ready to Transmit");
  bool xmitSuccess = kRadio.XMitWeather(weatherData, radioResponse);
  if (xmitSuccess) {
    Serial.println("Successful Transmit");
    sleepTime = ParseSleepTimeResponse(radioResponse);
  }

  Serial.print("Next sleep time: ");
  Serial.println(sleepTime);

  short numberOfCycles = sleepTime / 7.5;
  for (int i = 0; i < numberOfCycles; i++) {
    if (lightningDetected) {
      kLightning.translateIRQ(mod1016.getIRQ());
      lightningDetected = false;
    }
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  }
}





