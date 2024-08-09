#include "main.h"
extern ADC_HandleTypeDef hadc1;  // Declare the ADC handle

#ifndef INC_AMBIENT_H_
#define INC_AMBIENT_H_

	#define VREFINT_CAL_ADDR ((uint16_t*)((uint32_t)0x1FFF7A2A))
	#define TS_CAL1_ADDR ((uint16_t*)((uint32_t)0x1FFF7A2C))  // Temp sensor calibration value at 30 °C
	#define TS_CAL2_ADDR ((uint16_t*)((uint32_t)0x1FFF7A2E))  // Temp sensor calibration value at 110 °C
	#define VREFINT_CAL (*(uint16_t*)VREFINT_CAL_ADDR)
	#define TS_CAL1 (*(uint16_t*)TS_CAL1_ADDR)
	#define TS_CAL2 (*(uint16_t*)TS_CAL2_ADDR)

	#define TEMP110_CAL_VALUE 110   // Temp at calibration point 2 (110 °C)
	#define TEMP30_CAL_VALUE 30     // Temp at calibration point 1 (30 °C)

	float Read_Temperature(void);
	uint32_t read_adc_value(uint32_t channel);
	int Read_Temperature_INT(void);

#endif /* INC_AMBIENT_H_ */
