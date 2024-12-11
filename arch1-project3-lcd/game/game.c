
#include <msp430.h>
#include <stdlib.h>  // For rand()
#include "libTimer.h"
#include "lcdutils.h"
#include "lcddraw.h"

// Configurations
#define PLAYER_SIZE 10
#define OBJECT_SIZE 10
#define PLAYER_COLOR COLOR_BLUE
#define OBJECT_COLOR COLOR_RED
#define BG_COLOR COLOR_BLACK

#define LEFT_BUTTON BIT0
#define RIGHT_BUTTON BIT1
#define SWITCHES (LEFT_BUTTON | RIGHT_BUTTON)

int player_position = 1; // Start in the center (0: left, 1: center, 2: right)
int object_position = 1; // Object starts in the center
int object_y = 20;       // Vertical position of the object

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
    clear_object(); // Erase the object's previous position
    object_y += 5;  // Move the object downward

    if (object_y > 140) { // Reset if it goes off-screen
        object_y = 20;
        object_position = rand() % 3; // Randomize horizontal position
    }

    draw_object(); // Draw the object

    if (object_y >= 120 && object_position == player_position) { // Collision detected
        object_y = 20; // Reset object position
        object_position = rand() % 3; // Randomize position
    }
}

void button_handler() {
    if ((P2IN & LEFT_BUTTON) == 0 && player_position > 0) {
        player_position--; // Move left
    } else if ((P2IN & RIGHT_BUTTON) == 0 && player_position < 2) {
        player_position++; // Move right
    }
}

void main() {
    configureClocks();
    lcd_init();
    clearScreen(BG_COLOR);

    // Main game loop
    while (1) {
        draw_player();   // Draw the player
        game_update();   // Update the falling object
        __delay_cycles(500000); // Smooth animation
    }
}
