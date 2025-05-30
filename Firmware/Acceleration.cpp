// https://github.com/apple-oss-distributions/IOHIDFamily/blob/c56e1c1b2469d9956a585cc2518c8f0c51b5809d/IOHIDSystem/IOHIPointing.cpp

#include <Arduino.h>
#include <SPI.h>

#include <math.h>
#include <string.h>

#include "RingBuffer.h"
#include "Acceleration.h"


Offsets MouseAcceleration::update(double dx, double dy, unsigned long timestampMs, bool clear) {
  addEvent(dx, dy, timestampMs, clear);
  auto avg = calcAverages();
  return calcScroll(avg);
}

void MouseAcceleration::addEvent(double dx, double dy, unsigned long timestampMs, bool clear) {
  unsigned long timeDeltaMs = events.empty() ? 0 : max(timestampMs - events.back().timestampMs, 0UL);
  if (timeDeltaMs >= eventClearThresholdMs || clear) {
    events.clear();
    timeDeltaMs = eventClearThresholdMs;
  }

  events.emplace_back(dx, dy, timestampMs, timeDeltaMs);
}

MouseAcceleration::MoveAverage MouseAcceleration::calcAverages() const {
  double dxSum = 0.0;
  double dySum = 0.0;
  double dtSumMs = 0.0;
  int count = 0;

  // Iterate from newest to oldest event
  for (int i = events.size() - 1; i >= 0; --i) {
    MoveEvent event = events[i];

    if (event.timeDeltaMs <= 0 || event.timeDeltaMs >= eventGroupThresholdMs) {
      dtSumMs += eventGroupThresholdMs;
      count++;
      break;
    }

    dxSum += fabs(event.dx);
    dySum += fabs(event.dy);
    dtSumMs += event.timeDeltaMs;
    count++;

    if (dtSumMs >= eventClearThresholdMs) {
      break;
    }
  }

  if (count <= 0) {
    return {};
  }

  return { dxSum / count, dySum / count, dtSumMs / count };
}

Offsets MouseAcceleration::calcScroll(const MouseAcceleration::MoveAverage& avg) const {
  if (events.empty()) {
    return {};
  }

  double dtMs = avg.dtAvgMs * rateMultiplier;
  dtMs = max(min(dtMs, eventGroupThresholdMs), 1.0);

  double m = (multiplierA * dtMs * dtMs) - (multiplierB * dtMs) + multiplierC;
  m *= rateMultiplier;

  double xMult = m * avg.dxAvg;
  double yMult = m * avg.dyAvg;
  
  if (maxMultiplier >= 0) {
    xMult = min(xMult, maxMultiplier);
    yMult = min(yMult, maxMultiplier);
  }

  if (minMultiplier >= 0) {
    xMult = max(xMult, minMultiplier);
    yMult = max(yMult, minMultiplier);
  }

  const auto &event = events.back();
  double dx = event.dx * xMult;
  double dy = event.dy * yMult;

  return {dx, dy};
}