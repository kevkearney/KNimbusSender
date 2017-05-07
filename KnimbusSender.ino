#include "KnimbusRadio.h"
#include "KnimbusBarometer.h"
#include "KnimbusLux.h"
#include "KnimbusLightning.h"
#include "KnimbusDHT.h"
#include "KnimbusRadioContracts.h"

#include <LowPower.h>

#define MOD1016IRQ_PIN 2  //Lightning Sensor IRQ

KnimbusRadio kRadio;
KnimbusLux kLux;
KnimbusBarometer kBaro;
KnimbusLightning kLightning;
KnimbusDHT kDHT;

WeatherControlMsg weatherControl;

volatile bool lightningDetected = false;

void alert() {
  lightningDetected = true;
}

void HandleLightning(){
  LightningMsg lightningData;  
  kLightning.TranslateIRQ(lightningData.EventType, lightningData.Distance, lightningData.Intensity);

  kRadio.XMitLightning(lightningData);  
  lightningDetected = false;
}

void SleepCycle(int sleepTime){
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

//  weatherControl.SystemReset = false;
  weatherControl.SleepTime = 10;
  //weatherControl.LightningIndoors = false;
  //weatherControl.LightningTune = 0;
  //weatherControl.LightningNoiseFloor = 4;
  //weatherControl.RadioPower = 3;   
    
  kRadio.SetupRadio(3);
  kBaro.InitializeBarometer();
  kDHT.InitializeThermometer();
  kLux.InitializeLightSensor();
  
  pinMode(MOD1016IRQ_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(MOD1016IRQ_PIN), alert, RISING);  
  kLightning.InitializeLightningSensor(MOD1016IRQ_PIN, true, 4,2);
}

void loop() {
  WeatherDataMsg weatherData;
  WeatherControlMsg weatherControlData;
  
  bool tempSuccess = kDHT.GetThermometerValue(weatherData.Temperature, weatherData.Humidity);
  bool baroSuccess = kBaro.GetBarometerValue(weatherData.BaroPressure, weatherData.BaroTemperature);
  bool lightSuccess = kLux.GetLightValue(weatherData.Lux);  
 
  Serial.println(F("Ready to Transmit"));
  bool xmitSuccess = kRadio.XMitWeather(weatherData, weatherControlData);
  if (xmitSuccess) {
    Serial.println(F("Successful Transmit"));
     // Spew it
    Serial.println(F(", Got response "));
    Serial.print(F(",SleepTime: "));
    Serial.println(weatherControlData.SleepTime);    
    Serial.print(F(",lightningIndoors: "));
    Serial.println(weatherControlData.LightningIndoors); 
    Serial.print(F(",lightningTune: "));
    Serial.println(weatherControlData.LightningTune); 
    Serial.print(F(",lightningNoiseFloor: "));
    Serial.println(weatherControlData.LightningNoiseFloor);
    Serial.print(F(",radioPower: "));
    Serial.println(weatherControlData.RadioPower);   
    Serial.print(F(",systemreset: "));
    Serial.println(weatherControlData.SystemReset);  
  }
  //SleepCycle(weatherControl.SleepTime); 
}






