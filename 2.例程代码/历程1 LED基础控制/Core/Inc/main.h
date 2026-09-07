/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h5xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* Include from c */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <stddef.h>

/* Include from core */

#include "usart.h"
#include "gpio.h"
#include "spi.h"
#include "fdcan.h"
#include "tim.h"
#include "crc.h"

/* from sys */

#include "sys.h"
#include "config.h"

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SPI4_CS_Pin GPIO_PIN_3
#define SPI4_CS_GPIO_Port GPIOE
#define SPI4_INT_Pin GPIO_PIN_4
#define SPI4_INT_GPIO_Port GPIOE
#define SPI4_INT_EXTI_IRQn EXTI4_IRQn
#define LEFT_WKUP1_Pin GPIO_PIN_13
#define LEFT_WKUP1_GPIO_Port GPIOC
#define LEFT_WKUP2_Pin GPIO_PIN_1
#define LEFT_WKUP2_GPIO_Port GPIOC
#define RIGHT_WKUP1_Pin GPIO_PIN_0
#define RIGHT_WKUP1_GPIO_Port GPIOA
#define RIGHT_WKUP2_Pin GPIO_PIN_2
#define RIGHT_WKUP2_GPIO_Port GPIOA
#define SPI1_CS_Pin GPIO_PIN_4
#define SPI1_CS_GPIO_Port GPIOC
#define FLASH_WP_Pin GPIO_PIN_5
#define FLASH_WP_GPIO_Port GPIOC
#define FLASH_HOLD_Pin GPIO_PIN_0
#define FLASH_HOLD_GPIO_Port GPIOB
#define EN_VM_Pin GPIO_PIN_2
#define EN_VM_GPIO_Port GPIOB
#define EN_3V3_Pin GPIO_PIN_14
#define EN_3V3_GPIO_Port GPIOE
#define EN_5V_Pin GPIO_PIN_15
#define EN_5V_GPIO_Port GPIOE
#define SPI2_CS_Pin GPIO_PIN_12
#define SPI2_CS_GPIO_Port GPIOB
#define VOICE_PWR_Pin GPIO_PIN_15
#define VOICE_PWR_GPIO_Port GPIOD
#define VOICE_BUSY_Pin GPIO_PIN_8
#define VOICE_BUSY_GPIO_Port GPIOC
#define FDCAN_STB_Pin GPIO_PIN_8
#define FDCAN_STB_GPIO_Port GPIOA
#define SPI3_CS_Pin GPIO_PIN_15
#define SPI3_CS_GPIO_Port GPIOA
#define LED_Pin GPIO_PIN_4
#define LED_GPIO_Port GPIOB
#define FDCAN2_STB_Pin GPIO_PIN_7
#define FDCAN2_STB_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
