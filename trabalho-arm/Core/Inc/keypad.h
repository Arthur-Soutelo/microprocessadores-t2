#ifndef KEYPAD_H
#define KEYPAD_H

	// Function prototypes
	void keypad_init(void);
	char keypad_getkey(void);

	float read_temperature_keypad(char *buffer);

#endif // KEYPAD_H
