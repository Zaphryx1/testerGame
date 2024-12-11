    .sect ".text"
    .global __interrupt_vec_wdt

__interrupt_vec_wdt:
    push R4
    push R5
    call #updateClock
    call #drawClock
    pop R5
    pop R4
    reti
