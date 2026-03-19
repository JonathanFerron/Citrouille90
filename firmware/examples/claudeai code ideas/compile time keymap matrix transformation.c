#include <avr/pgmspace.h>

// Physical layout: 20 columns x 5 rows = 100 positions (with 10 empty)
// Electrical layout: 9 columns x 10 rows = 90 keys
// 3 layers total

// Define your sparse keymap layout (20 col x 5 row - physical layout)
// This macro maps the physical layout to the electrical 9x10 matrix
#define KEYMAP( \
    k00, k01, k02, k03, k04, k05, k06, k07, k08, k09, k0A, k0B, k0C, k0D, k0E, k0F, k0G, k0H, k0I, k0J, \
    k10, k11, k12, k13, k14, k15, k16, k17, k18, k19, k1A, k1B, k1C, k1D, k1E, k1F, k1G, k1H, k1I, k1J, \
    k20, k21, k22, k23, k24, k25, k26, k27, k28, k29, k2A, k2B, k2C, k2D, k2E, k2F, k2G, k2H, k2I, k2J, \
    k30, k31, k32, k33, k34, k35, k36, k37, k38, k39, k3A, k3B, k3C, k3D, k3E, k3F, k3G, k3H, k3I, k3J, \
    k40, k41, k42, k43, k44, k45, k46, k47, k48, k49, k4A, k4B, k4C, k4D, k4E, k4F, k4G, k4H, k4I, k4J  \
) { \
    /* TODO: Map your 100 physical positions to 90 electrical (row, col) positions */ \
    /* Example mapping - replace with your actual wiring: */ \
    { k00, k01, k02, k03, k04, k05, k06, k07, k08 }, /* elec row 0 */ \
    { k09, k0A, k0B, k0C, k0D, k0E, k0F, k0G, k0H }, /* elec row 1 */ \
    { k0I, k0J, k10, k11, k12, k13, k14, k15, k16 }, /* elec row 2 */ \
    { k17, k18, k19, k1A, k1B, k1C, k1D, k1E, k1F }, /* elec row 3 */ \
    { k1G, k1H, k1I, k1J, k20, k21, k22, k23, k24 }, /* elec row 4 */ \
    { k25, k26, k27, k28, k29, k2A, k2B, k2C, k2D }, /* elec row 5 */ \
    { k2E, k2F, k2G, k2H, k2I, k2J, k30, k31, k32 }, /* elec row 6 */ \
    { k33, k34, k35, k36, k37, k38, k39, k3A, k3B }, /* elec row 7 */ \
    { k3C, k3D, k3E, k3F, k3G, k3H, k3I, k3J, k40 }, /* elec row 8 */ \
    { k41, k42, k43, k44, k45, k46, k47, k48, k49 }  /* elec row 9 */ \
}

// Example keycodes (HID usage page)
#define KC_NO   0x00
#define KC_A    0x04
#define KC_B    0x05
#define KC_C    0x06
#define KC_D    0x07
#define KC_E    0x08
#define KC_F    0x09
#define KC_G    0x0A
#define KC_H    0x0B
#define KC_I    0x0C
#define KC_J    0x0D
#define KC_ESC  0x29
#define KC_ENT  0x28
#define KC_SPC  0x2C
#define KC_TRNS 0xFF  // Transparent (fall through to lower layer)

// Define your 3 layers in the physical layout (20x5 each)
const uint8_t PROGMEM keymaps[][10][9] = {
    // Layer 0 - Base layer
    [0] = KEYMAP(
        KC_ESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
        KC_A,    KC_B,    KC_C,    KC_D,    KC_E,    KC_F,    KC_G,    KC_H,    KC_I,    KC_J,    KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
        KC_NO,   KC_NO,   KC_NO,   KC_SPC,  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_ENT,  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO
    ),
    
    // Layer 1 - Function layer
    [1] = KEYMAP(
        KC_TRNS, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
        KC_NO,   KC_NO,   KC_NO,   KC_TRNS, KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_TRNS, KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO
    ),
    
    // Layer 2 - Additional layer
    [2] = KEYMAP(
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
        KC_NO,   KC_NO,   KC_NO,   KC_TRNS, KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_TRNS, KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO
    )
};

// Usage in scanning code:
uint8_t get_keycode(uint8_t layer, uint8_t row, uint8_t col) {
    return pgm_read_byte(&keymaps[layer][row][col]);
}

// Example: Matrix scanning with layer support
uint8_t active_layer = 0;

uint8_t keymap_key_to_keycode(uint8_t row, uint8_t col) {
    uint8_t keycode = get_keycode(active_layer, row, col);
    
    // Handle transparent keys (fall through to lower layers)
    if (keycode == KC_TRNS) {
        for (int8_t layer = active_layer - 1; layer >= 0; layer--) {
            keycode = get_keycode(layer, row, col);
            if (keycode != KC_TRNS) {
                break;
            }
        }
    }
    
    return keycode;
}