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
#define RainIRQ_PIN 3

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

bool RadioEnabled = true;
bool LightningSensorEnabled = true;

unsigned int numberOfCycles = 0;

volatile bool lightningDetected = false;
volatile int rainClickCount = 0;
int totalRainClicks = 0;

void lightningAlert() {
  lightningDetected = true;
}

void rainAlert() {
  rainClickCount++;
}

void HandleLightning() {
  Serial.println("Lightning Detected");
  LightningMsg lightningData;
  kLightning.TranslateIRQ(lightningData.EventType, lightningData.Distance, lightningData.Intensity);

  lightningDetected = false; 
  if (lightningData.EventType != -1 && RadioEnabled)kRadio.XMitLightning(lightningData);  
}

void HandleWind() {
  kWind.handleInterrupt();
}

void SleepCycle(int sleepTime) {
  short numberOfCycles = sleepTime / 7.5;
  for (int i = 0; i < numberOfCycles; i++) {
    if (lightningDetected) {
      HandleLightning();
    }else{
       LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    }   
  }
}

void setup() {
  wdt_enable(WDTO_8S);
  Serial.begin(9600);
  Serial.println(F("*********FULL RESET*********"));
  delay(3000);
 
  //Set initial control values
  WeatherControl.SystemReset = false;
  WeatherControl.SleepTime = 10;
  WeatherControl.LightningIndoors = false;
  WeatherControl.LightningTune = 2;
  WeatherControl.LightningNoiseFloor = 4;
  WeatherControl.RadioPower = 3;
  WeatherControl.EnableDisturbers = 1;

  if (RadioEnabled)kRadio.SetupRadio(WeatherControl.RadioPower);
  kBaro.InitializeBarometer();
  kDHT.InitializeThermometer();
  kLux.InitializeLightSensor();
  kWind.InitializeWindSensor();

  pinMode(RainIRQ_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RainIRQ_PIN), rainAlert, FALLING);

  if (LightningSensorEnabled) {
    pinMode(MOD1016IRQ_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(MOD1016IRQ_PIN), lightningAlert, RISING);
    kLightning.InitializeLightningSensor(MOD1016IRQ_PIN, WeatherControl.LightningIndoors, WeatherControl.LightningNoiseFloor, WeatherControl.LightningTune);
    kLightning.getIrq();
  }
  wdt_reset();
}

void loop() {
  wdt_enable(WDTO_8S);
  
  //Hack to cause watchdog reset
  if (WeatherControl.SystemReset)  {
    Serial.println(F("Resetting system."));
    while (1);
  }

  WeatherDataMsg weatherData;
  WeatherControlMsg newWeatherControlData;
  weatherData.Cycles = numberOfCycles;
  bool tempSuccess = kDHT.GetThermometerValue(weatherData.Temperature, weatherData.Humidity);
  bool baroSuccess = kBaro.GetBarometerValue(weatherData.BaroPressure, weatherData.BaroTemperature, weatherData.BaroHumidity);
  bool lightSuccess = kLux.GetLightValue(weatherData.Lux);
  
  weatherData.RainClicks = rainClickCount;
  totalRainClicks = totalRainClicks + rainClickCount;  
  weatherData.TotalRainClicks = totalRainClicks;
  
  attachPinChangeInterrupt(AnemometerPin, HandleWind, FALLING);        //Enables interrupts
  weatherData.WindSpeed = kWind.GetWindSpeedValue();
  detachPinChangeInterrupt(AnemometerPin);         //Disable interrupts
  weatherData.WindDirection = kWind.GetWindDirection();

  bool xmitSuccess;
  if (RadioEnabled) {
    Serial.println(F("Ready to Transmit"));    
    xmitSuccess = kRadio.XMitWeather(weatherData, newWeatherControlData); 
  }

  if (xmitSuccess) {    
    rainClickCount = rainClickCount - weatherData.RainClicks;  
    RadioFailureCount = 0;
    ControlValuesChanged = kValueHelper.SetNewControlValues(WeatherControl, newWeatherControlData);
  }  else
  {
    RadioFailureCount++;
    if (RadioFailureCount == RadioFailureLimit) {
      kRadio.SetupRadio(3);
    }
  }
  if (ControlValuesChanged) {
    wdt_reset();
    if (LightningSensorEnabled) {
      kLightning.InitializeLightningSensor(MOD1016IRQ_PIN, WeatherControl.LightningIndoors, WeatherControl.LightningNoiseFloor, WeatherControl.LightningTune);      
      if (WeatherControl.EnableDisturbers) kLightning.EnableDisturbers();
      else kLightning.DisableDisturbers();
      kLightning.getIrq();
    }
    if (RadioEnabled)kRadio.SetupRadio(WeatherControl.RadioPower);
    ControlValuesChanged = false;
  }
  wdt_reset();
  numberOfCycles++;
  SleepCycle(WeatherControl.SleepTime);
}





