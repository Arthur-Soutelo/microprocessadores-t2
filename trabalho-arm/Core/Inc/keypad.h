#ifndef KEYPAD_H
#define KEYPAD_H

	// Function prototypes
	void keypad_init(void);
	char keypad_getkey(void);

	float read_temperature_keypad(char *buffer);

	void read_login(char *login_number);
	void read_pwd(const char *login_line, char *pwd);

#endif // KEYPAD_H
