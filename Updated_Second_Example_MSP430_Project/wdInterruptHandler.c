#include <msp430.h>
#include "stateMachines.h"


void
__interrupt_vec(WDT_VECTOR) WDT(){
  switch_state_advance();
}
