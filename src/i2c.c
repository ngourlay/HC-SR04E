#ifdef TEST
  #include "stub_io.h"
  #include "stub_interrupt.h"
#else
  #include <avr/io.h>
  #include <avr/interrupt.h>
#endif /* TEST */

#include "i2c.h"

// ATtiny24/44/84-dependent definitions
#define DDR_I2C      DDRA
#define PORT_I2C     PORTA
#define PIN_I2C      PINA
#define PORT_I2C_SDA PA6
#define PORT_I2C_SCL PA4
#define PIN_I2C_SDA  PINA6

/*! Static Variables
 */

static unsigned char TWI_slaveAddress;
static volatile unsigned char USI_TWI_Overflow_State;


/*! Local variables
 */
static uint8_t TWI_RxBuf[TWI_RX_BUFFER_SIZE];
static volatile uint8_t TWI_RxHead;
static volatile uint8_t TWI_RxTail;

static uint8_t TWI_TxBuf[TWI_TX_BUFFER_SIZE];
static volatile uint8_t TWI_TxHead;
static volatile uint8_t TWI_TxTail;

/*! \brief Flushes the TWI buffers
 */
void flush_i2c_buffers(void)
{
    TWI_RxTail = 0;
    TWI_RxHead = 0;
    TWI_TxTail = 0;
    TWI_TxHead = 0;
}

void i2c_init( unsigned char TWI_ownAddress )
{
  flush_i2c_buffers();
  
  TWI_slaveAddress = TWI_ownAddress;
  
  PORT_I2C |=  (1<<PORT_I2C_SCL);                                 // Set SCL high
  PORT_I2C |=  (1<<PORT_I2C_SDA);                                 // Set SDA high
  DDR_I2C  |=  (1<<PORT_I2C_SCL);                                 // Set SCL as output
  DDR_I2C  &= ~(1<<PORT_I2C_SDA);                                 // Set SDA as input
  USICR    =  (1<<USISIE)|(0<<USIOIE)|                            // Enable Start Condition Interrupt. Disable Overflow Interrupt.
    (1<<USIWM1)|(1<<USIWM0)|                            // Set USI in Two-wire mode. No USI Counter overflow prior
    // to first Start Condition (potentail failure)
    (1<<USICS1)|(0<<USICS0)|(0<<USICLK)|                // Shift Register Clock Source = External, positive edge
    (0<<USITC);
  USISR    = 0xF0;                                                // Clear all flags and reset overflow counter
  USI_TWI_Overflow_State=i2c_StateNone; 
  
}


/*! \brief Puts data in the transmission buffer, Waits if buffer is full.
 */
void i2c_transmit_byte( unsigned char data )
{
  unsigned char tmphead;
  
  tmphead = ( TWI_TxHead + 1 ) & TWI_TX_BUFFER_MASK;         // Calculate buffer index.
  while ( tmphead == TWI_TxTail );                           // Wait for free space in buffer.
  TWI_TxBuf[tmphead] = data;                                 // Store data in buffer.
  TWI_TxHead = tmphead;                                      // Store new index.
}

/*! \brief Returns a byte from the receive buffer. Waits if buffer is empty.
 */
unsigned char i2c_receive_byte( void )
{
  unsigned char tmptail;
  unsigned char tmpRxTail;                                  // Temporary variable to store volatile
  tmpRxTail = TWI_RxTail;                                   // Not necessary, but prevents warnings
  while ( TWI_RxHead == tmpRxTail );
  tmptail = ( TWI_RxTail + 1 ) & TWI_RX_BUFFER_MASK;        // Calculate buffer index
  TWI_RxTail = tmptail;                                     // Store new index
  return TWI_RxBuf[tmptail];                                // Return data from the buffer.
}

/*! \brief Check if there is data in the receive buffer.
 */
unsigned char i2c_data_in_receive_buffer( void )
{
  unsigned char tmpRxTail;                            // Temporary variable to store volatile
  tmpRxTail = TWI_RxTail;                             // Not necessary, but prevents warnings
  return ( TWI_RxHead != tmpRxTail );                 // Return 0 (FALSE) if the receive buffer is empty.
}

/*! \brief Usi start condition ISR
 * Detects the USI_TWI Start Condition and intialises the USI
 * for reception of the "TWI Address" packet.
 */

