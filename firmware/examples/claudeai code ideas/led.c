// ============================================================================
// led.c
// ============================================================================

#include "led.h"

static uint8_t s_brightness = LED_BRIGHTNESS_DEFAULT;
static uint8_t s_layer_a    = LED_BRIGHTNESS_DEFAULT;
static uint8_t s_layer_b    = LED_BRIGHTNESS_DEFAULT;
static bool    s_on         = true;

static void _apply(uint8_t a, uint8_t b)
{
    TCA0.SINGLE.CMP2BUF = a;
        TCA0.SINGLE.CMP3BUF = b;
        }

        static uint8_t _add_clamped(uint8_t base, uint8_t delta)
        {
            uint16_t v = (uint16_t)base + delta;
                return (v > LED_BRIGHTNESS_MAX) ? LED_BRIGHTNESS_MAX : (uint8_t)v;
                }

                static void _enable_output(void)
                {
                    TCA0.SINGLE.CTRLB |= TCA_SINGLE_CMP2EN_bm | TCA_SINGLE_CMP3EN_bm;
                    }

                    static void _disable_output(void)
                    {
                        TCA0.SINGLE.CTRLB &= ~(TCA_SINGLE_CMP2EN_bm | TCA_SINGLE_CMP3EN_bm);
                            LED_PORT.OUTCLR = (1 << LED_A_PIN) | (1 << LED_B_PIN);
                            }

                            void led_init(void)
                            {
                                LED_PORT.DIRSET = (1 << LED_A_PIN) | (1 << LED_B_PIN);

                                    PORTMUX.TCAROUTEA = (PORTMUX.TCAROUTEA & ~PORTMUX_TCA0_gm)
                                                          | PORTMUX_TCA0_PORTA_gc;

                                                              TCA0.SINGLE.CTRLA = 0;
                                                                  TCA0.SINGLE.PER   = LED_BRIGHTNESS_MAX;

                                                                      _apply(s_brightness, s_brightness);

                                                                          TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_SINGLESLOPE_gc
                                                                                                | TCA_SINGLE_CMP2EN_bm
                                                                                                                      | TCA_SINGLE_CMP3EN_bm;

                                                                                                                          // f_PWM = 24 MHz / (64 × 256) ≈ 1.465 kHz
                                                                                                                              TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV64_gc | TCA_SINGLE_ENABLE_bm;
                                                                                                                              }

                                                                                                                              void led_set(uint8_t brightness)
                                                                                                                              {
                                                                                                                                  s_brightness = brightness;
                                                                                                                                      s_layer_a    = brightness;
                                                                                                                                          s_layer_b    = brightness;
                                                                                                                                              if (s_on) _apply(s_layer_a, s_layer_b);
                                                                                                                                              }

                                                                                                                                              void led_step(bool dir, uint8_t step)
                                                                                                                                              {
                                                                                                                                                  if (dir) {
                                                                                                                                                          s_brightness = (s_brightness > LED_BRIGHTNESS_MAX - step)
                                                                                                                                                                               ? LED_BRIGHTNESS_MAX
                                                                                                                                                                                                    : s_brightness + step;
                                                                                                                                                                                                        } else {
                                                                                                                                                                                                                s_brightness = (s_brightness < LED_BRIGHTNESS_MIN + step)
                                                                                                                                                                                                                                     ? LED_BRIGHTNESS_MIN
                                                                                                                                                                                                                                                          : s_brightness - step;
                                                                                                                                                                                                                                                              }

                                                                                                                                                                                                                                                                  s_layer_a = s_brightness;
                                                                                                                                                                                                                                                                      s_layer_b = s_brightness;
                                                                                                                                                                                                                                                                          if (s_on) _apply(s_layer_a, s_layer_b);
                                                                                                                                                                                                                                                                          }

                                                                                                                                                                                                                                                                          void led_update_layer(uint8_t layer)
                                                                                                                                                                                                                                                                          {
                                                                                                                                                                                                                                                                              switch (layer) {
                                                                                                                                                                                                                                                                                      case 0:
                                                                                                                                                                                                                                                                                                  s_layer_a = s_brightness;
                                                                                                                                                                                                                                                                                                              s_layer_b = s_brightness;
                                                                                                                                                                                                                                                                                                                          break;
                                                                                                                                                                                                                                                                                                                                  case 1:
                                                                                                                                                                                                                                                                                                                                              s_layer_a = _add_clamped(s_brightness, LED_BRIGHTNESS_DELTA);
                                                                                                                                                                                                                                                                                                                                                          s_layer_b = s_brightness;
                                                                                                                                                                                                                                                                                                                                                                      break;
                                                                                                                                                                                                                                                                                                                                                                              default:
                                                                                                                                                                                                                                                                                                                                                                                          s_layer_a = _add_clamped(s_brightness, LED_BRIGHTNESS_DELTA);
                                                                                                                                                                                                                                                                                                                                                                                                      s_layer_b = _add_clamped(s_brightness, LED_BRIGHTNESS_DELTA);
                                                                                                                                                                                                                                                                                                                                                                                                                  break;
                                                                                                                                                                                                                                                                                                                                                                                                                      }
                                                                                                                                                                                                                                                                                                                                                                                                                          if (s_on) _apply(s_layer_a, s_layer_b);
                                                                                                                                                                                                                                                                                                                                                                                                                          }

                                                                                                                                                                                                                                                                                                                                                                                                                          void led_off(void)
                                                                                                                                                                                                                                                                                                                                                                                                                          {
                                                                                                                                                                                                                                                                                                                                                                                                                              s_on = false;
                                                                                                                                                                                                                                                                                                                                                                                                                                  _disable_output();
                                                                                                                                                                                                                                                                                                                                                                                                                                  }

                                                                                                                                                                                                                                                                                                                                                                                                                                  void led_on(void)
                                                                                                                                                                                                                                                                                                                                                                                                                                  {
                                                                                                                                                                                                                                                                                                                                                                                                                                      s_on = true;
                                                                                                                                                                                                                                                                                                                                                                                                                                          _apply(s_layer_a, s_layer_b);
                                                                                                                                                                                                                                                                                                                                                                                                                                              _enable_output();
                                                                                                                                                                                                                                                                                                                                                                                                                                              }

                                                                                                                                                                                                                                                                                                                                                                                                                                              void led_toggle(void)
                                                                                                                                                                                                                                                                                                                                                                                                                                              {
                                                                                                                                                                                                                                                                                                                                                                                                                                                  if (s_on) { led_off(); } else { led_on(); }
                                                                                                                                                                                                                                                                                                                                                                                                                                                  }

                                                                                                                                                                                                                                                                                                                                                                                                                                                  bool led_is_on(void)
                                                                                                                                                                                                                                                                                                                                                                                                                                                  {
                                                                                                                                                                                                                                                                                                                                                                                                                                                      return s_on;
                                                                                                                                                                                                                                                                                                                                                                                                                                                      }

                                                                                                                                                                                                                                                                                                                                                                                                                                                      uint8_t led_get_brightness(void)
                                                                                                                                                                                                                                                                                                                                                                                                                                                      {
                                                                                                                                                                                                                                                                                                                                                                                                                                                          return s_brightness;
                                                                                                                                                                                                                                                                                                                                                                                                                                                          }