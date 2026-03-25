// ============================================================================
// led.h - LED Control for AmberClick90
// ============================================================================

#ifndef LED_H
#define LED_H

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>

// ----------------------------------------------------------------------------
// Configuration
// ----------------------------------------------------------------------------

// GPIO pins for PWM LEDs (both controlled together)
// PA4 = WO0 (TCA0 Compare Channel 0)
// PA5 = WO1 (TCA0 Compare Channel 1)
#define LED_PWM_PORT        PORTA
#define LED_A_PIN           4  // PA4
#define LED_B_PIN           5  // PA5

// Brightness limits
#define LED_BRIGHTNESS_MIN  0
#define LED_BRIGHTNESS_MAX  255
#define LED_BRIGHTNESS_DEFAULT  128  // 50% on power-up

// Default step size for brightness changes
#define LED_BRIGHTNESS_STEP 16  // ~6% per step

// ----------------------------------------------------------------------------
// Public API
// ----------------------------------------------------------------------------

/**
 * @brief Initialize LED subsystem (GPIO + TCA0 PWM)
 * 
 * Configures:
 * - PA4 and PA5 as outputs
 * - TCA0 in Single-Slope PWM mode
 * - 8-bit resolution (PER = 255)
 * - 1.465 kHz PWM frequency (prescaler = 64)
 * - Both LEDs start at LED_BRIGHTNESS_DEFAULT
 * 
 * Note: PWM frequency is 7.3× above 200 Hz matrix scan rate
 */
void setupLEDs(void);

/**
 * @brief Change LED brightness (both LEDs together)
 * 
 * @param direction true = increase, false = decrease
 * @param step Amount to change (typical: 16)
 * 
 * Clamps to LED_BRIGHTNESS_MIN/MAX automatically.
 */
void chgLEDBrightness(bool direction, uint8_t step);

/**
 * @brief Turn off LEDs (disable PWM output, preserve brightness state)
 * 
 * Disables PWM output on both channels. Brightness value is preserved
 * so turnOnLEDs() can restore to previous level.
 */
void turnOffLEDs(void);

/**
 * @brief Turn on LEDs (re-enable PWM output at previous brightness)
 * 
 * Restores PWM output to the brightness level that was active before
 * turnOffLEDs() was called.
 */
void turnOnLEDs(void);

/**
 * @brief Toggle LEDs on/off (preserves brightness state)
 * 
 * If LEDs are on, turns them off. If off, restores previous brightness.
 */
void toggleLEDs(void);

/**
 * @brief Set absolute brightness (both LEDs together)
 * 
 * @param brightness 0-255 (0 = off, 255 = full)
 * 
 * Useful for presets or direct control.
 */
void setLEDBrightness(uint8_t brightness);

/**
 * @brief Get current brightness level
 * 
 * @return Current brightness (0-255)
 */
uint8_t getLEDBrightness(void);

/**
 * @brief Update LED blink feedback (call from main loop or timer ISR)
 * 
 * Should be called periodically (e.g., every 100ms) to animate the
 * min/max brightness blink feedback. Automatically stops after 3 blinks.
 * 
 * Example: Call this from a 100ms timer or in main loop with timing check.
 */
void updateLEDFeedback(void);

#endif // LED_H


// ============================================================================
// led.c - LED Control Implementation
// ============================================================================

#include "led.h"

// ----------------------------------------------------------------------------
// Private State
// ----------------------------------------------------------------------------

static uint8_t led_brightness = LED_BRIGHTNESS_DEFAULT;
static bool led_output_enabled = true;  // Track if PWM output is active
static uint8_t blink_counter = 0;       // For min/max blink feedback

// ----------------------------------------------------------------------------
// Public Functions
// ----------------------------------------------------------------------------

