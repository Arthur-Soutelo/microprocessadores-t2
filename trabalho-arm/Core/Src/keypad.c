#include "main.h"
#include "keypad.h"

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
        HAL_Delay(10);  // Debounce delay
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
        // Set the current column to low
        HAL_GPIO_WritePin((col == 0) ? KEYPAD_COL1_GPIO_Port :
                          (col == 1) ? KEYPAD_COL2_GPIO_Port :
                          (col == 2) ? KEYPAD_COL3_GPIO_Port : KEYPAD_COL4_GPIO_Port,
                          (col == 0) ? KEYPAD_COL1_Pin :
                          (col == 1) ? KEYPAD_COL2_Pin :
                          (col == 2) ? KEYPAD_COL3_Pin : KEYPAD_COL4_Pin,
                          GPIO_PIN_RESET);

        for (row = 0; row < 4; row++) {
            if (!debounce(row, col)) {
                // Reset the column to high
                HAL_GPIO_WritePin((col == 0) ? KEYPAD_COL1_GPIO_Port :
                                  (col == 1) ? KEYPAD_COL2_GPIO_Port :
                                  (col == 2) ? KEYPAD_COL3_GPIO_Port : KEYPAD_COL4_GPIO_Port,
                                  (col == 0) ? KEYPAD_COL1_Pin :
                                  (col == 1) ? KEYPAD_COL2_Pin :
                                  (col == 2) ? KEYPAD_COL3_Pin : KEYPAD_COL4_Pin,
                                  GPIO_PIN_SET);
                return keys[row][col]; // Return the pressed key
            }
        }

        // Reset the column to high
        HAL_GPIO_WritePin((col == 0) ? KEYPAD_COL1_GPIO_Port :
                          (col == 1) ? KEYPAD_COL2_GPIO_Port :
                          (col == 2) ? KEYPAD_COL3_GPIO_Port : KEYPAD_COL4_GPIO_Port,
                          (col == 0) ? KEYPAD_COL1_Pin :
                          (col == 1) ? KEYPAD_COL2_Pin :
                          (col == 2) ? KEYPAD_COL3_Pin : KEYPAD_COL4_Pin,
                          GPIO_PIN_SET);
    }

    return 0; // Return 0 if no key is pressed
}
