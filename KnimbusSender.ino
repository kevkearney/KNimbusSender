#include "KnimbusRadio.h"
#include "KnimbusBarometer.h"
#include "KnimbusLux.h"
#include "KnimbusLightning.h"
#include "KnimbusDHT.h"

#include <LowPower.h>
#include <EEPROM.h>

const short stationAltitude = 342;
short sleepTime = 10;

#define MOD1016IRQ_PIN 2  //Lightning Sensor IRQ

KnimbusRadio kRadio;
KnimbusLux kLux;
KnimbusBarometer kBaro(stationAltitude);
KnimbusLightning kLightning;
KnimbusDHT kDHT;

volatile bool lightningDetected = false;

void alert() {
  lightningDetected = true;
}

void HandleLightning(){
  String eventType;
  int distance = -1;
  kLightning.TranslateIRQ(eventType, distance);
  kRadio.XMitLightning(eventType, distance);  
  lightningDetected = false;
}

int ParseSleepTimeResponse(String response) {
  int sleepTimeBeginIndex = response.indexOf(":") + 1;
  int sleepTimeEndIndex = response.indexOf("}");
  return response.substring(sleepTimeBeginIndex, sleepTimeEndIndex).toInt();;
}

void setup() {
  delay(3000);
  Serial.begin(9600);
  kRadio.SetupRadio();
  kBaro.InitializeBarometer();
  kDHT.InitializeThermometer();
  kLux.InitializeLightSensor();
  
  pinMode(MOD1016IRQ_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(MOD1016IRQ_PIN), alert, RISING);  
  kLightning.InitializeLightningSensor(MOD1016IRQ_PIN, false, 4);
}

void loop() {
  Weather_t weatherData;

  bool tempSuccess = kDHT.GetThermometerValue(weatherData.Temperature, weatherData.Humidity);
  bool baroSuccess = kBaro.GetBarometerValue(weatherData.BaroPressure, weatherData.BaroTemperature);
  bool lightSuccess = kLux.GetLightValue(weatherData.Lux);
 
  String radioResponse;

  Serial.println(F("Ready to Transmit"));
  bool xmitSuccess = kRadio.XMitWeather(weatherData, radioResponse);
  if (xmitSuccess) {
    Serial.println(F("Successful Transmit"));
    sleepTime = ParseSleepTimeResponse(radioResponse);
  }

  Serial.print(F("Next sleep time: "));
  Serial.println(sleepTime);

  short numberOfCycles = sleepTime / 7.5;
  for (int i = 0; i < numberOfCycles; i++) {
    if (lightningDetected) {
      HandleLightning();      
    }
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  }
}





