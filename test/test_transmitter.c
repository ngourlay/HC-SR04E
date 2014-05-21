#include "unity.h"
#include "transmitter.h"
#include "stub_io.h"

void setUp(void)
{
  tx_init();
}

void tearDown(void)
{
}

void test_DDRsCorrectAfterCreate(void)
{
  TEST_ASSERT_EQUAL_HEX8((1<<3)|(1<<7),DDRA & ((1<<3)|(1<<7)));
  TEST_ASSERT_EQUAL_HEX8(1<<2,DDRB & 1<<2);
}

void test_DDRsCorrectAfterDebugCreate(void)
{
  tx_debug(true);
  TEST_ASSERT_EQUAL_HEX8(1<<5,DDRA & 1<<5);
}

void test_PowerOffAfterCreate(void)
{
  TEST_ASSERT_EQUAL_HEX8(1<<3,DDRA & 1<<3);
}

void test_Stop(void)
{
  tx_stop();
  TEST_ASSERT_EQUAL_HEX8(1<<3,PORTA & 1<<3);
}

void test_Start(void)
{
  tx_start();
  TEST_ASSERT_EQUAL_HEX8(0,PORTA & 1<<3);
}

void test_Peak(void)
{
  tx_start();
  tx_trough();
  tx_peak();
  TEST_ASSERT_EQUAL_HEX8(1<<7,PORTA & 1<<7);
  TEST_ASSERT_EQUAL_HEX8(0,PORTB & 1<<2);
}

void test_Trough(void)
{
  tx_start();
  tx_peak();
  tx_trough();
  TEST_ASSERT_EQUAL_HEX8(0,PORTA & 1<<7);
  TEST_ASSERT_EQUAL_HEX8(1<<2,PORTB & 1<<2);
}

