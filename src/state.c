#include "state.h"
#include "transmitter.h"
#include "i2c.h"
#include "HC-SR04E.h"

// SENSOR STATES
static uint8_t sensor_state;
#define STOPPED      0
#define TRANSMITTING 1
#define RECEIVING    2

// EVENTS FROM CONTROL REGISTER
#define STOP       0x00
#define TRANSMIT   0x40
#define RECEIVE    0x80
#define ECHOLOCATE 0xC0

void event_loop( void )
{
  sensor_state = STOPPED;
  tx_init();

  while(1){
    const uint8_t rbcr = i2c_getRegister(RBCR);
    const uint8_t event = rbcr & ((1<<MODE1)|(1<<MODE0));

    switch(sensor_state){

    case(STOPPED):
      switch(event){
      case(STOP):
	// can probably sleep here ??
	break;
      case(TRANSMIT):
	tx_start();
	sensor_state = TRANSMITTING;
	break;
      case(RECEIVE):
	// rx_start();
	sensor_state = RECEIVING;
	break;
      case(ECHOLOCATE):
	tx_start();
	sensor_state = TRANSMITTING;
	break;
      }
      break;

    case(TRANSMITTING):
      switch(event){
      case(STOP):
	tx_stop();
	sensor_state = STOPPED;
	break;
      case(TRANSMIT):
	// send pulses, then once finished, set
	if(tx_finished()){
	  tx_stop();
	  sensor_state = STOPPED;
	}
	break;
      case(RECEIVE):
	tx_stop();
	// rx_start();
	sensor_state = RECEIVING;
	break;
      case(ECHOLOCATE):
	// send pules, then once finished, set
	if(tx_finished()){
	  tx_stop();
	  sensor_state = RECEIVING;
	}
	break;
      }
      break;

    case(RECEIVING):
      switch(event){
      case(STOP):
	// rx_stop();
	sensor_state = STOPPED;
	break;
      case(TRANSMIT):
	// do some set-up here
	sensor_state = TRANSMITTING;
	break;
      case(RECEIVE):
	// do work, then once finished, set
	sensor_state = STOPPED;
	break;
      case(ECHOLOCATE):
	// do work, then once finished, set
	sensor_state = STOPPED;
	break;
      }
      break;
    }
  }
}
