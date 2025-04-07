#ifndef MOUSE_H08309384
#define MOUSE_H08309384

#include "Acceleration.h"
#include "HID.h"

// order corresponds to HID mouse device button order
enum MouseButton : uint8_t {
  MOUSE_LEFT    = 0,
  MOUSE_RIGHT   = 1,
  MOUSE_BACK    = 2,
  MOUSE_MIDDLE  = 3,
  MOUSE_FORWARD = 4,
  MOUSE_EXTRA1  = 5,
  MOUSE_EXTRA2  = 6,
  MOUSE_NONE    = 7,
};


class Trackball_t {
public:
  Trackball_t();

  void begin();
  void end();

  [[nodiscard]] auto buttons() const -> uint8_t;

  [[nodiscard]] auto getMapping(uint8_t btnId) const -> uint8_t;
  void setMapping(uint8_t srcId, uint8_t tgtId);

  void setMappings(uint8_t* buf, size_t bufLen);
  void getMappings(uint8_t* buf, size_t bufLen) const;

  void set(uint8_t btnId, bool isDown);

  void move(double x, double y);
  void scroll(double x, double y);

  void setMoveScale(double scaleX, double scaleY);
  void setScrollScale(double scaleX, double scaleY);

  void send(uint64_t timestampMus);

private:
  uint8_t btnState = 0b00000000;
  uint8_t prevBtnState = 0b00000000;
  
  double moveX = 0.0;
  double moveY = 0.0;
  double moveScaleX = 1.0;
  double moveScaleY = 1.0;

  double scrollX = 0.0;
  double scrollY = 0.0;
  double scrollScaleX = 1.0;
  double scrollScaleY = 1.0;

  MouseAcceleration moveAccel{1.0, 0.1, 1.0};
  MouseAcceleration scrollAccel;

  uint8_t buttonMap[8] = {
    MOUSE_LEFT,
    MOUSE_RIGHT,
    MOUSE_BACK,
    MOUSE_MIDDLE,
    MOUSE_FORWARD,
    MOUSE_EXTRA1,
    MOUSE_EXTRA2,
    MOUSE_NONE,
  };

  uint8_t scrollButtonMap = 0b00000000 | (1 << MOUSE_BACK) | (1 << MOUSE_FORWARD);

  double accumulatedX = 0.0;
  double accumulatedY = 0.0;

  double deadZone = 25.0;

  uint8_t scrollButtonsInMode = 0b00000000;
  uint8_t prevScrollButtonsInMode = 0b00000000;

  void reset();
};

// singleton
extern Trackball_t Trackball;

#endif  // MOUSE_H08309384
