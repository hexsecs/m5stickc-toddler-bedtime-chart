#include <unity.h>

#include "BatteryGauge.h"

void setUp() {}
void tearDown() {}

// Feed the warm-up readings so the gauge starts showing `percent`.
void prime(BatteryGauge& b, int percent) {
  for (int i = 0; i < 4; ++i) b.add(percent);
}

void test_unknown_until_first_reading() {
  BatteryGauge b;
  TEST_ASSERT_FALSE(b.known());
  TEST_ASSERT_EQUAL_INT8(-1, b.percent());
  b.add(-1);  // read errors are ignored
  b.add(73);
  b.add(73);
  b.add(73);
  TEST_ASSERT_FALSE(b.known());  // still warming up
  b.add(73);
  TEST_ASSERT_TRUE(b.known());
  TEST_ASSERT_EQUAL_INT8(73, b.percent());
}

void test_noise_does_not_flicker() {
  BatteryGauge b;
  prime(b, 70);
  const int noisy[] = {72, 68, 71, 69, 72, 68, 70, 71, 69, 72};
  for (int r : noisy) {
    b.add(r);
    TEST_ASSERT_EQUAL_INT8(70, b.percent());
  }
}

void test_follows_real_change() {
  BatteryGauge b;
  prime(b, 80);
  for (int i = 0; i < 20; ++i) b.add(60);
  TEST_ASSERT_INT8_WITHIN(1, 60, b.percent());
}

void test_reaches_end_stops() {
  BatteryGauge b;
  prime(b, 90);
  for (int i = 0; i < 30; ++i) b.add(100);
  TEST_ASSERT_EQUAL_INT8(100, b.percent());
  for (int i = 0; i < 40; ++i) b.add(0);
  TEST_ASSERT_EQUAL_INT8(0, b.percent());
}

void test_clamps_out_of_range() {
  BatteryGauge b;
  prime(b, 130);
  TEST_ASSERT_EQUAL_INT8(100, b.percent());
}

void test_warmup_averages_a_noisy_first_sample() {
  BatteryGauge b;
  b.add(12);  // one bad sample right after waking
  b.add(25);
  b.add(26);
  b.add(25);
  TEST_ASSERT_EQUAL_INT8(22, b.percent());
}

void test_turns_low_when_average_drops_below_threshold() {
  BatteryGauge b;
  prime(b, 20);
  for (int i = 0; i < 20; ++i) b.add(19);
  TEST_ASSERT_EQUAL_INT8(19, b.percent());  // not stuck at 20, so the red warning shows
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_unknown_until_first_reading);
  RUN_TEST(test_noise_does_not_flicker);
  RUN_TEST(test_follows_real_change);
  RUN_TEST(test_reaches_end_stops);
  RUN_TEST(test_clamps_out_of_range);
  RUN_TEST(test_warmup_averages_a_noisy_first_sample);
  RUN_TEST(test_turns_low_when_average_drops_below_threshold);
  return UNITY_END();
}
