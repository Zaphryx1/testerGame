#include <msp430.h>
int score = 0;
#include <stdlib.h>  // For rand()
#include <math.h>    // For abs()
#include "libTimer.h"
#include "lcdutils.h"
#include "lcddraw.h"

// Player and Enemy Configurations
#define PLAYER_SIZE 10
#define ENEMY_SIZE 10
#define NUM_ENEMIES 5
#define PLAYER_COLOR COLOR_BLUE
#define ENEMY_COLOR COLOR_YELLOW
#define BG_COLOR COLOR_BLACK

// Button Configuration
#define LEFT_BUTTON BIT0
#define RIGHT_BUTTON BIT1
#define SWITCHES (LEFT_BUTTON | RIGHT_BUTTON)

// Buzzer Configuration
#define BUZZER BIT6 // Buzzer connected to P2.6

// Game State
typedef struct {
    short x, y;
    char active; // 1 if the enemy is active, 0 otherwise
} Enemy;

Enemy enemies[NUM_ENEMIES];
short playerX = screenWidth / 2; // Center horizontally
short playerY = screenHeight - 15; // Near bottom
char gameOver = 0;

int redrawScreen = 1;

// Melody
#define SONG_LENGTH 24
short melody[SONG_LENGTH] = {
    440, 494, 523, 587,     // 4 Sixteenth notes: A4, B4, C5, D5
    659, 698,               // 2 Quarter notes: E5, F5
    784, 880, 988,          // 3 Sixteenth notes: G5, A5, B5
    440, 494, 523, 587,     // 4 Sixteenth notes: A4, B4, C5, D5
    392, 440, 494, 523,     // 4 Sixteenth notes: G4, A4, B4, C5
    659, 698,               // 2 Quarter notes: E5, F5
    0,                      // Quarter rest
    784, 880, 988,          // 3 Sixteenth notes: G5, A5, B5
    523, 392                // Half note: C5, G4
};
short noteDurations[SONG_LENGTH] = {
    125, 125, 125, 125,     // 4 Sixteenth notes
    500, 500,               // 2 Quarter notes
    125, 125, 125,          // 3 Sixteenth notes
    125, 125, 125, 125,     // 4 Sixteenth notes
    125, 125, 125, 125,     // 4 Sixteenth notes
    500, 500,               // 2 Quarter notes
    500,                    // Quarter rest
    125, 125, 125,          // 3 Sixteenth notes
    1000, 1000              // Half notes
};
short currentNote = 0;
short noteTicks = 0;

void initGame();
void updateGame();
void drawGame();
void handleInput();
void switch_init();
void play_melody();
void buzzer_init();
void buzzer_set_period(short cycles);
void buzzer_off();

// Main Function
void main() {
    P1DIR |= BIT6;    // LED for debug
    P1OUT |= BIT6;
    configureClocks();
    lcd_init();
    buzzer_init();      // Initialize buzzer
    clearScreen(BG_COLOR);
    switch_init();      // Initialize buttons
    enableWDTInterrupts(); // Enable watchdog timer interrupts
    initGame();         // Initialize the game state

    or_sr(0x18); // CPU off, GIE on

    while (1) {
        if (redrawScreen) {
            redrawScreen = 0;
            drawGame();
        }
        if (gameOver) {
            clearScreen(COLOR_RED); 
            buzzer_off();           
            while (1) {
                // Stuck in game-over state
            }
        }
        P1OUT &= ~BIT6; // Turn off debug LED
        or_sr(0x10);    // CPU off
        P1OUT |= BIT6;  // Turn on debug LED
    }
}

// Initialize Game State
void initGame() {
    for (int i = 0; i < NUM_ENEMIES; i++) {
        enemies[i].active = 0; // All enemies start inactive
    }
}

