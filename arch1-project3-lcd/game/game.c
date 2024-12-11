
#include <msp430.h>
#include <stdlib.h>  // For rand()
#include "libTimer.h"
#include "lcdutils.h"
#include "lcddraw.h"

// Player and Object Configurations
#define PLAYER_SIZE 10
#define OBJECT_SIZE 10
#define PLAYER_COLOR COLOR_BLUE
#define OBJECT_COLOR COLOR_RED
#define BG_COLOR COLOR_BLACK

// Button Configuration
#define LEFT_BUTTON BIT0
#define RIGHT_BUTTON BIT1
#define SWITCHES (LEFT_BUTTON | RIGHT_BUTTON)

// Player and object positions
int player_position = 1; // Middle position (0: left, 1: center, 2: right)
int object_position = 1; // Starting position of the falling object (center)
int object_y = 20;       // Vertical position of the falling object

// Draw the player on the screen
void draw_player() {
    fillRectangle(10 + player_position * 40, 120, PLAYER_SIZE, PLAYER_SIZE, PLAYER_COLOR);
}

// Draw the falling object on the screen
void draw_object() {
    fillRectangle(10 + object_position * 40, object_y, OBJECT_SIZE, OBJECT_SIZE, OBJECT_COLOR);
}

// Clear the object's previous position
void clear_object() {
    fillRectangle(10 + object_position * 40, object_y, OBJECT_SIZE, OBJECT_SIZE, BG_COLOR);
}

// Update the game state (object movement and collision detection)
void game_update() {
    clear_object(); // Clear the object's previous position
    object_y += 5;  // Move the object downward

    if (object_y > 140) { // If the object falls off the screen
        object_y = 20;             // Reset to the top
        object_position = rand() % 3; // Randomize horizontal position (0, 1, 2)
    }

    draw_object(); // Redraw the object

    // Check for collision
    if (object_y >= 120 && object_position == player_position) {
        object_y = 20;             // Reset the object position on collision
        object_position = rand() % 3; // Randomize horizontal position
    }
}

// Handle button presses to move the player
void button_handler() {
    if ((P2IN & LEFT_BUTTON) == 0 && player_position > 0) { // Move left
        player_position--;
    } else if ((P2IN & RIGHT_BUTTON) == 0 && player_position < 2) { // Move right
        player_position++;
    }
}

// Main function
void main() {
    configureClocks();
    lcd_init();
    clearScreen(BG_COLOR);

    // Main game loop
    while (1) {
        draw_player();   // Draw the player
        game_update();   // Update the falling object
        __delay_cycles(500000); // Delay for smooth animation
    }
}
