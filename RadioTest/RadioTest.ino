#include <RF24.h>
#include "KnimbusResponseParser.h"
#include <Arduino.h>

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(7, 8);
const uint64_t pipes[2] = { 0xF0F0F0F0E1, 0xF0F0F0F0D2 };

//Weather Struct
typedef struct {
  int Temperature;
  int Humidity;
  int BaroPressure;
  int BaroTemperature;
  unsigned int Lux;
  bool test;
}
weathermsg;
 weathermsg send_payload;


void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.setAutoAck(1);                    // Ensure autoACK is enabled
  radio.enableAckPayload();               // Allow optional ack payloads
  radio.setPALevel(RF24_PA_MAX);
  radio.setRetries(100, 15);                // Smallest time between retries, max no. of retries
  //radio.setPayloadSize(4);              // Here we are sending 1-byte payloads to test the call-response speed
  radio.openWritingPipe(pipes[1]);        // Both radios listen on the same pipes by default, and switch when writing
  radio.openReadingPipe(1, pipes[0]);
  radio.printDetails();

 send_payload.BaroPressure = 9782;
 send_payload.Temperature = 2632;
 send_payload.Humidity = 6053;
  send_payload.Lux = 1023;
  send_payload.BaroTemperature = 2563;
  send_payload.test = true;
}

void loop(){ 
 
  send_payload.BaroTemperature++;
  send_payload.test = !send_payload.test; 
  
  int payloadSize = sizeof(send_payload);
  XMit(&send_payload, payloadSize); 
 
  }

  void XMit(void* buf, int size){
     radio.powerUp();
    
  
  delay(1000);
  radio.stopListening();
  
  if (!radio.write( buf, 12)) {
    Serial.println(F("failed"));
    return false;
  }

  radio.startListening();                                   // Now, continue listening

  unsigned long started_waiting_at = micros();               // Set up a timeout period, get the current microseconds
  boolean timeout = false;                                   // Set up a variable to indicate if a response was received or not

  while ( ! radio.available() ) {
    Serial.println(F("Waiting for response."));// While nothing is received
    if (micros() - started_waiting_at > 200000 ) {           // If waited longer than 200ms, indicate timeout and exit while loop
      timeout = true;
      break;
    }
  }

  if ( timeout ) {                                            // Describe the results
    Serial.println(F("Failed, response timed out."));
    return false;
  } else {
    char got_payload[32] = {0};   
    // Grab the response, compare, and send to debugging spew

    WeatherControl weathermsg;
    radio.read( &weathermsg, sizeof(WeatherControl));

    // Spew it
    Serial.println(F(", Got response "));
    Serial.print(F(",SleepTime: "));
    Serial.println(weathermsg.sleepTime);    
    Serial.print(F(",lightningIndoors: "));
    Serial.println(weathermsg.lightningIndoors); 
    Serial.print(F(",lightningTune: "));
    Serial.println(weathermsg.lightningTune); 
      Serial.print(F(",lightningNoiseFloor: "));
    Serial.println(weathermsg.lightningNoiseFloor);
Serial.print(F(",radioPower: "));
    Serial.println(weathermsg.radioPower);
  
  }
  radio.powerDown();
  delay(1000);
  }




