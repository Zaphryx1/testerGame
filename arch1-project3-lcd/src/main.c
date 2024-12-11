
#include <msp430.h>
#include "lcdutils.h"
#include "lcddraw.h"
#include "buzzer.h"
#include "wdt_handler.h"

volatile uint8_t game_state = 0; // 0: Start, 1: Playing, 2: Paused, 3: Game Over
volatile uint8_t player_position = 1; // 0: left, 1: middle, 2: right
volatile uint8_t falling_position = 0; // 0: left, 1: middle, 2: right
volatile uint8_t dodges = 0; // Number of successful dodges

extern void custom_function(); // Assembly function placeholder

void setup_game() {
    configureClocks();
    lcd_init();
    buzzer_init();
    enableWDTInterrupts();
    P1DIR |= BIT0 | BIT6; // LEDs (Green on P1.0, Red on P1.6)
    P1OUT &= ~(BIT0 | BIT6); // Turn off both LEDs
    or_sr(0x8); // Enable global interrupts
}

void play_hyrule_theme() {
    static uint16_t tones[] = {1000, 1200, 1500, 1700, 2000, 0}; // 8-bit-like melody
    static uint8_t tone_index = 0;
    buzzer_set_period(tones[tone_index]);
    tone_index = (tone_index + 1) % (sizeof(tones) / sizeof(tones[0]));
}

void stop_sound() {
    buzzer_set_period(0);
}

void display_start_screen() {
    clearScreen(COLOR_BLACK);
    drawString11x16(20, 40, "START GAME", COLOR_GREEN, COLOR_BLACK);
    drawString5x7(25, 90, "Press BTN1", COLOR_WHITE, COLOR_BLACK);
}

void display_game() {
    clearScreen(COLOR_BLACK);
    drawString5x7(10, 10, "Dodges:", COLOR_WHITE, COLOR_BLACK);
    drawChar5x7(70, 10, '0' + dodges, COLOR_WHITE, COLOR_BLACK);

    // Draw the player
    switch (player_position) {
        case 0: drawString5x7(10, 100, "P", COLOR_BLUE, COLOR_BLACK); break;
        case 1: drawString5x7(60, 100, "P", COLOR_BLUE, COLOR_BLACK); break;
        case 2: drawString5x7(110, 100, "P", COLOR_BLUE, COLOR_BLACK); break;
    }

    // Draw the falling object
    switch (falling_position) {
        case 0: drawString5x7(10, 20, "X", COLOR_RED, COLOR_BLACK); break;
        case 1: drawString5x7(60, 20, "X", COLOR_RED, COLOR_BLACK); break;
        case 2: drawString5x7(110, 20, "X", COLOR_RED, COLOR_BLACK); break;
    }
}

void display_game_over() {
    clearScreen(COLOR_BLACK);
    drawString11x16(20, 40, "GAME OVER", COLOR_RED, COLOR_BLACK);
    drawString5x7(25, 90, "Press BTN1", COLOR_WHITE, COLOR_BLACK);
}

void wdt_c_handler() {
    static int frame_count = 0;
    frame_count++;

    if (frame_count == 125) { // ~0.5 seconds interval
        if (game_state == 1) { // Game is in "Playing" state
            play_hyrule_theme(); // Play the Hyrule Field Theme
            falling_position = (falling_position + 1) % 3; // Update falling tool position

            if (falling_position == player_position) { // Collision detected
                game_state = 3; // Game over
                P1OUT |= BIT6; // Turn on red LED
                stop_sound();
            } else {
                dodges++; // Successful dodge
                P1OUT ^= BIT0; // Toggle green LED
            }
        }
        frame_count = 0;
    }
}

void button_interrupt_handler() {
    if (P1IFG & BIT3) { // BTN1 pressed (Start/Restart)
        P1IFG &= ~BIT3; // Clear interrupt flag
        if (game_state == 0 || game_state == 3) { // From Start/Game Over to Playing
            game_state = 1;
            dodges = 0; // Reset dodge counter
            P1OUT &= ~(BIT0 | BIT6); // Turn off LEDs
            play_hyrule_theme();
        }
    } else if (P1IFG & BIT4) { // BTN2 pressed (Pause/Resume)
        P1IFG &= ~BIT4; // Clear interrupt flag
        if (game_state == 1) { // Pause the game
            game_state = 2;
            stop_sound();
        } else if (game_state == 2) { // Resume the game
            game_state = 1;
            play_hyrule_theme();
        }
    }
}

int main() {
    setup_game();
    while (1) {
        switch (game_state) {
            case 0: // Start screen
                display_start_screen();
                break;
            case 1: // Playing
                display_game();
                break;
            case 2: // Paused
                drawString5x7(20, 80, "PAUSED", COLOR_YELLOW, COLOR_BLACK);
                break;
            case 3: // Game over
                display_game_over();
                break;
        }
    }
    return 0;
}
