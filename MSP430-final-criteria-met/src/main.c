
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

  // Game simulation (this should be replaced with actual game logic)
  int collision_detected = 1; // Simulate collision for testing

  if (collision_detected) {
    display_game_over(); // Show Game Over screen and activate buzzer
  }

  buzzer_set_period(0); // Stop buzzer after Game Over logic
  or_sr(0x18); // CPU off, GIE on
}
