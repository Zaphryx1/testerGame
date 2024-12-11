
#include <msp430.h>
#include "libTimer.h"
#include "lcdutils.h"
#include "lcddraw.h"

// Configurations
#define PLAYER_SIZE 10
#define BLOCK_SIZE 10
#define PLAYER_COLOR COLOR_BLUE
#define BLOCK_COLOR COLOR_YELLOW
#define BG_COLOR COLOR_BLACK

// Button Configuration
#define LEFT_BUTTON BIT0
#define RIGHT_BUTTON BIT1
#define SWITCHES (LEFT_BUTTON | RIGHT_BUTTON)

// Player and Block Variables
int player_position = 1; // Player starts in the center (0: left, 1: center, 2: right)
int block_position = 1;  // Block starts in the center
int block_y = 20;        // Vertical position of the block
int score = 0;           // Score counter

void draw_player() {
    fillRectangle(20 + player_position * 40, 120, PLAYER_SIZE, PLAYER_SIZE, PLAYER_COLOR);
}

void draw_block() {
    fillRectangle(20 + block_position * 40, block_y, BLOCK_SIZE, BLOCK_SIZE, BLOCK_COLOR);
}

void clear_block() {
    fillRectangle(20 + block_position * 40, block_y, BLOCK_SIZE, BLOCK_SIZE, BG_COLOR);
}

void display_score() {
    char buffer[10];
    sprintf(buffer, "Score: %d", score);
    drawString5x7(10, 10, buffer, COLOR_WHITE, BG_COLOR);
}

void display_win_message() {
    drawString5x7(30, 60, "YOU WIN!", COLOR_GREEN, BG_COLOR);
}

void game_update() {
    if (score >= 10) { // Win condition
        display_win_message();
        return;
    }

    clear_block(); // Erase the block's previous position
    block_y += 5;  // Move the block downward

    if (block_y > 140) { // Reset if it goes off-screen
        block_y = 20;
        block_position = rand() % 3; // Randomize horizontal position
    }

    draw_block(); // Draw the block

    if (block_y >= 120 && block_position == player_position) { // Block caught
        score++;            // Increment score
        block_y = 20;       // Reset block position
        block_position = rand() % 3; // Randomize position
        display_score();    // Update the score display
    }
}

void button_handler() {
    if ((P2IN & LEFT_BUTTON) == 0 && player_position > 0) { // Move left
        player_position--;
    } else if ((P2IN & RIGHT_BUTTON) == 0 && player_position < 2) { // Move right
        player_position++;
    }
}

void main(void) {
    configureClocks();
    lcd_init();
    clearScreen(BG_COLOR);

    P2REN |= SWITCHES;  // Enable resistors for buttons
    P2IE |= SWITCHES;   // Enable interrupts from buttons
    P2OUT |= SWITCHES;  // Pull-ups for buttons
    P2DIR &= ~SWITCHES; // Set buttons' bits for input

    display_score(); // Initial score display

    while (1) {
        draw_player();   // Draw the player
        game_update();   // Update the falling block
        __delay_cycles(500000); // Smooth animation
    }
}
