
#include <msp430.h>
#include <stdlib.h>  // For rand()
#include "lcdutils.h"
#include "lcddraw.h"
#include "buzzer.h"

// Player and Enemy Configurations
#define PLAYER_SIZE 10
#define TOOL_SIZE 10
#define PLAYER_COLOR COLOR_BLUE
#define TOOL_COLOR COLOR_RED
#define BG_COLOR COLOR_BLACK

// Button Configuration
#define LEFT_BUTTON BIT0
#define RIGHT_BUTTON BIT1
#define SWITCHES (LEFT_BUTTON | RIGHT_BUTTON)

// LED Configuration
#define GREEN_LED BIT0
#define RED_LED BIT6

// Game State Definitions
#define GAME_START 0
#define GAME_PLAYING 1
#define GAME_OVER 2

volatile uint8_t game_state = GAME_START;
volatile uint8_t player_position = 1; // Player's initial position (0: left, 1: center, 2: right)
volatile uint8_t tool_position = 1;   // Tool's initial position
volatile uint8_t dodges = 0;          // Successful dodges count

// Function Prototypes
void play_hyrule_theme();
void stop_sound();
void reset_game();
void draw_player();
void draw_tool();

void play_hyrule_theme() {
    static uint16_t tones[] = {1000, 1200, 1400, 1600, 1800, 0};
    static uint8_t tone_index = 0;
    buzzer_set_period(tones[tone_index]);
    tone_index = (tone_index + 1) % (sizeof(tones) / sizeof(tones[0]));
}

void stop_sound() {
    buzzer_set_period(0);
}

void reset_game() {
    game_state = GAME_START;
    player_position = 1;
    tool_position = 1;
    dodges = 0;
    P1OUT &= ~(GREEN_LED | RED_LED); // Turn off LEDs
    stop_sound();
    clearScreen(BG_COLOR);
}

void draw_player() {
    fillRectangle(10 + player_position * 40, 120, PLAYER_SIZE, PLAYER_SIZE, PLAYER_COLOR);
}

void draw_tool() {
    fillRectangle(10 + tool_position * 40, 20, TOOL_SIZE, TOOL_SIZE, TOOL_COLOR);
}

void game_update() {
    static int frame_count = 0;
    frame_count++;

    if (frame_count == 125) { // ~0.5 seconds
        if (game_state == GAME_PLAYING) {
            play_hyrule_theme();
            clearScreen(BG_COLOR);
            draw_player();

            // Move tool position randomly
            tool_position = rand() % 3;
            draw_tool();

            // Check collision
            if (tool_position == player_position) {
                game_state = GAME_OVER;
                P1OUT |= RED_LED; // Turn on red LED
                stop_sound();
            } else {
                dodges++;
                P1OUT ^= GREEN_LED; // Toggle green LED on successful dodge
            }
        }
        frame_count = 0;
    }
}

void game_start_screen() {
    clearScreen(BG_COLOR);
    drawString11x16(10, 40, "START GAME", COLOR_GREEN, BG_COLOR);
    drawString5x7(20, 100, "Press BTN1 to Play", COLOR_WHITE, BG_COLOR);
}

void game_over_screen() {
    clearScreen(BG_COLOR);
    drawString11x16(10, 40, "GAME OVER", COLOR_RED, BG_COLOR);
    drawString5x7(20, 100, "Press BTN1 to Restart", COLOR_WHITE, BG_COLOR);
}

void button_handler() {
    if ((P1IN & LEFT_BUTTON) == 0) { // Left button pressed
        if (game_state == GAME_PLAYING && player_position > 0) {
            player_position--;
        }
    } else if ((P1IN & RIGHT_BUTTON) == 0) { // Right button pressed
        if (game_state == GAME_PLAYING && player_position < 2) {
            player_position++;
        }
    } else if ((P1IN & BIT3) == 0) { // BTN1 pressed (Start/Restart)
        if (game_state == GAME_START || game_state == GAME_OVER) {
            game_state = GAME_PLAYING;
            reset_game();
        }
    }
}
