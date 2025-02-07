#include <HID.h>

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
            0x75, 0x10,    // REPORT SIZE (16),
            0x95, 0x02,    // REPORT COUNT (2),
            0x81, 0x06,    // INPUT (Var, Rel)

            0xa1, 0x02,    // COLLECTION (Logical)
                0x85, 0x02,    // REPORT_ID (0x02)
                0x09, 0x48,    // USAGE (Resolution Multiplier)
                0x15, 0x00,    // LOGICAL MINIMUM (0)
                0x25, 0x01,    // LOGICAL MAXIMUM (1)
                0x35, 0x01,    // PHYSICAL MINIMUM (1)
                0x45, 0x04,    // PHYSICAL MAXIMUM (4)
                0x95, 0x01,    // REPORT COUNT (1)
                0x75, 0x08,    // REPORT SIZE (8)
                0x91, 0x02,    // FEATURE (Var, Abs)

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
                0x15, 0x00,    // LOGICAL MINIMUM (0)
                0x25, 0x01,    // LOGICAL MAXIMUM (1)
                0x35, 0x01,    // PHYSICAL MINIMUM (1)
                0x45, 0x04,    // PHYSICAL MAXIMUM (4)
                0x95, 0x01,    // REPORT COUNT (1)
                0x75, 0x08,    // REPORT SIZE (8)
                0x91, 0x02,    // FEATURE (Var, Abs)

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
    0xc0     // END COLLECTION (Application)
};
// clang-format on

Trackball_t::Trackball_t() {
    static HIDSubDescriptor node(
      hidReportDescriptor,
      sizeof(hidReportDescriptor)
    );

    HID().AppendDescriptor(&node);
}

void Trackball_t::begin() {
}

void Trackball_t::end() {
}

// WARN make sure only one instance exists else
//   HID().AppendDescriptor() will be called once for each instance
Trackball_t Trackball;

