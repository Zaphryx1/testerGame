
#include <msp430.h>
#include "libTimer.h"
#include "switches.h"
#include "buzzer.h"
#include "lcdutils.h"
#include "lcddraw.h"

#define PLAYER_SIZE 10
#define OBJECT_SIZE 10
#define PLAYER_COLOR COLOR_BLUE
#define OBJECT_COLOR COLOR_RED
#define BG_COLOR COLOR_BLACK

int player_position = 1; // Player starts in the center (0: left, 1: center, 2: right)
int object_position = 1; // Object starts in the center
int object_y = 20;       // Vertical position of the object

void draw_player() {
    fillRectangle(20 + player_position * 40, 120, PLAYER_SIZE, PLAYER_SIZE, PLAYER_COLOR);
}

void draw_object() {
    fillRectangle(20 + object_position * 40, object_y, OBJECT_SIZE, OBJECT_SIZE, OBJECT_COLOR);
}

void clear_object() {
    fillRectangle(20 + object_position * 40, object_y, OBJECT_SIZE, OBJECT_SIZE, BG_COLOR);
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
    if ((P2IN & BIT0) == 0 && player_position > 0) { // Left button
        player_position--;
    } else if ((P2IN & BIT1) == 0 && player_position < 2) { // Right button
        player_position++;
    }
}

void main(void) {
    configureClocks();
    switch_init();
    buzzer_init();
    enableWDTInterrupts();
    lcd_init();
    clearScreen(COLOR_BLACK);

    while (1) {
        draw_player();   // Draw the player
        game_update();   // Update the falling object
        __delay_cycles(500000); // Smooth animation
    }
}
