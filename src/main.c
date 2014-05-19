#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "i2c.h"
#include "transmitter.h"

int main( void )
{
  uint8_t i2c_slaveAddress, temp;

  DDRA  |= 1<<PA0;
  PORTA |= 1<<PA0;

  // Own TWI slave address
  i2c_slaveAddress = 0x10;
  i2c_init( i2c_slaveAddress );
  tx_init();

  sei();

  for(;;)
  {

    temp = i2c_getRegister(3);
    if(temp % 2){
      PORTA |= (1<<PA0);
      tx_peak();
    } else {
      PORTA &= ~(1<<PA0);
      tx_trough();
    }
  }
}

