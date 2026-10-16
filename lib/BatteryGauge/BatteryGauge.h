#pragma once
// Smooths raw battery-percent readings into a steady number for the screen.
// The Plus2 reads its battery through a noisy ADC (and the buzzer makes the
// voltage sag), so raw values jump around by a few percent. Kept free of
// Arduino/M5 headers so it can be unit tested on the host.

#include <math.h>
#include <stdint.h>

class BatteryGauge {
 public:
  // Feed one raw reading, 0-100. Negative means "couldn't read" and is ignored.
  void add(int32_t raw) {
    if (raw < 0) return;
    if (raw > 100) raw = 100;
    if (warmup_ < kWarmupReadings) {  // average the first few before showing anything
      avg_ += (float)raw;
      if (++warmup_ == kWarmupReadings) {
        avg_ /= kWarmupReadings;
        shown_ = (int8_t)lroundf(avg_);
      }
      return;
    }
    avg_ += ((float)raw - avg_) * kSmoothing;
    // Only move the number once the average is clearly past a rounding
    // boundary, so it doesn't flicker between neighbours but never lags by
    // more than a fraction of a percent (the low-battery colour depends on it).
    if (fabsf(avg_ - shown_) >= kStep) shown_ = (int8_t)lroundf(avg_);
  }

  bool known() const { return shown_ >= 0; }
  // 0-100, or -1 until the first few good readings are in.
  int8_t percent() const { return shown_; }

 private:
  static constexpr float kSmoothing = 0.25f;  // weight of each new reading
  static constexpr float kStep = 0.75f;       // change needed to update the display
  static constexpr uint8_t kWarmupReadings = 4;

  float avg_ = 0;
  uint8_t warmup_ = 0;
  int8_t shown_ = -1;
};
