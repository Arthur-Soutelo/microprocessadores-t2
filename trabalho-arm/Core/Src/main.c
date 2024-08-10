/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "keypad.h"
#include "lcd.h"
#include "ambient.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

/* USER CODE BEGIN PV */
volatile unsigned char tempo_irrigacao = 15;
volatile unsigned char elapsed_time = 0;
volatile char flag_irrigacao_em_andamento = 0;

volatile char flag_temperatura_acima_limite = 0;	// 1 = Acima do Limite, 0 = Abaixo do Limite
volatile char flag_turno_dia = 1;		// 1 = Dia, 0 = Noite
char variedade = 0;		// 0-Alface, 1-Pimentao, 2-Morango

float temperatura_limite = 25.0;
volatile float temperatura_atual;

volatile char selected_menu = 0; // 0-Principal, 1-Informacoes Sistema, 2-Selecao, 3-Intensidade Solar

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */
void select_params(void);
void menu_main(void);
void menu_selection(void);
void menu_plant_selection(void);
void menu_temperature_selection(void);
void get_name(char code, char* buffer);
void get_day_night(char code, char* buffer);
void menu_actual_state(void);
void menu_light(void);

void menu_selection_operator(void);
char validate_user(const char* login, const char* password);
char menu_operator_login(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// Interrupt handler for TIM2 Channel 1 and Channel 2

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	// TIMER 3 -> IRRIGACAO (Periodo 1s)
    if (htim->Instance == TIM3)
    {
    	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

    	/* ============================== IRRIGACAO ============================== */
    	elapsed_time++;
    	if(elapsed_time >= tempo_irrigacao && flag_irrigacao_em_andamento == 1){
    		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
    		flag_irrigacao_em_andamento=0;
    	}
    	else if(elapsed_time < tempo_irrigacao && flag_irrigacao_em_andamento == 0){
    		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
    		elapsed_time=0;
    		flag_irrigacao_em_andamento=1;
    	}
    	else if(elapsed_time >= 60 && flag_irrigacao_em_andamento == 0){
			elapsed_time=0;
		}

    }
    // TIMER 2
	if (htim->Instance == TIM2){
		/* ==================== DEFINE VARIAVEIS DO SISTEMA ==================== */
		temperatura_atual = Read_Temperature();
		if(temperatura_atual > temperatura_limite){
			flag_temperatura_acima_limite = 1;	// 1 = Acima do Limite, 0 = Abaixo do Limite
		}else{
			flag_temperatura_acima_limite = 0;	// 1 = Acima do Limite, 0 = Abaixo do Limite
		}
		Classify_Day_or_Night(&flag_turno_dia);
		select_params();
		Regulate_Light_Intensity();
	}
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
//	uint32_t ldr1_value, ldr2_value;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

  /* Enable interrupt by timer 2*/
  HAL_TIM_Base_Start_IT(&htim2);
  /* Enable interrupt by timer 3*/
  HAL_TIM_Base_Start_IT(&htim3);
  /* Enable PWM by timer 4 CH4*/
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);

  init_LCD();
  keypad_init();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	/* ==================== RECEBE TECLA ==================== */
	 char key = keypad_getkey();
	 if(key != 0){
		 menu_selection();
	 }
	/* ==================== ATUALIZA MENU ==================== */

	// 0-Principal, 1-Informacoes Sistema, 2-Selecao
	switch(selected_menu) {
		case 0:
			menu_main();
			break;
		case 1:
			menu_actual_state();
			break;
		case 2:
			break;
		case 3:
			menu_light();
			break;
	}

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 35999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_DOWN;
  htim2.Init.Period = 999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 7199;
  htim3.Init.CounterMode = TIM_COUNTERMODE_DOWN;
  htim3.Init.Period = 9999;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 7199;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 119;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 60;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  __HAL_TIM_DISABLE_OCxPRELOAD(&htim4, TIM_CHANNEL_4);
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
  HAL_TIM_MspPostInit(&htim4);

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, IRRIGACAO_Pin|LCD_D4_Pin|LCD_D5_Pin|LCD_D6_Pin
                          |LCD_D7_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, KEYPAD_COL1_Pin|KEYPAD_COL2_Pin|KEYPAD_COL3_Pin|KEYPAD_COL4_Pin
                          |LCD_RS_Pin|LCD_E_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : IRRIGACAO_Pin LCD_D4_Pin LCD_D5_Pin LCD_D6_Pin
                           LCD_D7_Pin */
  GPIO_InitStruct.Pin = IRRIGACAO_Pin|LCD_D4_Pin|LCD_D5_Pin|LCD_D6_Pin
                          |LCD_D7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : KEYPAD_COL1_Pin KEYPAD_COL2_Pin KEYPAD_COL3_Pin KEYPAD_COL4_Pin
                           LCD_RS_Pin LCD_E_Pin */
  GPIO_InitStruct.Pin = KEYPAD_COL1_Pin|KEYPAD_COL2_Pin|KEYPAD_COL3_Pin|KEYPAD_COL4_Pin
                          |LCD_RS_Pin|LCD_E_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : KEYPAD_ROW1_Pin KEYPAD_ROW2_Pin KEYPAD_ROW3_Pin KEYPAD_ROW4_Pin */
  GPIO_InitStruct.Pin = KEYPAD_ROW1_Pin|KEYPAD_ROW2_Pin|KEYPAD_ROW3_Pin|KEYPAD_ROW4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void select_params(void){

	switch(variedade){
		// ALFACE
		case 0:
			if(flag_turno_dia == 1 && flag_temperatura_acima_limite == 1){
				tempo_irrigacao = 17;
			}
			else if(flag_turno_dia == 1 && flag_temperatura_acima_limite == 0){
				tempo_irrigacao = 15;
			}
			else if(flag_turno_dia == 0 && flag_temperatura_acima_limite == 1){
				tempo_irrigacao = 10;
			}
			else if(flag_turno_dia == 0 && flag_temperatura_acima_limite == 0){
				tempo_irrigacao = 7;
			}
			break;
		// PIMENTAO
		case 1:
			if(flag_turno_dia == 1 && flag_temperatura_acima_limite == 1){
				tempo_irrigacao = 13;
			}
			else if(flag_turno_dia == 1 && flag_temperatura_acima_limite == 0){
				tempo_irrigacao = 10;
			}
			else if(flag_turno_dia == 0 && flag_temperatura_acima_limite == 1){
				tempo_irrigacao = 6;
			}
			else if(flag_turno_dia == 0 && flag_temperatura_acima_limite == 0){
				tempo_irrigacao = 3;
			}
			break;
		// MORANGO
		case 2:
			if(flag_turno_dia == 1 && flag_temperatura_acima_limite == 1){
				tempo_irrigacao = 19;
			}
			else if(flag_turno_dia == 1 && flag_temperatura_acima_limite == 0){
				tempo_irrigacao = 17;
			}
			else if(flag_turno_dia == 0 && flag_temperatura_acima_limite == 1){
				tempo_irrigacao = 13;
			}
			else if(flag_turno_dia == 0 && flag_temperatura_acima_limite == 0){
				tempo_irrigacao = 10;
			}
			break;
	}

}