void setupLEDs(void)
{
    // Configure GPIO: PA4 and PA5 as outputs
    LED_PWM_PORT.DIRSET = (1 << LED_A_PIN) | (1 << LED_B_PIN);
    
    // Configure TCA0 for 8-bit PWM at 1.465 kHz
    // f_PWM = 24 MHz / (64 * 256) = 1,465 Hz
    // Separation from 200 Hz matrix scan: 7.3× (excellent)
    
    // Stop timer during configuration
    TCA0.SINGLE.CTRLA = 0;
    
    // Set prescaler: DIV64 (24 MHz / 64 = 375 kHz timer clock)
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV64_gc;
    
    // Set period: 255 for 8-bit resolution (0-255)
    TCA0.SINGLE.PER = LED_BRIGHTNESS_MAX;
    
    // Set initial compare values (both LEDs same brightness)
    TCA0.SINGLE.CMP0 = led_brightness;  // LED A (PA4)
    TCA0.SINGLE.CMP1 = led_brightness;  // LED B (PA5)
    
    // Configure PWM mode and enable compare channels
    TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_SINGLESLOPE_gc |  // Single-slope PWM
                        TCA_SINGLE_CMP0EN_bm |               // Enable WO0 (PA4)
                        TCA_SINGLE_CMP1EN_bm;                // Enable WO1 (PA5)
    
    // Start timer
    TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;
}

void chgLEDBrightness(bool direction, uint8_t step)
{
    bool at_limit = false;
    
    if (direction) {
        // Increase brightness (with saturation)
        if (led_brightness > LED_BRIGHTNESS_MAX - step) {
            led_brightness = LED_BRIGHTNESS_MAX;
            at_limit = true;
        } else {
            led_brightness += step;
        }
    } else {
        // Decrease brightness (with saturation)
        if (led_brightness < LED_BRIGHTNESS_MIN + step) {
            led_brightness = LED_BRIGHTNESS_MIN;
            at_limit = true;
        } else {
            led_brightness -= step;
        }
    }
    
    // Update both LEDs if output is enabled
    if (led_output_enabled) {
        TCA0.SINGLE.CMP0BUF = led_brightness;  // LED A
        TCA0.SINGLE.CMP1BUF = led_brightness;  // LED B
    }
    
    // Trigger blink feedback if we hit a limit
    if (at_limit) {
        blink_counter = 6;  // 3 blinks (6 half-cycles)
    }
}

void turnOffLEDs(void)
{
    // Disable PWM output (pins driven low by port)
    TCA0.SINGLE.CTRLB &= ~(TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_CMP1EN_bm);
    
    // Ensure pins are driven low
    LED_PWM_PORT.OUTCLR = (1 << LED_A_PIN) | (1 << LED_B_PIN);
    
    led_output_enabled = false;
    // Note: led_brightness value is preserved!
}

void turnOnLEDs(void)
{
    // Restore compare values to current brightness
    TCA0.SINGLE.CMP0BUF = led_brightness;
    TCA0.SINGLE.CMP1BUF = led_brightness;
    
    // Re-enable PWM output
    TCA0.SINGLE.CTRLB |= (TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_CMP1EN_bm);
    
    led_output_enabled = true;
}

void toggleLEDs(void)
{
    if (led_output_enabled) {
        turnOffLEDs();
    } else {
        turnOnLEDs();
    }
}

void setLEDBrightness(uint8_t brightness)
{
    // Clamp to valid range
    if (brightness > LED_BRIGHTNESS_MAX) {
        brightness = LED_BRIGHTNESS_MAX;
    }
    
    led_brightness = brightness;
    
    // Update both LEDs
    TCA0.SINGLE.CMP0BUF = led_brightness;
    TCA0.SINGLE.CMP1BUF = led_brightness;
}

uint8_t getLEDBrightness(void)
{
    return led_brightness;
}

