#include "KnimbusRadio.h"
#include "KnimbusBarometer.h"
#include "KnimbusLux.h"
#include "KnimbusLightning.h"
#include "KnimbusDHT.h"
#include "KnimbusResponseParser.h"

#include <LowPower.h>

WeatherControl weatherControl;

#define MOD1016IRQ_PIN 2  //Lightning Sensor IRQ

KnimbusRadio kRadio;
KnimbusLux kLux;
KnimbusBarometer kBaro;
KnimbusLightning kLightning;
KnimbusDHT kDHT;
KnimbusParser kParser;

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

void setup() {
  delay(3000);
  Serial.begin(9600);

  weatherControl.sleepTime = 10;
  weatherControl.lightningIndoors = false;
  weatherControl.lightningTune = 0;
  weatherControl.lightningNoiseFloor = 4;
  weatherControl.radioPower = 3;   
    
  kRadio.SetupRadio(weatherControl.radioPower);
  kBaro.InitializeBarometer();
  kDHT.InitializeThermometer();
  kLux.InitializeLightSensor();
  
  pinMode(MOD1016IRQ_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(MOD1016IRQ_PIN), alert, RISING);  
  kLightning.InitializeLightningSensor(MOD1016IRQ_PIN, weatherControl.lightningIndoors, weatherControl.lightningNoiseFloor,weatherControl.lightningTune);
}

void loop() {
  Weather_t weatherData;

  bool tempSuccess = kDHT.GetThermometerValue(weatherData.Temperature, weatherData.Humidity);
  bool baroSuccess = kBaro.GetBarometerValue(weatherData.BaroPressure, weatherData.BaroTemperature);
  bool lightSuccess = kLux.GetLightValue(weatherData.Lux);
 
  WeatherControl radioResponse;

  Serial.println(F("Ready to Transmit"));
  bool xmitSuccess = kRadio.XMitWeather(weatherData, radioResponse);
  if (xmitSuccess) {
    Serial.println(F("Successful Transmit"));
    //TODO: add the response parser
    //sleepTime = ParseSleepTimeResponse(radioResponse);
  }

  Serial.print(F("Next sleep time: "));
  Serial.println(weatherControl.sleepTime);

  short numberOfCycles = weatherControl.sleepTime / 7.5;
  for (int i = 0; i < numberOfCycles; i++) {
    if (lightningDetected) {
      HandleLightning();      
    }
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  }
}





