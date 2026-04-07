/* matrix.c — Key matrix scan for Citrouille90 / AVR64DU32
 * Column-output-low / row-input-pullup scheme.
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Pin assignment (32-pin package):
 *
 *   Columns (output)
 *     PF0–PF5  →  cols 1–6   (COL_F_MASK 0x3F)
 *     PD5–PD7  →  cols 7–9   (COL_D_MASK 0xE0)
 *
 *   Rows (input, pull-up, active-low)
 *     PD0–PD4  →  rows 1–5   (ROW_D_MASK 0x1F)
 *     PA3      →  row  6
 *     PA6      →  row  7
 *     PA2      →  row  8
 *     PA1      →  row  9
 *     PA0      →  row 10     (ROW_A_MASK 0x4F)
 *
 * Note: PORTD is mixed — PD0–PD4 are row inputs, PD5–PD7 are col outputs.
 */

#include <avr/io.h>
#include <stdint.h>
#include "gpio.h"
#include "matrix.h"

/* ---------------------------------------------------------
 * Port-wide masks
 * --------------------------------------------------------- */
#define COL_F_MASK  0x3F    /* PF0–PF5  cols 1–6  */
#define COL_D_MASK  0xE0    /* PD5–PD7  cols 7–9  */
#define ROW_D_MASK  0x1F    /* PD0–PD4  rows 1–5  */
#define ROW_A_MASK  0x4F    /* PA0–PA3, PA6 rows 6–10 */

/* ---------------------------------------------------------
 * Column pin descriptors
 * --------------------------------------------------------- */
static const gpio_pin_t COL_1  = GPIO_PIN(PORTF, 0);
static const gpio_pin_t COL_2  = GPIO_PIN(PORTF, 1);
static const gpio_pin_t COL_3  = GPIO_PIN(PORTF, 2);
static const gpio_pin_t COL_4  = GPIO_PIN(PORTF, 3);
static const gpio_pin_t COL_5  = GPIO_PIN(PORTF, 4);
static const gpio_pin_t COL_6  = GPIO_PIN(PORTF, 5);
static const gpio_pin_t COL_7  = GPIO_PIN(PORTD, 5);
static const gpio_pin_t COL_8  = GPIO_PIN(PORTD, 6);
static const gpio_pin_t COL_9  = GPIO_PIN(PORTD, 7);

/* ---------------------------------------------------------
 * Row pin descriptors (init only — pull-up config)
 * --------------------------------------------------------- */
static const gpio_pin_t ROW_1  = GPIO_PIN(PORTD, 0);
static const gpio_pin_t ROW_2  = GPIO_PIN(PORTD, 1);
static const gpio_pin_t ROW_3  = GPIO_PIN(PORTD, 2);
static const gpio_pin_t ROW_4  = GPIO_PIN(PORTD, 3);
static const gpio_pin_t ROW_5  = GPIO_PIN(PORTD, 4);
static const gpio_pin_t ROW_6  = GPIO_PIN(PORTA, 3);
static const gpio_pin_t ROW_7  = GPIO_PIN(PORTA, 6);
static const gpio_pin_t ROW_8  = GPIO_PIN(PORTA, 2);
static const gpio_pin_t ROW_9  = GPIO_PIN(PORTA, 1);
static const gpio_pin_t ROW_10 = GPIO_PIN(PORTA, 0);

/* ---------------------------------------------------------
 * PORTA row bit -> logical row index (0-based)
 * __builtin_ctz(bit) gives the pin number as the LUT index.
 *   PA0 → row 9   PA1 → row 8   PA2 → row 7
 *   PA3 → row 5   PA6 → row 6
 * 0xFF marks unused/invalid entries.
 * --------------------------------------------------------- */
static const uint8_t row_a_lut[8] = {
    9, 8, 7, 5, 0xFF, 0xFF, 6, 0xFF
};

/* ---------------------------------------------------------
 * matrix_init
 * --------------------------------------------------------- */
void matrix_init(void) {
    /* Columns: latch high before enabling output to avoid glitch */
    PORTF.OUTSET = COL_F_MASK;
    PORTF.DIRSET = COL_F_MASK;
    PORTD.OUTSET = COL_D_MASK;
    PORTD.DIRSET = COL_D_MASK;

    /* Rows: inputs at reset by default — enable pull-ups only */
    GPIO_PULLUP_ENABLE(ROW_1);
    GPIO_PULLUP_ENABLE(ROW_2);
    GPIO_PULLUP_ENABLE(ROW_3);
    GPIO_PULLUP_ENABLE(ROW_4);
    GPIO_PULLUP_ENABLE(ROW_5);
    GPIO_PULLUP_ENABLE(ROW_6);
    GPIO_PULLUP_ENABLE(ROW_7);
    GPIO_PULLUP_ENABLE(ROW_8);
    GPIO_PULLUP_ENABLE(ROW_9);
    GPIO_PULLUP_ENABLE(ROW_10);
}

/* ---------------------------------------------------------
 * scan_col — drive one column low, read both row ports,
 *            dispatch any active (low) rows, release column.
 * --------------------------------------------------------- */
static inline void scan_col(const gpio_pin_t col, uint8_t col_idx) {
    GPIO_LOW(col);
    __builtin_avr_nop();                        /* RC settle */

    uint8_t rows_d = ~PORTD.IN & ROW_D_MASK;   /* single IN read */
    uint8_t rows_a = ~PORTA.IN & ROW_A_MASK;   /* single IN read */

    GPIO_HIGH(col);

    /* PORTD rows: PD0–PD4 map directly to row indices 0–4 */
    uint8_t rd = rows_d;
    while (rd) {
        uint8_t bit = rd & -rd;                 /* isolate lowest set bit */
        register_key(col_idx, __builtin_ctz(bit));
        rd &= rd - 1;                           /* clear lowest set bit   */
    }

    /* PORTA rows: non-contiguous pins resolved via LUT */
    uint8_t ra = rows_a;
    while (ra) {
        uint8_t bit = ra & -ra;
        register_key(col_idx, row_a_lut[__builtin_ctz(bit)]);
        ra &= ra - 1;
    }
}

/* ---------------------------------------------------------
 * matrix_scan — call once per 1KHz tick
 * --------------------------------------------------------- */
void matrix_scan(void) {
    scan_col(COL_1, 0);
    scan_col(COL_2, 1);
    scan_col(COL_3, 2);
    scan_col(COL_4, 3);
    scan_col(COL_5, 4);
    scan_col(COL_6, 5);
    scan_col(COL_7, 6);
    scan_col(COL_8, 7);
    scan_col(COL_9, 8);
}
