#include <msp430.h>
#include "libTimer.h"

// Buzzer Configuration
#define BUZZER BIT6 // Buzzer connected to P2.6

// Melody Configuration
#define SONG_LENGTH 24
short melody[SONG_LENGTH] = {
    440, 494, 523, 587,     // 4 Sixteenth notes: A4, B4, C5, D5
    659, 698,               // 2 Quarter notes: E5, F5
    784, 880, 988,          // 3 Sixteenth notes: G5, A5, B5
    440, 494, 523, 587,     // 4 Sixteenth notes: A4, B4, C5, D5
    392, 440, 494, 523,     // 4 Sixteenth notes: G4, A4, B4, C5
    659, 698,               // 2 Quarter notes: E5, F5
    0,                      // Quarter rest
    784, 880, 988,          // 3 Sixteenth notes: G5, A5, B5
    523, 392                // Half note: C5, G4
};
short noteDurations[SONG_LENGTH] = {
    125, 125, 125, 125,     // 4 Sixteenth notes
    500, 500,               // 2 Quarter notes
    125, 125, 125,          // 3 Sixteenth notes
    125, 125, 125, 125,     // 4 Sixteenth notes
    125, 125, 125, 125,     // 4 Sixteenth notes
    500, 500,               // 2 Quarter notes
    500,                    // Quarter rest
    125, 125, 125,          // 3 Sixteenth notes
    1000, 1000              // Half notes
};
short currentNote = 0;
short noteTicks = 0;

void buzzer_init();
void buzzer_set_period(short cycles);
void play_melody();
void wdt_c_handler();

void main() {
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer
    configureClocks();        // Configure clocks
    buzzer_init();            // Initialize the buzzer
    enableWDTInterrupts();    // Enable WDT interrupts

    or_sr(0x18); // CPU off, GIE on (interrupts enabled)

    while (1) {
        // Low-power mode; melody is handled in WDT interrupt
        or_sr(0x10); // CPU off
    }
}

void buzzer_init() {
    P2DIR |= BUZZER;       // Set P2.6 as output
    P2SEL |= BUZZER;       // Select TA0.1 output
    TA0CTL = TASSEL_2 + MC_1; // SMCLK, up mode
    TA0CCTL1 = OUTMOD_7;   // Reset/set mode
}

void buzzer_set_period(short cycles) {
    if (cycles == 0) {
        TA0CCR0 = 0;
        TA0CCR1 = 0;
    } else {
        TA0CCR0 = cycles;
        TA0CCR1 = cycles >> 1; // 50% duty cycle
    }
}

void play_melody() {
    if (noteTicks <= 0) {
        if (currentNote < SONG_LENGTH) {
            buzzer_set_period(1000000 / melody[currentNote]); // Set frequency
            noteTicks = noteDurations[currentNote];
            currentNote++;
        } else {
            buzzer_set_period(0); // End melody
            currentNote = 0;      // Restart melody
        }
    }
    noteTicks--; // Decrease note timer
}

// Watchdog Timer interrupt
void __interrupt_vec(WDT_VECTOR) WDT() {
    play_melody();
}
