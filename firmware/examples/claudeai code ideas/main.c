// main.c - AmberClick90 Keyboard Firmware
// Main entry point and system initialization

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include "config.h"
#include "keyboard.h"

// Millisecond counter for timing
volatile uint32_t system_millis = 0;

// Timer interrupt for 1ms tick (using TCB0)
ISR(TCB0_INT_vect) {
    system_millis++;
    TCB0.INTFLAGS = TCB_CAPT_bm;  // Clear interrupt flag
}

// Public function to get current milliseconds
uint32_t millis(void) {
    uint32_t ms;
    cli();
    ms = system_millis;
    sei();
    return ms;
}

void system_init(void) {
    // Configure system clock (AVR64DU32 defaults to 24MHz internal oscillator)
    // No need to change if using default
    
    // Configure 1ms timer using TCB0
    TCB0.CCMP = (F_CPU / 1000) - 1;  // 1ms period
    TCB0.INTCTRL = TCB_CAPT_bm;      // Enable compare interrupt
    TCB0.CTRLA = TCB_CLKSEL_DIV1_gc | TCB_ENABLE_bm;  // Enable with no prescaler
    
    // Enable global interrupts
    sei();
}

int main(void) {
    // Disable watchdog timer
    wdt_disable();
    
    // Initialize system
    system_init();
    
    // Initialize keyboard
    keyboard_init();
    
    // Wait for USB to be ready
    _delay_ms(100);
    
    // Main loop
    while (1) {
        keyboard_task();
        
        // Small delay to prevent overwhelming the system
        // Adjust based on your needs - 1ms gives 1000Hz scan rate
        _delay_ms(1);
    }
    
    return 0;
}