#include "ambient.h"

// Temperature read function using floating-point calculation
float Read_Temperature(void) {
    uint32_t adcValue = 0;

    adcValue = read_adc_value(ADC_CHANNEL_TEMPSENSOR);

    // Convert ADC value to voltage
	float adcVoltage = (adcValue / 4095.0) * 3.3; // Assuming 12-bit resolution and 3.3V reference

	// Temperature calculation
	// Temperature = (Vadc - V25) / Slope + T25
	// T25 = 25°C (reference temperature)

	float temperature = ((adcVoltage - VOLTAGE_AT_25C) / AVG_SLOPE) + 25;

    return temperature;
}


uint32_t read_adc_value(uint32_t channel) {
    ADC_ChannelConfTypeDef sConfig = {0};
    uint32_t adcValue = 0;

    // Configure the ADC channel
    sConfig.Channel = channel;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;  // Adjust as needed
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        // Handle error
        return -1;
    }

    // Start the ADC conversion
    HAL_ADC_Start(&hadc1);

    // Poll for conversion completion
    if (HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) == HAL_OK) {
        // Get the ADC value
        adcValue = HAL_ADC_GetValue(&hadc1);
    }

    // Stop the ADC
    HAL_ADC_Stop(&hadc1);

    return adcValue;
}

float read_light_outside(void) {
    uint32_t adcValue = 0;

    // Read LDR1 value from PA0 (ADC1_IN0)
    adcValue = read_adc_value(ADC_CHANNEL_0);

   // Convert ADC value to a percentage of light intensity
   float lightIntensity = (float)adcValue / ADC_FULL_SCALE;

   // Invert the value to reflect higher ADC values as lower light intensity
   return 1.0f - lightIntensity;
}

float read_light_inside(void) {
    uint32_t adcValue = 0;

    // Read LDR1 value from PA0 (ADC1_IN0)
    adcValue = read_adc_value(ADC_CHANNEL_1);

   // Convert ADC value to a percentage of light intensity
   float lightIntensity = (float)adcValue / ADC_FULL_SCALE;

   // Invert the value to reflect higher ADC values as lower light intensity
   return 1.0f - lightIntensity;
}


void Regulate_Light_Intensity(void) {
    // Passo 1: Calcular a porcentagem do LDR
    float ldr_percentage = read_light_inside();

    // Obter o ciclo de trabalho atual do PWM
    uint32_t current_compare = __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_4);

    if (ldr_percentage < LDR_MIN_THRESHOLD - LDR_DEAD_ZONE) {
        // Aumentar a intensidade da luz (aumentar o ciclo de trabalho do PWM) se estiver abaixo do limite
        if (current_compare < (htim4.Init.Period - PWM_STEP_SIZE)) {
            __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, current_compare + PWM_STEP_SIZE);
        }
    } else if (ldr_percentage > LDR_MAX_THRESHOLD + LDR_DEAD_ZONE) {
        // Diminuir a intensidade da luz (diminuir o ciclo de trabalho do PWM) se estiver acima do limite
    	if (current_compare > PWM_STEP_SIZE) {
    	    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, current_compare - PWM_STEP_SIZE);
    	} else {
    	    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, 0);
    	}
    } else {
        // Se a leitura do LDR estiver dentro da faixa desejada, verifique a resposta do ajuste
        // Faça um ajuste pequeno para verificar a resposta
		uint32_t adjusted_compare = (current_compare > PWM_ADJUSTMENT_STEP) ?
									current_compare - PWM_ADJUSTMENT_STEP : 0;
		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, adjusted_compare);
		HAL_Delay(ADJUSTMENT_DELAY);  // Aguarde um curto período para estabilizar a leitura

        // Leia o LDR após o ajuste
        float new_ldr_percentage = read_light_inside();

        // Se o LDR ainda estiver dentro da faixa, considere o ajuste como adequado
        if (new_ldr_percentage >= ldr_percentage - LDR_DEAD_ZONE &&
            new_ldr_percentage <= ldr_percentage + LDR_DEAD_ZONE) {
            // Se o LDR não mudou significativamente, o ajuste foi adequado
        } else {
            // Se o LDR mudou significativamente, ajuste o PWM de volta ao valor anterior
            __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, current_compare);
        }
    }

    // Garantir que o ciclo de trabalho do PWM permaneça dentro da faixa válida (0 a ARR)
	uint32_t final_compare = __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_4);
	if (final_compare > htim4.Init.Period) {
		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, htim4.Init.Period);
	} else if (final_compare < 0) {
		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, 0);
	}

}

void Classify_Day_or_Night(char *flag_turno_dia) {
    // Passo 1: Calcular a porcentagem do LDR
    float ldr_percentage = read_light_outside();

    // Verifica se é NOITE
    if (ldr_percentage < LDR_NIGHT_THRESHOLD - LDR_DEAD_ZONE) {
    	flag_turno_dia = 0;		// 1 = Dia, 0 = Noite
    // Verifica se é DIA
    } else if (ldr_percentage > LDR_DAY_THRESHOLD + LDR_DEAD_ZONE) {
    	flag_turno_dia = 1;		// 1 = Dia, 0 = Noite
    } else {

    }
}
