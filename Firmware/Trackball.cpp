#include <math.h>

#include "Trackball.h"

// clang-format off
static const uint8_t hidReportDescriptor[] PROGMEM = {
    0x05, 0x01,        // USAGE PAGE (Generic Desktop)
    0x09, 0x02,        // USAGE (Mouse)
    0xa1, 0x01,        // COLLECTION (Application)

        0x05, 0x01,        // USAGE PAGE (Generic Desktop)
        0x09, 0x02,        // USAGE (Mouse)
        0xa1, 0x02,        // COLLECTION (Logical)

            0x85, 0x01,    // REPORT_ID (0x01)
            0x09, 0x01,    // USAGE (Pointer)
            0xa1, 0x00,    // COLLECTION (Physical)

                0x05, 0x09,    // USAGE PAGE (Buttons)
                0x19, 0x01,    // USAGE MINIMUM (1)
                0x29, 0x08,    // USAGE MAXIMUM (8)
                0x15, 0x00,    // LOGICAL MINIMUM (0)
                0x25, 0x01,    // LOGICAL MAXIMUM (1)
                0x95, 0x08,    // REPORT COUNT (8)
                0x75, 0x01,    // REPORT SIZE (1)
                0x81, 0x02,    // INPUT (Var, Abs)

                0x05, 0x01,    // USAGE PAGE (Generic Desktop)
                0x09, 0x30,    // USAGE (X)
                0x09, 0x31,    // USAGE (Y)
                0x16, 0x00, 0x80, // LOGICAL MINIMUM (-32,768)
                0x26, 0xff, 0x7f, // LOGICAL MAXIMUM (32,767)
                0x36, 0x00, 0x80, // PHYSICAL MINIMUM (-32,768)
                0x46, 0xff, 0x7f, // PHYSICAL MAXIMUM (32,767)
                0x95, 0x02,    // REPORT COUNT (2),
                0x75, 0x10,    // REPORT SIZE (16),
                0x81, 0x06,    // INPUT (Var, Rel)

                // in resolution multipliers, physical min and max must be
                // greater than 0. see "4.3.1 Resolution Multiplier" in "HID Usage
                // Tables for Universal Serial Bus Version 1.6" (hut1_6.pdf)

                // FIXME resolution multipler does nothing (tested Win11)

                0xa1, 0x02,    // COLLECTION (Logical)
                  0x85, 0x02,    // REPORT_ID (0x02)
                  0x09, 0x48,    // USAGE (Resolution Multiplier)
                  0x95, 0x01,    // REPORT COUNT (1)
                  0x75, 0x02,    // REPORT SIZE (2)
                  0x15, 0x00,    // LOGICAL MINIMUM (0)
                  0x25, 0x01,    // LOGICAL MAXIMUM (1)
                  0x35, 0x01,    // PHYSICAL MINIMUM (1)
                  0x45, 0x78,    // PHYSICAL MAXIMUM (120)
                  0xb1, 0x02,    // FEATURE (Var, Abs)

                  0x95, 0x01,    // REPORT COUNT (1)
                  0x75, 0x06,    // REPORT SIZE (6)
                  0xb1, 0x03,    // FEATURE (Cnst, Var, Abs)

                  0x85, 0x01,    // REPORT_ID (0x01)
                  0x09, 0x38,    // USAGE (Wheel)
                  0x35, 0x00,    // PHYSICAL MINIMUM (0)
                  0x45, 0x00,    // PHYSICAL MAXIMUM (0)
                  0x16, 0x00, 0x80,  // LOGICAL MINIMUM (-32768)
                  0x26, 0xFF, 0x7F,  // LOGICAL MAXIMUM (32767)
                  0x95, 0x01,    // REPORT COUNT (1)
                  0x75, 0x10,    // REPORT SIZE (16)
                  0x81, 0x06,    // INPUT (Var, Rel)
                0xc0,    // END COLLECTION (Logical)

                0xa1, 0x02,    // COLLECTION (Logical)
                  0x85, 0x02,    // REPORT_ID (0x02)
                  0x09, 0x48,    // USAGE (Resolution Multiplier)
                  0x95, 0x01,    // REPORT COUNT (1)
                  0x75, 0x02,    // REPORT SIZE (2)
                  0x15, 0x00,    // LOGICAL MINIMUM (0)
                  0x25, 0x01,    // LOGICAL MAXIMUM (1)
                  0x35, 0x01,    // PHYSICAL MINIMUM (1)
                  0x45, 0x78,    // PHYSICAL MAXIMUM (120)
                  0xb1, 0x02,    // FEATURE (Var, Abs)

                  0x95, 0x01,    // REPORT COUNT (1)
                  0x75, 0x06,    // REPORT SIZE (6)
                  0xb1, 0x03,    // FEATURE (Cnst, Var, Abs)

                  0x85, 0x01,    // REPORT_ID (0x01)
                  0x05, 0x0c,    // USAGE PAGE (Consumer Trackballs)
                  0x0a, 0x38, 0x02, // USAGE (AC Pan)
                  0x35, 0x00,    // PHYSICAL MINIMUM (0)
                  0x45, 0x00,    // PHYSICAL MAXIMUM (0)
                  0x16, 0x00, 0x80,  // LOGICAL MINIMUM (-32768)
                  0x26, 0xFF, 0x7F,  // LOGICAL MAXIMUM (32767)
                  0x95, 0x01,    // REPORT COUNT (1)
                  0x75, 0x10,    // REPORT SIZE (16)
                  0x81, 0x06,    // INPUT (Var, Rel)
                0xc0,    // END COLLECTION (Logical)
            0xc0,    // END COLLECTION (Physical)
        0xc0,    // END COLLECTION (Logical)
    0xc0,    // END COLLECTION (Application)
};
// clang-format on


