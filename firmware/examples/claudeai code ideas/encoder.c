// encoder.h - AmberClick90 Keyboard Firmware
// Rotary encoder with Alt-Tab functionality

#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>
#include <stdbool.h>
#include "config.h"

void encoder_init(void);
void encoder_read(void);
void encoder_task(void);

#endif // ENCODER_H

// ============================================================================
// encoder.c
// ============================================================================

#include "encoder.h"
#include "keycode.h"
#include "usb_hid.h"
#include <avr/io.h>
#include <util/delay.h>

// Encoder state
static uint8_t encoder_state = 0;
static int8_t encoder_delta = 0;

// Alt-Tab state machine
static bool alt_held = false;
static uint32_t alt_press_time = 0;

// Helper function to get current time in ms (needs timer implementation)
extern uint32_t millis(void);

static inline bool read_encoder_a(void) {
    return (PORTD.IN & (1 << (ENCODER_PIN_A & 0x0F))) != 0;
}

static inline bool read_encoder_b(void) {
    return (PORTD.IN & (1 << (ENCODER_PIN_B & 0x0F))) != 0;
}

void encoder_init(void) {
    // Configure encoder pins as inputs with pullups
    PORTD.DIRCLR = (1 << (ENCODER_PIN_A & 0x0F)) | (1 << (ENCODER_PIN_B & 0x0F));
    
    // Enable pullups on encoder pins
    volatile uint8_t *pinctrl_a = (volatile uint8_t *)((ENCODER_PIN_A & 0xF0) + 0x10 + (ENCODER_PIN_A & 0x0F));
    volatile uint8_t *pinctrl_b = (volatile uint8_t *)((ENCODER_PIN_B & 0xF0) + 0x10 + (ENCODER_PIN_B & 0x0F));
    *pinctrl_a |= PORT_PULLUPEN_bm;
    *pinctrl_b |= PORT_PULLUPEN_bm;
    
    // Read initial state
    encoder_state = (read_encoder_a() << 1) | read_encoder_b();
    encoder_delta = 0;
    alt_held = false;
    alt_press_time = 0;
}

void encoder_read(void) {
    // Quadrature decoding using state machine
    static const int8_t encoder_table[16] = {
        0, -1,  1,  0,   // 00 -> 00, 01, 10, 11
        1,  0,  0, -1,   // 01 -> 00, 01, 10, 11
       -1,  0,  0,  1,   // 10 -> 00, 01, 10, 11
        0,  1, -1,  0    // 11 -> 00, 01, 10, 11
    };
    
    uint8_t new_state = (read_encoder_a() << 1) | read_encoder_b();
    uint8_t index = (encoder_state << 2) | new_state;
    encoder_delta += encoder_table[index];
    encoder_state = new_state;
}

void encoder_task(void) {
    // Check for rotation
    if (encoder_delta >= 4) {
        // Clockwise rotation - Alt+Tab
        encoder_delta -= 4;
        
        if (!alt_held) {
            // Press Alt
            keyboard_report.mods |= MOD_LALT;
            send_keyboard_report();
            alt_held = true;
        }
        
        // Press Tab
        keyboard_report.keys[0] = KC_TAB;
        send_keyboard_report();
        
        // Release Tab
        keyboard_report.keys[0] = 0;
        send_keyboard_report();
        
        // Update timeout
        alt_press_time = millis();
    }
    else if (encoder_delta <= -4) {
        // Counter-clockwise rotation - Alt+Shift+Tab
        encoder_delta += 4;
        
        if (!alt_held) {
            // Press Alt
            keyboard_report.mods |= MOD_LALT;
            send_keyboard_report();
            alt_held = true;
        }
        
        // Press Shift+Tab
        keyboard_report.mods |= MOD_LSFT;
        keyboard_report.keys[0] = KC_TAB;
        send_keyboard_report();
        
        // Release Shift+Tab
        keyboard_report.mods &= ~MOD_LSFT;
        keyboard_report.keys[0] = 0;
        send_keyboard_report();
        
        // Update timeout
        alt_press_time = millis();
    }
    
    // Check if Alt should be released (timeout)
    if (alt_held && (millis() - alt_press_time) >= ENCODER_ALT_TIMEOUT_MS) {
        keyboard_report.mods &= ~MOD_LALT;
        send_keyboard_report();
        alt_held = false;
    }
}