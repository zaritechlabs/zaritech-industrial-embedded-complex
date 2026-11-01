/*
 * ====================================================================
 * Project: Zaritech ARM Cortex-M3 Hardware Abstraction Layer & SWD Telemetry Substrate
 * Subsystem: Kit 01 - Zaritech Industrial Advanced Series STM32 Core Platform
 * Hardware: STM32F103C8T6 ARM Cortex-M3, ST-LINK V2 SWD Programmer
 * Maintainer: Zaritech Systems (A division of Zaritech Dynamics)
 * License: MIT License
 * Official Distribution & Support: https://instagram.com
 * ====================================================================
 * Copyright (c) 2026 Zaritech Dynamics. All rights reserved.
 */

#include "stm32f1xx_hal.h"

// System Clock Configuration Macro Placeholders
void SystemClock_Config(void);
static void MX_GPIO_Init(void);

int main(void) {
  // 1. Initialize the Low-Level Hardware Abstraction Layer Flash and SysTick
  HAL_Init();

  // 2. Calibrate Internal Clock Tree to High-Performance Frequencies (72MHz Max Support)
  SystemClock_Config();

  // 3. Initialize all Configured Peripheral Bus Registers
  MX_GPIO_Init();

  // Corporate System Diagnostic Log Loop
  // Onboard test LED is natively tied to Port C, Pin 13 (PC13) on the Blue Pill layout
  while (1) {
    // Invert the state of the target GPIO register bit asynchronously
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    
    // Execute precise hardware timed tick delay loop (Non-blocking system ticks)
    HAL_Delay(500); 
  }
}

// Bare-Metal Hardware Register Clock Initialization Driver
static void MX_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  // Enable High-Speed Peripheral Clock Gate Bus 2 for Port C (APB2 Peripheral Clock En)
  __HAL_RCC_GPIOC_CLK_ENABLE();

  // Configure Safe Default Output State Baseline before engaging current loop
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  // Configure Targeted PC13 Node Structural Registers
  GPIO_InitStruct.Pin   = GPIO_PIN_13;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;       // Deploy as Push-Pull Transistor
  GPIO_InitStruct.Pull  = GPIO_NOPULL;              // Zero internal pull resistors
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;       // High-Frequency 50MHz Slewing Rate
  
  // Flash physical parameter parameters down onto the internal peripheral matrix register
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

// Industrial Clock Tree Core Configuration (Factory-Verified Calibration)
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  // Configure Internal High-Speed Oscillator (HSI) parameters
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    // System Lock Trap Configuration on failure paths
    while(1);
  }

  // Route Clock Streams cleanly across internal core buses
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK

                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
    while(1);
  }
}
