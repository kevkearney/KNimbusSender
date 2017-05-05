#include "KnimbusRadio.h"
/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(7, 8);
const uint64_t pipes[2] = { 0xF0F0F0F0E1, 0xF0F0F0F0D2 };

void KnimbusRadio::SetupRadio(int powerLevel) {
  
  radio.begin();
  radio.setAutoAck(1);                    // Ensure autoACK is enabled
  radio.enableAckPayload();               // Allow optional ack payloads
  SetPowerLevel(powerLevel);
  radio.setRetries(0, 15);                // Smallest time between retries, max no. of retries
  //radio.setPayloadSize(4);              // Here we are sending 1-byte payloads to test the call-response speed
  radio.openWritingPipe(pipes[1]);        // Both radios listen on the same pipes by default, and switch when writing
  radio.openReadingPipe(1, pipes[0]);
  radio.printDetails();
}

void KnimbusRadio::SetPowerLevel(int pwr){
  
  switch (pwr) {
    case 0:
      radio.setPALevel(RF24_PA_MIN);
      break;
    case 1:
      radio.setPALevel(RF24_PA_LOW);
      break;
    case 2:
      radio.setPALevel(RF24_PA_HIGH);
      break;
    case 3:
      radio.setPALevel(RF24_PA_MAX);
      break;
    default:
      radio.setPALevel(RF24_PA_MAX);
      break; 
   }
}

bool KnimbusRadio::XMitWeather(Weather_t weatherData, const String &responseMsg) {
  return PowerOnRadioAndXMit(&weatherData, sizeof(weatherData), responseMsg);
}

void KnimbusRadio::XMitLightning(String lightningType, int lightningDistance) {
  String lightningMessage;
  if (lightningDistance > 0) {
    lightningMessage = lightningType + lightningDistance;
  }
  else {
    lightningMessage = lightningType;
  }

  char charBuf[lightningMessage.length()];
  lightningMessage.toCharArray(charBuf, 26);

  uint8_t data[lightningMessage.length()];
  strcpy((char*)data, charBuf);

  Serial.println(lightningMessage);
  Serial.println(sizeof(data));
  String responseMsg;
  PowerOnRadioAndXMit(&data, sizeof(data), responseMsg);
}

bool KnimbusRadio::PowerOnRadioAndXMit(void* buf, int size, const String &responseMsg) {
  radio.powerUp();

  delay(1000);
  radio.stopListening();
  if (!radio.write( buf, size)) {
    Serial.println(F("failed"));
    return false;
  }

  radio.startListening();                                   // Now, continue listening

  unsigned long started_waiting_at = micros();               // Set up a timeout period, get the current microseconds
  boolean timeout = false;                                   // Set up a variable to indicate if a response was received or not

  while ( ! radio.available() ) {
    //Serial.println(F("Waiting for response."));// While nothing is received
    if (micros() - started_waiting_at > 200000 ) {           // If waited longer than 200ms, indicate timeout and exit while loop
      timeout = true;
      break;
    }
  }

  if ( timeout ) {                                            // Describe the results
    Serial.println(F("Failed, response timed out."));
    return false;
  } else {
    char got_payload[32] = {0};                                 // Grab the response, compare, and send to debugging spew
    radio.read( &got_payload, sizeof(got_payload));

    // Spew it
    Serial.print(F(", Got response "));
    Serial.print(got_payload);
    responseMsg = got_payload;
  }
  radio.powerDown();
  return true;
}




