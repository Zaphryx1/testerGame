#include <msp430.h>
#include <stdlib.h>  // For rand()
#include <math.h>    // For abs()
#include "libTimer.h"
#include "lcdutils.h"

#include "lcdutils.h"
#include "lcddraw.h"

void display_game_over() {
  // Clear the screen to a base color (optional)
  clearScreen(COLOR_RED);

  // Draw "GAME OVER" in black with a red outline
  drawString5x7(20, 60, "GAME OVER", COLOR_BLACK, COLOR_RED); // Black text with red background
}

  // Example condition to trigger Game Over (replace with actual game logic)
  if (collision_detected) {
    display_game_over(); // Trigger Game Over display
    while (1) {} // Halt the game loop to keep the Game Over screen visible
  }