ISR(USI_START_vect)
{
  // Set default starting conditions for new TWI package
  USI_TWI_Overflow_State = i2c_CHECK_ADDRESS;
  DDR_I2C  &= ~(1<<PORT_I2C_SDA);                                 // Set SDA as input
  
  while ( (PIN_I2C & (1<<PORT_I2C_SCL)) && (!(PIN_I2C & (1<<PORT_I2C_SDA))));
  if (!( PIN_I2C & ( 1 << PIN_I2C_SDA )))
    {
      // NO Stop
      USICR   = (1<<USISIE)|(1<<USIOIE)|                            // Enable Overflow and Start Condition Interrupt. (Keep StartCondInt to detect RESTART)
	(1<<USIWM0)|(1<<USIWM1)|                            // Set USI in Two-wire mode.
	(1<<USICS1)|(0<<USICS0)|(0<<USICLK)|                // Shift Register Clock Source = External, positive edge
	(0<<USITC);
    }
  else
    {     // STOP
      USICR   = (1<<USISIE)|(0<<USIOIE)|                            // Enable Overflow and Start Condition Interrupt. (Keep StartCondInt to detect RESTART)
	(1<<USIWM1)|(0<<USIWM1)|                            // Set USI in Two-wire mode.
	(1<<USICS1)|(0<<USICS0)|(0<<USICLK)|                // Shift Register Clock Source = External, positive edge
	(0<<USITC);
      
    }
  USISR  =    (1<<USISIF)|(1<<USIOIF)|(1<<USIPF)|(1<<USIDC)|      // Clear flags
    (0x0<<USICNT0);                                     // Set USI to sample 8 bits i.e. count 16 external pin toggles.    
  
}


/*! \brief USI counter overflow ISR
 * Handels all the comunication. Is disabled only when waiting
 * for new Start Condition.
 */
ISR(USI_OVF_vect)
{
  unsigned char tmpTxTail;     // Temporary variables to store volatiles
  unsigned char tmpUSIDR;
  
  switch (USI_TWI_Overflow_State)
    {
      // ---------- Address mode ----------
      // Check address and send ACK (and next i2c_SEND_DATA) if OK, else reset USI.
    case i2c_CHECK_ADDRESS:
      if ((USIDR == 0) || (( USIDR>>1 ) == TWI_slaveAddress))
	{
	  
	  if ( USIDR & 0x01 )
	    {
	      USI_TWI_Overflow_State = i2c_SEND_DATA;
	    }
	  else
	    {
	      USI_TWI_Overflow_State = i2c_REQUEST_DATA;
	    }
	  SET_USI_TO_SEND_ACK();
          
	}
      else
	{
	  SET_USI_TO_TWI_START_CONDITION_MODE();          // not my Adress.....
	}
      break;
      
      
      // ----- Master write data mode ------
      // Check reply and goto i2c_SEND_DATA if OK, else reset USI.
    case i2c_CHECK_REPLY_FROM_SEND_DATA:
      if ( USIDR ) // If NACK, the master does not want more data.
	{
	  SET_USI_TO_TWI_START_CONDITION_MODE();
	  return;
	}
      // From here we just drop straight into i2c_SEND_DATA if the master sent an ACK
      
      // Copy data from buffer to USIDR and set USI to shift byte. Next i2c_REQUEST_REPLY_FROM_SEND_DATA
    case i2c_SEND_DATA:
      
      // Get data from Buffer
      tmpTxTail = TWI_TxTail;           // Not necessary, but prevents warnings
      if ( TWI_TxHead != tmpTxTail )
	{
	  TWI_TxTail = ( TWI_TxTail + 1 ) & TWI_TX_BUFFER_MASK;
	  USIDR = TWI_TxBuf[TWI_TxTail];
	}
      else // If the buffer is empty then:
	{
          SET_USI_TO_TWI_START_CONDITION_MODE();
          return;
	}
      USI_TWI_Overflow_State = i2c_REQUEST_REPLY_FROM_SEND_DATA;
      SET_USI_TO_SEND_DATA();
      break;
      
      // Set USI to sample reply from master. Next i2c_CHECK_REPLY_FROM_SEND_DATA
    case i2c_REQUEST_REPLY_FROM_SEND_DATA:
      USI_TWI_Overflow_State = i2c_CHECK_REPLY_FROM_SEND_DATA;
      SET_USI_TO_READ_ACK();
      break;
      
      // ----- Master read data mode ------
      // Set USI to sample data from master. Next i2c_GET_DATA_AND_SEND_ACK.
    case i2c_REQUEST_DATA:
      USI_TWI_Overflow_State = i2c_GET_DATA_AND_SEND_ACK;
      SET_USI_TO_READ_DATA();
      break;
      
      // Copy data from USIDR and send ACK. Next i2c_REQUEST_DATA
    case i2c_GET_DATA_AND_SEND_ACK:
      // Put data into Buffer
      tmpUSIDR = USIDR;             // Not necessary, but prevents warnings
      TWI_RxHead = ( TWI_RxHead + 1 ) & TWI_RX_BUFFER_MASK;
      TWI_RxBuf[TWI_RxHead] = tmpUSIDR;
      
      USI_TWI_Overflow_State = i2c_REQUEST_DATA;
      SET_USI_TO_SEND_ACK();
      break;
    }
}
