#ifndef _TRANSMITTER_H
#define _TRANSMITTER_H
#include <inttypes.h>
#include <stdbool.h>

extern const uint8_t TX_TCCRA;
extern const uint8_t TX_TCCRB;

// debug will pulse the PA5 (MISO) pin on transmit
void
tx_init( void );

void
tx_debug( bool on );

void
tx_power( bool on );

void
tx_peak( void );

void
tx_trough( void );

#endif /* _TRANSMITTER_H */
