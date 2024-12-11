
#include <msp430.h>
#include "libTimer.h"
#include "lcdutils.h"
#include "lcddraw.h"

#define STATE_PLAY 0
#define STATE_GAME_OVER 1

volatile unsigned char gameState = STATE_PLAY;
volatile unsigned char playerPosition = 1; // Player position: 0, 1, 2
volatile unsigned char boxPosition = 0;   // Box position: 0, 1, 2
volatile unsigned char boxSpeed = 25;     // Initial box speed (lower is faster)
volatile unsigned int dodgeCount = 0;     // Number of successful dodges

// Function prototypes
void drawGameOverScreen();
void drawGameScreen();
void resetGame();

void main() {
    configureClocks();
    lcd_init();
    clearScreen(COLOR_BLACK);

    P1IE |= BIT3;  // Enable button interrupt for P1.3
    P1REN |= BIT3; // Enable pull-up resistor
    P1OUT |= BIT3;
    P1IFG &= ~BIT3; // Clear interrupt flag

    enableWDTInterrupts(); // Enable Watchdog Timer interrupts
    or_sr(0x18); // CPU off, GIE on

    while (1) {
        if (gameState == STATE_PLAY) {
            drawGameScreen();
        } else if (gameState == STATE_GAME_OVER) {
            drawGameOverScreen();
        }
        or_sr(0x10); // Low-power mode
    }
}

// Button interrupt handler
void __interrupt_vec(PORT1_VECTOR) Port_1() {
    if (P1IFG & BIT3) {
        P1IFG &= ~BIT3; // Clear flag
        if (gameState == STATE_PLAY) {
            playerPosition = (playerPosition + 1) % 3; // Move player to next position
        } else if (gameState == STATE_GAME_OVER) {
            resetGame();
        }
    }
}

// Watchdog timer interrupt handler
void __interrupt_vec(WDT_VECTOR) WDT() {
    static int timerCount = 0;
    if (gameState == STATE_PLAY) {
        timerCount++;
        if (timerCount >= boxSpeed) {
            if (boxPosition == playerPosition) {
                gameState = STATE_GAME_OVER; // Collision detected
            } else {
                dodgeCount++;
                if (dodgeCount % 5 == 0 && boxSpeed > 10) {
                    boxSpeed--; // Speed up every 5 dodges
                }
                boxPosition = (boxPosition + 1) % 3; // Move box to next position
            }
            timerCount = 0;
        }
    }
}

void resetGame() {
    gameState = STATE_PLAY;
    playerPosition = 1;
    boxPosition = 0;
    boxSpeed = 25;
    dodgeCount = 0;
}

void drawGameOverScreen() {
    clearScreen(COLOR_RED);
    drawString5x7(20, 50, "Game Over", COLOR_WHITE, COLOR_RED);
}

void drawGameScreen() {
    clearScreen(COLOR_BLACK);
    // Draw player
    fillRectangle(10, playerPosition * 30 + 10, 10, 10, COLOR_GREEN);
    // Draw box
    fillRectangle(100, boxPosition * 30 + 10, 10, 10, COLOR_RED);
}
