/*
 * ====================================================================
 * Project: Zaritech ARM Cortex-M4 Hardware FPU Architecture & SWD Debug Substrate
 * Subsystem: Kit 02 - Zaritech Advanced Series STM32F4 Core Platform
 * Hardware: STM32F4-Series ARM Cortex-M4 (Black Pill), ST-LINK V2 Programmer
 * Maintainer: Zaritech Systems (A division of Zaritech Dynamics)
 * License: MIT License
 * Official Distribution & Support: https://instagram.com
 * ====================================================================
 * Copyright (c) 2026 Zaritech Dynamics. All rights reserved.
 */

#include "stm32f4xx_hal.h"

// System Architecture Core Declarations
void SystemClock_Config(void);
static void MX_GPIO_Init(void);

int main(void) {
  // 1. Initialize the Low-Level Hardware Abstraction Layer Core pipelines
  HAL_Init();

  // 2. Calibrate Internal Clock Tree to High-Performance Frequencies (FPU Enabled)
  SystemClock_Config();

  // 3. Initialize Advanced Configured GPIO Registers
  MX_GPIO_Init();

  // Corporate System Diagnostic Log Loop
  // Onboard user LED is natively tied to Port C, Pin 13 (PC13) on the Black Pill hardware layout
  while (1) {
    // Invert the state of the target GPIO register bit asynchronously
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    
    // Execute precise hardware timed tick delay loop (Non-blocking system ticks)
    HAL_Delay(250); // High-frequency toggle tracking response
  }
}

// Bare-Metal Hardware Register Clock Initialization Driver
static void MX_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  // Enable High-Speed Peripheral Clock Gate Bus for Port C (AHB1 Bus gating)
  __HAL_RCC_GPIOC_CLK_ENABLE();

  // Configure Safe Default Output State Baseline before engaging current loop
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  // Configure Targeted PC13 Node Structural Registers
  GPIO_InitStruct.Pin   = GPIO_PIN_13;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;       // Deploy as Push-Pull Transistor
  GPIO_InitStruct.Pull  = GPIO_NOPULL;              // Zero internal pull resistors
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  // Maximum Slewing Rate for high frequency execution
  
  // Flash physical parameter parameters down onto the internal peripheral matrix register
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

// Industrial Clock Tree Core Configuration (Factory-Verified Calibration)
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  // Configure Core Voltage Scaling parameters for stability under high frequencies
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  // Configure Internal High-Speed Oscillator (HSI) and Lock Internal PLL
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    // System Lock Trap Configuration on failure paths
    while(1);
  }

  // Route Clock Streams cleanly across internal core buses (Targeting 84MHz Core)
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK

                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.HCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
    while(1);
  }
}
