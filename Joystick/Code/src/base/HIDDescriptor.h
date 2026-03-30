#ifndef HIDDESCRIPTOR_H
#define HIDDESCRIPTOR_H

#include <Arduino.h>
#include "shared/config.h"

// Joystick stuff
#define NUM_AXIS (2 + (2 * (INPUT_JOYSTICK_COUNT)))
#define NUM_BUTTONS ((5 * INPUT_5WAY_COUNT) + INPUT_BUTTON_COUNT)
#define BUTTON_BYTES ((NUM_BUTTONS + 7) / 8)

// Axis helpers
#define AXIS_USAGE(n) (0x30 + (n))

#define AXIS_1 0x09, AXIS_USAGE(0)
#define AXIS_2 AXIS_1, 0x09, AXIS_USAGE(1)
#define AXIS_3 AXIS_2, 0x09, AXIS_USAGE(2)
#define AXIS_4 AXIS_3, 0x09, AXIS_USAGE(3)
#define AXIS_5 AXIS_4, 0x09, AXIS_USAGE(4)
#define AXIS_6 AXIS_5, 0x09, AXIS_USAGE(5)

#if NUM_AXIS == 1
    #define AXES AXIS_1
#elif NUM_AXIS == 2
    #define AXES AXIS_2
#elif NUM_AXIS == 3
    #define AXES AXIS_3
#elif NUM_AXIS == 4
    #define AXES AXIS_4
#elif NUM_AXIS == 5
    #define AXES AXIS_5
#elif NUM_AXIS == 6
    #define AXES AXIS_6
#else
    #error "Unsupported number of axes"
#endif

// Button padding
#define BUTTON_PADDING ((8 - (NUM_BUTTONS % 8)) % 8)

// HID Descriptor (also known as magic)
const uint8_t desc_hid_report[] = {
    // Joystick Stuff
    0x05, 0x01,
    0x09, 0x04,
    0xA1, 0x01,
    0x85, 0x01,                 // Report ID 1

    // Buttons
    0x05, 0x09,
    0x19, 0x01,
    0x29, NUM_BUTTONS,
    0x15, 0x00,
    0x25, 0x01,
    0x75, 0x01,
    0x95, NUM_BUTTONS,
    0x81, 0x02,

    // Padding
    #if BUTTON_PADDING > 0
        0x75, 0x01,
        0x95, BUTTON_PADDING,
        0x81, 0x03,
    #endif

    // Axes
    0x05, 0x01,
    0x15, 0x81,
    0x25, 0x7F,
    AXES,
    0x75, 0x08,
    0x95, NUM_AXIS,
    0x81, 0x02,

    0xC0,

    // Config stuff
    0x06, 0x00, 0xFF,       // Usage Page (Vendor Defined 0xFF00)
    0x09, 0x01,                 // Usage (0x01)
    0xA1, 0x01,                 // Collection (Application)
    
    // IN report (Device → Host)
    0x85, 0x02,                         // Report ID = 2
    0x09, 0x03,               // Usage (0x03)
    0x15, 0x00,
    0x26, 0xFF, 0x00,
    0x75, 0x08,
    0x95, 0x3F,
    0x81, 0x02,               // Input (Data,Var,Abs)

    // OUT report (Host → Device)
    0x85, 0x03,                         // Report ID = 3
    0x09, 0x02,                 // Usage (0x02)
    0x15, 0x00,                // Logical Min (0)
    0x26, 0xFF, 0x00,    // Logical Max (255)
    0x75, 0x08,               // Report Size (8 bits)
    0x95, 0x3F,               // Report Count (64 bytes)
    0x91, 0x02,               // Output (Data,Var,Abs)

    0xC0,
};

typedef struct __attribute__((packed)) {
    uint8_t buttons[BUTTON_BYTES];
    int8_t axes[NUM_AXIS];
} joystick_hid_report_t;

#endif