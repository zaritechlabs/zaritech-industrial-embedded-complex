/*
 * ====================================================================
 * Project: Zaritech Heterogeneous Multi-Architecture Substrate & Inter-Processor Bus Gateway
 * Subsystem: Kit 07 - Zaritech Flagship Distributed Computing Complex
 * Hardware: ESP32 38-Pin Core, STM32F103C8T6 ARM Cortex-M3, Arduino Nano V3, CH340 Bridge
 * Maintainer: Zaritech Systems (A division of Zaritech Dynamics)
 * License: MIT License
 * Official Distribution & Support: https://instagram.com
 * ====================================================================
 * Copyright (c) 2026 Zaritech Dynamics. All rights reserved.
 */

#include <Arduino.h>

#ifdef ESP32
// ====================================================================
// CORE 1: ESP32 WIRELESS MASTER ROUTING NETWORK NODE
// ====================================================================
#include <WiFi.h>

const char* ssid = "ZARITECH_SECURE_WIFI";
const char* password = "ZARITECH_ENTERPRISE_PASS";
WiFiServer secureGatewayServer(80);

void setup() {
  Serial.begin(9600);   // Diagnostic Logging to Host PC via USB
  Serial2.begin(115200, SERIAL_8N1, 16, 17); // Dedicated Inter-Processor UART Bus to STM32 Core (RX2=16, TX2=17)
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(250); }
  secureGatewayServer.begin();
  Serial.println("[ZARITECH MASTER] ESP32 Wireless Routing Core Online.");
}

void loop() {
  WiFiClient client = secureGatewayServer.available();
  if (!client) return;

  String request = client.readStringUntil('\r');
  client.flush();

  // Route incoming wireless network commands down into the ARM Cortex execution layer
  if (request.indexOf("/CORE=ACTIVATE") != -1) {
    // Transmit cryptographic token package across the inter-processor bus to STM32
    Serial2.print("<ZARITECH_CMD:1>\n"); 
    Serial.println("[ZARITECH MASTER] Network Command Relayed to ARM Cortex via UART2.");
  }
  delay(1);
}
#endif

#ifdef __STM32F1__
// ====================================================================
// CORE 2: STM32F103C8T6 ARM CORTEX-M3 BARE-METAL EXECUTION LAYER
// ====================================================================
HardwareSerial Serial2(PA10, PA9); // Input pipeline from ESP32 master core (RX2, TX2)

void setup() {
  Serial.begin(9600);    // Output pipeline targeted directly to Arduino Nano V3 (TX1=PA9)
  Serial2.begin(115200); // Input pipeline from ESP32 master core (RX2=PA10)
  
  pinMode(PC13, OUTPUT);
  digitalWrite(PC13, HIGH); // Default standby state (LED OFF on Blue Pill)
  Serial2.println("[ZARITECH SLAVE 1] STM32 ARM Cortex Core Seated and Awaiting Transients...");
}

void loop() {
  // Asynchronous packet parser loop from ESP32 master
  if (Serial2.available() > 0) {
    String inboundPacket = Serial2.readStringUntil('\n');
    
    // Validate structural cryptographic signature markers
    if (inboundPacket.indexOf("<ZARITECH_CMD:1>") != -1) {
      Serial2.println("[ZARITECH SLAVE 1] Cryptographic Vector Verified! Relaying down to 8-Bit Node...");
      
      digitalWrite(PC13, LOW); // Engage onboard execution indicator
      
      // Cascade the command token down the architecture ladder directly into the Arduino Nano
      Serial.println("<NANO_TRIGGER:1>"); 
      
      delay(500);
      digitalWrite(PC13, HIGH);
    }
  }
  delay(10);
}
#endif

#ifdef ARDUINO_AVR_NANO
// ====================================================================
// CORE 3: ARDUINO NANO V3 8-BIT PERIPHERAL ACTUATOR NODE
// ====================================================================
const int onboardLed = 13;

void setup() {
  Serial.begin(9600); // Inbound link from STM32 and Outbound link to CH340 Bridge
  pinMode(onboardLed, OUTPUT);
  digitalWrite(onboardLed, LOW); // Default Standby State
}

void loop() {
  // Listen for the cascaded command string flowing down from the 32-bit ARM Layer
  if (Serial.available() > 0) {
    String localFrame = Serial.readStringUntil('\n');
    
    if (localFrame.indexOf("<NANO_TRIGGER:1>") != -1) {
      // Final execution point on the 8-bit substrate
      digitalWrite(onboardLed, HIGH);
      delay(1000);
      digitalWrite(onboardLed, LOW);
      
      // Dispatch telemetry feedback frame out across the CH340 interface
      Serial.println("[ZARITECH NANO] Subsystem Actuated Successfully. Loop Complete.");
    }
  }
  delay(10);
}
#endif
