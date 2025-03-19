#ifndef TRACKBALL_H08309384
#define TRACKBALL_H08309384

#include "HID.h"

class Trackball_t {
public:
  Trackball_t();

  void begin();
  void end();

  [[nodiscard]] auto buttons() const -> uint8_t;

  void down(uint8_t buttons);
  void up(uint8_t buttons);
  void set(uint8_t buttons);

  void move(double x, double y);
  void scroll(double x, double y);

  void setMoveScale(double scaleX, double scaleY);
  void setScrollScale(double scaleX, double scaleY);

  auto send(bool force = false) -> bool;

private:
  uint8_t buttons_ = 0;

  double moveX = 0.0;
  double moveY = 0.0;
  double moveScaleX = 1.0;
  double moveScaleY = 1.0;

  double scrollX = 0.0;
  double scrollY = 0.0;
  double scrollScaleX = 1.0;
  double scrollScaleY = 1.0;

  bool stateModified = false;
  bool hidInitialized = false;

  void reset();
};

// singleton
extern Trackball_t Trackball;

#endif  // TRACKBALL_H08309384
