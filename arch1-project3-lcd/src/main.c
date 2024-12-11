
#include <msp430.h>
#include "lcdutils.h"
#include "lcddraw.h"
#include "buzzer.h"
#include "wdt_handler.h"

volatile uint8_t game_state = 0; // 0: Start screen, 1: Playing, 2: Game over
volatile uint8_t player_position = 1; // Default position (1: middle)
volatile uint8_t falling_object_position = 0; // Random falling position (0: left, 1: middle, 2: right)
volatile uint8_t success_count = 0; // Count of successful dodges

void game_setup() {
  configureClocks();
  lcd_init();
  buzzer_init();
  enableWDTInterrupts();
  P1DIR |= BIT0 | BIT6;  // LEDs as output
  P1OUT &= ~(BIT0 | BIT6); // Turn off LEDs initially
  or_sr(0x8); // GIE (General Interrupt Enable)
}

void display_start_screen() {
  clearScreen(COLOR_BLACK);
  drawString5x7(20, 60, "START GAME", COLOR_GREEN, COLOR_BLACK);
}

void display_game() {
  clearScreen(COLOR_BLACK);
  drawString5x7(20, 10, "Dodges:", COLOR_WHITE, COLOR_BLACK);
  drawChar5x7(60, 10, '0' + success_count, COLOR_WHITE, COLOR_BLACK);
  
  // Draw the player
  switch (player_position) {
    case 0: drawString5x7(10, 100, "P", COLOR_BLUE, COLOR_BLACK); break;
    case 1: drawString5x7(60, 100, "P", COLOR_BLUE, COLOR_BLACK); break;
    case 2: drawString5x7(110, 100, "P", COLOR_BLUE, COLOR_BLACK); break;
  }

  // Draw the falling object
  switch (falling_object_position) {
    case 0: drawString5x7(10, 20, "X", COLOR_RED, COLOR_BLACK); break;
    case 1: drawString5x7(60, 20, "X", COLOR_RED, COLOR_BLACK); break;
    case 2: drawString5x7(110, 20, "X", COLOR_RED, COLOR_BLACK); break;
  }
}

void display_game_over() {
  clearScreen(COLOR_BLACK);
  drawString5x7(20, 60, "GAME OVER", COLOR_RED, COLOR_BLACK);
}

void wdt_c_handler() {
  static int frame_count = 0;
  frame_count++;

  if (frame_count == 125) { // Roughly every 0.5s
    if (game_state == 1) {
      // Update falling object position
      falling_object_position = (falling_object_position + 1) % 3;

      // Check for collision
      if (falling_object_position == player_position) {
        game_state = 2; // Game over
        P1OUT |= BIT6; // Turn on red LED
        P1OUT &= ~BIT0; // Turn off green LED
      } else {
        success_count++;
        P1OUT ^= BIT0; // Toggle green LED on successful dodge
      }
    }
    frame_count = 0;
  }
}

int main() {
  game_setup();
  while (1) {
    switch (game_state) {
      case 0: // Start screen
        display_start_screen();
        if (P1IN & BIT3) { // Check for start button press
          game_state = 1;
        }
        break;
      case 1: // Playing
        display_game();
        break;
      case 2: // Game over
        display_game_over();
        if (P1IN & BIT3) { // Reset button press
          game_state = 0;
          success_count = 0;
          P1OUT &= ~(BIT0 | BIT6); // Turn off LEDs
        }
        break;
    }
  }
  return 0;
}
