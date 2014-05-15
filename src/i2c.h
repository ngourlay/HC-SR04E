#include <inttypes.h>

void
i2c_init( uint8_t );

void
i2c_transmit_byte( uint8_t );

uint8_t
i2c_receive_byte( void );

uint8_t
i2c_data_in_receive_buffer( void );