void menu_selection(void){
	selected_menu = 2;
	const char *options[] = {"  Informacoes", " Modo Operador", "Incidencia Solar", "      Sair"};
	char num_options = sizeof(options) / sizeof(options[0]);
	char option = navigate_options(options, num_options);
	switch(option){
		case 0:
			menu_actual_state();
			selected_menu = 1;
			break;
		case 1:
			char response = menu_operator_login();
			selected_menu = 1;
			if(response){
				menu_selection_operator();
			}else{
				clear_display();
				write_string_line(1, " Login Invalido");
				HAL_Delay(3000);
			}
			break;
		case 2:
			menu_temperature_selection();
			menu_main();
			select_params();
			selected_menu = 0;
			break;
		case 3:
			menu_plant_selection();
			menu_main();
			select_params();
			selected_menu = 0;
			break;
		case 4:
			menu_light();
			selected_menu = 3;
			break;
		case 5:
			menu_main();
			selected_menu = 0;
			break;
	}
}

void menu_selection_operator(void){
	selected_menu = 2;
	const char *options[] = {" Mudar Temp " "\xDF" "C", "  Mudar Planta", "      Sair"};
	char num_options = sizeof(options) / sizeof(options[0]);
	char option = navigate_options(options, num_options);
	switch(option){
		case 0:
			menu_temperature_selection();
			menu_main();
			select_params();
			selected_menu = 0;
			break;
		case 1:
			menu_plant_selection();
			menu_main();
			select_params();
			selected_menu = 0;
			break;
		case 2:
			menu_main();
			selected_menu = 0;
			break;
	}
}

