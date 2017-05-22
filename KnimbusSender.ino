#include <avr/wdt.h>
#include "KnimbusRadio.h"
#include "KnimbusBarometer.h"
#include "KnimbusLux.h"
#include "KnimbusLightning.h"
#include "KnimbusDHT.h"
#include "KnimbusRadioContracts.h"
#include "KnimbusControlValueHelper.h"
#include "KnimbusWind.h"
#include <PinChangeInt.h>

#include <LowPower.h>

#define MOD1016IRQ_PIN 2  //Lightning Sensor IRQ

KnimbusRadio kRadio;
KnimbusLux kLux;
KnimbusBarometer kBaro;
KnimbusLightning kLightning;
KnimbusDHT kDHT;
KnimbusControlValueHelper kValueHelper;
KnimbusWind kWind;

WeatherControlMsg WeatherControl;
bool ControlValuesChanged = false;

int RadioFailureCount = 0;
int RadioFailureLimit = 3;

volatile bool lightningDetected = false;

void alert() {
  lightningDetected = true;
}

void HandleLightning() {
  Serial.println("Lightning Detected");
  LightningMsg lightningData;
  kLightning.TranslateIRQ(lightningData.EventType, lightningData.Distance, lightningData.Intensity);
    
  lightningDetected = false;
  kLightning.DisableDisturbers();  
  delay(1000);
  if(lightningData.EventType != -1)kRadio.XMitLightning(lightningData);
  kLightning.EnableDisturbers();
}

void HandleWind() {
 kWind.handleInterrupt();
  
}

void SleepCycle(int sleepTime) {
  short numberOfCycles = sleepTime / 7.5;
  for (int i = 0; i < numberOfCycles; i++) {
    if (lightningDetected) {
      delay(3000);
      HandleLightning();
    }
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  }
}

void setup() {    
  Serial.begin(9600);
  Serial.println(F("*********FULL RESET*********"));
  delay(3000);
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
  kWind.InitializeWindSensor();

  pinMode(MOD1016IRQ_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(MOD1016IRQ_PIN), alert, RISING);
  kLightning.InitializeLightningSensor(MOD1016IRQ_PIN, WeatherControl.LightningIndoors, WeatherControl.LightningNoiseFloor, WeatherControl.LightningTune);
  kLightning.getIrq(); 
}

void loop() {
  //wdt_enable(WDTO_8S);
 
  //Hack to cause watchdog reset
  if(WeatherControl.SystemReset)  {
     Serial.println(F("Resetting system."));
     while(1);
  }
    
  
  if(ControlValuesChanged){
    wdt_reset();
    kLightning.InitializeLightningSensor(MOD1016IRQ_PIN, WeatherControl.LightningIndoors, WeatherControl.LightningNoiseFloor, WeatherControl.LightningTune); 
    kRadio.SetupRadio(WeatherControl.RadioPower);
    ControlValuesChanged = false;
  }
  
  WeatherDataMsg weatherData;
  WeatherControlMsg newWeatherControlData;

  bool tempSuccess = kDHT.GetThermometerValue(weatherData.Temperature, weatherData.Humidity);
  bool baroSuccess = kBaro.GetBarometerValue(weatherData.BaroPressure, weatherData.BaroTemperature, weatherData.BaroHumidity);
  bool lightSuccess = kLux.GetLightValue(weatherData.Lux);
  attachPinChangeInterrupt(AnemometerPin, HandleWind, FALLING);        //Enables interrupts
  weatherData.WindSpeed = kWind.GetWindSpeedValue();
  detachPinChangeInterrupt(AnemometerPin);         //Disable interrupts
  weatherData.WindDirection = kWind.GetWindDirection();

  Serial.println(F("Ready to Transmit"));
  kLightning.DisableDisturbers();  
  bool xmitSuccess = kRadio.XMitWeather(weatherData, newWeatherControlData);
  
  if (xmitSuccess) {  
    RadioFailureCount = 0; 
    ControlValuesChanged = kValueHelper.SetNewControlValues(WeatherControl,newWeatherControlData);
  }  else
  {
    RadioFailureCount++;
    if(RadioFailureCount >= RadioFailureLimit){
      kRadio.SetupRadio(3);
    }
  }
  kLightning.getIrq(); 
  kLightning.EnableDisturbers();
  wdt_reset();
  SleepCycle(WeatherControl.SleepTime);
}





