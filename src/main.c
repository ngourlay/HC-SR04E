#include <avr/io.h>
#include <avr/interrupt.h>
#include "i2c.h"

int main( void )
{
  uint8_t i2c_slaveAddress, temp;

  DDRA  |= 1<<0;
  PORTA |= 1<<0;


  // Own TWI slave address
  i2c_slaveAddress = 0x10;
  i2c_init( i2c_slaveAddress );

  sei();

  for(;;)
  {

    while( i2c_data_in_receive_buffer() )
    {
        temp = i2c_receive_byte();
	if(temp % 2){
	  PORTA ^= 1;
	}
    }
  }
}