static auto subtractMaxIntegral(double& value, double scale) -> int16_t {
  double moveXNowF = 0.0;
  value = modf(value * scale, &moveXNowF) / scale;
  return static_cast<int16_t>(moveXNowF);
}


Trackball_t::Trackball_t() {
  static HIDSubDescriptor node = {
    hidReportDescriptor,
    sizeof(hidReportDescriptor),
  };

  HID().AppendDescriptor(&node);

  static const uint8_t resMult[] = { 0x00, 0x78 };
  HID().SetFeature(0x02, resMult, sizeof(resMult) * sizeof(resMult[0]));
}

void Trackball_t::begin() {
  reset();

  static bool hidInitialized = false;
  if (!hidInitialized) {
    HID().begin();
    hidInitialized = true;
  }

  send(micros());
}

void Trackball_t::end() {
  reset();
}

void Trackball_t::reset() {
  btnState = 0;

  moveX = 0;
  moveY = 0;
  moveScaleX = 1.0;
  moveScaleY = 1.0;

  scrollX = 0;
  scrollY = 0;
  scrollScaleX = 1.0;
  scrollScaleY = 1.0;
}

auto Trackball_t::buttons() const -> uint8_t {
  return btnState;
}

[[nodiscard]] auto Trackball_t::getMapping(uint8_t btnId) const -> uint8_t {
  return buttonMap[btnId];
}

void Trackball_t::setMapping(uint8_t srcId, uint8_t tgtId) {
  buttonMap[srcId] = tgtId;
}

void Trackball_t::setMappings(uint8_t* buf, size_t bufLen) {
  memcpy(buttonMap, buf, min(bufLen, sizeof(buttonMap)));
}

void Trackball_t::getMappings(uint8_t* buf, size_t bufLen) const {
  memcpy(buf, buttonMap, min(bufLen, sizeof(buttonMap)));
}

void Trackball_t::set(uint8_t btnId, bool isDown) {
  uint8_t mask = (isDown << btnId);
  btnState |= mask;  // down
  btnState &= mask | ~(1 << btnId); // up
}

void Trackball_t::move(double x, double y) {
  moveX += x;
  moveY += y;
}

void Trackball_t::scroll(double x, double y) {
  scrollX += x;
  scrollY += y;
}

void Trackball_t::setMoveScale(double scaleX, double scaleY) {
  moveX /= scaleX;
  moveY /= scaleY;
  moveScaleX = scaleX;
  moveScaleY = scaleY;
}

void Trackball_t::setScrollScale(double scaleX, double scaleY) {
  scrollX /= scaleX;
  scrollY /= scaleY;
  scrollScaleX = scaleX;
  scrollScaleY = scaleY;
}


