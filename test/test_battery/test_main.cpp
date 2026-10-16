#include <unity.h>

#include "BatteryGauge.h"

void setUp() {}
void tearDown() {}

void test_unknown_until_first_reading() {
  BatteryGauge b;
  TEST_ASSERT_FALSE(b.known());
  TEST_ASSERT_EQUAL_INT8(-1, b.percent());
  b.add(-1);  // read errors are ignored
  TEST_ASSERT_FALSE(b.known());
  b.add(73);
  TEST_ASSERT_TRUE(b.known());
  TEST_ASSERT_EQUAL_INT8(73, b.percent());
}

void test_noise_does_not_flicker() {
  BatteryGauge b;
  b.add(70);
  const int noisy[] = {72, 68, 71, 69, 72, 68, 70, 71, 69, 72};
  for (int r : noisy) {
    b.add(r);
    TEST_ASSERT_EQUAL_INT8(70, b.percent());
  }
}

void test_follows_real_change() {
  BatteryGauge b;
  b.add(80);
  for (int i = 0; i < 20; ++i) b.add(60);
  TEST_ASSERT_INT8_WITHIN(1, 60, b.percent());
}

void test_reaches_end_stops() {
  BatteryGauge b;
  b.add(90);
  for (int i = 0; i < 30; ++i) b.add(100);
  TEST_ASSERT_EQUAL_INT8(100, b.percent());
  for (int i = 0; i < 40; ++i) b.add(0);
  TEST_ASSERT_EQUAL_INT8(0, b.percent());
}

void test_clamps_out_of_range() {
  BatteryGauge b;
  b.add(130);
  TEST_ASSERT_EQUAL_INT8(100, b.percent());
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_unknown_until_first_reading);
  RUN_TEST(test_noise_does_not_flicker);
  RUN_TEST(test_follows_real_change);
  RUN_TEST(test_reaches_end_stops);
  RUN_TEST(test_clamps_out_of_range);
  return UNITY_END();
}
