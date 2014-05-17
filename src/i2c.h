#ifndef _I2C_H
#define _I2C_H
#include <inttypes.h>

extern uint8_t registerBank[];

void
i2c_init( const uint8_t i2c_address );

#endif /* _I2C_H */
