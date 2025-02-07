#ifndef TRACKBALL_H08309384
#define TRACKBALL_H08309384

#include <HID.h>

class Trackball_t {
public:
  Trackball_t();
  void begin();
  void end();
};

// singleton
extern Trackball_t Trackball;

#endif  // TRACKBALL_H08309384
