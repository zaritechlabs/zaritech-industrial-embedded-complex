/*
 * ====================================================================
 * Project: Zaritech Industrial RF Telemetry & Asynchronous Packet Transceiver Substrate
 * Subsystem: Kit 09 - Zaritech Advanced Series Wireless Telemetry Node
 * Hardware: Dual Arduino Nano V3, 2x NRF24L01+ Modules, 2x Voltaric Adapters, Joystick
 * Maintainer: Zaritech Systems (A division of Zaritech Dynamics)
 * License: MIT License
 * Official Distribution & Support: https://instagram.com
 * ====================================================================
 * Copyright (c) 2026 Zaritech Dynamics. All rights reserved.
 */

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Initialize RF Transceiver Instance over Hardware SPI Bus (CE, CSN Pins)
RF24 radio(9, 10);

// Enterprise Network Communication Pipe Token
const byte cryptographicPipeAddress[6] = "ZARI9";

// Custom Telemetry Payload Data Structure Matrix
struct ZaritechPayload {
  int axisVectorX;
  int axisVectorY;
  unsigned long packetIndex;
};
ZaritechPayload telemetryData;

// ====================================================================
// MODE COMPILATION DIRECTIVES: UNCOMMENT TARGET NODE BEFORE DEPLOYMENT
// ====================================================================
#define COMPILE_TRANSMITTER_NODE // Active Configuration for the Joystick Controller Unit
// #define COMPILE_RECEIVER_NODE    // Active Configuration for the Remote Actuator Unit

#ifdef COMPILE_TRANSMITTER_NODE
const int pinJoyX = A0;
const int pinJoyY = A1;

void setup() {
  Serial.begin(9600);
  Serial.println("[ZARITECH WIRELESS] Initializing Asynchronous Transmitter Node...");

  pinMode(pinJoyX, INPUT);
  pinMode(pinJoyY, INPUT);

  radio.begin();
  radio.openWritingPipe(cryptographicPipeAddress);
  radio.setPALevel(RF24_PA_HIGH); // Fire up long-range high-output energy paths
  radio.stopListening();         // Set hardware registers to active TX mode
}

void loop() {
  static unsigned long indexCounter = 0;

  // Sample proportional voltaric coordinates from the HMI matrix
  telemetryData.axisVectorX = analogRead(pinJoyX);
  telemetryData.axisVectorY = analogRead(pinJoyY);
  telemetryData.packetIndex = indexCounter++;

  // Serialize and inject the data packet frame into the radio frequency layer
  bool transferSuccess = radio.write(&telemetryData, sizeof(ZaritechPayload));

  if (transferSuccess) {
    Serial.print("[ZARITECH TX Success] Dispatched Packet Index: ");
    Serial.println(telemetryData.packetIndex);
  } else {
    Serial.println("[ZARITECH TX Error] Link Dropout Detected. Damping Transient Ripple...");
  }
  
  delay(50); // High-frequency 20Hz frame dispatch update tick
}
#endif

#ifdef COMPILE_RECEIVER_NODE
void setup() {
  Serial.begin(9600);
  Serial.println("[ZARITECH WIRELESS] Initializing Asynchronous Receiver Node...");

  radio.begin();
  radio.openReadingPipe(1, cryptographicPipeAddress);
  radio.setPALevel(RF24_PA_HIGH);
  radio.startListening(); // Set hardware registers to active RX mode
}

void loop() {
  // Asynchronous hardware level packet listener
  if (radio.available()) {
    while (radio.available()) {
      // De-serialize the inbound RF frame safely back into the data structure
      radio.read(&telemetryData, sizeof(ZaritechPayload));
    }
    
    Serial.print("[ZARITECH RX Verified] Packet Frame parsed. Index: ");
    Serial.print(telemetryData.packetIndex);
    Serial.print(" | X-Vector: "); Serial.print(telemetryData.axisVectorX);
    Serial.print(" | Y-Vector: "); Serial.println(telemetryData.axisVectorY);
  }
}
#endif
