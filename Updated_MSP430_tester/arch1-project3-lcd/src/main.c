
#include <msp430.h>
#include "lcdutils.h"
#include "lcddraw.h"
#include "buzzer.h"
#include "wdt_handler.h"

volatile uint8_t game_state = 0; // 0: Start screen, 1: Playing, 2: Game over

void game_setup() {
  configureClocks();
  lcd_init();
  buzzer_init();
  enableWDTInterrupts();
  or_sr(0x8); // GIE (General Interrupt Enable)
}

void display_start_screen() {
  clearScreen(COLOR_BLACK);
  drawString5x7(20, 60, "START GAME", COLOR_GREEN, COLOR_BLACK);
}

void display_game() {
  clearScreen(COLOR_BLACK);
  drawString5x7(20, 60, "PLAYING...", COLOR_WHITE, COLOR_BLACK);
}

void display_game_over() {
  clearScreen(COLOR_BLACK);
  drawString5x7(20, 60, "GAME OVER", COLOR_RED, COLOR_BLACK);
}

void wdt_c_handler() {
  static int state_count = 0;
  state_count++;
  if (state_count == 250) { // Example timer interval
    if (game_state == 1) {
      display_game();
    }
    state_count = 0;
  }
}

int main() {
  game_setup();
  while (1) {
    switch (game_state) {
      case 0: // Start screen
        display_start_screen();
        if (button_pressed) { // Simulate button press
          game_state = 1;
          button_pressed = 0;
        }
        break;
      case 1: // Playing
        display_game();
        break;
      case 2: // Game over
        display_game_over();
        break;
    }
  }
  return 0;
}
