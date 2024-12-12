
#include <msp430.h>
#include "lcdutils.h"
#include "lcddraw.h"
#include "libTimer.h"
#include "buzzer.h"

void display_game_over() {
  clearScreen(COLOR_RED); // Clear screen to red
  drawString5x7(20, 60, "GAME OVER", COLOR_WHITE, COLOR_RED); // Display "GAME OVER" in white
  buzzer_set_period(1000); // Play a tone for Game Over
}

void main() {
  configureClocks();
  lcd_init();
  buzzer_init(); // Initialize the buzzer

  clearScreen(COLOR_BLUE); // Initial game state with blue background

  // Game simulation logic (replace with real collision detection)
  int collision_detected = 0; // Flag to simulate collision
  while (1) {
    // Simulate some game behavior
    __delay_cycles(2000000); // Delay to mimic game loop
    collision_detected = 1;  // Simulate a collision event

    if (collision_detected) {
      display_game_over(); // Show Game Over screen and play buzzer
      break; // Exit the loop on Game Over
    }
  }

  buzzer_set_period(0); // Stop buzzer after Game Over
  or_sr(0x18); // CPU off, GIE on
}
