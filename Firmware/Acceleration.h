#ifndef ACCELERATION_H22659411
#define ACCELERATION_H22659411

#include "RingBuffer.h"


struct Offsets {
  double dx = 0.0;
  double dy = 0.0;

  Offsets() = default;

  Offsets(double dx, double dy)
    : dx(dx), dy(dy) {}
};


class MouseAcceleration {
protected:
  struct MoveAverage {
    double dxAvg = 0.0;
    double dyAvg = 0.0;
    double dtAvgMs = 0.0;

    MoveAverage() = default;

    MoveAverage(double dx, double dy, double dtMs)
      : dxAvg(dx), dyAvg(dy), dtAvgMs(dtMs) {}
  };

  struct MoveEvent {
    double dx = 0.0;
    double dy = 0.0;

    unsigned long timestampMs = 0;
    double timeDeltaMs = 0.0;

    MoveEvent() = default;

    MoveEvent(double dx, double dy, unsigned long timestampMs, double deltaTime)
      : timestampMs(timestampMs), timeDeltaMs(deltaTime), dx(dx), dy(dy) {}
  };
public:
  MouseAcceleration() = default;

  MouseAcceleration(
    double rateMultiplier,
    double minMultiplier,
    double maxMultiplier
  ) : rateMultiplier(rateMultiplier),
  minMultiplier(minMultiplier),
  maxMultiplier(maxMultiplier) {}

  Offsets update(
    double dx,
    double dy,
    unsigned long timestampMs,
    bool clear = false
  );

  void clear() {
    events.clear();
  }

  double getRateMultiplier() {
    return rateMultiplier;
  }

  void setRateMultiplier(double mult) {
    rateMultiplier = mult;
  }

  double getMinMultiplier() {
    return minMultiplier;
  }

  void setMinMultiplier(double mult) {
    minMultiplier = mult;
  }

  double getMaxMultiplier() {
    return maxMultiplier;
  }

  void setMaxMultiplier(double mult) {
    maxMultiplier = mult;
  }

  double getGroupThresholdMs() {
    return eventGroupThresholdMs;
  }

  void setGroupThresholdMs(double thresholdMs) {
    eventGroupThresholdMs = thresholdMs;
  }

  double getEventClearThresholdMs() {
    return eventClearThresholdMs;
  }

  void setEventClearThresholdMs(double thresholdMs) {
    eventClearThresholdMs = thresholdMs;
  }

private:
  constexpr static int max_deltas = 16;

  // min multiplier (kIOFixedOne >> 4) defined in IOHIPointing.cpp
  double minMultiplier = 4096.0 / pow(2, 16);

  // arbitrary value. negative means unlimited
  double maxMultiplier = -1;

  // constants from IOHIPointing.cpp
  double multiplierA = 2.0 / pow(2, 16);      // ~0.00003052
  double multiplierB = 955.0 / pow(2, 16);    // ~0.01458
  double multiplierC = 98305.0 / pow(2, 16);  // ~1.50002

  double eventGroupThresholdMs = 150.0;  // threshold for event grouping
  double eventClearThresholdMs = 500.0;  // if time gap exceeds this, state is reset

  double rateMultiplier = 1.0;
  RingBuffer<MoveEvent, max_deltas> events;

  void addEvent(double dx, double dy, unsigned long timestampMs, bool clear = false);
  MoveAverage calcAverages() const;
  Offsets calcScroll(const MoveAverage& avg) const;
};



#endif  // ACCELERATION_H22659411
