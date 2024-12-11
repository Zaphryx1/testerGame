#include <msp430.h>
#include "mspapi.h"


// -------------------- GAME STATES ------------------
/*
 * Phase in which the game waits for a player interaction to start the game.
 * Next state is always INIT_WAIT.
 */
#define LOBBY        0
/*
 * Phase in which the program waits 5 seconds with the red led on before beginning the game.
 * If the user presses the button in this state the game new state will be GAME_ABORT.
 * If the state times out correctly the next state will become PLAYER_INPUT.
 */
#define INIT_WAIT    1
/*
 * Phase used only to signal to the main routine that the user pressed the button before the game started.
 * The main routine will display the values and enter SCORE_SHOW state.
 */
#define GAME_ABORT   2
/*
 * Phase in which no led is on and the user should press the button as soon as he can.
 * In every case the next state is GAME_END and the result is decided trough the timer counter.
 */
#define PLAYER_INPUT 3
/*
 * Phase used only to signal to the main routine that the game ended.
 * Both button interaction and phase timeout will lead to this state.
 * The main routine will then display the results and enter SCORE_SHOW.
 */
#define GAME_END     4
/*
 * Phase in which the result is displayed using the leds present in the MSP430.
 * There is no need to clear this state as the next round start is decided only trough the button.
 */
#define SCORE_SHOW   5

// Use button P1.1 (known as button 2)
#define START_BUTTON BUTTON_2
#define STOP_BUTTON  BUTTON_1


void setup();
void run();


/**
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    setup(); // Setup leds, button and timer
    run();// Start the game

    return 0;
}

// -------------------- SETUP --------------------

void setup_timer() {
    TA0CTL |= TASSEL__SMCLK | ID__8 | MC__UP;
    // 1'000'000 / 8 = 125000 Hz
    TA0CCR0 = 12500;
    // 125000 / 12500 = 10 Hz

    // f = 10 Hz
    // T = 0.1 s

    set_off(TA0CTL, TAIFG);// Reset interrupt
    set_on(TA0CTL, TAIE);// Enable interrupt
}

void setup() {
    setup_led(RED_LED);
    setup_led(GREEN_LED);
    setup_button(START_BUTTON, PULL_UP, FALLING_EDGE, INTERRUPT_DISABLED);
    setup_button(STOP_BUTTON, PULL_UP, FALLING_EDGE, INTERRUPT_ENABLED);
    setup_timer();
    __enable_interrupt();
}

// -------------------- END SETUP --------------------
// -------------------- PROGRAM --------------------


int state = LOBBY;// State holder
int counter = 0; // Counter updated by the timer

void run() {
    while (1) {
        while (!is_pressed(START_BUTTON));// Wait until the start button gets pressed
        counter = 0;// Reset the counter
        led_on(RED_LED);// Light up the red led
        state = INIT_WAIT;
        while (state == INIT_WAIT);// Wait until the state changes
        if (state == GAME_ABORT) {// Check if the user pressed the button before the game started
            // Keep the red led on
            counter = 0;
            state = SCORE_SHOW;
            while (state == SCORE_SHOW);
            led_off(RED_LED);
            continue;// Next round
        }
        // state == PLAYER_INPUT
        led_off(RED_LED);// Shut down the red led
        counter = 0;// Reset counter
        while (state == PLAYER_INPUT);// Wait until user interaction or phase timeout
        if (counter < 2) {// reflex < 200ms
            led_on(GREEN_LED);
        } else if (counter < 5) {// 200 <= reflex < 500ms
            led_on(GREEN_LED);
            led_on(RED_LED);
        } else {// reflex >= 500ms
            led_on(RED_LED);
        }
        counter = 0;
        state = SCORE_SHOW;
        while (state == SCORE_SHOW);// Wait until phase timeout
        // Turn off all leds
        led_off(GREEN_LED);
        led_off(RED_LED);
        // Ready for next round!
    }
}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void timer1_isr(void) {
    switch (state) {
    case INIT_WAIT:
        if (++counter >= 50) {
            // Init timeout reached (5 seconds)
            state = PLAYER_INPUT;
        }
        break;
    case PLAYER_INPUT:
        if (++counter >= 20) {
            // Game timeout reached (2 seconds of reflex?)
            state = GAME_END;
        }
        break;
    case SCORE_SHOW:
        if (++counter >= 20) {
            // Score show timeout reached (2 seconds)
            state = LOBBY;
        }
        break;
    }
    set_off(TA0CTL, TAIFG);
}

#pragma vector=BTN_VECTOR(STOP_BUTTON)
__interrupt void stop_button_isr(void) {
    switch (state) {
    case PLAYER_INPUT:
        // Normal player input (end game normally)
        state = GAME_END;
        break;
    case INIT_WAIT:
        // Player input in the init phase (abort the game)
        state = GAME_ABORT;
        break;
    }
    reset_interrupt(STOP_BUTTON);
}

// -------------------- END PROGRAM --------------------
