// config.h - AmberClick90 Keyboard Firmware
// Hardware and firmware configuration

#ifndef CONFIG_H
#define CONFIG_H

#include <avr/io.h>

// ============================================================================
// Keyboard Information
// ============================================================================
#define KEYBOARD_NAME       "AmberClick90"
#define VENDOR_ID           0xFEED  // Change to your vendor ID
#define PRODUCT_ID          0x0090  // Change to your product ID
#define DEVICE_VERSION      0x0001
#define MANUFACTURER        "YourName"
#define PRODUCT             "AmberClick90"

// ============================================================================
// Matrix Configuration
// ============================================================================
#define MATRIX_ROWS         10
#define MATRIX_COLS         9
#define MATRIX_KEYS         (MATRIX_ROWS * MATRIX_COLS)  // 90 keys

// Debounce time in milliseconds
#define DEBOUNCE_MS         5

// ============================================================================
// Layer Configuration
// ============================================================================
#define NUM_LAYERS          3
#define MAX_LAYER_KEYS      4  // Max simultaneous layer hold keys

// ============================================================================
// LED Configuration
// ============================================================================
#define LED_BRIGHTNESS_BASE     64   // Layer 0 base brightness (0-255)
#define LED_BRIGHTNESS_STEP     16   // Adjustment step
#define LED_BRIGHTNESS_DELTA    64   // Layer 1/2 brightness increase
#define LED_BRIGHTNESS_MIN      0
#define LED_BRIGHTNESS_MAX      255

// LED pins (adjust based on your PCB)
#define LED1_PIN            PORTC_PIN0  // Change to actual pin
#define LED2_PIN            PORTC_PIN1  // Change to actual pin

// Timer for PWM (AVR64DU32 has TCA, TCB, TCD timers)
// Using TCA0 in split mode for 2 PWM channels
#define LED_TIMER_FREQUENCY 1000  // 1kHz PWM frequency

// ============================================================================
// Rotary Encoder Configuration
// ============================================================================
#define ENCODER_PIN_A       PORTD_PIN0  // Change to actual pin
#define ENCODER_PIN_B       PORTD_PIN1  // Change to actual pin

// Alt-Tab timeout in milliseconds (time before releasing Alt)
#define ENCODER_ALT_TIMEOUT_MS  1000

// ============================================================================
// Matrix Pin Definitions (AVR64DU32 specific)
// ============================================================================
// Row pins (outputs)
#define ROW0_PIN            PORTA_PIN0
#define ROW1_PIN            PORTA_PIN1
#define ROW2_PIN            PORTA_PIN2
#define ROW3_PIN            PORTA_PIN3
#define ROW4_PIN            PORTA_PIN4
#define ROW5_PIN            PORTA_PIN5
#define ROW6_PIN            PORTA_PIN6
#define ROW7_PIN            PORTA_PIN7
#define ROW8_PIN            PORTB_PIN0
#define ROW9_PIN            PORTB_PIN1

// Column pins (inputs with pullups)
#define COL0_PIN            PORTB_PIN2
#define COL1_PIN            PORTB_PIN3
#define COL2_PIN            PORTB_PIN4
#define COL3_PIN            PORTB_PIN5
#define COL4_PIN            PORTF_PIN0
#define COL5_PIN            PORTF_PIN1
#define COL6_PIN            PORTF_PIN2
#define COL7_PIN            PORTF_PIN3
#define COL8_PIN            PORTF_PIN4

// ============================================================================
// USB Configuration
// ============================================================================
#define USB_ENDPOINT_SIZE   64  // Endpoint size in bytes
#define USB_POLLING_RATE    1   // 1ms polling rate (1000Hz)

// HID Report IDs
#define REPORT_ID_KEYBOARD  1
#define REPORT_ID_CONSUMER  2

// ============================================================================
// Macro Configuration
// ============================================================================
#define MAX_HELD_MACROS     6  // Max simultaneous held macros

// ============================================================================
// Timing Configuration
// ============================================================================
#define F_CPU               24000000UL  // 24MHz system clock (typical for AVR64DU32)

#endif // CONFIG_H