
#include <msp430.h>
#include <stdlib.h>  // For rand()
#include <math.h>    // For abs()
#include "libTimer.h"
#include "lcdutils.h"
#include "lcddraw.h"

// Player and Coin Configurations
#define PLAYER_SIZE 10
#define COIN_SIZE 10
#define PLAYER_COLOR COLOR_BLUE
#define COIN_COLOR COLOR_YELLOW
#define BG_COLOR COLOR_BLACK

// Button Configuration
#define LEFT_BUTTON BIT0
#define RIGHT_BUTTON BIT1
#define SWITCHES (LEFT_BUTTON | RIGHT_BUTTON)

// Score Variables
int score = 0;

// Player and Coin Variables
int player_position = 1;  // Player starts in the center (0: left, 1: center, 2: right)
int coin_position = 1;    // Coin starts in the center
int coin_y = 20;          // Vertical position of the coin

void draw_player() {
    fillRectangle(20 + player_position * 40, 120, PLAYER_SIZE, PLAYER_SIZE, PLAYER_COLOR);
}

void draw_coin() {
    fillRectangle(20 + coin_position * 40, coin_y, COIN_SIZE, COIN_SIZE, COIN_COLOR);
}

void clear_coin() {
    fillRectangle(20 + coin_position * 40, coin_y, COIN_SIZE, COIN_SIZE, BG_COLOR);
}

void display_score() {
    char buffer[10];
    sprintf(buffer, "Score: %d", score);
    drawString5x7(10, 10, buffer, COLOR_WHITE, BG_COLOR);
}

void game_update() {
    clear_coin(); // Erase the coin's previous position
    coin_y += 5;  // Move the coin downward

    if (coin_y > 140) { // Reset if it goes off-screen
        coin_y = 20;
        coin_position = rand() % 3; // Randomize horizontal position
    }

    draw_coin(); // Draw the coin

    if (coin_y >= 120 && coin_position == player_position) { // Coin caught
        score++;            // Increment score
        coin_y = 20;       // Reset coin position
        coin_position = rand() % 3; // Randomize position
        display_score();    // Update the score display
    }

    if (score >= 10) { // Win condition
        clearScreen(COLOR_GREEN); // Turn the screen green
        while (1);                // Freeze the game
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
        game_update();   // Update the falling coin
        __delay_cycles(500000); // Smooth animation
    }
}
