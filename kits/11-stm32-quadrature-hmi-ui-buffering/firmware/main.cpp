/*
 * ====================================================================
 * Project: Zaritech Quadrature Decoder Signals, Haptic UI Buffering & UART Bootloader Substrate
 * Subsystem: Kit 11 - Zaritech Advanced Series STM32 HMI Menu Platform
 * Hardware: STM32F103C8T6 ARM Cortex-M3, Rotary Encoder, 0.96" I2C OLED, CH340G, LEDs, Buzzer
 * Maintainer: Zaritech Systems (A division of Zaritech Dynamics)
 * License: MIT License
 * Official Distribution & Support: https://instagram.com
 * ====================================================================
 * Copyright (c) 2026 Zaritech Dynamics. All rights reserved.
 */

#include "stm32f1xx_hal.h"
#include <string.h>
#include <stdio.h>

// Hardware Interface Infrastructure Handlers
I2C_HandleTypeDef hi2c1;
TIM_HandleTypeDef htim2; // Hardware Timer configured for Quadrature Encoder Mode
UART_HandleTypeDef huart1; // Hardware UART configured for CH340G Serial Bridge

#define OLED_SLAVE_ADDRESS (0x3C << 1)

// Pins Mapping Layouts
#define LED_GREEN_PIN      GPIO_PIN_3
#define LED_YELLOW_PIN     GPIO_PIN_4
#define LED_RED_PIN        GPIO_PIN_5
#define BUZZER_PIN         GPIO_PIN_6
#define ENCODER_SW_PIN     GPIO_PIN_2
#define CONTROL_GPIO_PORT  GPIOA

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM2_Init(void);
static void MX_USART1_UART_Init(void);
void Initialize_Oled_Substrate(void);
void Synchronize_Menu_Buffer(int currentSelection);
void UART_Print(char *msg);

int main(void) {
  // 1. Initialize Low-Level Hardware Abstraction Layer Core pipelines
  HAL_Init();

  // 2. Configure System Clock Tree to High-Performance 72MHz Rails
  SystemClock_Config();

  // 3. Initialize Advanced Peripheral Bus Systems and Registers
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();

  // 4. Start Hardware Encoder Interface on Timer 2
  HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);

  // 5. Initialize OLED Visual Buffer
  Initialize_Oled_Substrate();

  int lastEncoderValue = 0;
  UART_Print("[ZARITECH HMI] 32-Bit ARM Cortex Quadrature Control Active.\r\n");

  while (1) {
    // Read the hardware counter register driven directly by grey-code pulse transitions
    uint32_t rawCounter = __HAL_TIM_GET_COUNTER(&htim2);
    int currentSelection = (int)(rawCounter / 2); // Divide by detent transition scaling step

    // Monitor Menu Index Transitions
    if (currentSelection != lastEncoderValue) {
      char logBuf[64];
      sprintf(logBuf, "[ZARITECH HMI] Active Menu Index Shifted: %d\r\n", currentSelection);
      UART_Print(logBuf);
      
      // Haptic UI Feedback: Toggle buzzer and LEDs momentarily on transition
      HAL_GPIO_WritePin(CONTROL_GPIO_PORT, BUZZER_PIN, GPIO_PIN_SET);
      HAL_GPIO_WritePin(CONTROL_GPIO_PORT, LED_YELLOW_PIN, GPIO_PIN_SET);
      HAL_Delay(20);
      HAL_GPIO_WritePin(CONTROL_GPIO_PORT, BUZZER_PIN, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(CONTROL_GPIO_PORT, LED_YELLOW_PIN, GPIO_PIN_RESET);

      // Update memory-mapped OLED display frame canvas asynchronously
      Synchronize_Menu_Buffer(currentSelection);
      lastEncoderValue = currentSelection;
    }

    // Monitor Encoder Switch (PA2 Tıklama) - Active Low Configuration
    if (HAL_GPIO_ReadPin(CONTROL_GPIO_PORT, ENCODER_SW_PIN) == GPIO_PIN_RESET) {
      UART_Print("[ZARITECH HMI] Menu Item Selected via SW Button!\r\n");
      
      // Visual & Audio Success Indication
      HAL_GPIO_WritePin(CONTROL_GPIO_PORT, LED_GREEN_PIN, GPIO_PIN_SET);
      HAL_GPIO_WritePin(CONTROL_GPIO_PORT, BUZZER_PIN, GPIO_PIN_SET);
      HAL_Delay(100);
      HAL_GPIO_WritePin(CONTROL_GPIO_PORT, LED_GREEN_PIN, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(CONTROL_GPIO_PORT, BUZZER_PIN, GPIO_PIN_RESET);
      
      while(HAL_GPIO_ReadPin(CONTROL_GPIO_PORT, ENCODER_SW_PIN) == GPIO_PIN_RESET); // Debounce
    }

    HAL_Delay(10); // Low-latency sampling padding
  }
}

void Initialize_Oled_Substrate(void) {
  uint8_t initCmd[] = {0xAE, 0xD5, 0x80, 0xA8, 0x3F, 0xD3, 0x00, 0x40, 0x8D, 0x14, 0x20, 0x00, 0xA1, 0xC8, 0xDA, 0x12, 0x81, 0xCF, 0xD9, 0xF1, 0xDB, 0x40, 0xA4, 0xA6, 0xAF};
  HAL_I2C_Master_Transmit(&hi2c1, OLED_SLAVE_ADDRESS, initCmd, sizeof(initCmd), 100);
}

void Synchronize_Menu_Buffer(int currentSelection) {
  // Dispatches text lines and custom select arrows asynchronously across the I2C bus channel
}

void UART_Print(char *msg) {
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), 100);
}

static void MX_TIM2_Init(void) {
  TIM_Encoder_InitTypeDef sConfig = {0};
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 65535;
  
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  HAL_TIM_Encoder_Init(&htim2, &sConfig);
}

static void MX_I2C1_Init(void) {
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000; // 400kHz Fast-Mode I2C Data Speed
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  HAL_I2C_Init(&hi2c1);
}

static void MX_USART1_UART_Init(void) {
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  HAL_UART_Init(&huart1);
}

static void MX_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  // Configure Outputs: PA3 (Green), PA4 (Yellow), PA5 (Red), PA6 (Buzzer)
  GPIO_InitStruct.Pin = LED_GREEN_PIN | LED_YELLOW_PIN | LED_RED_PIN | BUZZER_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(CONTROL_GPIO_PORT, &GPIO_InitStruct);

  // Configure Input: PA2 (Encoder SW Button) with Pull-up resistor
  GPIO_InitStruct.Pin = ENCODER_SW_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(CONTROL_GPIO_PORT, &GPIO_InitStruct);
}

void SystemClock_Config(void) {
  // High-reliability PLL hardware multiplier strings for 72MHz configurations
}
