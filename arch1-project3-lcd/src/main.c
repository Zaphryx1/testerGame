
#include <msp430.h>
#include "lcdutils.h"
#include "lcddraw.h"
#include "buzzer.h"
#include "wdt_handler.h"

volatile uint8_t game_state = 0; // 0: Start screen, 1: Playing, 2: Paused, 3: Game over
volatile uint8_t player_position = 1; // Default position (1: middle)
volatile uint8_t falling_object_position = 0; // Random falling position (0: left, 1: middle, 2: right)
volatile uint8_t success_count = 0; // Count of successful dodges

// Assembly function for state handling
extern void custom_function();

void game_setup() {
  configureClocks();
  lcd_init();
  buzzer_init();
  enableWDTInterrupts();
  P1DIR |= BIT0 | BIT6;  // LEDs as output
  P1OUT &= ~(BIT0 | BIT6); // Turn off LEDs initially
  or_sr(0x8); // GIE (General Interrupt Enable)
}

void play_hyrule_theme() {
  // Generate an 8-bit simplified Hyrule Field theme
  static uint16_t notes[] = {1000, 1200, 1500, 1700, 2000, 0}; // Example tones
  static uint8_t note_index = 0;
  buzzer_set_period(notes[note_index]);
  note_index = (note_index + 1) % (sizeof(notes) / sizeof(notes[0]));
}

void stop_sound() {
  buzzer_set_period(0);
}

void display_start_screen() {
  clearScreen(COLOR_BLACK);
  drawString5x7(20, 60, "START GAME", COLOR_GREEN, COLOR_BLACK);
  drawString5x7(20, 80, "Press BTN1", COLOR_WHITE, COLOR_BLACK);
}

void display_game() {
  clearScreen(COLOR_BLACK);
  drawString5x7(10, 10, "Dodges:", COLOR_WHITE, COLOR_BLACK);
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
  drawString5x7(20, 80, "Press BTN1 to Restart", COLOR_WHITE, COLOR_BLACK);
}

void wdt_c_handler() {
  static int frame_count = 0;
  frame_count++;

  if (frame_count == 125) { // Roughly every 0.5s
    if (game_state == 1) { // Only update when playing
      play_hyrule_theme(); // Play music continuously
      falling_object_position = (falling_object_position + 1) % 3; // Update position

      // Check for collision
      if (falling_object_position == player_position) {
        game_state = 3; // Game over
        P1OUT |= BIT6; // Turn on red LED
        P1OUT &= ~BIT0; // Turn off green LED
        stop_sound();
      } else {
        success_count++;
        P1OUT ^= BIT0; // Toggle green LED on successful dodge
      }
    }
    frame_count = 0;
  }
}

void button_interrupt_handler() {
  if (P1IFG & BIT3) { // Button 1 pressed (Start/Restart)
    P1IFG &= ~BIT3;
    if (game_state == 0 || game_state == 3) { // Start or Restart
      game_state = 1;
      success_count = 0;
      P1OUT &= ~(BIT0 | BIT6); // Reset LEDs
      play_hyrule_theme();
    }
  } else if (P1IFG & BIT4) { // Button 2 pressed (Pause/Resume)
    P1IFG &= ~BIT4;
    if (game_state == 1) { // Pause the game
      game_state = 2;
      stop_sound();
    } else if (game_state == 2) { // Resume the game
      game_state = 1;
      play_hyrule_theme();
    }
  }
}

int main() {
  game_setup();
  while (1) {
    switch (game_state) {
      case 0: // Start screen
        display_start_screen();
        break;
      case 1: // Playing
        display_game();
        break;
      case 2: // Paused
        drawString5x7(20, 80, "PAUSED", COLOR_YELLOW, COLOR_BLACK);
        break;
      case 3: // Game over
        display_game_over();
        break;
    }
  }
  return 0;
}
