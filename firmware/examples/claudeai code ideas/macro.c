// macro.h - AmberClick90 Keyboard Firmware
// Macro execution with hybrid approach

#ifndef MACRO_H
#define MACRO_H

#include <stdint.h>
#include "keycode.h"

// Macro action types
typedef enum {
        MACRO_ACTION_DOWN,      // Press key/modifier
            MACRO_ACTION_UP,        // Release key/modifier
                MACRO_ACTION_TAP,       // Press and release
                    MACRO_ACTION_WAIT,      // Delay in ms
                        MACRO_ACTION_END        // End of macro sequence
} macro_action_type_t;

// Macro action structure
typedef struct {
        uint8_t type;           // macro_action_type_t
            uint8_t keycode;        // Key or modifier to act on
} macro_action_t;

// Public macros for defining sequences (properly namespaced)
#define MACRO_DOWN(kc)      {MACRO_ACTION_DOWN, kc}
#define MACRO_UP(kc)        {MACRO_ACTION_UP, kc}
#define MACRO_TAP(kc)       {MACRO_ACTION_TAP, kc}
#define MACRO_WAIT(ms)      {MACRO_ACTION_WAIT, ms}
#define MACRO_END           {MACRO_ACTION_END, 0}

// Execute a macro
void execute_macro(uint16_t keycode);

#endif // MACRO_H

// ============================================================================
// macro.c
// ============================================================================

#include "macro.h"
#include "usb_hid.h"
#include <avr/pgmspace.h>
#include <util/delay.h>

// Private convenience aliases for use only in this file
#define MD(kc)  MACRO_DOWN(kc)
#define MU(kc)  MACRO_UP(kc)
#define MT(kc)  MACRO_TAP(kc)
#define MW(ms)  MACRO_WAIT(ms)

// ============================================================================
// Helper Functions
// ============================================================================

// Helper: Send modifier + key combination (for simple macros)
static void send_mod_key(uint8_t mod, uint8_t key) {
        keyboard_report.mods |= mod;
            keyboard_report.keys[0] = key;
                send_keyboard_report();
                    keyboard_report.mods &= ~mod;
                        keyboard_report.keys[0] = 0;
                            send_keyboard_report();
}

// Helper: Press a modifier or key
static void press_key(uint8_t kc) {
        if (IS_MOD_KEY(kc)) {
                    keyboard_report.mods |= MOD_BIT(kc);
        } else {
                    // Find empty slot in keys array
                            for (uint8_t i = 0; i < 6; i++) {
                                            if (keyboard_report.keys[i] == 0) {
                                                                keyboard_report.keys[i] = kc;
                                                                                break;
                                            }
                            }
        }
}

// Helper: Release a modifier or key
static void release_key(uint8_t kc) {
        if (IS_MOD_KEY(kc)) {
                    keyboard_report.mods &= ~MOD_BIT(kc);
        } else {
                    // Remove from keys array
                            for (uint8_t i = 0; i < 6; i++) {
                                            if (keyboard_report.keys[i] == kc) {
                                                                keyboard_report.keys[i] = 0;
                                            }
                            }
        }
}

// Helper: Tap (press and release) a key
static void tap_key(uint8_t kc) {
        press_key(kc);
            send_keyboard_report();
                release_key(kc);
                    send_keyboard_report();
}

// Execute a macro sequence from PROGMEM
static void run_macro_sequence(const macro_action_t *sequence) {
        for (uint8_t i = 0; i < 32; i++) {  // Max 32 actions per macro
                macro_action_t action;
                        
                                // Read from program memory
                                        action.type = pgm_read_byte(&sequence[i].type);
                                                action.keycode = pgm_read_byte(&sequence[i].keycode);
                                                        
                                                                switch (action.type) {
                                                                                case MACRO_ACTION_DOWN:
                                                                                                press_key(action.keycode);
                                                                                                                send_keyboard_report();
                                                                                                                                break;
                                                                                                                                                
                                                                                                                                                            case MACRO_ACTION_UP:
                                                                                                                                                                            release_key(action.keycode);
                                                                                                                                                                                            send_keyboard_report();
                                                                                                                                                                                                            break;
                                                                                                                                                                                                                            
                                                                                                                                                                                                                                        case MACRO_ACTION_TAP:
                                                                                                                                                                                                                                                        tap_key(action.keycode);
                                                                                                                                                                                                                                                                        break;
                                                                                                                                                                                                                                                                                        
                                                                                                                                                                                                                                                                                                    case MACRO_ACTION_WAIT:
                                                                                                                                                                                                                                                                                                                    _delay_ms(action.keycode);  // keycode field holds delay
                                                                                                                                                                                                                                                                                                                                    break;
                                                                                                                                                                                                                                                                                                                                                    
                                                                                                                                                                                                                                                                                                                                                                case MACRO_ACTION_END:
                                                                                                                                                                                                                                                                                                                                                                                // Clear everything to be safe
                                                                                                                                                                                                                                                                                                                                                                                                keyboard_report.mods = 0;
                                                                                                                                                                                                                                                                                                                                                                                                                for (uint8_t j = 0; j < 6; j++) {
                                                                                                                                                                                                                                                                                                                                                                                                                                        keyboard_report.keys[j] = 0;
                                                                                                                                                                                                                                                                                                                                                                                                                }
                                                                                                                                                                                                                                                                                                                                                                                                                                send_keyboard_report();
                                                                                                                                                                                                                                                                                                                                                                                                                                                return;
                                                                }
        }
}

