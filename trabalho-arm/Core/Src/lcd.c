#include "main.h"
#include "lcd.h"
#include "keypad.h"

// Function to send a command to the LCD
void write_command_LCD(uint8_t command) {
    // Send the higher nibble
    HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_E_GPIO_Port, LCD_E_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LCD_D4_GPIO_Port, LCD_D4_Pin, (command & 0x10) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D5_GPIO_Port, LCD_D5_Pin, (command & 0x20) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D6_GPIO_Port, LCD_D6_Pin, (command & 0x40) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D7_GPIO_Port, LCD_D7_Pin, (command & 0x80) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(LCD_E_GPIO_Port, LCD_E_Pin, GPIO_PIN_RESET);
    HAL_Delay(1);

    // Send the lower nibble
    HAL_GPIO_WritePin(LCD_E_GPIO_Port, LCD_E_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LCD_D4_GPIO_Port, LCD_D4_Pin, (command & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D5_GPIO_Port, LCD_D5_Pin, (command & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D6_GPIO_Port, LCD_D6_Pin, (command & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D7_GPIO_Port, LCD_D7_Pin, (command & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(LCD_E_GPIO_Port, LCD_E_Pin, GPIO_PIN_RESET);
    HAL_Delay(1);
}

// Function to send data to the LCD
void write_data_LCD(uint8_t data) {
    // Send the higher nibble
    HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LCD_E_GPIO_Port, LCD_E_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LCD_D4_GPIO_Port, LCD_D4_Pin, (data & 0x10) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D5_GPIO_Port, LCD_D5_Pin, (data & 0x20) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D6_GPIO_Port, LCD_D6_Pin, (data & 0x40) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D7_GPIO_Port, LCD_D7_Pin, (data & 0x80) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(LCD_E_GPIO_Port, LCD_E_Pin, GPIO_PIN_RESET);
    HAL_Delay(1);

    // Send the lower nibble
    HAL_GPIO_WritePin(LCD_E_GPIO_Port, LCD_E_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LCD_D4_GPIO_Port, LCD_D4_Pin, (data & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D5_GPIO_Port, LCD_D5_Pin, (data & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D6_GPIO_Port, LCD_D6_Pin, (data & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D7_GPIO_Port, LCD_D7_Pin, (data & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(LCD_E_GPIO_Port, LCD_E_Pin, GPIO_PIN_RESET);
    HAL_Delay(1);
}

// Function to initialize the LCD
void init_LCD(void) {
    // Initialize the LCD
    HAL_Delay(50);
    write_command_LCD(0x33);
    write_command_LCD(0x32);
    write_command_LCD(0x28);
    write_command_LCD(0x0C);
    write_command_LCD(0x06);
    write_command_LCD(0x01);
    HAL_Delay(2);
}

// Function to write a string to the LCD
void write_string_LCD(const char *text) {
    while (*text) {
        write_data_LCD(*text++);
    }
}

void write_string_line(char line, const char *text) {
    if (line == 1) {
        write_command_LCD(0x80); // Set cursor to start of line 1
    } else if (line == 2) {
        write_command_LCD(0xC0); // Set cursor to start of line 2
    }
    write_string_LCD(text);
}

void clear_display(void) {
    write_command_LCD(0x01); // Clear display
    HAL_Delay(2);
}

// Function to display options and navigate between them
char navigate_options(const char *options[], char num_options) {
    short current_option = 0;
    clear_display();
    write_string_line(1, options[current_option]);
    write_string_line(2, "<-B  A->   [#OK]");
    char key;
    while (1) {
        key = keypad_getkey(); // Implement keypad_getkey for ARM
        if (key != 0) {
            switch (key) {
                case 'B':
                    current_option = (current_option - 1 + num_options) % num_options;
                    clear_display();
                    write_string_line(1, options[current_option]);
                    write_string_line(2, "<-B  A->   [#OK]");
                    break;
                case 'A':
                    current_option = (current_option + 1) % num_options;
                    clear_display();
                    write_string_line(1, options[current_option]);
                    write_string_line(2, "<-B  A->   [#OK]");
                    break;
                case '#':
                    return current_option;
                default:
                    break;
            }
        }
    }
}
