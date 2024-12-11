
#include <msp430.h>
#include "lcdutils.h"
#include "lcddraw.h"
#include "buzzer.h"
#include "wdt_handler.h"

volatile uint8_t game_state = 0; // 0: Start screen, 1: Playing, 2: Paused, 3: Game Over
volatile uint8_t player_position = 1; // Player position: 0 (left), 1 (middle), 2 (right)
volatile uint8_t falling_tool_position = 0; // Falling tool position
volatile uint8_t dodge_count = 0; // Tracks successful dodges

extern void custom_function(); // Placeholder for assembly integration

void setup_game() {
    configureClocks();
    lcd_init();
    buzzer_init();
    enableWDTInterrupts();
    P1DIR |= BIT0 | BIT6; // LEDs as output (P1.0: Green, P1.6: Red)
    P1OUT &= ~(BIT0 | BIT6); // Turn off LEDs
    or_sr(0x8); // Enable global interrupts
}

void play_hyrule_theme() {
    static uint16_t tones[] = {1000, 1200, 1400, 1600, 1800, 0};
    static uint8_t tone_index = 0;
    buzzer_set_period(tones[tone_index]);
    tone_index = (tone_index + 1) % (sizeof(tones) / sizeof(tones[0]));
}

void stop_sound() {
    buzzer_set_period(0);
}

void display_start_screen() {
    clearScreen(COLOR_BLACK);
    drawString11x16(10, 40, "START GAME", COLOR_GREEN, COLOR_BLACK);
    drawString5x7(20, 90, "Press BTN1 to Play", COLOR_WHITE, COLOR_BLACK);
}

void display_gameplay() {
    clearScreen(COLOR_BLACK);
    drawString5x7(10, 10, "Dodges:", COLOR_WHITE, COLOR_BLACK);
    drawChar5x7(80, 10, '0' + dodge_count, COLOR_WHITE, COLOR_BLACK);

    // Draw the player character
    switch (player_position) {
        case 0: drawString5x7(10, 100, "P", COLOR_BLUE, COLOR_BLACK); break;
        case 1: drawString5x7(60, 100, "P", COLOR_BLUE, COLOR_BLACK); break;
        case 2: drawString5x7(110, 100, "P", COLOR_BLUE, COLOR_BLACK); break;
    }

    // Draw the falling tool
    switch (falling_tool_position) {
        case 0: drawString5x7(10, 20, "X", COLOR_RED, COLOR_BLACK); break;
        case 1: drawString5x7(60, 20, "X", COLOR_RED, COLOR_BLACK); break;
        case 2: drawString5x7(110, 20, "X", COLOR_RED, COLOR_BLACK); break;
    }
}

void display_game_over() {
    clearScreen(COLOR_BLACK);
    drawString11x16(10, 40, "GAME OVER", COLOR_RED, COLOR_BLACK);
    drawString5x7(20, 90, "Press BTN1 to Restart", COLOR_WHITE, COLOR_BLACK);
}

void wdt_c_handler() {
    static int frame_count = 0;
    frame_count++;

    if (frame_count == 125) { // Update every ~0.5 seconds
        if (game_state == 1) { // Active gameplay
            play_hyrule_theme(); // Play music
            falling_tool_position = (falling_tool_position + 1) % 3; // Move tool

            if (falling_tool_position == player_position) { // Collision detected
                game_state = 3; // Transition to Game Over state
                P1OUT |= BIT6; // Turn on red LED
                stop_sound();
            } else {
                dodge_count++; // Increment dodge count
                P1OUT ^= BIT0; // Toggle green LED
            }
        }
        frame_count = 0;
    }
}

void button_interrupt_handler() {
    if (P1IFG & BIT3) { // Button 1 pressed (Start/Restart)
        P1IFG &= ~BIT3; // Clear interrupt flag
        if (game_state == 0 || game_state == 3) { // Start or Restart
            game_state = 1; // Transition to Playing
            dodge_count = 0; // Reset score
            P1OUT &= ~(BIT0 | BIT6); // Turn off LEDs
            play_hyrule_theme();
        }
    } else if (P1IFG & BIT4) { // Button 2 pressed (Pause/Resume)
        P1IFG &= ~BIT4; // Clear interrupt flag
        if (game_state == 1) { // Pause
            game_state = 2;
            stop_sound();
        } else if (game_state == 2) { // Resume
            game_state = 1;
            play_hyrule_theme();
        }
    }
}

int main() {
    setup_game();
    while (1) {
        switch (game_state) {
            case 0: // Start Screen
                display_start_screen();
                break;
            case 1: // Gameplay
                display_gameplay();
                break;
            case 2: // Paused
                drawString5x7(20, 80, "PAUSED", COLOR_YELLOW, COLOR_BLACK);
                break;
            case 3: // Game Over
                display_game_over();
                break;
        }
    }
    return 0;
}
