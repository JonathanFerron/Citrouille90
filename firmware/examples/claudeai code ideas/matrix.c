// matrix.h - AmberClick90 Keyboard Firmware
// Matrix scanning and debouncing

#ifndef MATRIX_H
#define MATRIX_H

#include <stdint.h>
#include <stdbool.h>
#include "config.h"

// Matrix state tracking
extern bool matrix_current[MATRIX_ROWS][MATRIX_COLS];
extern bool matrix_previous[MATRIX_ROWS][MATRIX_COLS];

void matrix_init(void);
void matrix_scan(void);
bool matrix_is_pressed(uint8_t row, uint8_t col);
bool matrix_is_key_pressed(uint8_t row, uint8_t col);
bool matrix_is_key_released(uint8_t row, uint8_t col);

#endif // MATRIX_H

// ============================================================================
// matrix.c
// ============================================================================

#include "matrix.h"
#include <avr/io.h>
#include <util/delay.h>

// Matrix state
bool matrix_current[MATRIX_ROWS][MATRIX_COLS];
bool matrix_previous[MATRIX_ROWS][MATRIX_COLS];

// Debouncing state
static uint8_t debounce_counters[MATRIX_ROWS][MATRIX_COLS];
static bool debounced_state[MATRIX_ROWS][MATRIX_COLS];

// Row pins array for easy iteration
static const uint8_t row_pins[MATRIX_ROWS] = {
    ROW0_PIN, ROW1_PIN, ROW2_PIN, ROW3_PIN, ROW4_PIN,
    ROW5_PIN, ROW6_PIN, ROW7_PIN, ROW8_PIN, ROW9_PIN
};

// Column pins array
static const uint8_t col_pins[MATRIX_COLS] = {
    COL0_PIN, COL1_PIN, COL2_PIN, COL3_PIN, COL4_PIN,
    COL5_PIN, COL6_PIN, COL7_PIN, COL8_PIN
};

// Helper functions to abstract pin operations
static inline void set_pin_output(uint8_t pin) {
    // AVR64DU32 uses PORTx.DIRSET to set pin as output
    volatile uint8_t *port_dir = (volatile uint8_t *)((pin & 0xF0) + 0x00);
    *port_dir |= (1 << (pin & 0x0F));
}

static inline void set_pin_input_pullup(uint8_t pin) {
    // Set as input with pullup
    volatile uint8_t *port_dir = (volatile uint8_t *)((pin & 0xF0) + 0x00);
    volatile uint8_t *port_pinctrl = (volatile uint8_t *)((pin & 0xF0) + 0x10 + (pin & 0x0F));
    
    *port_dir &= ~(1 << (pin & 0x0F));  // Input
    *port_pinctrl |= PORT_PULLUPEN_bm;  // Enable pullup
}

static inline void write_pin_high(uint8_t pin) {
    volatile uint8_t *port_outset = (volatile uint8_t *)((pin & 0xF0) + 0x05);
    *port_outset = (1 << (pin & 0x0F));
}

static inline void write_pin_low(uint8_t pin) {
    volatile uint8_t *port_outclr = (volatile uint8_t *)((pin & 0xF0) + 0x06);
    *port_outclr = (1 << (pin & 0x0F));
}

static inline bool read_pin(uint8_t pin) {
    volatile uint8_t *port_in = (volatile uint8_t *)((pin & 0xF0) + 0x08);
    return (*port_in & (1 << (pin & 0x0F))) == 0;  // Active low (pullup)
}

void matrix_init(void) {
    // Initialize all row pins as outputs, set high
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        set_pin_output(row_pins[row]);
        write_pin_high(row_pins[row]);
    }
    
    // Initialize all column pins as inputs with pullups
    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
        set_pin_input_pullup(col_pins[col]);
    }
    
    // Initialize state arrays
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            matrix_current[row][col] = false;
            matrix_previous[row][col] = false;
            debounce_counters[row][col] = 0;
            debounced_state[row][col] = false;
        }
    }
}

void matrix_scan(void) {
    // Save previous state
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            matrix_previous[row][col] = matrix_current[row][col];
        }
    }
    
    // Scan matrix
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        // Set current row low
        write_pin_low(row_pins[row]);
        _delay_us(1);  // Brief settling time
        
        // Read all columns
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            bool is_pressed = read_pin(col_pins[col]);
            
            // Debouncing logic (symmetric eager debouncing)
            if (is_pressed != debounced_state[row][col]) {
                debounce_counters[row][col]++;
                if (debounce_counters[row][col] >= DEBOUNCE_MS) {
                    debounced_state[row][col] = is_pressed;
                    matrix_current[row][col] = is_pressed;
                    debounce_counters[row][col] = 0;
                }
            } else {
                debounce_counters[row][col] = 0;
                matrix_current[row][col] = debounced_state[row][col];
            }
        }
        
        // Set row back high
        write_pin_high(row_pins[row]);
    }
}

bool matrix_is_pressed(uint8_t row, uint8_t col) {
    if (row >= MATRIX_ROWS || col >= MATRIX_COLS) return false;
    return matrix_current[row][col];
}

// Returns true if key was just pressed (rising edge)
bool matrix_is_key_pressed(uint8_t row, uint8_t col) {
    if (row >= MATRIX_ROWS || col >= MATRIX_COLS) return false;
    return matrix_current[row][col] && !matrix_previous[row][col];
}

// Returns true if key was just released (falling edge)
bool matrix_is_key_released(uint8_t row, uint8_t col) {
    if (row >= MATRIX_ROWS || col >= MATRIX_COLS) return false;
    return !matrix_current[row][col] && matrix_previous[row][col];
}