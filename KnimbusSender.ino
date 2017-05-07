#include "KnimbusRadio.h"
#include "KnimbusBarometer.h"
#include "KnimbusLux.h"
#include "KnimbusLightning.h"
#include "KnimbusDHT.h"
#include "KnimbusRadioContracts.h"
#include "KnimbusControlValueHelper.h"

#include <LowPower.h>

#define MOD1016IRQ_PIN 2  //Lightning Sensor IRQ

KnimbusRadio kRadio;
KnimbusLux kLux;
KnimbusBarometer kBaro;
KnimbusLightning kLightning;
KnimbusDHT kDHT;
KnimbusControlValueHelper kValueHelper;

WeatherControlMsg WeatherControl;
bool ControlValuesChanged = false;

volatile bool lightningDetected = false;

void alert() {
  lightningDetected = true;
}

void HandleLightning() {
  LightningMsg lightningData;
  kLightning.TranslateIRQ(lightningData.EventType, lightningData.Distance, lightningData.Intensity);

  kRadio.XMitLightning(lightningData);
  lightningDetected = false;
}

void SleepCycle(int sleepTime) {
  short numberOfCycles = sleepTime / 7.5;
  for (int i = 0; i < numberOfCycles; i++) {
    if (lightningDetected) {
      HandleLightning();
    }
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  }
}

void setup() {
  delay(3000);
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);

  //Set initial control values
  WeatherControl.SystemReset = false;
  WeatherControl.SleepTime = 10;
  WeatherControl.LightningIndoors = false;
  WeatherControl.LightningTune = 2;
  WeatherControl.LightningNoiseFloor = 4;
  WeatherControl.RadioPower = 3;

  kRadio.SetupRadio(WeatherControl.RadioPower);
  kBaro.InitializeBarometer();
  kDHT.InitializeThermometer();
  kLux.InitializeLightSensor();

  pinMode(MOD1016IRQ_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(MOD1016IRQ_PIN), alert, RISING);
  kLightning.InitializeLightningSensor(MOD1016IRQ_PIN, WeatherControl.LightningIndoors, WeatherControl.LightningNoiseFloor, WeatherControl.LightningTune);
}

void loop() {
  if(ControlValuesChanged){
    kLightning.InitializeLightningSensor(MOD1016IRQ_PIN, WeatherControl.LightningIndoors, WeatherControl.LightningNoiseFloor, WeatherControl.LightningTune); 
    kRadio.SetupRadio(WeatherControl.RadioPower);
  }
  
  WeatherDataMsg weatherData;
  WeatherControlMsg newWeatherControlData;

  bool tempSuccess = kDHT.GetThermometerValue(weatherData.Temperature, weatherData.Humidity);
  bool baroSuccess = kBaro.GetBarometerValue(weatherData.BaroPressure, weatherData.BaroTemperature);
  bool lightSuccess = kLux.GetLightValue(weatherData.Lux);

  Serial.println(F("Ready to Transmit"));
  bool xmitSuccess = kRadio.XMitWeather(weatherData, newWeatherControlData);
  if (xmitSuccess) {   
    ControlValuesChanged = kValueHelper.SetNewControlValues(WeatherControl,newWeatherControlData);
  }
  SleepCycle(WeatherControl.SleepTime);
}





