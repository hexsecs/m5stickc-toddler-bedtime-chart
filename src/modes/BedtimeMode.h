#pragma once
// Bedtime Adventure: a picture checklist for getting ready for bed.
//
// Kid:       big front button (A) = "I did it!" -> celebration, next step.
// Grown-up:  tap B, then press and hold B, press A to pick, let go of B
//              A once       = go back one step (oops)
//              A twice      = skip this step (e.g. not a bath night)
//              A 3 times    = start the routine over
//              A 4 times    = cancel (presses keep cycling: 5 = back, ...)
//
// Finishing every step earns a star on the weekly chart; 7 stars earns
// "Super Sleeper". Then the screen fades to a sleepy moon and sleeps.

#include <Checklist.h>

#include "../Draw.h"
#include "../Mode.h"

class BedtimeMode : public Mode {
 public:
  BedtimeMode();
  void enter() override;
  void update(M5Canvas& gfx, uint32_t now) override;
  void onSleep() override;
  bool wantsSleep() const override { return state_ == State::Asleep; }
  bool allowIdleSleep() const override;
  bool showsBattery() const override { return state_ == State::Step && !list_.finished(); }

 private:
  enum class State { Step, Cheer, AllDone, SuperSleeper, Goodnight, Asleep };

  void setState(State s, uint32_t now);
  // Returns true if it used this frame's A press (B held + A = back).
  bool handleGrownUpButton(uint32_t now, bool aPressed);
  void finishRoutine(uint32_t now);

  void drawStep(M5Canvas& g, uint32_t now);
  void drawCheer(M5Canvas& g, uint32_t now);
  void drawAllDone(M5Canvas& g, uint32_t now);
  void drawSuperSleeper(M5Canvas& g, uint32_t now);
  void drawGoodnight(M5Canvas& g, uint32_t now);
  void drawProgress(M5Canvas& g, uint32_t now);
  void drawWeek(M5Canvas& g, int y, uint32_t now, bool animateNewest);
  void drawTitle(M5Canvas& g, const char* line1, const char* line2, uint32_t color);

  Checklist list_;
  StarChart week_;
  State state_ = State::Step;
  uint32_t stateAt_ = 0;
  uint8_t cheerStep_ = 0;  // which step is being celebrated
  bool earnedPrize_ = false;

  bool waitForARelease_ = true;  // ignore the press that woke us up
  uint32_t bDownAt_ = 0;
  uint8_t aCount_ = 0;  // A presses while B is held; see grownUpAction()
  bool bArmed_ = false;     // this B hold followed a quick B tap, so A presses count
  uint32_t lastBTapAt_ = 0;  // when a quick B tap was let go, to arm the next press
  uint32_t flashUntil_ = 0;  // brief visual feedback for grown-up actions
  const char* flashText_ = nullptr;
};
