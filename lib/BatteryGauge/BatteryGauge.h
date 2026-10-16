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
    if (shown_ < 0) {  // first reading: show it straight away
      avg_ = (float)raw;
      shown_ = (int8_t)raw;
      return;
    }
    avg_ += ((float)raw - avg_) * kSmoothing;
    // Only move the number once the average has clearly changed, so it
    // doesn't flicker between neighbours. Always settle on the end stops.
    int rounded = (int)lroundf(avg_);
    if (fabsf(avg_ - shown_) >= kStep || rounded == 0 || rounded == 100) shown_ = (int8_t)rounded;
  }

  bool known() const { return shown_ >= 0; }
  // 0-100, or -1 before the first good reading.
  int8_t percent() const { return shown_; }

 private:
  static constexpr float kSmoothing = 0.25f;  // weight of each new reading
  static constexpr float kStep = 1.5f;        // change needed to update the display

  float avg_ = 0;
  int8_t shown_ = -1;
};
