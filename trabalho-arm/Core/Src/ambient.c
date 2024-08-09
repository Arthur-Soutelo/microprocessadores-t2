#include "ambient.h"


int Read_Temperature_INT(void) {
    uint32_t adcValue = 0;
    int temperature = 0;

    // Read ADC value from the temperature sensor
    adcValue = read_adc_value(ADC_CHANNEL_TEMPSENSOR);

    // Calculate temperature based on the calibration values
    int temp_range = TEMP110_CAL_VALUE - TEMP30_CAL_VALUE;
    int adc_range = TS_CAL2 - TS_CAL1;
    int temp_offset = TEMP30_CAL_VALUE;

    // Avoid floating-point operations by using integer arithmetic
    if (adc_range != 0) {
        temperature = temp_offset + (adcValue - TS_CAL1) * temp_range / adc_range;
    } else {
        temperature = -1;  // Return an error value if adc_range is 0
    }

    return temperature;
}

// Temperature read function using floating-point calculation
float Read_Temperature(void) {
    uint32_t adcValue = 0;
    float temperature = 0.0f;

    adcValue = read_adc_value(ADC_CHANNEL_TEMPSENSOR);

    // Calculate temperature based on the calibration values
    float temp_range = (float)(TEMP110_CAL_VALUE - TEMP30_CAL_VALUE);
    float adc_range = (float)(TS_CAL2 - TS_CAL1);
    float temp_offset = (float)TEMP30_CAL_VALUE;

    // Avoid floating-point operations by using integer arithmetic
    if (adc_range != 0) {
        temperature = temp_offset + (float)(adcValue - TS_CAL1) * temp_range / adc_range;
    } else {
        temperature = -1.0f;  // Return an error value if adc_range is 0
    }

    return temperature;
}


uint32_t read_adc_value(uint32_t channel) {
    ADC_ChannelConfTypeDef sConfig = {0};
    uint32_t adcValue = 0;

    // Configure the ADC channel
    sConfig.Channel = channel;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;  // Adjust as needed
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