void updateLEDFeedback(void)
{
    // If no blink in progress, do nothing
    if (blink_counter == 0) {
        return;
    }
    
    // Toggle PWM output on/off for blink effect
    if (blink_counter % 2 == 0) {
        // Even counts: turn off
        TCA0.SINGLE.CTRLB &= ~(TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_CMP1EN_bm);
        LED_PWM_PORT.OUTCLR = (1 << LED_A_PIN) | (1 << LED_B_PIN);
    } else {
        // Odd counts: turn on at current brightness
        TCA0.SINGLE.CMP0BUF = led_brightness;
        TCA0.SINGLE.CMP1BUF = led_brightness;
        TCA0.SINGLE.CTRLB |= (TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_CMP1EN_bm);
    }
    
    blink_counter--;
    
    // When blink sequence finishes, restore proper state
    if (blink_counter == 0) {
        if (led_output_enabled) {
            // Ensure LEDs are on at correct brightness
            TCA0.SINGLE.CMP0BUF = led_brightness;
            TCA0.SINGLE.CMP1BUF = led_brightness;
            TCA0.SINGLE.CTRLB |= (TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_CMP1EN_bm);
        } else {
            // Ensure LEDs stay off (user had turned them off)
            TCA0.SINGLE.CTRLB &= ~(TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_CMP1EN_bm);
            LED_PWM_PORT.OUTCLR = (1 << LED_A_PIN) | (1 << LED_B_PIN);
        }
    }
}


// ============================================================================
// Example Usage in User Code (e.g., keymap.c or main.c)
// ============================================================================

/*

// In main.c initialization:
int main(void)
{
    // ... other init ...
    setupLEDs();  // Initialize PWM LEDs at 50% brightness
    
    // Main loop
    while (1) {
        scanMatrix();
        processKeys();
        updateEncoder();
        
        // Call this periodically for blink feedback (e.g., every 100ms)
        static uint16_t led_feedback_timer = 0;
        if (++led_feedback_timer >= 100) {  // Adjust based on loop timing
            led_feedback_timer = 0;
            updateLEDFeedback();  // Animate blink if needed
        }
        
        // ... rest of main loop ...
    }
}

// In keymap.c or layer handler (Layer 2 = System Settings):
void process_system_layer_key(uint8_t keycode)
{
    switch (keycode) {
        case KEY_LED_BRIGHTNESS_UP:
            chgLEDBrightness(true, LED_BRIGHTNESS_STEP);  // +16 (6%)
            // LEDs will blink 3 times if max brightness reached
            break;
            
        case KEY_LED_BRIGHTNESS_DOWN:
            chgLEDBrightness(false, LED_BRIGHTNESS_STEP); // -16 (6%)
            // LEDs will blink 3 times if min brightness reached
            break;
            
        case KEY_LED_TOGGLE:
            toggleLEDs();  // Turn off (preserves brightness) or turn back on
            break;
            
        case KEY_LED_OFF:
            turnOffLEDs();  // Explicitly turn off (preserves brightness)
            break;
            
        case KEY_LED_ON:
            turnOnLEDs();   // Restore to previous brightness
            break;
            
        case KEY_LED_PRESET_DIM:
            setLEDBrightness(64);  // 25% brightness
            break;
            
        case KEY_LED_PRESET_MEDIUM:
            setLEDBrightness(128); // 50% brightness
            break;
            
        case KEY_LED_PRESET_BRIGHT:
            setLEDBrightness(255); // 100% brightness
            break;
    }
}

// Alternative: Direct bitmask manipulation if you prefer
void custom_led_control(void)
{
    // Direct register access (if you want fine control)
    TCA0.SINGLE.CMP0BUF = 200;  // LED A to 78% brightness
    TCA0.SINGLE.CMP1BUF = 200;  // LED B to 78% brightness
    
    // Check current timer state
    uint8_t current = TCA0.SINGLE.CNT;  // Read current counter value
    
    // Check if LEDs are currently enabled
    bool leds_on = (TCA0.SINGLE.CTRLB & TCA_SINGLE_CMP0EN_bm) != 0;
}

// Timer-based approach (if using a timer ISR for precise timing):
ISR(TCB0_INT_vect)
{
    static uint8_t feedback_ticks = 0;
    
    // Call every 100ms (configure TCB0 for 10Hz)
    if (++feedback_ticks >= 10) {  // Adjust based on ISR frequency
        feedback_ticks = 0;
        updateLEDFeedback();
    }
    
    TCB0.INTFLAGS = TCB_CAPT_bm;  // Clear interrupt flag
}

*/