#include "KnimbusLightning.h"

void KnimbusLightning::InitializeLightningSensor(int irqPin, bool indoorMode, int noiseFloor, int tuneNum) {
  while (!Serial) {}
  Serial.println(F("Welcome to the MOD-1016!"));
  //I2C
  mod1016.init(irqPin);
  
  if (tuneNum > 0 & tuneNum <= 10)
    mod1016.setTuneCaps(tuneNum);
  else
    autoTuneCaps(irqPin);
    
  if(indoorMode) mod1016.setIndoors();
  else mod1016.setOutdoors();
  mod1016.setNoiseFloor(noiseFloor);

  Serial.println(F("TUNE\tIN/OUT\tNOISEFLOOR"));
  Serial.print(mod1016.getTuneCaps(), HEX);
  Serial.print(F("\t"));
  Serial.print(mod1016.getAFE(), BIN);
  Serial.print(F("\t"));
  Serial.println(mod1016.getNoiseFloor(), HEX);
  Serial.print(F("\n"));
 
  mod1016.getIRQ();
  Serial.println(F("after interrupt"));
}

void KnimbusLightning::printDistance() {
  int distance = mod1016.calculateDistance();
  if (distance == -1)
    Serial.println(F("Lightning out of range"));
  else if (distance == 1)
    Serial.println(F("Distance not in table"));
  else if (distance == 0)
    Serial.println(F("Lightning overhead"));
  else {
    Serial.print(F("Lightning ~"));
    Serial.print(distance);
    Serial.println(F("km away\n"));
  }
}
void KnimbusLightning::DisableDisturbers(){
  mod1016.disableDisturbers();
}

void KnimbusLightning::EnableDisturbers(){
  mod1016.enableDisturbers();
}

void KnimbusLightning::TranslateIRQ(String &eventType, int &distance, int &intensity) {
  uns8 irq = mod1016.getIRQ();
  intensity = -1;
  distance = -1;
  switch (irq) {
    case 1:
      eventType = "Noise";            
      Serial.println(F("NOISE DETECTED"));
      break;
    case 4:
      eventType = "Disturber";
      Serial.println(F("DISTURBER DETECTED"));
      break;
    case 8:
      eventType = "Lightning";
      Serial.println(F("LIGHTNING DETECTED"));
      distance = mod1016.calculateDistance();
      intensity = mod1016.getIntensity();
      printDistance();
      break;
  }
}
