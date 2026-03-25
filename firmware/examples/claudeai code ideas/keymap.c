// keymap.h - AmberClick90 Keyboard Firmware
// Keymap lookup functions

#ifndef KEYMAP_H
#define KEYMAP_H

#include <stdint.h>
#include <avr/pgmspace.h>
#include "config.h"
#include "keycode.h"

// Lookup keycode for a given layer, row, col
uint16_t keymap_key_to_keycode(uint8_t layer, uint8_t row, uint8_t col);

#endif // KEYMAP_H

// ============================================================================
// keymap.c
// ============================================================================

#include "keymap.h"

// Define your keymap here
// This is an example layout - customize to your needs
const uint16_t PROGMEM keymaps[NUM_LAYERS][MATRIX_ROWS][MATRIX_COLS] = {
    // Layer 0 - Base layer
    [0] = {
        // Row 0
        { KC_ESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8    },
        // Row 1
        { KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I    },
        // Row 2
        { KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K    },
        // Row 3
        { KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM },
        // Row 4
        { KC_LCTL, KC_LGUI, KC_LALT, KC_SPC,  XXXXXXX, XXXXXXX, KC_RALT, LY_MO1,  KC_RCTL },
        // Row 5
        { KC_GRV,  KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC, KC_LBRC, KC_RBRC, KC_BSLS },
        // Row 6
        { KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS, KC_DEL,  KC_PGUP, KC_PGDN, KC_HOME },
        // Row 7
        { KC_L,    KC_SCLN, KC_QUOT, KC_ENT,  KC_END,  KC_UP,   KC_DOWN, KC_LEFT, KC_RGHT },
        // Row 8
        { KC_DOT,  KC_SLSH, KC_RSFT, KC_INS,  KC_PSCR, KC_SLCK, KC_PAUS, XXXXXXX, XXXXXXX },
        // Row 9
        { KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9   },
    },
    
    // Layer 1 - Function layer
    [1] = {
        // Row 0
        { ______,  ______,  ______,  ______,  ______,  ______,  ______,  ______,  ______ },
        // Row 1
        { ______,  MC_COPY, MC_PSTE, ______,  CC_VOLU, CC_VOLD, CC_MUTE, ______,  ______ },
        // Row 2
        { ______,  MC_SALL, MC_SAVE, ______,  MC_FIND, ______,  ______,  ______,  ______ },
        // Row 3
        { ______,  MC_UNDO, MC_CUT,  MC_COPY, MC_PSTE, ______,  ______,  ______,  ______ },
        // Row 4
        { ______,  ______,  ______,  ______,  XXXXXXX, XXXXXXX, ______,  ______,  LY_MO2 },
        // Row 5
        { ______,  ______,  ______,  ______,  ______,  ______,  ______,  ______,  ______ },
        // Row 6
        { ______,  ______,  ______,  ______,  ______,  ______,  ______,  ______,  ______ },
        // Row 7
        { ______,  ______,  ______,  ______,  ______,  CC_MPLY, CC_MNXT, CC_MPRV, CC_MSTP },
        // Row 8
        { ______,  ______,  ______,  ______,  ______,  ______,  ______,  XXXXXXX, XXXXXXX },
        // Row 9
        { ______,  ______,  ______,  ______,  ______,  ______,  ______,  ______,  ______ },
    },
    
    // Layer 2 - System/config layer
    [2] = {
        // Row 0
        { SYS_BOOT, SYS_RST, ______,  ______,  ______,  ______,  ______,  ______,  ______ },
        // Row 1
        { ______,   ______,  ______,  ______,  LD_BRIU, LD_BRID, ______,  ______,  ______ },
        // Row 2
        { ______,   ______,  ______,  ______,  ______,  ______,  ______,  ______,  ______ },
        // Row 3
        { ______,   ______,  ______,  ______,  ______,  ______,  ______,  ______,  ______ },
        // Row 4
        { ______,   ______,  ______,  ______,  XXXXXXX, XXXXXXX, ______,  ______,  ______ },
        // Row 5
        { ______,   ______,  ______,  ______,  ______,  ______,  ______,  ______,  ______ },
        // Row 6
        { ______,   ______,  ______,  ______,  ______,  ______,  ______,  ______,  ______ },
        // Row 7
        { ______,   ______,  ______,  ______,  ______,  ______,  ______,  ______,  ______ },
        // Row 8
        { ______,   ______,  ______,  ______,  ______,  ______,  ______,  XXXXXXX, XXXXXXX },
        // Row 9
        { KC_F10,   KC_F11,  KC_F12,  ______,  ______,  ______,  ______,  ______,  ______ },
    },
};

// Lookup keycode with transparent support
uint16_t keymap_key_to_keycode(uint8_t layer, uint8_t row, uint8_t col) {
    // Bounds checking
    if (layer >= NUM_LAYERS || row >= MATRIX_ROWS || col >= MATRIX_COLS) {
        return KC_NO;
    }
    
    uint16_t keycode = pgm_read_word(&keymaps[layer][row][col]);
    
    // Handle transparent keys - fall through to lower layers
    if (keycode == KC_TRNS && layer > 0) {
        return keymap_key_to_keycode(layer - 1, row, col);
    }
    
    return keycode;
}