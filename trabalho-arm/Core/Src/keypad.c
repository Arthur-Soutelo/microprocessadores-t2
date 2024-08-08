#include "keypad.h"

// Debounce function to check the key press stability
static uint8_t debounce(uint8_t row, uint8_t col) {
//    uint8_t count = 0;
//    uint8_t keylast = 0;
    uint8_t keynow = 1;
//
//    while (count < 7) {
//        HAL_Delay(10); // Adjust debounce delay as needed
        keynow = HAL_GPIO_ReadPin(KEYPAD_GPIO_PORT, row);
//        if (keynow == keylast) {
//            count++;
//        } else {
//            count = 0;
//        }
//        keylast = keynow;
//    }
    return keynow;
}

void keypad_init(void) {
    // Set columns as outputs (low)
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    GPIO_InitStruct.Pin = COL1_PIN | COL2_PIN | COL3_PIN | COL4_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(KEYPAD_GPIO_PORT, &GPIO_InitStruct);
    
    HAL_GPIO_WritePin(KEYPAD_GPIO_PORT, COL1_PIN | COL2_PIN | COL3_PIN | COL4_PIN, GPIO_PIN_RESET);
    
    // Set rows as inputs with pull-up resistors
    GPIO_InitStruct.Pin = ROW1_PIN | ROW2_PIN | ROW3_PIN | ROW4_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(KEYPAD_GPIO_PORT, &GPIO_InitStruct);
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
        HAL_GPIO_WritePin(KEYPAD_GPIO_PORT, (COL1_PIN << col), GPIO_PIN_RESET);
        
        for (row = 0; row < 4; row++) {
            if (!HAL_GPIO_ReadPin(KEYPAD_GPIO_PORT, (ROW1_PIN << row))) { // Check if the key is pressed
                if (debounce(ROW1_PIN << row, COL1_PIN << col)) { // Check if key is pressed and debounced
                    // Reset the column to high
                    HAL_GPIO_WritePin(KEYPAD_GPIO_PORT, (COL1_PIN << col), GPIO_PIN_SET);
                    return keys[row][col]; // Return the pressed key
                }
            }
        }
        
        // Reset the column to high
        HAL_GPIO_WritePin(KEYPAD_GPIO_PORT, (COL1_PIN << col), GPIO_PIN_SET);
    }
    
    return 0; // Return 0 if no key is pressed
}

