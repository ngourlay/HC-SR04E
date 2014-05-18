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

uint8_t getOneByte(uint8_t address)
{
  doSTART();
  USIDR = (address<<1)+1;

  isr_USI_OVF_vect(); // CHECK ADDRESS

  if(USICR & (1<<USIOIE)){
    isr_USI_OVF_vect(); // SEND_DATA
  }
  return USIDR;
}

void setUp(void)
{
  registerBank[0] = 42; // just some number to be tested
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
  uint8_t temp = getOneByte(address);
  TEST_ASSERT_EQUAL_UINT8(42,temp);
}
