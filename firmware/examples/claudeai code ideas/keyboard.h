// keyboard.h - AmberClick90 Keyboard Firmware
// Main keyboard logic and key processing

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include "config.h"

void keyboard_init(void);
void keyboard_task(void);
void process_key_press(uint8_t row, uint8_t col);
void process_key_release(uint8_t row, uint8_t col);

#endif // KEYBOARD_H

// ============================================================================
// keyboard.c
// ============================================================================

#include "keyboard.h"
#include "matrix.h"
#include "layer.h"
#include "keymap.h"
#include "keycode.h"
#include "macro.h"
#include "led.h"
#include "encoder.h"
#include "usb_hid.h"
#include <avr/wdt.h>
#include <avr/interrupt.h>

// Track which keys are currently pressed with their keycodes
typedef struct {
        uint8_t row;
            uint8_t col;
                uint16_t keycode;
} pressed_key_t;

static pressed_key_t pressed_keys[MATRIX_KEYS];
static uint8_t pressed_key_count = 0;

void keyboard_init(void) {
        // Disable watchdog if enabled by bootloader
            wdt_disable();
                
                    // Initialize all subsystems
                        layer_init();
                            matrix_init();
                                macro_init();
                                    led_init();
                                        encoder_init();
                                            usb_init();
                                                
                                                    // Clear pressed keys tracking
                                                        pressed_key_count = 0;
                                                            for (uint8_t i = 0; i < MATRIX_KEYS; i++) {
                                                                        pressed_keys[i].row = 0xFF;
                                                                                pressed_keys[i].col = 0xFF;
                                                                                        pressed_keys[i].keycode = KC_NO;
                                                            }
                                                                
                                                                    // Enable interrupts
                                                                        sei();
}

void keyboard_task(void) {
        // Scan matrix
            matrix_scan();
                
                    // Process all key state changes
                        for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
                                    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
                                                    if (matrix_is_key_pressed(row, col)) {
                                                                        process_key_press(row, col);
                                                    }
                                                                else if (matrix_is_key_released(row, col)) {
                                                                                    process_key_release(row, col);
                                                                }
                                    }
                        }
                            
                                // Update encoder
                                    encoder_read();
                                        encoder_task();
                                            
                                                // Update LEDs based on current layer
                                                    static uint8_t last_layer = 0xFF;
                                                        if (current_layer != last_layer) {
                                                                    led_update_for_layer(current_layer);
                                                                            last_layer = current_layer;
                                                        }
                                                            
                                                                // USB task
                                                                    usb_task();
}

