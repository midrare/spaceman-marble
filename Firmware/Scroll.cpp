// https://github.com/apple-oss-distributions/IOHIDFamily/blob/c56e1c1b2469d9956a585cc2518c8f0c51b5809d/IOHIDSystem/IOHIPointing.cpp

#include <Arduino.h>
#include <SPI.h>

#include <math.h>
#include <string.h>

#include "RingBuffer.h"
#include "Scroll.h"

ScrollOffsets ScrollAcceleration::scroll(double dx, double dy, unsigned long timestampMs, bool clear) {
  addEvent(dx, dy, timestampMs, clear);
  auto avg = calcAverages();
  return calcScroll(avg);
}

void ScrollAcceleration::addEvent(double dx, double dy, unsigned long timestampMs, bool clear) {
  if (dx == 0.0 && dy == 0.0) {
    return;
  }
  unsigned long timeDeltaMs = events.empty() ? 0 : max(timestampMs - events.back().timestampMs, 0UL);
  if (timeDeltaMs >= SCROLL_CLEAR_THRESHOLD_MS || clear) {
    events.clear();
    timeDeltaMs = SCROLL_CLEAR_THRESHOLD_MS;
  }

  events.emplace_back(dx, dy, timestampMs, timeDeltaMs);
}

ScrollAverage ScrollAcceleration::calcAverages() const {
  double dxSum = 0.0;
  double dySum = 0.0;
  double dtSumMs = 0.0;
  int count = 0;

  // Iterate from newest to oldest event
  for (int i = events.size() - 1; i >= 0; --i) {
    ScrollEvent event = events[i];

    if (event.timeDeltaMs <= 0 || event.timeDeltaMs >= SCROLL_EVENT_THRESHOLD_MS) {
      dtSumMs += SCROLL_EVENT_THRESHOLD_MS;
      count++;
      break;
    }

    dxSum += fabs(event.dx);
    dySum += fabs(event.dy);
    dtSumMs += event.timeDeltaMs;
    count++;

    if (dtSumMs >= SCROLL_CLEAR_THRESHOLD_MS) {
      break;
    }
  }

  if (count <= 0) {
    return {};
  }

  return { dxSum / count, dySum / count, dtSumMs / count };
}

ScrollOffsets ScrollAcceleration::calcScroll(const ScrollAverage& avg) const {
  if (events.empty()) {
    return {};
  }

  double dtMs = avg.dtAvgMs * rateMultiplier;
  dtMs = max(min(dtMs, SCROLL_EVENT_THRESHOLD_MS), 1.0);

  double m = (SCROLL_MULTIPLIER_A * dtMs * dtMs) - (SCROLL_MULTIPLIER_B * dtMs) + SCROLL_MULTIPLIER_C;
  m *= rateMultiplier;

  const auto &event = events.back();
  double dx = event.dx * max(m * avg.dxAvg, MIN_MULTIPLIER);
  double dy = event.dy * max(m * avg.dyAvg, MIN_MULTIPLIER);

  return {dx, dy};
}