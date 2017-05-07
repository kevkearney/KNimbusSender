#include <RF24.h>
#include "KnimbusResponseParser.h"
#include <Arduino.h>

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(7, 8);
const uint64_t pipes[2] = { 0xF0F0F0F0E1, 0xF0F0F0F0D2 };



//Weather Message Struct
typedef struct {
  byte MessageType;
  int Temperature;
  int Humidity;
  int BaroPressure;
  int BaroTemperature;
  unsigned int Lux;
}
WeatherDataMsg;

//Lightning Message Struct
typedef struct {
  byte MessageType;
  int EventType;
  int Distance;
  int Intensity;
}
LightningMsg;


bool test;

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
  //radio.printDetails();







}

void loop() {

  WeatherDataMsg send_payload;
  LightningMsg lightningMsg;

  send_payload.MessageType = 1;
  send_payload.BaroPressure = 9782;
  send_payload.Temperature = 2222;
  send_payload.Humidity = 6053;
  send_payload.Lux = 22;
  send_payload.BaroTemperature = 2563;

  lightningMsg.MessageType = 2;
  lightningMsg.EventType = 3;
  lightningMsg.Distance = 87;
  lightningMsg.Intensity = 5;

  //send_payload.BaroTemperature++;

  if (test) {
    XMit(&send_payload, 11);

  }
  else {
    XMit(&lightningMsg, 7);
  }
  test = !test;
}





void XMit(void* buf, int payloadsize) {

  radio.powerUp()


  delay(1000);
  radio.stopListening();

  if (!radio.write( buf, payloadsize) ) {
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





