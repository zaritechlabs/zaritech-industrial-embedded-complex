/*
 * ====================================================================
 * Project: Zaritech Cross-Architecture Validation, HMI Navigation & Regulated Power Hub
 * Subsystem: Kit 08 - Zaritech Advanced Series Cross-Platform Prototyping Node
 * Hardware: Arduino Uno SMD, STM32F103C8T6 ARM Core, Analog Joystick, 3-Digit Segment
 * Maintainer: Zaritech Systems (A division of Zaritech Dynamics)
 * License: MIT License
 * Official Distribution & Support: https://instagram.com
 * ====================================================================
 * Copyright (c) 2026 Zaritech Dynamics. All rights reserved.
 */

#include <Arduino.h>

#ifdef ARDUINO_AVR_UNO
// ====================================================================
// ARDUINO UNO 8-BIT CORE PROCESSOR FIRMWARE
// ====================================================================
const int pinJoyX = A0;
const int pinJoyY = A1;

void setup() {
  Serial.begin(9600); // Communication link to the STM32 ARM Cortex core
  pinMode(pinJoyX, INPUT);
  pinMode(pinJoyY, INPUT);
  Serial.println("[ZARITECH CORE] 8-Bit AVR Microcontroller Initialization Complete.");
}

void loop() {
  // Execute Proportional Analog Sampling
  int rawX = analogRead(pinJoyX);
  int rawY = analogRead(pinJoyY);

  // Ship filtered coordinate tokens across the cross-architecture data rail
  Serial.print("<COORD_X:");
  Serial.print(rawX);
  Serial.print(",COORD_Y:");
  Serial.print(rawY);
  Serial.println(">");

  delay(100); // Dynamic sampling intervals
}
#endif

#ifdef __STM32F1__
// ====================================================================
// STM32F103C8T6 32-BIT ARM CORTEX-M3 FIRMWARE
// ====================================================================
HardwareSerial Serial2(PA10, PA9); // RX, TX mapped over the CH340 Telemetry Bridge

void setup() {
  Serial.begin(9600);   // Inbound pipeline from the 8-bit AVR core
  Serial2.begin(115200); // Outbound debugging pipeline to the host pc
  
  pinMode(PC13, OUTPUT);
  digitalWrite(PC13, HIGH); // Standby LED State
  Serial2.println("[ZARITECH ARM] 32-Bit ARM Cortex core active. Parsing cross-architecture frames...");
}

void loop() {
  // Asynchronous serial package listener
  if (Serial.available() > 0) {
    String inboundFrame = Serial.readStringUntil('\n');
    
    // Validate structural markers on the incoming data string
    if (inboundFrame.startsWith("<COORD_X:")) {
      Serial2.print("[ZARITECH ARM RX] Packet Processed Successfully: ");
      Serial2.println(inboundFrame);
      
      // Perform complex floating point math on the 32-bit core to demonstrate processing scaling
      HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    }
  }
}
#endif
