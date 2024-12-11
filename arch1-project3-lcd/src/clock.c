#include "lcddraw.h"
#include "lcdutils.h"
#include <stdio.h>

static u_char hours = 0, minutes = 0, seconds = 0;

// Initialize the clock state
void initClock() {
    hours = 12;   // Set starting time to 12:00:00
    minutes = 0;
    seconds = 0;
}

// Update the clock state (called by the WDT)
void updateClock() {
    seconds++;
    if (seconds == 60) {
        seconds = 0;
        minutes++;
        if (minutes == 60) {
            minutes = 0;
            hours = (hours + 1) % 24;
        }
    }
}

// Draw the clock on the screen
void drawClock() {
    char timeString[9]; // Buffer for "HH:MM:SS"
    sprintf(timeString, "%02d:%02d:%02d", hours, minutes, seconds);

    clearScreen(COLOR_BLACK);
    drawString5x7(40, 70, timeString, COLOR_WHITE, COLOR_BLACK);
}
