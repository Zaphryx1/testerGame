#include "switches.h"
#include "buzzer.h"
#include "stateMachines.h"
#include <msp430.h>
#include "lcdutils.h"
#include "lcddraw.h"


void switch_state_advance(){
  u_char width = screenWidth, height = screenHeight;
  int col = 0, row = 0;
  int centerCol = width / 2, centerRow = height / 2;

  switch (switch_state){
  case 1:
    clearScreen(COLOR_BLACK);
    buzzer_set_period(1300);//buzzer is set to a input when a certain button is pressed
    draw_spiral(screenWidth, screenHeight, COLOR_PURPLE);// calls the draw spiral function below.
    drawString5x7(0, 0, "Nice!", COLOR_WHITE, COLOR_BLACK); //make the text display
    break;
  case 2:
    clearScreen(COLOR_BLACK);
    buzzer_set_period(200);
    draw_spiral(screenWidth, screenHeight, COLOR_YELLOW);// calls the draw spiral function below.
    drawString5x7(0, 0, "You did it!", COLOR_FIREBRICK, COLOR_BLACK);
    break;
  case 3:
    clearScreen(COLOR_BLACK);
    buzzer_set_period(700);
    drawString5x7(0, 0, "Congrats!", COLOR_SEA_GREEN, COLOR_BLACK);
    break;
  case 4:
    clearScreen(COLOR_BLACK);
    buzzer_off();
    drawString5x7(0, 0, "Mute!", COLOR_YELLOW, COLOR_BLACK);
    break;

  }
  switch_state =0; 
}

void draw_spiral(int width, int height,u_int RBG_COLOR) {
  int x = width / 2;   // Start at the center of the screen
  int y = height / 2;  // Start at the center of the screen

  int dx = 1, dy = 0;  // Initial direction (moving right)
  int step = 1;        // Number of steps in the current direction
  int steps_left = step;
  int change_direction = 0; // Counter to track direction changes

  for (int i = 0; i < width * height; i++) {
    // Set the pixel at the current position
    drawPixel(x, y, RBG_COLOR);

    // Move in the current direction
    x += dx;
    y += dy;

    // Decrease steps left in the current direction
    steps_left--;

    // If steps are complete in the current direction, change direction
    if (steps_left == 0) {
      change_direction++;
      if (change_direction % 2 == 0) {
	step+=10; // Increase step size every two turns
      }
      steps_left = step;

      // Change direction: right -> down -> left ->
      int temp = dx;
      dx = -dy;
      dy = temp;
    }

    // Stop if we go out of bounds (edge of the LCD)
    if (x < 0 || x >= width || y < 0 || y >= height) {
      break;
    }
  }
}
