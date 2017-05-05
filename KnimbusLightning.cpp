#include "KnimbusLightning.h"

void KnimbusLightning::InitializeLightningSensor(int irqPin, bool indoorMode, int noiseFloor) {
  while (!Serial) {}
  Serial.println(F("Welcome to the MOD-1016!"));
  //I2C
  mod1016.init(irqPin);
  autoTuneCaps(irqPin);
  //mod1016.setTuneCaps(2);
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

void KnimbusLightning::TranslateIRQ(String &eventType, int &distance) {
  uns8 irq = mod1016.getIRQ();
  switch (irq) {
    case 1:
      eventType = "LNoise";
      Serial.println(F("NOISE DETECTED"));
      break;
    case 4:
      eventType = "LDisturber";
      Serial.println(F("DISTURBER DETECTED"));
      break;
    case 8:
      eventType = "LLightning";
      Serial.println(F("LIGHTNING DETECTED"));
      distance = mod1016.calculateDistance();
      printDistance();
      break;
  }
}
