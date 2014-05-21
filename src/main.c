#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "i2c.h"
#include "state.h"

int main( void )
{
  const uint8_t i2c_slaveAddress = 0x10;

  i2c_init( i2c_slaveAddress );
  sei();
  event_loop();
}

