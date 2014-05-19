#ifdef TEST
  #include "stub_io.h"
  #include "stub_interrupt.h"
#else
  #include <avr/io.h>
  #include <avr/interrupt.h>
#endif /* TEST */

#include "transmitter.h"

#define TX1      PA7
#define TX2      PB2
#define MAX232   PA3
#define DBUG     PA5
#define TX1_PORT PORTA
#define TX2_PORT PORTB
#define TX1_DDR  DDRA
#define TX2_DDR  DDRB

static bool debug = false;

void
tx_init( void )
{
  TX1_PORT |= (1<<MAX232); // Turn off the MAX232
  TX1_DDR  |= (1<<MAX232)|(1<<TX1);
  TX2_DDR  |= (1<<TX2);
}

void
tx_power(bool on)
{
  if(on){
    TX1_PORT &= ~(1<<TX1); // Turn off Tx1
    TX2_PORT &= ~(1<<TX2); // Turn off Tx2
    TX1_PORT &= ~(1<<MAX232); // Turn on the MAX232 (LOW)
  } else {
    TX1_PORT |= (1<<MAX232); // Turn off the MAX232 (HIGH)
  }

  if(debug){
    TX1_PORT &= ~(1<<DBUG);
  }
}

void
tx_debug(bool on)
{
  debug = on;
  if(on){
    TX1_DDR |= (1<<DBUG);
  }
}

void
tx_peak( void )
{
  TX1_PORT |=  (1<<TX1);
  TX2_PORT &= ~(1<<TX2);
}

void
tx_trough( void )
{
  TX1_PORT &= ~(1<<TX1);
  TX2_PORT |=  (1<<TX2);
}

