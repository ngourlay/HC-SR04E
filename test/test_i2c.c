#include "unity.h"
#include "i2c.h"
#include "stub_io.h"

void setUp(void)
{
  PORTA = 0xFF;
  DDRA = 0xFF;
  i2c_init(66);
}

void tearDown(void)
{
}

void test_DDRsCorrectAfterCreate(void)
{
  TEST_ASSERT_EQUAL_HEX8(0xAF,DDRA);
}

void test_PORTsCorrectAfterCreate(void)
{
  TEST_ASSERT_EQUAL_HEX8(0xAF,PORTA);
}

