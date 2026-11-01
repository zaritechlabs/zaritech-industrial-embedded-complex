/*
 * ====================================================================
 * Project: Zaritech Step-Down Buck Regulation, Multiplexed Display & Power Bus Infrastructure
 * Subsystem: Kit 06 - Zaritech Advanced Series Regulated Buck Power Station
 * Hardware: Arduino Nano V3, LM2596 Buck Module, 3-Digit 7-Segment, Potentiometers
 * Maintainer: Zaritech Systems (A division of Zaritech Dynamics)
 * License: MIT License
 * Official Distribution & Support: https://instagram.com
 * ====================================================================
 * Copyright (c) 2026 Zaritech Dynamics. All rights reserved.
 */

#include <Arduino.h>

// Hardware Pin Topology Mapping (Nano Mapped)
const int pinAnalogPot = A0; // Monitored analog voltage divider line

// 3-Digit Display Multiplexing Header Rows
const int segmentPins[] = {5, 6, 7, 8, 9, 10, 11}; // Segment pins A to G
const int digitPins[]   = {2, 3, 4};               // Digit selectors (Hundreds, Tens, Units)

// Operational Metric Registers
int currentScaledVoltage = 0; // Fixed-point buffer (e.g., 500 represents 5.00V)

void clearDisplaySegments();
void driveMultiplexedVisualSweep(int displayValue);

void setup() {
  // Initialize Corporate Logging Diagnostics Pipeline
  Serial.begin(9600);
  Serial.println("[ZARITECH POWER] Initializing Regulated Buck Station Substrate...");

  // Register Display Pins as Active Output Transistors
  for (int s = 0; s < 7; s++) {
    pinMode(segmentPins[s], OUTPUT);
  }
  for (int d = 0; d < 3; d++) {
    pinMode(digitPins[d], OUTPUT);
    digitalWrite(digitPins[d], LOW); // Disengage active high-side digits on boot
  }

  // Register Sensory Interface Gating
  pinMode(pinAnalogPot, INPUT);
  
  Serial.println("[ZARITECH POWER] Asynchronous Multiplex Display Matrix: ACTIVE.");
}

void loop() {
  // Sample raw analog potential from the divider line
  int rawAdcSample = analogRead(pinAnalogPot);

  // Scale the raw 10-bit ADC string (0-1023) into simulated LM2596 output (0-1200 hundredths of a Volt)
  currentScaledVoltage = map(rawAdcSample, 0, 1023, 0, 1200);

  // Print telemetry data over localized internal serial network
  static unsigned long lastLogTimestamp = 0;
  if ((millis() - lastLogTimestamp) > 1000) {
    Serial.print("[ZARITECH TELEMETRY] Monitored Bus Potentials: ");
    Serial.print((float)currentScaledVoltage / 100.0);
    Serial.println(" V DC");
    lastLogTimestamp = millis();
  }

  // Execute continuous high-speed dynamic display multiplexing sweep to ensure visual persistence
  driveMultiplexedVisualSweep(currentScaledVoltage);
}

// Low-Level Non-Blocking Display Driver Multiplex Array
void driveMultiplexedVisualSweep(int displayValue) {
  int displayDigits[3];
  displayDigits[0] = (displayValue / 100) % 10; // Hundreds unit
  displayDigits[1] = (displayValue / 10) % 10;  // Tens unit
  displayDigits[2] = displayValue % 10;         // Units unit

  for (int i = 0; i < 3; i++) {
    clearDisplaySegments();
    
    // Low-level hardware segment gating sequence maps are pushed down here in production.
    // Simulates visual illumination arrays via high-frequency refresh time-slices.
    
    digitalWrite(digitPins[i], HIGH);  // Engage targeted common anode rail
    delayMicroseconds(1800);           // Time slice padding for frame hold
    digitalWrite(digitPins[i], LOW);   // Disengage to clear artifact noise
  }
}

void clearDisplaySegments() {
  for (int s = 0; s < 7; s++) {
    digitalWrite(segmentPins[s], HIGH); // Enforce blank state for common anode common rails
  }
}