void Trackball_t::send(uint64_t timestampMus) {
  uint8_t scrollBtnState = btnState & scrollButtonMap;
  uint8_t prevScrollBtnState = prevBtnState & scrollButtonMap;

  if (scrollBtnState != 0) {
    bool isInScrollMode = (scrollButtonsInMode & scrollBtnState) != 0;
    if (!isInScrollMode) {
      bool isInDeadZone = fabs(accumulatedX + moveX) <= deadZone
          && fabs(accumulatedY + moveY) <= deadZone;
      if (isInDeadZone) {
        // in dead zone; accumulate movements
        accumulatedX += moveX;
        accumulatedY += moveY;

        moveX = 0.0;
        moveY = 0.0;
      } else {
        // exceeded dead zone; enter scroll mode
        scrollButtonsInMode |= scrollBtnState;
      }
    }
  } else {
    accumulatedX = 0.0;
    accumulatedY = 0.0;
  }

  // if in scroll mode, turn cursor movement into scrolling
  if ((scrollButtonsInMode & scrollBtnState) != 0) {
    scrollX += moveX + accumulatedX;
    scrollY += moveY + accumulatedY;

    moveX = 0.0;
    moveY = 0.0;

    accumulatedX = 0.0;
    accumulatedY = 0.0;
  }

  // effective button state: suppress buttons in scroll mode
  uint8_t effectiveBtnState = btnState & ~scrollBtnState;

  // scroll buttons released without entering scroll mode?
  if ((prevScrollBtnState & ~scrollBtnState & ~scrollButtonsInMode) != 0) {
    // register press event. will flip to release event on next send
    effectiveBtnState |= (prevScrollBtnState & ~scrollBtnState);
  }

  scrollButtonsInMode &= btnState;

  // map buttons to HID report positions
  uint8_t sendButtons =
    (((effectiveBtnState & (1 << MOUSE_LEFT)) != 0)      << buttonMap[MOUSE_LEFT])
    | (((effectiveBtnState & (1 << MOUSE_RIGHT)) != 0)   << buttonMap[MOUSE_RIGHT])
    | (((effectiveBtnState & (1 << MOUSE_BACK)) != 0)    << buttonMap[MOUSE_BACK])
    | (((effectiveBtnState & (1 << MOUSE_MIDDLE)) != 0)  << buttonMap[MOUSE_MIDDLE])
    | (((effectiveBtnState & (1 << MOUSE_FORWARD)) != 0) << buttonMap[MOUSE_FORWARD])
    | (((effectiveBtnState & (1 << MOUSE_EXTRA1)) != 0)  << buttonMap[MOUSE_EXTRA1])
    | (((effectiveBtnState & (1 << MOUSE_EXTRA2)) != 0)  << buttonMap[MOUSE_EXTRA2]);

  auto moveOff = moveAccel.update(moveX, moveY, timestampMus / 1000);

  auto moveXNow = static_cast<int16_t>(floor(moveOff.dx * moveScaleX));
  auto moveYNow = static_cast<int16_t>(floor(moveOff.dy * moveScaleY));

  moveX = 0.0;
  moveY = 0.0;


  auto scrollOff = scrollAccel.update(
    scrollX,
    scrollY,
    timestampMus / 1000,
    prevScrollButtonsInMode == 0
  );

  auto scrollXNow = static_cast<int16_t>(floor(scrollOff.dx * scrollScaleX));
  auto scrollYNow = static_cast<int16_t>(-floor(scrollOff.dy * scrollScaleY));

  scrollX = 0.0;
  scrollY = 0.0;


  uint8_t mouseReport[] = {
    static_cast<uint8_t>(sendButtons & 0xff),
    static_cast<uint8_t>(moveXNow & 0xFF),
    static_cast<uint8_t>((moveXNow >> 8) & 0xFF),
    static_cast<uint8_t>(moveYNow & 0xFF),
    static_cast<uint8_t>((moveYNow >> 8) & 0xFF),
    static_cast<uint8_t>(scrollYNow & 0xff),
    static_cast<uint8_t>((scrollYNow >> 8) & 0xff),
    static_cast<uint8_t>(scrollXNow & 0xff),
    static_cast<uint8_t>((scrollXNow >> 8) & 0xff),
  };
  HID().SendReport(0x01, mouseReport, sizeof(mouseReport) / sizeof(mouseReport[0]));

  prevBtnState = btnState;
  prevScrollButtonsInMode = scrollButtonsInMode;
}

// WARN make sure only one instance exists else
//   HID().AppendDescriptor() will be called once for each instance
Trackball_t Trackball;
