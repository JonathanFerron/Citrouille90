// led.h - AmberClick90 Keyboard Firmware
// LED PWM control

#ifndef LED_H
#define LED_H

#include <stdint.h>
#include "config.h"

extern uint8_t led_base_brightness;

void led_init(void);
void led_update_for_layer(uint8_t layer);
void led_brightness_increase(void);
void led_brightness_decrease(void);
void set_led1_pwm(uint8_t brightness);
void set_led2_pwm(uint8_t brightness);

#endif // LED_H

// ============================================================================
// led.c
// ============================================================================

#include "led.h"
#include <avr/io.h>

uint8_t led_base_brightness = LED_BRIGHTNESS_BASE;

void led_init(void) {
    // Configure LED pins as outputs
    // Assuming PC0 and PC1 for LEDs (adjust based on your PCB)
    PORTC.DIRSET = PIN0_bm | PIN1_bm;
    
    // Configure TCA0 in split mode for 2 independent PWM channels
    // TCA0 WO0 -> PC0 (LED1)
    // TCA0 WO1 -> PC1 (LED2)
    
    // Disable TCA0 before configuration
    TCA0.SPLIT.CTRLA = 0;
    
    // Enable split mode
    TCA0.SPLIT.CTRLD = TCA_SPLIT_SPLITM_bm;
    
    // Set prescaler (F_CPU / 64 = 375kHz for 24MHz)
    // With 8-bit counter, PWM frequency = 375kHz / 256 = ~1.46kHz
    TCA0.SPLIT.CTRLA = TCA_SPLIT_CLKSEL_DIV64_gc;
    
    // Enable compare channels 0 and 1
    TCA0.SPLIT.CTRLB = TCA_SPLIT_LCMP0EN_bm | TCA_SPLIT_LCMP1EN_bm;
    
    // Set initial brightness
    TCA0.SPLIT.LCMP0 = led_base_brightness;  // LED1
    TCA0.SPLIT.LCMP1 = led_base_brightness;  // LED2
    
    // Enable TCA0
    TCA0.SPLIT.CTRLA |= TCA_SPLIT_ENABLE_bm;
    
    // Configure pin multiplexing if needed (check datasheet)
    // PORTMUX.TCAROUTEA may need configuration
}

void set_led1_pwm(uint8_t brightness) {
    TCA0.SPLIT.LCMP0 = brightness;
}

void set_led2_pwm(uint8_t brightness) {
    TCA0.SPLIT.LCMP1 = brightness;
}

void led_update_for_layer(uint8_t layer) {
    switch(layer) {
        case 0:
            // Both LEDs at base brightness
            set_led1_pwm(led_base_brightness);
            set_led2_pwm(led_base_brightness);
            break;
            
        case 1:
            // LED1 brighter (+delta), LED2 at base
            set_led1_pwm(led_base_brightness + LED_BRIGHTNESS_DELTA);
            set_led2_pwm(led_base_brightness);
            break;
            
        case 2:
            // Both LEDs at elevated brightness
            {
                uint8_t bright = led_base_brightness + LED_BRIGHTNESS_DELTA;
                set_led1_pwm(bright);
                set_led2_pwm(bright);
            }
            break;
            
        default:
            break;
    }
}

void led_brightness_increase(void) {
    if (led_base_brightness <= (LED_BRIGHTNESS_MAX - LED_BRIGHTNESS_STEP)) {
        led_base_brightness += LED_BRIGHTNESS_STEP;
    } else {
        led_base_brightness = LED_BRIGHTNESS_MAX;
    }
    // Re-apply brightness for current layer
    extern uint8_t current_layer;
    led_update_for_layer(current_layer);
}

void led_brightness_decrease(void) {
    if (led_base_brightness >= LED_BRIGHTNESS_STEP) {
        led_base_brightness -= LED_BRIGHTNESS_STEP;
    } else {
        led_base_brightness = LED_BRIGHTNESS_MIN;
    }
    // Re-apply brightness for current layer
    extern uint8_t current_layer;
    led_update_for_layer(current_layer);
}