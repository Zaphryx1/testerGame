
#include <msp430.h>
#include "lcdutils.h"
#include "lcddraw.h"
#include "libTimer.h"

#define BG_COLOR COLOR_BLACK
#define COIN_COLOR COLOR_YELLOW
#define PLAYER_COLOR COLOR_BLUE

int player_pos = 64;  // Player's x position
int coin_pos = 64;    // Coin's x position
int coin_y = 10;      // Coin's y position
int score = 0;

void draw_player() {
    fillRectangle(player_pos, 140, 10, 10, PLAYER_COLOR);
}

void draw_coin() {
    fillRectangle(coin_pos, coin_y, 10, 10, COIN_COLOR);
}

void erase_coin() {
    fillRectangle(coin_pos, coin_y, 10, 10, BG_COLOR);
}

void display_score() {
    char buffer[10];
    sprintf(buffer, "Score: %d", score);
    drawString5x7(5, 5, buffer, COLOR_WHITE, BG_COLOR);
}

void main() {
    configureClocks();
    lcd_init();
    clearScreen(BG_COLOR);

    display_score();
    draw_player();

    while (1) {
        erase_coin();
        coin_y += 5;

        if (coin_y > 150) {  // Reset if off screen
            coin_y = 10;
            coin_pos = (rand() % 3) * 40 + 20;
        }

        draw_coin();

        if (coin_y > 130 && coin_pos == player_pos) {  // Collision detected
            score++;
            coin_y = 10;
            coin_pos = (rand() % 3) * 40 + 20;
            display_score();
        }

        if (score == 10) {
            clearScreen(COLOR_GREEN);  // Win screen
            while (1);  // Stop everything
        }

        __delay_cycles(500000);
    }
}
