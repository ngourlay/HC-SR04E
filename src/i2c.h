#ifndef _I2C_H
#define _I2C_H
#include <inttypes.h>

void
i2c_init( const uint8_t i2c_address );

uint8_t
i2c_getRegister( const uint8_t reg );

void
i2c_setRegister( const uint8_t reg, const uint8_t data );

void
i2c_setRegisterReadWriteMasks( const uint8_t reg, const uint8_t rmask, const uint8_t wmask );

#endif /* _I2C_H */
