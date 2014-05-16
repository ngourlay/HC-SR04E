#include <inttypes.h>
#include <stdbool.h>

void
i2c_init( uint8_t );

void
i2c_transmit_byte( void );

uint8_t
i2c_receive_byte( uint8_t );