void process_key_press(uint8_t row, uint8_t col) {
        // Get keycode from current active layer
            uint16_t keycode = keymap_key_to_keycode(current_layer, row, col);
                
                    // Skip if no key or transparent (shouldn't happen after lookup)
                        if (keycode == KC_NO || keycode == KC_TRNS) {
                                    return;
                        }
                            
                                // Handle layer keys
                                    if (IS_LAYER_KEY(keycode)) {
                                                uint8_t target_layer = GET_LAYER(keycode);
                                                        layer_key_pressed(row, col, target_layer);
                                                                return;
                                    }
                                        
                                            // Handle LED keys
                                                if (IS_LED_KEY(keycode)) {
                                                            switch(keycode) {
                                                                            case LD_BRIU:
                                                                                            led_brightness_increase();
                                                                                                            break;
                                                                                                                        case LD_BRID:
                                                                                                                                        led_brightness_decrease();
                                                                                                                                                        break;
                                                            }
                                                                    return;
                                                }
                                                    
                                                        // Handle system keys
                                                            if (IS_SYSTEM_KEY(keycode)) {
                                                                        switch(keycode) {
                                                                                        case SYS_RST:
                                                                                                        // Software reset
                                                                                                                        wdt_enable(WDTO_15MS);
                                                                                                                                        while(1);
                                                                                                                                                        break;
                                                                                                                                                                        
                                                                                                                                                                                    case SYS_BOOT:
                                                                                                                                                                                                    // Enter bootloader
                                                                                                                                                                                                                    // Implementation depends on bootloader
                                                                                                                                                                                                                                    // Typically involves writing a magic value to RAM
                                                                                                                                                                                                                                                    // and triggering a watchdog reset
                                                                                                                                                                                                                                                                    // TODO: Implement bootloader entry
                                                                                                                                                                                                                                                                                    break;
                                                                        }
                                                                                return;
                                                            }
                                                                
                                                                    // Handle macros - execute immediately, no tracking needed
                                                                        if (IS_MACRO_KEY(keycode)) {
                                                                                    execute_macro(keycode);
                                                                                            return;
                                                                        }
                                                                            
                                                                                // Handle consumer keys
                                                                                    if (IS_CONSUMER_KEY(keycode)) {
                                                                                                send_consumer_report(keycode);
                                                                                                        
                                                                                                                // Track this key for release
                                                                                                                        if (pressed_key_count < MATRIX_KEYS) {
                                                                                                                                        pressed_keys[pressed_key_count].row = row;
                                                                                                                                                    pressed_keys[pressed_key_count].col = col;
                                                                                                                                                                pressed_keys[pressed_key_count].keycode = keycode;
                                                                                                                                                                            pressed_key_count++;
                                                                                                                        }
                                                                                                                                return;
                                                                                    }
                                                                                        
                                                                                            // Handle basic HID keys
                                                                                                if (IS_BASIC_KEY(keycode)) {
                                                                                                            add_key_to_report(keycode);
                                                                                                                    send_keyboard_report();
                                                                                                                            
                                                                                                                                    // Track this key for release
                                                                                                                                            if (pressed_key_count < MATRIX_KEYS) {
                                                                                                                                                            pressed_keys[pressed_key_count].row = row;
                                                                                                                                                                        pressed_keys[pressed_key_count].col = col;
                                                                                                                                                                                    pressed_keys[pressed_key_count].keycode = keycode;
                                                                                                                                                                                                pressed_key_count++;
                                                                                                                                            }
                                                                                                                                                    return;
                                                                                                }
}

void process_key_release(uint8_t row, uint8_t col) {
        // Check if this was a layer key
            layer_key_released(row, col);
                
                    // Macros don't need release handling - they execute immediately
                        
                            // Find this key in pressed_keys array and handle release
                                for (uint8_t i = 0; i < pressed_key_count; i++) {
                                            if (pressed_keys[i].row == row && pressed_keys[i].col == col) {
                                                            uint16_t keycode = pressed_keys[i].keycode;
                                                                        
                                                                                    // Handle consumer keys
                                                                                                if (IS_CONSUMER_KEY(keycode)) {
                                                                                                                    clear_consumer_report();
                                                                                                                                    send_consumer_report(KC_NO);
                                                               T                                 }
                                                                                                            // Handle basic keys
                                                                                                                        else if (IS_BASIC_KEY(keycode)) {
                                                                                                                                            remove_key_from_report(keycode);
                                                                                                                                                            send_keyboard_report();
                                                                                                                        }
                                                                                                                                    
                                                                                                                                                // Remove from array
                                                                                                                                                            for (uint8_t j = i; j < pressed_key_count - 1; j++) {
                                                                                                                                                                                pressed_keys[j] = pressed_keys[j + 1];
                                                                                                                                                            }
                                                                                                                                                                        pressed_key_count--;
                                                                                                                                                                                    pressed_keys[pressed_key_count].row = 0xFF;
                                                                                                                                                                                                pressed_keys[pressed_key_count].col = 0xFF;
                                                                                                                                                                                                            pressed_keys[pressed_key_count].keycode = KC_NO;
                                                                                                                                                                                                                        return;
                                            }
                                }
}
                                                                                                                                                            }
                                                                                                                        }
                                                                                                }
                                            }
                                }
}
                                                                                                                                            }
                                                                                                }
                                                                                                                        }
                                                                                    }
                                                                        }
                                                                        }
                                                            }
                                                            }
                                                }
                                    }
                        }
}
                                                        }
                                                                }
                                                    }
                                    }
                        }
}
                                                            }
}
}