
#include <msp430.h>
#include "lcdutils.h"
#include "lcddraw.h"

// Configurations for the player and the falling object
#define PLAYER_SIZE 10
#define OBJECT_SIZE 10
#define PLAYER_COLOR COLOR_BLUE
#define OBJECT_COLOR COLOR_RED
#define BG_COLOR COLOR_BLACK

// Player and object positions
int player_position = 1; // Middle position (0: left, 1: center, 2: right)
int object_position = 1; // Starting object position (center)
int object_y = 20;       // Starting vertical position of the falling object

// Draw the player on the screen
void draw_player() {
    fillRectangle(10 + player_position * 40, 120, PLAYER_SIZE, PLAYER_SIZE, PLAYER_COLOR);
}

// Draw the falling object on the screen
void draw_object() {
    fillRectangle(10 + object_position * 40, object_y, OBJECT_SIZE, OBJECT_SIZE, OBJECT_COLOR);
}

// Clear the object's previous position from the screen
void clear_object() {
    fillRectangle(10 + object_position * 40, object_y, OBJECT_SIZE, OBJECT_SIZE, BG_COLOR);
}

// Update the game state (object movement and collision detection)
void game_update() {
    clear_object(); // Erase the current object
    object_y += 5;  // Move the object downward

    if (object_y > 140) { // If the object falls off the screen
        object_y = 20;             // Reset to the top of the screen
        object_position = rand() % 3; // Randomize horizontal position (0, 1, or 2)
    }

    draw_object(); // Redraw the object in its new position

    // Check for collisions
    if (object_y >= 120 && object_position == player_position) {
        object_y = 20;             // Reset object position
        object_position = rand() % 3; // Randomize position
    }
}

// Handle button presses to move the player
void button_handler() {
    if ((P2IN & BIT0) == 0 && player_position > 0) { // Move left if not at the leftmost position
        player_position--;
    } else if ((P2IN & BIT1) == 0 && player_position < 2) { // Move right if not at the rightmost position
        player_position++;
    }
}

// Main function
void main() {
    configureClocks();
    lcd_init();
    clearScreen(BG_COLOR);

    while (1) {
        draw_player();   // Draw the player
        game_update();   // Update the falling object
        __delay_cycles(500000); // Delay for smooth movement
    }
}
