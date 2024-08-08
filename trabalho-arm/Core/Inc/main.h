/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
#define LDR_1_Pin GPIO_PIN_0
#define LDR_1_GPIO_Port GPIOA
#define LDR_2_Pin GPIO_PIN_1
#define LDR_2_GPIO_Port GPIOA
#define KEYPAD_COL1_Pin GPIO_PIN_1
#define KEYPAD_COL1_GPIO_Port GPIOB
#define KEYPAD_COL2_Pin GPIO_PIN_2
#define KEYPAD_COL2_GPIO_Port GPIOB
#define KEYPAD_COL3_Pin GPIO_PIN_10
#define KEYPAD_COL3_GPIO_Port GPIOB
#define KEYPAD_COL4_Pin GPIO_PIN_11
#define KEYPAD_COL4_GPIO_Port GPIOB
#define KEYPAD_ROW1_Pin GPIO_PIN_12
#define KEYPAD_ROW1_GPIO_Port GPIOB
#define KEYPAD_ROW2_Pin GPIO_PIN_13
#define KEYPAD_ROW2_GPIO_Port GPIOB
#define KEYPAD_ROW3_Pin GPIO_PIN_14
#define KEYPAD_ROW3_GPIO_Port GPIOB
#define KEYPAD_ROW4_Pin GPIO_PIN_15
#define KEYPAD_ROW4_GPIO_Port GPIOB
#define LCD_D4_Pin GPIO_PIN_9
#define LCD_D4_GPIO_Port GPIOA
#define LCD_D5_Pin GPIO_PIN_10
#define LCD_D5_GPIO_Port GPIOA
#define LCD_D6_Pin GPIO_PIN_11
#define LCD_D6_GPIO_Port GPIOA
#define LCD_D7_Pin GPIO_PIN_12
#define LCD_D7_GPIO_Port GPIOA
#define LCD_RS_Pin GPIO_PIN_3
#define LCD_RS_GPIO_Port GPIOB
#define LCD_E_Pin GPIO_PIN_4
#define LCD_E_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
