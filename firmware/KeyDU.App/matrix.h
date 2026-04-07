/* matrix.h — Key matrix interface for Citrouille90 / AVR64DU32
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef MATRIX_H
#define MATRIX_H

#include <stdint.h>

/* Matrix dimensions */
#define MATRIX_COLS     9
#define MATRIX_ROWS     10

/* Initialise column outputs and row input pull-ups. */
void matrix_init(void);

/* Scan the full matrix. Call once per 1KHz tick.
 * Calls register_key(col, row) for every active (pressed) switch,
 * where col and row are 0-based indices. */
void matrix_scan(void);

/* Provided by the keymap/event layer — called for each pressed key. */
void register_key(uint8_t col, uint8_t row);

#endif /* MATRIX_H */
