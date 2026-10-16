#pragma once
// Battery level for the grown-ups' indicator. Reads the hardware every few
// seconds (not every frame) and smooths it with BatteryGauge.

#include <BatteryGauge.h>

#include "Platform.h"

class Battery {
 public:
  void update(uint32_t now) {
    if (read_ && now - lastRead_ < kReadEveryMs) return;
    read_ = true;
    lastRead_ = now;
    gauge_.add(M5.Power.getBatteryLevel());
    // The Plus2 can't tell, so it reports charge_unknown: no bolt there.
    charging_ = M5.Power.isCharging() == m5::Power_Class::is_charging;
  }

  bool known() const { return gauge_.known(); }
  int8_t percent() const { return gauge_.percent(); }
  bool charging() const { return charging_; }

 private:
  static constexpr uint32_t kReadEveryMs = 5000;

  BatteryGauge gauge_;
  uint32_t lastRead_ = 0;
  bool read_ = false;
  bool charging_ = false;
};

extern Battery battery;
