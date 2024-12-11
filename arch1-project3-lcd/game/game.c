
#include <msp430.h>
#include <stdlib.h>
#include "lcdutils.h"
#include "lcddraw.h"

// Player and Falling Object Configurations
#define PLAYER_SIZE 10
#define OBJECT_SIZE 10
#define PLAYER_COLOR COLOR_BLUE
#define OBJECT_COLOR COLOR_RED
#define BG_COLOR COLOR_BLACK

// Button Configuration
#define LEFT_BUTTON BIT0
#define RIGHT_BUTTON BIT1
#define SWITCHES (LEFT_BUTTON | RIGHT_BUTTON)

volatile uint8_t player_position = 1; // Player's position (0: left, 1: center, 2: right)
volatile uint8_t object_position = 1; // Falling object's horizontal position
volatile uint8_t object_y = 20;       // Falling object's vertical position

void draw_player() {
    fillRectangle(10 + player_position * 40, 120, PLAYER_SIZE, PLAYER_SIZE, PLAYER_COLOR);
}

void draw_object() {
    fillRectangle(10 + object_position * 40, object_y, OBJECT_SIZE, OBJECT_SIZE, OBJECT_COLOR);
}

void clear_object() {
    fillRectangle(10 + object_position * 40, object_y, OBJECT_SIZE, OBJECT_SIZE, BG_COLOR);
}

void game_update() {
    clear_object(); // Clear the object's previous position
    object_y += 5;  // Move the object down

    if (object_y > 140) { // If the object moves off-screen
        object_y = 20; // Reset to the top of the screen
        object_position = rand() % 3; // Randomize horizontal position
    }

    draw_object(); // Redraw the object at its new position

    // Check for collision
    if (object_y >= 120 && object_position == player_position) {
        object_y = 20; // Reset object position on collision
        object_position = rand() % 3;
    }
}

void button_handler() {
    if ((P1IN & LEFT_BUTTON) == 0 && player_position > 0) { // Move left
        player_position--;
    } else if ((P1IN & RIGHT_BUTTON) == 0 && player_position < 2) { // Move right
        player_position++;
    }
}

void main() {
    configureClocks();
    lcd_init();
    clearScreen(BG_COLOR);

    // Main game loop
    while (1) {
        draw_player(); // Draw the player
        game_update(); // Update the falling object
        __delay_cycles(500000); // Add delay for smoother animation
    }
}
