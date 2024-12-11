#include <msp430.h>
#include "libTimer.h"
#include "lcdutils.h"   // For lcd_init and COLOR constants
#include "lcddraw.h"    // For clearScreen
#include "clock.h"      // For initClock, updateClock, drawClock

void main() {
    configureClocks();           // Set up clocks
    lcd_init();                  // Initialize the LCD
    clearScreen(COLOR_BLACK);    

    initClock();                 // Initialize the clock logic
    enableWDTInterrupts();       // Enable the Watchdog Timer for periodic interrupts
    or_sr(0x18);                 // CPU off, GIE on

    while (1) {                  // Enter low-power mode
        or_sr(0x10);
    }
}
