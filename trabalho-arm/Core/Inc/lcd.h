#ifndef LCD_H_
#define LCD_H_

	#define LCD_WIDTH  16

	void init_LCD(void);
	void write_command_LCD(uint8_t command);
	void write_data_LCD(uint8_t data);
	void write_string_LCD(const char *text);

	void clear_display(void);
	void write_string_line(char line, const char *text);
	char navigate_options(const char *options[], char num_options);

#endif // LCD_H_
