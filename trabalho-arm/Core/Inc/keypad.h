#ifndef KEYPAD_H
#define KEYPAD_H

	#include "stm32f1xx_hal.h"

	// Define the GPIO port and pins for the keypad
	#define COL1_PIN GPIO_PIN_1
	#define COL2_PIN GPIO_PIN_2
	#define COL3_PIN GPIO_PIN_10
	#define COL4_PIN GPIO_PIN_11
	#define ROW1_PIN GPIO_PIN_12
	#define ROW2_PIN GPIO_PIN_13
	#define ROW3_PIN GPIO_PIN_14
	#define ROW4_PIN GPIO_PIN_15

	#define KEYPAD_GPIO_PORT GPIOB


	// Function prototypes
	void keypad_init(void);
	char keypad_getkey(void);

#endif // KEYPAD_H
