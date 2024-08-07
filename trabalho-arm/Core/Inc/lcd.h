#ifndef LCD_H_
#define LCD_H_

	#include "stm32f1xx_hal.h"

	// Define the pins for the LCD
	#define RS_PIN     GPIO_PIN_8
	#define E_PIN      GPIO_PIN_9

	#define D4_PIN     GPIO_PIN_10
	#define D5_PIN     GPIO_PIN_11
	#define D6_PIN     GPIO_PIN_12
	#define D7_PIN     GPIO_PIN_13

	#define LCD_PORT   GPIOA
	#define LCD_WIDTH  16

	void init_LCD(void);
	void write_command_LCD(uint8_t command);
	void write_data_LCD(uint8_t data);
	void write_string_LCD(const char *text);

	void clear_display(void);
	void write_string_line(char line, const char *text);
	char navigate_options(const char *options[], char num_options);

#endif // LCD_H_
