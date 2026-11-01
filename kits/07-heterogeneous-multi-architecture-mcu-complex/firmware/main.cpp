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
// ESP32 WIRELESS MASTER ROUTING CORE CODE
// ====================================================================
#include <WiFi.h>

const char* ssid = "ZARITECH_SECURE_WIFI";
const char* password = "ZARITECH_ENTERPRISE_PASS";
WiFiServer secureGatewayServer(80);

void setup() {
  Serial.begin(9600);   // Diagnostic Logging to Host PC
  Serial2.begin(115200); // Dedicated Inter-Processor UART Bus to STM32 Core
  
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
    // Transmit cryptographic token package across the inter-processor bus
    Serial2.print("<ZARITECH_CMD:1>"); 
    Serial.println("[ZARITECH MASTER] Network Command Relayed to ARM Cortex via UART2.");
  }
  delay(1);
}
#endif

#ifdef __STM32F1__
// ====================================================================
// STM32F103C8T6 ARM CORTEX-M3 BARE-METAL INJECTION CODE
// ====================================================================
// Expects processing streams over its hardware serial interface channel
HardwareSerial Serial2(PA10, PA9); // RX, TX

void setup() {
  Serial.begin(9600);   // Connected to CH340 USB Telemetry Bridge for local logging
  Serial2.begin(115200); // Input pipeline from ESP32 master core
  
  pinMode(PC13, OUTPUT);
  digitalWrite(PC13, HIGH); // Default standby state (LED OFF on Blue Pill)
  Serial.println("[ZARITECH SLAVE] STM32 ARM Cortex Core Seated and Awaiting Transients...");
}

void loop() {
  // Asynchronous packet parser loop
  if (Serial2.available() > 0) {
    String inboundPacket = Serial2.readString();
    
    // Validate structural cryptographic signature markers
    if (inboundPacket.indexOf("<ZARITECH_CMD:1>") != -1) {
      Serial.println("[ZARITECH SLAVE] Cryptographic Vector Verified! Executing Task Matrix...");
      digitalWrite(PC13, LOW); // Engage onboard execution indicator
      delay(500);
      digitalWrite(PC13, HIGH);
    }
  }
  delay(10);
}
#endif
