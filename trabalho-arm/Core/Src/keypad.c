#include "main.h"
#include "keypad.h"

// Debounce function to check the key press stability
static uint8_t debounce(GPIO_TypeDef* row_port, uint16_t row_pin) {
    uint8_t count = 0;
    uint8_t keylast = 0;
    uint8_t keynow = 1;

    while (count < 7) {
        HAL_Delay(10); // Adjust debounce delay as needed
        keynow = HAL_GPIO_ReadPin(row_port, row_pin);
        if (keynow == keylast) {
            count++;
        } else {
            count = 0;
        }
        keylast = keynow;
    }
    return keynow;
}

void keypad_init(void) {
    // Set columns as outputs (low)
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = KEYPAD_COL1_Pin | KEYPAD_COL2_Pin | KEYPAD_COL3_Pin | KEYPAD_COL4_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(KEYPAD_COL1_GPIO_Port, &GPIO_InitStruct);

    HAL_GPIO_WritePin(KEYPAD_COL1_GPIO_Port, KEYPAD_COL1_Pin | KEYPAD_COL2_Pin | KEYPAD_COL3_Pin | KEYPAD_COL4_Pin, GPIO_PIN_RESET);

    // Set rows as inputs with pull-up resistors
    GPIO_InitStruct.Pin = KEYPAD_ROW1_Pin | KEYPAD_ROW2_Pin | KEYPAD_ROW3_Pin | KEYPAD_ROW4_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(KEYPAD_ROW1_GPIO_Port, &GPIO_InitStruct);
}

char keypad_getkey(void) {
    short row, col;
    const char keys[4][4] = {
        {'1', '2', '3', 'A'},
        {'4', '5', '6', 'B'},
        {'7', '8', '9', 'C'},
        {'*', '0', '#', 'D'}
    };

    for (col = 0; col < 4; col++) {
        // Set the current column to low
        HAL_GPIO_WritePin(KEYPAD_COL1_GPIO_Port, (1 << col) * KEYPAD_COL1_Pin, GPIO_PIN_RESET);

        for (row = 0; row < 4; row++) {
            if (!HAL_GPIO_ReadPin((row == 0) ? KEYPAD_ROW1_GPIO_Port :
                                  (row == 1) ? KEYPAD_ROW2_GPIO_Port :
                                  (row == 2) ? KEYPAD_ROW3_GPIO_Port : KEYPAD_ROW4_GPIO_Port,
                                  (row == 0) ? KEYPAD_ROW1_Pin :
                                  (row == 1) ? KEYPAD_ROW2_Pin :
                                  (row == 2) ? KEYPAD_ROW3_Pin : KEYPAD_ROW4_Pin)) {
                // Check if key is pressed and debounced
                if (debounce((row == 0) ? KEYPAD_ROW1_GPIO_Port :
                             (row == 1) ? KEYPAD_ROW2_GPIO_Port :
                             (row == 2) ? KEYPAD_ROW3_GPIO_Port : KEYPAD_ROW4_GPIO_Port,
                             (row == 0) ? KEYPAD_ROW1_Pin :
                             (row == 1) ? KEYPAD_ROW2_Pin :
                             (row == 2) ? KEYPAD_ROW3_Pin : KEYPAD_ROW4_Pin)) {
                    // Reset the column to high
                    HAL_GPIO_WritePin(KEYPAD_COL1_GPIO_Port, (1 << col) * KEYPAD_COL1_Pin, GPIO_PIN_SET);
                    return keys[row][col]; // Return the pressed key
                }
            }
        }

        // Reset the column to high
        HAL_GPIO_WritePin(KEYPAD_COL1_GPIO_Port, (1 << col) * KEYPAD_COL1_Pin, GPIO_PIN_SET);
    }

    return 0; // Return 0 if no key is pressed
}
