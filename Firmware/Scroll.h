#ifndef SCROLL_H22659411
#define SCROLL_H22659411

#include "RingBuffer.h"

// Constants that roughly correspond to the original fixed–point values.
// The original algorithm computes:
//   multiplier = (A * (avgTime^2)) - (B * avgTime) + C,
// then multiplies by the rate multiplier and the average delta.
#define SCROLL_TIME_DELTA_COUNT 8
const double SCROLL_EVENT_THRESHOLD_MS = 150.0;  // threshold for event grouping
const double SCROLL_CLEAR_THRESHOLD_MS = 500.0;    // if time gap exceeds this, state is reset

// The following constants come from the original fixed–point values:
//  SCROLL_MULTIPLIER_A = 0x00000002, SCROLL_MULTIPLIER_B = 0x000003bb, SCROLL_MULTIPLIER_C = 0x00018041.
// When expressed as doubles (dividing by 2^16), we get:
const double SCROLL_MULTIPLIER_A = 2.0 / 65536.0;      // ~0.00003052
const double SCROLL_MULTIPLIER_B = 955.0 / 65536.0;      // ~0.01458
const double SCROLL_MULTIPLIER_C = 98305.0 / 65536.0;    // ~1.50002

// Minimum multiplier (kIOFixedOne >> 4, i.e. 4096 in fixed–point, or 4096/65536 = 0.0625)
const double MIN_MULTIPLIER = 0.0625;


struct ScrollAverage {
  double dxAvg = 0.0;
  double dyAvg = 0.0;
  double dtAvgMs = 0.0;

  ScrollAverage() = default;

  ScrollAverage(double dx, double dy, double dtMs)
    : dxAvg(dx), dyAvg(dy), dtAvgMs(dtMs) {}
};


struct ScrollEvent {
  double dx = 0.0;
  double dy = 0.0;

  unsigned long timestampMs = 0;
  double timeDeltaMs = 0.0;

  ScrollEvent() = default;

  ScrollEvent(double dx, double dy, unsigned long timestampMs, double deltaTime)
    : timestampMs(timestampMs), timeDeltaMs(deltaTime), dx(dx), dy(dy) {}
};


struct ScrollOffsets {
  double dx = 0.0;
  double dy = 0.0;

  ScrollOffsets() = default;

  ScrollOffsets(double dx, double dy)
    : dx(dx), dy(dy) {}
};


class ScrollAcceleration {
public:
  ScrollAcceleration() = default;

  ScrollOffsets scroll(double dx, double dy, unsigned long timestampMs, bool clear);

  inline void clear() {
    events.clear();
  }

  inline double getRateMultiplier() {
    return rateMultiplier;
  }

  inline void setRateMultiplier(double mult) {
    rateMultiplier = mult;
  }

private:
  double rateMultiplier = 1.0;
  RingBuffer<ScrollEvent, SCROLL_TIME_DELTA_COUNT> events;

  void addEvent(double dx, double dy, unsigned long timestampMs, bool clear = false);
  ScrollAverage calcAverages() const;
  ScrollOffsets calcScroll(const ScrollAverage& avg) const;
};



#endif  // SCROLL_H22659411
