#include "KnimbusUV.h"

Adafruit_SI1145 uv = Adafruit_SI1145();

bool KnimbusUV::InitializeUVSensor() {
   Serial.println("Adafruit SI1145 test");
  
  if (! uv.begin()) {
    Serial.println("Didn't find Si1145");
    while (1);
  }

  Serial.println("OK!");
  return true;
}

bool KnimbusUV::GetUVValue(unsigned int &uvValue) {
	Serial.println("===================");
  Serial.print(F("Vis: ")); Serial.println(uv.readVisible());
  Serial.print(F("IR: ")); Serial.println(uv.readIR());
  
  uvValue = uv.readUV();
  // the index is multiplied by 100 so to get the
  // integer index, divide by 100! 
  
  Serial.print(F("UV: "));  Serial.println(uvValue);
 
}