// Update Game Logic
void updateGame() {
    // Move enemies
    for (int i = 0; i < NUM_ENEMIES; i++) {
        if (enemies[i].active) {
            enemies[i].y += 8; // Move down
            if (enemies[i].y > screenHeight) {
                enemies[i].active = 0; // Deactivate when out of screen
            }
        } else {
            // Spawn a new enemy at random x position
            enemies[i].x = (rand() % (screenWidth - ENEMY_SIZE)) + ENEMY_SIZE / 2;
            enemies[i].y = 0;
            enemies[i].active = 1;
        }
    }

    // Check for collisions
    for (int i = 0; i < NUM_ENEMIES; i++) {
        if (enemies[i].active) {
            // Check if the enemy overlaps with the player
            if (abs(enemies[i].x - playerX) < (PLAYER_SIZE / 2 + ENEMY_SIZE / 2) &&
                abs(enemies[i].y - playerY) < (PLAYER_SIZE / 2 + ENEMY_SIZE / 2)) {
                score++;
                enemies[i].active = 0; // Deactivate enemy after collision
                if (score >= 10) {
                    drawGreenScreen();
                }
            }
        }
    }
            }
        }
    }
}

// Draw Game Screen
void drawGame() {
    clearScreen(BG_COLOR);

    // Draw player square
    fillRectangle(playerX - PLAYER_SIZE / 2, playerY - PLAYER_SIZE / 2,
                  PLAYER_SIZE, PLAYER_SIZE, PLAYER_COLOR);

    // Draw active enemies
    for (int i = 0; i < NUM_ENEMIES; i++) {
        if (enemies[i].active) {
            fillRectangle(enemies[i].x - ENEMY_SIZE / 2, enemies[i].y - ENEMY_SIZE / 2,
                          ENEMY_SIZE, ENEMY_SIZE, ENEMY_COLOR);
        }
    }
}

// Handle Button Input
void handleInput() {
    char switches = ~P2IN & SWITCHES; // Read button states (active low)

    if (switches & LEFT_BUTTON) { // Move left
        playerX -= 5;
        if (playerX < PLAYER_SIZE / 2) {
            playerX = PLAYER_SIZE / 2; // Prevent moving out of bounds
        }
    }
    if (switches & RIGHT_BUTTON) { // Move right
        playerX += 5;
        if (playerX > screenWidth - PLAYER_SIZE / 2) {
            playerX = screenWidth - PLAYER_SIZE / 2; // Prevent moving out of bounds
        }
    }
}

// Watchdog Timer Interrupt Handler
void wdt_c_handler() {
    static int count = 0;

    if (gameOver) {
        buzzer_off(); // Turn off buzzer if game over
        return;
    }

    if (++count >= 125) { // Approx. 2 updates/sec
        count = 0;
        updateGame();
        redrawScreen = 1;
    }

    play_melody(); // Handle melody playback
}

// Button Initialization
void switch_init() {
    P2REN |= SWITCHES;        // Enable resistors for switches
    P2OUT |= SWITCHES;        // Pull-up resistors
    P2DIR &= ~SWITCHES;       // Set buttons as input
    P2IE |= SWITCHES;         // Enable interrupts for switches
    P2IES |= SWITCHES;        // Interrupt on high-to-low transition
    P2IFG &= ~SWITCHES;       // Clear pending interrupts
}

// Play Melody
void play_melody() {
    if (noteTicks <= 0) {
        if (currentNote < SONG_LENGTH) {
            buzzer_set_period(1000000 / melody[currentNote]); // Set frequency
            noteTicks = noteDurations[currentNote];
            currentNote++;
        } else {
            currentNote = 0; // Reset melody
        }
    }
    noteTicks--; // Decrease note timer
}

// Buzzer Functions
void buzzer_init() {
    P2DIR |= BUZZER;       // Set P2.6 as output
    P2SEL |= BUZZER;       // Select TA0.1 output
    TA0CTL = TASSEL_2 + MC_1; // SMCLK, up mode
    TA0CCTL1 = OUTMOD_7;   // Reset/set mode
}

void buzzer_set_period(short cycles) {
    if (cycles == 0) {
        TA0CCR0 = 0;
        TA0CCR1 = 0;
    } else {
        TA0CCR0 = cycles;
        TA0CCR1 = cycles >> 1; // 50% duty cycle
    }
}

void buzzer_off() {
    buzzer_set_period(0);
}

// Port 2 Interrupt Handler for Buttons
void __interrupt_vec(PORT2_VECTOR) Port_2() {
    if (P2IFG & SWITCHES) { // Check if a button caused the interrupt
        P2IFG &= ~SWITCHES; // Clear pending interrupts
        handleInput();      // Process button input
        redrawScreen = 1;   // Request screen redraw
    }
}
