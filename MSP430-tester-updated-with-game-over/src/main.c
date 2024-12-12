#include <msp430.h>



#include "lcdutils.h"
#include "lcddraw.h"

void display_game_over() {
  clearScreen(COLOR_RED); // Clear screen to red
  drawString5x7(20, 60, "GAME OVER", COLOR_WHITE, COLOR_RED); // Display "GAME OVER" in white
}
#include "libTimer.h"



#include "buzzer.h"



void main() {

  configureClocks();

  lcd_init();

  buzzer_init();  // Initialize the buzzer



  buzzer_set_period(1000); // Play a 1 kHz tone for testing

  or_sr(0x18);             // CPU off, GIE on

}
