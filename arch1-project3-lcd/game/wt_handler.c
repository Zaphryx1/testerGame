
#include <msp430.h>

void __attribute__((interrupt(WDT_VECTOR))) wt_c_handler() {
    // Minimal Watchdog Timer handler implementation
    // This could handle periodic game updates if needed
}
