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
    // Step 1: Calculate the LDR percentage
    float ldr_percentage = read_light_inside();

    // Get the current PWM duty cycle
    uint32_t current_compare = __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_4);

    // Step 2: Adjust the PWM duty cycle based on LDR percentage
    if (ldr_percentage < LDR_MIN_THRESHOLD - (PWM_DEAD_ZONE / 100.0f)) {
        // Increase light intensity (increase PWM duty cycle) if it's below the threshold
        if (current_compare < (htim4.Init.Period - PWM_STEP_SIZE)) {
            __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, current_compare + PWM_STEP_SIZE);
        }
    } else if (ldr_percentage > LDR_MAX_THRESHOLD + (PWM_DEAD_ZONE / 100.0f)) {
        // Decrease light intensity (decrease PWM duty cycle) if it's above the threshold
        if (current_compare > PWM_STEP_SIZE) {
            __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, current_compare - PWM_STEP_SIZE);
        }
    }

    // Ensure PWM duty cycle stays within valid range (0 to ARR)
    if (__HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_4) > htim4.Init.Period) {
        __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, htim4.Init.Period);
    } else if (__HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_4) < 0) {
        __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, 0);
    }
}

