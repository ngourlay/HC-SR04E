#include "unity.h"
#include "i2c.h"
#include "stub_io.h"

static uint8_t address = 0b00010000;

// ATtiny24/44/84-dependent definitions
#define DDR_I2C      DDRA
#define PORT_I2C     PORTA
#define PIN_I2C      PINA
#define PORT_I2C_SDA PA6
#define PORT_I2C_SCL PA4
#define PIN_I2C_SDA  PINA6

void doSTART(void)
{
  // setting SCL high will cause an infinite loop,
  // so DON'T DO IT!
  PIN_I2C = (0<<PORT_I2C_SCL)|(0<<PORT_I2C_SDA);
  isr_USI_START_vect();
}

void checkAddress(uint8_t address)
{
  doSTART();
  USIDR = (address<<1);
  isr_USI_OVF_vect();
}

void setRegisterAddress(uint8_t address,uint8_t r)
{
  doSTART();
  USIDR = (address<<1)+0;

  isr_USI_OVF_vect(); // CHECK ADDRESS

  if(USICR & (1<<USIOIE)){
    isr_USI_OVF_vect(); // REQUEST_DATA
  }

  USIDR = r;
  if(USICR & (1<<USIOIE)){
    isr_USI_OVF_vect(); // GET_DATA_AND_SEND_ACK
  }
}

uint8_t getOneByte(uint8_t address,uint8_t r)
{
  uint8_t ret;
  setRegisterAddress(address,r);

  doSTART();
  USIDR = (address<<1)+1;

  isr_USI_OVF_vect(); // CHECK ADDRESS

  if(USICR & (1<<USIOIE)){
    isr_USI_OVF_vect(); // SEND_DATA
  }

  ret = USIDR;

  if(USICR & (1<<USIOIE)){
    isr_USI_OVF_vect(); // REQUEST_REPLY_FROM_SEND_DATA
  }
  if(USICR & (1<<USIOIE)){
    isr_USI_OVF_vect(); // CHECK_REPLY_FROM_SEND_DATA
  }
  
  return ret;
}

void setOneByte(uint8_t address,uint8_t r,uint8_t b)
{
  doSTART();
  USIDR = (address<<1)+0;

  isr_USI_OVF_vect(); // CHECK ADDRESS

  if(USICR & (1<<USIOIE)){
    isr_USI_OVF_vect(); // REQUEST_DATA
  }

  USIDR = r;
  if(USICR & (1<<USIOIE)){
    isr_USI_OVF_vect(); // GET_DATA_AND_SEND_ACK
  }
  if(USICR & (1<<USIOIE)){
    isr_USI_OVF_vect(); // REQUEST_DATA
  }

  USIDR = b;
  if(USICR & (1<<USIOIE)){
    isr_USI_OVF_vect(); // GET_DATA_AND_SEND_ACK
  }
}

void setUp(void)
{

  PORTA = 0;
  DDRA = 1<<PORT_I2C_SDA;
  USICR = 0;
  USISR = 0;
  PIN_I2C = 0;
  i2c_init(address);
}

void tearDown(void)
{
}

void test_DDRsCorrectAfterCreate(void)
{
  TEST_ASSERT_EQUAL_HEX8(0b00010000,DDRA);
}

void test_PORTsCorrectAfterCreate(void)
{
  TEST_ASSERT_EQUAL_HEX8(0b01010000,PORTA);
}

void test_USICRsCorrectAfterCreate(void)
{
  TEST_ASSERT_EQUAL_HEX8(0b10111000,USICR);
}

void test_USISRsCorrectAfterCreate(void)
{
  TEST_ASSERT_EQUAL_HEX8(0b11110000,USISR);
}

void test_OverflowDisabledAfterStartIsr_STOP(void)
{
  PIN_I2C = (1<<PIN_I2C_SDA); // Data line has gone high = STOP
  isr_USI_START_vect();
  TEST_ASSERT_EQUAL_HEX8(0,USICR & (1<<USIOIE));
}

void test_OverflowEnabledAfterStartIsr_START(void)
{
  doSTART();
  TEST_ASSERT_EQUAL_HEX8(1<<USIOIE,USICR & (1<<USIOIE));
}

void test_InCorrectAddressDoesNotWork(void)
{
  checkAddress(address+1);
  TEST_ASSERT_EQUAL_HEX8(0,DDR_I2C & (1<<PORT_I2C_SDA));
}

void test_GlobalAddressWorks(void)
{
  checkAddress(0);
  TEST_ASSERT_EQUAL_HEX8(1<<PORT_I2C_SDA,DDR_I2C & (1<<PORT_I2C_SDA));
}

void test_CorrectAddressWorks(void)
{
  checkAddress(address);
  TEST_ASSERT_EQUAL_HEX8(1<<PORT_I2C_SDA,DDR_I2C & (1<<PORT_I2C_SDA));
}

void test_getOneByteIsCorrect(void)
{
  i2c_setRegister(3,26);
  TEST_ASSERT_EQUAL_UINT8(26,getOneByte(address,3));
}

void test_setOneByteIsCorrect(void)
{
  setOneByte(address,1,43);
  TEST_ASSERT_EQUAL_UINT8(43,getOneByte(address,1));
}

void test_getOneByteWithReadMask(void)
{
  i2c_setRegister(4,0xFF);
  i2c_setRegisterReadWriteMasks(4,0xF0,0x00);
  TEST_ASSERT_EQUAL_HEX8(0x0F,getOneByte(address,4));
}

void test_setOneByteWithWriteMask(void)
{
  i2c_setRegister(4,0);
  i2c_setRegisterReadWriteMasks(4,0x00,0xF0);
  setOneByte(address,4,0xFF);
  TEST_ASSERT_EQUAL_HEX8(0x0F,getOneByte(address,4));
}

