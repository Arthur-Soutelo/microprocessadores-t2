#include "main.h"
extern ADC_HandleTypeDef hadc1;  // Declare the ADC handle

#ifndef INC_AMBIENT_H_
#define INC_AMBIENT_H_

	#define AVG_SLOPE (4.3F)
	#define VOLTAGE_AT_25C (1.43F)
	#define VOLTAGE_REF_INT (1.2F)

	float Read_Temperature(void);
	uint32_t read_adc_value(uint32_t channel);

	float read_light_outside(void);
	float read_light_inside(void);

	extern TIM_HandleTypeDef htim4;
	extern ADC_HandleTypeDef hadc1;

	#define LDR_DAY_THRESHOLD 0.60  // 60% of full scale
	#define LDR_NIGHT_THRESHOLD 0.30  // 30% of full scale

	#define LDR_MIN_THRESHOLD 0.60  // 60% of full scale
	#define LDR_MAX_THRESHOLD 0.70  // 70% of full scale
	#define ADC_FULL_SCALE    4095  // Assuming 12-bit ADCs

	#define PWM_STEP_SIZE      2  // Define a smaller step size for adjustments
	#define LDR_DEAD_ZONE      0.05  // Define uma zona morta de 5% em torno dos limites

//	#define PWM_ADJUSTMENT_STEP 2  // Tamanho do ajuste pequeno para verificar a leitura do LDR
//	#define ADJUSTMENT_DELAY   50  // Atraso para permitir a estabilização da leitura

	void Regulate_Light_Intensity(void);

	void Classify_Day_or_Night(volatile char *flag_turno_dia);

#endif /* INC_AMBIENT_H_ */
