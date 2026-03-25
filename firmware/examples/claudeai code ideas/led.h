// ============================================================================
// led.h
// ============================================================================

#ifndef LED_H
#define LED_H

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>

#define LED_PORT              PORTA
#define LED_A_PIN             4        // PA4 = TCA0 WO4/CMP2
#define LED_B_PIN             5        // PA5 = TCA0 WO5/CMP3

#define LED_BRIGHTNESS_MIN        0
#define LED_BRIGHTNESS_MAX      255
#define LED_BRIGHTNESS_DEFAULT  128    // 50% on power-up
#define LED_BRIGHTNESS_DELTA     48    // Per-layer brightness offset

void    led_init(void);
void    led_set(uint8_t brightness);
void    led_step(bool dir, uint8_t step);
void    led_update_layer(uint8_t layer);
void    led_off(void);
void    led_on(void);
void    led_toggle(void);
bool    led_is_on(void);
uint8_t led_get_brightness(void);

#endif // LED_H