void menu_temperature_selection(void){
	clear_display();
	write_string_line(1,"Digite o Valor:");
	char buffer[3];
	float temp = read_temperature_keypad(buffer);
	if(temp != -1.0){
		temperatura_limite = temp;
		clear_display();
		write_string_line(1, "Temp Selecionada");
		write_string_line(2, "      ");
		write_string_LCD(buffer);
		write_string_LCD("\xDF" "C");
		HAL_Delay(3000);
	}else{

	}

}

void menu_plant_selection(void){
	const char *options[] = {"     Alface", "    Pimentao", "    Morango"};
	char num_options = sizeof(options) / sizeof(options[0]);
	variedade = navigate_options(options, num_options);
	char buffer[16];
	get_name(variedade, buffer);
	clear_display();
	write_string_line(1, "   ");
	write_string_LCD(buffer);
	write_string_line(2, "  Selecionado");
	HAL_Delay(3000);
}

void get_name(char code, char* buffer) {
    switch(code) {
        case 0:
            strcpy(buffer, "Alface  ");
            break;
        case 1:
            strcpy(buffer, "Pimentao");
            break;
        case 2:
            strcpy(buffer, "Morango ");
            break;
        default:
            strcpy(buffer, "Unknown "); // Handle unexpected code values
            break;
    }
}

void get_day_night(char code, char* buffer) {
    switch(code) {
        case 0:
            strcpy(buffer, "Noite");
            break;
        case 1:
            strcpy(buffer, "Dia");
            break;
        default:
            strcpy(buffer, "Unknown"); // Handle unexpected code values
            break;
    }
}

void menu_main(void){
	char buffer [16];
	sprintf(buffer, "%.2f", temperatura_atual);  // Convert float to string with 2 decimal places
	clear_display();
	write_string_line(1,"   Smart-fARM");
	write_string_line(2,"    ");
	write_string_LCD(buffer);
	write_string_LCD("\xDF" "C");
}

void menu_actual_state(void){
	char buffer [16];
	sprintf(buffer, "%.2f", temperatura_atual);  // Convert float to string with 2 decimal places
	clear_display();
	write_string_line(1,"");
	write_string_LCD(buffer);
	write_string_LCD("\xDF" "C |TL:");
	sprintf(buffer, "%.0f", temperatura_limite);  // Convert float to string with 2 decimal places
	write_string_LCD(buffer);
	write_string_LCD("\xDF" "C");
	get_name(variedade,buffer);
	write_string_line(2,buffer);
	write_string_LCD("| ");
	get_day_night(flag_turno_dia,buffer);
	write_string_LCD(buffer);
}

void menu_light(void){
	// Mostra intensidade luminosa
	float light;
	char buffer[16];
	clear_display();
	write_string_line(1,"Interior : ");
	light = read_light_inside();
	sprintf(buffer, "%.2f", light);
	write_string_LCD(buffer);
	write_string_line(2,"Exterior : ");
	light = read_light_outside();
	sprintf(buffer, "%.2f", light);
	write_string_LCD(buffer);
}

char menu_operator_login(void){
	clear_display();
	write_string_line(1, " MODO OPERADOR");
	HAL_Delay(3000);

	char login[7]; // Ajuste o tamanho conforme necessário
	read_login(login);

	// Define the source string
	char login_line[16] = "User: ";
	// Concatenate the source string to the destination string
	strncat(login_line, login, 7);

	char pwd[7]; // Ajuste o tamanho conforme necessário
	read_pwd(login_line, pwd);

	char response;
	response = validate_user(login, pwd);
	return response;
}

// Função para validar credenciais de usuário
char validate_user(const char* login, const char* password) {
	if(login[0] != 0 && password[0] != 0){ // Garante que login e senha não estão vazios
		char user[] = "123456";
		char pwd[] = "123456";

		if (strcmp(user, login) == 0 && strcmp(pwd, password) == 0) {
			return 1; // Credenciais correspondem
		}else{
			return 0; // Credenciais não encontradas
		}
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
