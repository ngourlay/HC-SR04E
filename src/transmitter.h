#ifndef _TRANSMITTER_H
#define _TRANSMITTER_H
#include <inttypes.h>
#include <stdbool.h>

// debug will pulse the PA5 (MISO) pin on transmit
void
tx_init(bool debug);

void
tx_fire(void);

#endif /* _TRANSMITTER_H */
