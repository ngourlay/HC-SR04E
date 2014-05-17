#ifndef _HC_SR04_H
#define _HC_SR04_H

#include <inttypes.h>
#include "i2c.h"

__inline__ uint8_t getRegisterU8(const uint8_t);
__inline__ uint8_t setRegisterU8(const uint8_t,const uint8_t);

__inline__ uint8_t
getRegisterU8(const uint8_t r)
{
  return registerBank[r];
}

__inline__ uint8_t
setRegisterU8(const uint8_t r,const uint8_t c)
{
  registerBank[r] = c;
}


#endif /* _HC_SR04_H */
