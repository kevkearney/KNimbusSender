#include "KnimbusControlValueHelper.h"

bool KnimbusControlValueHelper::SetNewControlValues(WeatherControlMsg &weatherControl,WeatherControlMsg &newWeatherControlValues) {  
  bool controlValuesChanged = false;
  if (weatherControl.SleepTime != newWeatherControlValues.SleepTime){
    weatherControl.SleepTime = newWeatherControlValues.SleepTime;
    Serial.print(F("Sleeptime changed to: "));
    Serial.println(weatherControl.SleepTime);
  }
  if(weatherControl.LightningIndoors != newWeatherControlValues.LightningIndoors){
    weatherControl.LightningIndoors = newWeatherControlValues.LightningIndoors;
    Serial.print(F("LightningIndoors changed to: "));
    Serial.println(weatherControl.LightningIndoors);
    controlValuesChanged = true;
  }
   if(weatherControl.LightningNoiseFloor != newWeatherControlValues.LightningNoiseFloor){
    weatherControl.LightningNoiseFloor = newWeatherControlValues.LightningNoiseFloor;
    Serial.print(F("LightningNoiseFloor changed to: "));
    Serial.println(weatherControl.LightningNoiseFloor);
    controlValuesChanged = true;
  }
  if(weatherControl.LightningTune != newWeatherControlValues.LightningTune){
    weatherControl.LightningTune = newWeatherControlValues.LightningTune;
    Serial.print(F("LightningTune changed to: "));
    Serial.println(weatherControl.LightningTune);
    controlValuesChanged = true;
  }
  if(weatherControl.RadioPower != newWeatherControlValues.RadioPower){
    weatherControl.RadioPower = newWeatherControlValues.RadioPower;
    Serial.print(F("RadioPower changed to: "));
    Serial.println(weatherControl.RadioPower);
    controlValuesChanged = true;
  }  
   if(weatherControl.SystemReset != newWeatherControlValues.SystemReset){
    weatherControl.SystemReset = newWeatherControlValues.SystemReset;
    Serial.print(F("SystemReset changed to: "));
    Serial.println(weatherControl.SystemReset);    
  }  
  return controlValuesChanged;
}
