#include "main.h"
extern ADC_HandleTypeDef hadc1;  // Declare the ADC handle

#ifndef INC_AMBIENT_H_
#define INC_AMBIENT_H_

	#define AVG_SLOPE (4.3F)
	#define VOLTAGE_AT_25C (1.43F)
	#define VOLTAGE_REF_INT (1.2F)

	float Read_Temperature(void);
	uint32_t read_adc_value(uint32_t channel);

#endif /* INC_AMBIENT_H_ */