// ============================================================================
// Macro Definitions (stored in PROGMEM to save RAM)
// ============================================================================

// Excel: Alt, E, S sequence (Paste Special)
static const macro_action_t PROGMEM macro_excel_paste_special[] = {
        MT(KC_LALT), MT(KC_E), MT(KC_S), MACRO_END
};

// Excel: Ctrl+Shift+Right (select to end of data)
static const macro_action_t PROGMEM macro_excel_select_right[] = {
        MD(KC_LCTL), MD(KC_LSFT), MT(KC_RGHT), MU(KC_LSFT), MU(KC_LCTL), MACRO_END
};

// Excel: Ctrl+Shift+Down (select to end of column)
static const macro_action_t PROGMEM macro_excel_select_down[] = {
        MD(KC_LCTL), MD(KC_LSFT), MT(KC_DOWN), MU(KC_LSFT), MU(KC_LCTL), MACRO_END
};

// Add more complex macros here as needed...

// ============================================================================
// Macro Execution
// ============================================================================

void execute_macro(uint16_t keycode) {
        // Simple modifier+key macros use the efficient helper
            switch (keycode) {
                        case MC_COPY:
                                    send_mod_key(MOD_LCTL, KC_C);
                                                break;
                                                        case MC_PSTE:
                                                                    send_mod_key(MOD_LCTL, KC_V);
                                                                                break;
                                                                                        case MC_CUT:
                                                                                                    send_mod_key(MOD_LCTL, KC_X);
                                                                                                                break;
                                                                                                                        case MC_UNDO:
                                                                                                                                    send_mod_key(MOD_LCTL, KC_Z);
                                                                                                                                                break;
                                                                                                                                                        case MC_REDO:
                                                                                                                                                                    send_mod_key(MOD_LCTL, KC_Y);
                                                                                                                                                                                break;
                                                                                                                                                                                        case MC_SAVE:
                                                                                                                                                                                                    send_mod_key(MOD_LCTL, KC_S);
                                                                                                                                                                                                                break;
                                                                                                                                                                                                                        case MC_FIND:
                                                                                                                                                                                                                                    send_mod_key(MOD_LCTL, KC_F);
                                                                                                                                                                                                                                                break;
                                                                                                                                                                                                                                                        case MC_SALL:
                                                                                                                                                                                                                                                                    send_mod_key(MOD_LCTL, KC_A);
                                                                                                                                                                                                                                                                                break;
                                                                                                                                                                                                                                                                                            
                                                                                                                                                                                                                                                                                                    // Complex macros use sequences
                                                                                                                                                                                                                                                                                                            // Uncomment and add to keycode.h as needed:
                                                                                                                                                                                                                                                                                                                    /*
                                                                                                                                                                                                                                                                                                                            case MC_XL_PSTSP:
                                                                                                                                                                                                                                                                                                                                        run_macro_sequence(macro_excel_paste_special);
                                                                                                                                                                                                                                                                                                                                                    break;
                                                                                                                                                                                                                                                                                                                                                            case MC_XL_SELR:
                                                                                                                                                                                                                                                                                                                                                                        run_macro_sequence(macro_excel_select_right);
                                                                                                                                                                                                                                                                                                                                                                                    break;
                                                                                                                                                                                                                                                                                                                                                                                            case MC_XL_SELD:
                                                                                                                                                                                                                                                                                                                                                                                                        run_macro_sequence(macro_excel_select_down);
                                                                                                                                                                                                                                                                                                                                                                                                                    break;
                                                                                                                                                                                                                                                                                                                                                                                                                            */
                                                                                                                                                                                                                                                                                                                                                                                                                                        
                                                                                                                                                                                                                                                                                                                                                                                                                                                default:
                                                                                                                                                                                                                                                                                                                                                                                                                                                            break;
            }
}

// Undefine private convenience aliases
#undef MD
#undef MU
#undef MT
#undef MW
            }
}
}
}
}
                                                                                                                                                                                                                                                                                                                                                                                                                }
                                                                }}
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