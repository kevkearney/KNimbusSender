#include "KnimbusLightning.h"

void KnimbusLightning::InitializeLightningSensor(int irqPin, bool indoorMode, int noiseFloor) {
  while (!Serial) {}
  Serial.println("Welcome to the MOD-1016!");
  //I2C
  mod1016.init(irqPin);
  autoTuneCaps(irqPin);
  //mod1016.setTuneCaps(2);
  if(indoorMode) mod1016.setIndoors();
  else mod1016.setOutdoors();
  
 
  mod1016.setNoiseFloor(noiseFloor);

  Serial.println("TUNE\tIN/OUT\tNOISEFLOOR");
  Serial.print(mod1016.getTuneCaps(), HEX);
  Serial.print("\t");
  Serial.print(mod1016.getAFE(), BIN);
  Serial.print("\t");
  Serial.println(mod1016.getNoiseFloor(), HEX);
  Serial.print("\n");
 
  mod1016.getIRQ();
  Serial.println("after interrupt");
}



void KnimbusLightning::printDistance() {
  int distance = mod1016.calculateDistance();
  if (distance == -1)
    Serial.println("Lightning out of range");
  else if (distance == 1)
    Serial.println("Distance not in table");
  else if (distance == 0)
    Serial.println("Lightning overhead");
  else {
    Serial.print("Lightning ~");
    Serial.print(distance);
    Serial.println("km away\n");
  }
}

void KnimbusLightning::translateIRQ(String &eventType, int &distance) {
  uns8 irq = mod1016.getIRQ();
  switch (irq) {
    case 1:
      eventType = "Noise";
      Serial.println("NOISE DETECTED");
      break;
    case 4:
      eventType = "Disturber";
      Serial.println("DISTURBER DETECTED");
      break;
    case 8:
      eventType = "Lightning";
      Serial.println("LIGHTNING DETECTED");
      distance = mod1016.calculateDistance();
      printDistance();
      break;
  }
}
