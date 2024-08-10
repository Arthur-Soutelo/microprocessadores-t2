#include "main.h"
#include "keypad.h"
#include "lcd.h"

// Debounce function to check the key press stability
static unsigned char debounce(unsigned char row, unsigned char col) {
    unsigned char count = 0;
    unsigned char keylast = 1;  // Assume key is not pressed initially
    unsigned char keynow = 1;
    GPIO_TypeDef* row_port;
    uint16_t row_pin;

    // Map row to its GPIO port and pin
    switch(row) {
        case 0:
            row_port = KEYPAD_ROW1_GPIO_Port;
            row_pin = KEYPAD_ROW1_Pin;
            break;
        case 1:
            row_port = KEYPAD_ROW2_GPIO_Port;
            row_pin = KEYPAD_ROW2_Pin;
            break;
        case 2:
            row_port = KEYPAD_ROW3_GPIO_Port;
            row_pin = KEYPAD_ROW3_Pin;
            break;
        case 3:
            row_port = KEYPAD_ROW4_GPIO_Port;
            row_pin = KEYPAD_ROW4_Pin;
            break;
        default:
            return 1;  // Return '1' if invalid row
    }

    while (count < 7) {
        HAL_Delay(1);  // Debounce delay
        keynow = HAL_GPIO_ReadPin(row_port, row_pin);

        if (keynow == keylast) {
            count++;
        } else {
            count = 0;
        }
        keylast = keynow;
    }
    return keynow;  // Return '0' if key is pressed, '1' if not pressed
}

void keypad_init(void) {
    // Initialize all columns to high
    HAL_GPIO_WritePin(KEYPAD_COL1_GPIO_Port, KEYPAD_COL1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(KEYPAD_COL2_GPIO_Port, KEYPAD_COL2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(KEYPAD_COL3_GPIO_Port, KEYPAD_COL3_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(KEYPAD_COL4_GPIO_Port, KEYPAD_COL4_Pin, GPIO_PIN_SET);
}

char keypad_getkey(void) {
    unsigned char row, col;
    const char keys[4][4] = {
        {'1', '2', '3', 'A'},
        {'4', '5', '6', 'B'},
        {'7', '8', '9', 'C'},
        {'*', '0', '#', 'D'}
    };

    for (col = 0; col < 4; col++) {
        GPIO_TypeDef* col_port;
        uint16_t col_pin;

        // Set the current column to low
        switch (col) {
            case 0:
                col_port = KEYPAD_COL1_GPIO_Port;
                col_pin = KEYPAD_COL1_Pin;
                break;
            case 1:
                col_port = KEYPAD_COL2_GPIO_Port;
                col_pin = KEYPAD_COL2_Pin;
                break;
            case 2:
                col_port = KEYPAD_COL3_GPIO_Port;
                col_pin = KEYPAD_COL3_Pin;
                break;
            case 3:
                col_port = KEYPAD_COL4_GPIO_Port;
                col_pin = KEYPAD_COL4_Pin;
                break;
            default:
                col_port = NULL;  // Invalid column
                col_pin = 0;
                break;
        }

        HAL_GPIO_WritePin(col_port, col_pin, GPIO_PIN_RESET);

        for (row = 0; row < 4; row++) {

            if (!debounce(row, col)) {
                // Reset the column to high
                HAL_GPIO_WritePin(col_port, col_pin, GPIO_PIN_SET);
                return keys[row][col]; // Return the pressed key
            }
        }

        // Reset the column to high
        HAL_GPIO_WritePin(col_port, col_pin, GPIO_PIN_SET);
    }

    return 0; // Return 0 if no key is pressed
}

float read_temperature_keypad(char *buffer) {
	char key;
	short index = 0;
	float temperature_value;

	// Initialize buffer
	memset(buffer, 0, 2 + 1);

	while (index < 2) {
		key = keypad_getkey();
		if (key != 0) { // Check if a key is pressed
			if (key >= '0' && key <= '9') { // Check if the key is a digit
				buffer[index++] = key; // Store the digit in the buffer
				//write_data_LCD(key);
				clear_display();
				write_string_line(1,"Digite o Valor:");
				write_string_line(2,buffer);
				} else if (key == '#') { // Use '#' as an enter key
				break; // Exit loop when '#' is pressed
				} else if (key == '*') { // Use '*' to cancel input
				// Optionally, clear the buffer
				memset(buffer, 0, 2 + 1);
				index = 0; // Reset index
				clear_display();
				write_string_line(1,"Digite o Valor:");
				write_string_line(2,buffer);
			}
			// Add a small delay to debounce
			HAL_Delay(100);
		}
	}
	buffer[index] = '\0'; // Null-terminate the card number

	temperature_value = atof(buffer);
	if (temperature_value >= 19.0 && temperature_value <= 32.0) {
		return temperature_value;
		} else {
		clear_display();
		write_string_line(1,"Valor Invalido!");
		write_string_line(2," 19 < Temp < 32");
		HAL_Delay(2000); // Exibe a mensagem de erro por 2 segundos
		return -1.0; // Envia erro
	}

}

// Function to read login number from the keypad
void read_login(char *login_number) {
    char key;
    char index_login = 0;

    clear_display();
    write_string_line(1, "User: ");
    write_string_line(2, "Pwd : ");

    // Initialize login buffer
    memset(login_number, 0, 6 + 1);

    while (index_login < 6) {
        key = keypad_getkey();
        if (key != 0) { // Check if a key is pressed
            if (key >= '0' && key <= '9') { // Check if the key is a digit
                login_number[index_login++] = key; // Store the digit in the card_number buffer
                clear_display();
                write_string_line(1, "User: ");
                write_string_LCD(login_number);
                write_string_line(2, "Pwd : ");
            } else if (key == '#') { // Use '#' as an enter key
                break; // Exit loop when '#' is pressed
            } else if (key == '*') { // Use '*' to cancel input
                // Optionally, clear the card_number buffer
                memset(login_number, 0, 6 + 1);
                index_login = 0; // Reset index
                clear_display();
                write_string_line(1, "User: ");
                write_string_LCD(login_number);
                write_string_line(2, "Pwd : ");
            }
            // Add a small delay to debounce
            HAL_Delay(100); // Adjust delay as needed
        }
    }
    login_number[index_login] = '\0'; // Null-terminate the card number
}

// Function to read a card number from the keypad
void read_pwd(const char *login_line, char *pwd) {
    char key;
    char index_pwd = 0;

    clear_display();
    write_string_line(1, login_line);
    write_string_line(2, "Pwd : ");

    // Initialize pwd buffer
    memset(pwd, 0, 6 + 1);

    while (index_pwd < 6) {
        key = keypad_getkey();
        if (key != 0) { // Check if a key is pressed
            if (key >= '0' && key <= '9') { // Check if the key is a digit
                pwd[index_pwd++] = key; // Store the digit in the card_number buffer
                clear_display();
                write_string_line(1, login_line);
                write_string_line(2, "Pwd : ");
                for (char i = 0; i < index_pwd; i++) {
                    write_string_LCD("*"); // Mask the password input
                }
            } else if (key == '#') { // Use '#' as an enter key
                break; // Exit loop when '#' is pressed
            } else if (key == '*') { // Use '*' to cancel input
                // Optionally, clear the card_number buffer
                memset(pwd, 0, 6 + 1);
                index_pwd = 0; // Reset index
                clear_display();
                write_string_line(1, login_line);
                write_string_line(2, "Pwd : ");
                write_string_LCD(pwd);
            }
            HAL_Delay(100); // Adjust delay as needed
        }
    }
    pwd[index_pwd] = '\0'; // Null-terminate the card number
}

