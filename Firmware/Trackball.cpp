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
                    0x45, 0x04,    // PHYSICAL MAXIMUM (4)
                    0x91, 0x02,    // FEATURE (Var, Abs)

                    0x95, 0x01,    // REPORT COUNT (1)
                    0x75, 0x06,    // REPORT SIZE (6)
                    0x91, 0x03,    // FEATURE (Cnst, Var, Abs)

                    0x85, 0x01,    // REPORT_ID (0x01)
                    0x09, 0x38,    // USAGE (Wheel)
                    0x15, 0x81,    // LOGICAL MINIMUM (-127)
                    0x25, 0x7f,    // LOGICAL MAXIMUM (127)
                    0x35, 0x81,    // PHYSICAL MINIMUM (-127)
                    0x45, 0x7f,    // PHYSICAL MAXIMUM (127)
                    0x95, 0x01,    // REPORT COUNT (1)
                    0x75, 0x08,    // REPORT SIZE (8)
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
                    0x45, 0x04,    // PHYSICAL MAXIMUM (4)
                    0x91, 0x02,    // FEATURE (Var, Abs)

                    0x95, 0x01,    // REPORT COUNT (1)
                    0x75, 0x06,    // REPORT SIZE (6)
                    0x91, 0x03,    // FEATURE (Cnst, Var, Abs)

                    0x85, 0x01,    // REPORT_ID (0x01)
                    0x05, 0x0c,    // USAGE PAGE (Consumer Devices)
                    0x0a, 0x38, 0x02, // USAGE (AC Pan)
                    0x15, 0x81,    // LOGICAL MINIMUM (-127)
                    0x25, 0x7f,    // LOGICAL MAXIMUM (127)
                    0x35, 0x81,    // PHYSICAL MINIMUM (-127)
                    0x45, 0x7f,    // PHYSICAL MAXIMUM (127)
                    0x95, 0x01,    // REPORT COUNT (1)
                    0x75, 0x08,    // REPORT SIZE (8)
                    0x81, 0x06,    // INPUT (Var, Rel)
                0xc0,    // END COLLECTION (Logical)
            0xc0,    // END COLLECTION (Physical)
        0xc0,    // END COLLECTION (Logical)

        // Custom vendor report for smooth scrolling
        0xa1, 0x01,    // COLLECTION (Application)
            0x85, 0x03,    // REPORT_ID (0x03)
            0x06, 0xFF, 0xFF, // USAGE PAGE (Vendor-defined)
            0x09, 0x01,    // USAGE (Vendor-defined)
            0x16, 0x00, 0x00, // LOGICAL MINIMUM (0)
            0x26, 0xFF, 0x00, // LOGICAL MAXIMUM (255)
            0x95, 0x08,    // REPORT COUNT (8)
            0x75, 0x08,    // REPORT SIZE (8)
            0x81, 0x02,    // INPUT (Var, Abs)
        0xc0,          // END COLLECTION (Application)
    0xc0,    // END COLLECTION (Application)
};
// clang-format on


auto subtractMaxIntegral(double& value, double scale) -> int16_t {
    double moveXNowF = 0.0;
    value = modf(value * scale, &moveXNowF) / scale;
    return static_cast<int16_t>(moveXNowF);
}


Trackball_t::Trackball_t() {
    static HIDSubDescriptor node(
      hidReportDescriptor,
      sizeof(hidReportDescriptor)
    );

    HID().AppendDescriptor(&node);
}

void Trackball_t::begin() {
    buttons_ = 0;

    moveX = 0;
    moveY = 0;
    scrollY = 0;
    scrollX = 0;

    scrollResX = 1;
    scrollResY = 1;

    stateModified = false;
    resModified = false;

    send(true);
}

void Trackball_t::end() {
}

auto Trackball_t::buttons() const -> uint8_t {
    return buttons_;
}

void Trackball_t::set(uint8_t buttons) {
    stateModified = buttons_ || buttons_ != buttons;
    buttons_ = buttons;
}

void Trackball_t::down(uint8_t buttons) {
    stateModified = stateModified || (~buttons_ & buttons) != 0;
    buttons_ |= buttons;
}

void Trackball_t::up(uint8_t buttons) {
    stateModified = stateModified || (buttons_ & ~buttons) != 0;
    buttons_ &= ~buttons;
}

void Trackball_t::move(double x, double y) {
    stateModified = stateModified || x != 0.0 || y != 0.0;
    moveX += x;
    moveY += y;
}

void Trackball_t::scroll(double x, double y) {
    stateModified = stateModified || x != 0.0 || y != 0.0;
    scrollX += x;
    scrollY += y;
}

void Trackball_t::setScrollResolutionMultiplier(uint8_t x, uint8_t y) {
    resModified = resModified || scrollResX != x || scrollResY != y;
    scrollResX = x;
    scrollResY = y;
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

auto Trackball_t::send(bool force) -> bool {
    if (!stateModified && !resModified && !force) {
        return false;
    }

    // order matters: resolution multiplier before movements
    if (resModified || force) {
        uint8_t dat2[] = { scrollResX, scrollResY };
        HID().SendReport(0x02, dat2, sizeof(dat2) / sizeof(dat2[0]));
        resModified = false;
    }

    // TODO periodically ping PC to see if custom driver is listening

    if (stateModified || force) {
        auto moveXNow = subtractMaxIntegral(moveX, moveScaleX);
        auto moveYNow = subtractMaxIntegral(moveY, moveScaleY);

        auto scrollXNow = subtractMaxIntegral(scrollX, scrollScaleX);
        auto scrollYNow = subtractMaxIntegral(scrollY, scrollScaleY);

        uint8_t dat1[] = {
            static_cast<uint8_t>(buttons_ & 0xff),
            static_cast<uint8_t>(moveXNow & 0xFF),
            static_cast<uint8_t>((moveXNow >> 8) & 0xFF),
            static_cast<uint8_t>(moveYNow & 0xFF),
            static_cast<uint8_t>((moveYNow >> 8) & 0xFF),
            static_cast<uint8_t>(scrollYNow & 0xff),
            static_cast<uint8_t>(scrollXNow & 0xff)
        };

        HID().SendReport(0x01, dat1, sizeof(dat1) / sizeof(dat1[0]));
        stateModified = false;
    }

    return true;
}

// WARN make sure only one instance exists else
//   HID().AppendDescriptor() will be called once for each instance
Trackball_t Trackball;

