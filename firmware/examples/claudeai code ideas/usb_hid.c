// usb_hid.h - AmberClick90 Keyboard Firmware
// USB HID interface declarations
// Implementation depends on USB library (LUFA, ASF, etc.)

#ifndef USB_HID_H
#define USB_HID_H

#include <stdint.h>
#include <stdbool.h>
#include "config.h"

// Boot keyboard HID report structure (6KRO)
typedef struct {
    uint8_t mods;      // Modifier keys bitmap
    uint8_t reserved;  // Reserved (always 0)
    uint8_t keys[6];   // Up to 6 simultaneous key presses
} keyboard_report_t;

// Consumer control report (media keys)
typedef struct {
    uint16_t usage;    // Consumer control usage ID
} consumer_report_t;

// Global report structures
extern keyboard_report_t keyboard_report;
extern consumer_report_t consumer_report;

// USB initialization and task functions
void usb_init(void);
void usb_task(void);
bool usb_configured(void);

// Report sending functions
void send_keyboard_report(void);
void send_consumer_report(uint16_t keycode);
void clear_keyboard_report(void);
void clear_consumer_report(void);

// Key management functions
void add_key_to_report(uint8_t keycode);
void remove_key_from_report(uint8_t keycode);
bool is_key_in_report(uint8_t keycode);

// Modifier management
void add_mods_to_report(uint8_t mods);
void remove_mods_from_report(uint8_t mods);

#endif // USB_HID_H

// ============================================================================
// usb_hid.c - Stub implementation
// You'll need to implement this using your chosen USB library
// ============================================================================

/*
#include "usb_hid.h"
#include <string.h>

keyboard_report_t keyboard_report;
consumer_report_t consumer_report;

void usb_init(void) {
    // Initialize USB peripheral
    // Configure endpoints
    // Set up descriptors
    // TODO: Implement using USB library
    
    memset(&keyboard_report, 0, sizeof(keyboard_report_t));
    memset(&consumer_report, 0, sizeof(consumer_report_t));
}

void usb_task(void) {
    // Handle USB events and state machine
    // TODO: Implement using USB library
}

bool usb_configured(void) {
    // Return true if USB is enumerated and ready
    // TODO: Implement using USB library
    return true;
}

void send_keyboard_report(void) {
    if (!usb_configured()) return;
    
    // Send keyboard_report via USB endpoint
    // TODO: Implement using USB library
}

void send_consumer_report(uint16_t keycode) {
    if (!usb_configured()) return;
    
    // Map keycode to consumer control usage ID
    uint16_t usage = 0;
    switch(keycode) {
        case CC_MUTE: usage = 0xE2; break;
        case CC_VOLU: usage = 0xE9; break;
        case CC_VOLD: usage = 0xEA; break;
        case CC_MNXT: usage = 0xB5; break;
        case CC_MPRV: usage = 0xB6; break;
        case CC_MPLY: usage = 0xCD; break;
        case CC_MSTP: usage = 0xB7; break;
        default: break;
    }
    
    consumer_report.usage = usage;
    // Send consumer_report via USB endpoint
    // TODO: Implement using USB library
}

void clear_keyboard_report(void) {
    memset(&keyboard_report, 0, sizeof(keyboard_report_t));
}

void clear_consumer_report(void) {
    consumer_report.usage = 0;
}

void add_key_to_report(uint8_t keycode) {
    // Handle modifiers
    if (IS_MOD_KEY(keycode)) {
        keyboard_report.mods |= MOD_BIT(keycode);
        return;
    }
    
    // Add regular key to keys array if not already present
    if (!is_key_in_report(keycode)) {
        for (uint8_t i = 0; i < 6; i++) {
            if (keyboard_report.keys[i] == 0) {
                keyboard_report.keys[i] = keycode;
                return;
            }
        }
    }
}

void remove_key_from_report(uint8_t keycode) {
    // Handle modifiers
    if (IS_MOD_KEY(keycode)) {
        keyboard_report.mods &= ~MOD_BIT(keycode);
        return;
    }
    
    // Remove regular key from keys array
    for (uint8_t i = 0; i < 6; i++) {
        if (keyboard_report.keys[i] == keycode) {
            keyboard_report.keys[i] = 0;
        }
    }
}

bool is_key_in_report(uint8_t keycode) {
    for (uint8_t i = 0; i < 6; i++) {
        if (keyboard_report.keys[i] == keycode) {
            return true;
        }
    }
    return false;
}

void add_mods_to_report(uint8_t mods) {
    keyboard_report.mods |= mods;
}

void remove_mods_from_report(uint8_t mods) {
    keyboard_report.mods &= ~mods;
}
*/