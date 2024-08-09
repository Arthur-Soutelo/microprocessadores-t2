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

    return adcValue;
}

float read_light_inside(void) {
    uint32_t adcValue = 0;

    // Read LDR1 value from PA0 (ADC1_IN0)
    adcValue = read_adc_value(ADC_CHANNEL_1);

    return adcValue;
}
