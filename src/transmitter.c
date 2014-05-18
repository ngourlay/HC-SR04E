#ifdef TEST
  #include "stub_io.h"
  #include "stub_interrupt.h"
#else
  #include <avr/io.h>
  #include <avr/interrupt.h>
#endif /* TEST */

#include "transmitter.h"

void
tx_init(bool debug)
{
  DDRA |= (1<<3)|(1<<7);
  DDRB |= (1<<2);

  if(debug){
    DDRA |= (1<<5);
  }
}

