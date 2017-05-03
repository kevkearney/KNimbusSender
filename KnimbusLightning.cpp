#include "KnimbusLightning.h"


void KnimbusLightning::InitializeLightningSensor(int irqPin) {
  while (!Serial) {}
  Serial.println("Welcome to the MOD-1016!");
  //I2C
  //Wire.begin();
  mod1016.init(irqPin);
  autoTuneCaps(irqPin);
  //mod1016.setTuneCaps(2);
  mod1016.setOutdoors();
  mod1016.setNoiseFloor(4);

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

void KnimbusLightning::translateIRQ(uns8 irq) {
  switch (irq) {
    case 1:
      Serial.println("NOISE DETECTED");
      kRadio.XMitLightning("LNoise", 0);
      break;
    case 4:
      Serial.println("DISTURBER DETECTED");
      kRadio.XMitLightning("LDisturber", 0);
      break;
    case 8:
      Serial.println("LIGHTNING DETECTED");
      int lightningDistance = mod1016.calculateDistance();
      kRadio.XMitLightning("Lightning:", lightningDistance);
      printDistance();
      break;
  }
}
