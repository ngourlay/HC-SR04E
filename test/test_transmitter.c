#include "unity.h"
#include "transmitter.h"
#include "stub_io.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_DDRsCorrectAfterCreate(void)
{
  tx_init(false);
  TEST_ASSERT_EQUAL_HEX8((1<<3)|(1<<7),DDRA & ((1<<3)|(1<<7)));
  TEST_ASSERT_EQUAL_HEX8(1<<2,DDRB & 1<<2);
}

void test_DDRsCorrectAfterDebugCreate(void)
{
  tx_init(true);
  TEST_ASSERT_EQUAL_HEX8((1<<3)|(1<<5)|(1<<7),DDRA & ((1<<3)|(1<<5)|(1<<7)));
  TEST_ASSERT_EQUAL_HEX8(1<<2,DDRB & 1<<2);
}

