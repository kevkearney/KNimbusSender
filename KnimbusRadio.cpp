#include "KnimbusRadio.h"

void KnimbusRadio::SetupRadio(){
  radio.begin();
  radio.setAutoAck(1);                    // Ensure autoACK is enabled
  radio.enableAckPayload();               // Allow optional ack payloads
  radio.setPALevel(RF24_PA_HIGH);
  radio.setRetries(0,15);                 // Smallest time between retries, max no. of retries
  //radio.setPayloadSize(4);                // Here we are sending 1-byte payloads to test the call-response speed
  radio.openWritingPipe(pipes[1]);        // Both radios listen on the same pipes by default, and switch when writing
  radio.openReadingPipe(1,pipes[0]);
  radio.printDetails();
}

bool KnimbusRadio::XMitWeather(Weather_t weatherData, const String &responseMsg){
   return PowerOnRadioAndXMit(&weatherData,sizeof(weatherData), responseMsg);
}

bool KnimbusRadio::XMitLightning(String lightningType,int lightningDistance){
  String lightningMessage;
  if(lightningDistance > 0){
    lightningMessage = lightningType + lightningDistance;
  }
  else{
    lightningMessage = lightningType;
  }
  
  char charBuf[lightningMessage.length()];
  lightningMessage.toCharArray(charBuf, 26);
  
  uint8_t data[lightningMessage.length()];
  strcpy((char*)data, charBuf);
  
  Serial.println(lightningMessage);
  Serial.println(sizeof(data));
  String responseMsg;
  PowerOnRadioAndXMit(&data,sizeof(data),responseMsg);
}

bool PowerOnRadioAndXMit(void* buf,int size,const String &responseMsg){
  radio.powerUp();
  
  delay(2000);
  radio.stopListening();
  if (!radio.write( buf,size)){
    Serial.println(F("failed"));
    return false;
  }
  
  radio.startListening();                                   // Now, continue listening
    
  unsigned long started_waiting_at = micros();               // Set up a timeout period, get the current microseconds
  boolean timeout = false;                                   // Set up a variable to indicate if a response was received or not
  
  while ( ! radio.available() ){
    //Serial.println(F("Waiting for response."));// While nothing is received
    if (micros() - started_waiting_at > 200000 ){            // If waited longer than 200ms, indicate timeout and exit while loop
        timeout = true;
        break;
    }      
  }
  
  if ( timeout ){                                             // Describe the results
        Serial.println(F("Failed, response timed out."));
        return false;
    }else{
        char got_payload[32] = {0};                                 // Grab the response, compare, and send to debugging spew
        radio.read( &got_payload, sizeof(got_payload));
        
        // Spew it      
        Serial.print(F(", Got response "));
        Serial.print(got_payload); 
        responseMsg = got_payload;
    }
    radio.powerDown();
    delay(1000);
  return true;
}