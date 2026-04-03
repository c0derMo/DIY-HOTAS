#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include "AxisPostProcessor.h"
#include "StickReader.h"
#include <Arduino.h>
#include <Adafruit_TinyUSB.h>

typedef struct {
    uint16_t min;
    uint16_t center;
    uint16_t max;
    bool invert;
    double curve_exp;
} axis_config;

typedef struct {
    axis_config main_x;
    axis_config main_y;
    axis_config thumb_x;
    axis_config thumb_y;
    double main_alpha;
    double main_deadzone;
    double thumb_alpha;
    double thumb_deadzone;
} joystick_eeprom_config;

enum JoystickAxis : uint8_t {
    MAIN_X = 0x01,
    MAIN_Y = 0x02,
    THUMB_X = 0x03,
    THUMB_Y = 0x04,
};

enum AxisBoundary : uint8_t {
    MIN = 0x01,
    CENTER = 0x02,
    MAX = 0x03,
};

enum hid_config_command : uint8_t {
    COMMAND_WRITE = 0x01,
    COMMAND_READ = 0x02,
    COMMAND_RESET = 0x03,
    COMMAND_READ_AXIS = 0x10,
    COMMAND_RECORD_BOUNDARY = 0x20,
    COMMAND_TOGGLE_INVERT = 0x21,
    COMMAND_SET_ALPHA = 0x22,
    COMMAND_SET_DEADZONE = 0x23,
    COMMAND_SET_VALUE = 0x24,
    COMMAND_SET_CURVE_EXP = 0x25,
};

class ConfigManager {
    private:
        Adafruit_USBD_HID *device;
        StickReader *main_stick;
        AxisPostProcessor *thumb_x;
        AxisPostProcessor *thumb_y;

        uint8_t pending_response[63];
        bool is_pending;
        joystick_eeprom_config config;

        void set_success_response(hid_config_command inputCommand);

        void set_defaults();

        void toggle_invert(JoystickAxis axis);
        void record_value(JoystickAxis axis, AxisBoundary value);
        void reconfigure_axis(JoystickAxis axis);
        void set_axis_deadzone(JoystickAxis axis, double deadzone);
        void set_axis_alpha(JoystickAxis axis, double alpha);
        void set_axis_curve_exp(JoystickAxis, double exp);
        void set_uint16_value(JoystickAxis axis, AxisBoundary valueToSet, uint16_t value);
        void set_axis_config(JoystickAxis axis, axis_config config);

        void read_axis(JoystickAxis axis);
        
        double get_axis_alpha(JoystickAxis axis);
        double get_axis_deadzone(JoystickAxis axis);
        axis_config get_axis_config(JoystickAxis axis);
        AxisPostProcessor *get_axis_processor(JoystickAxis axis);
    public:
        ConfigManager(Adafruit_USBD_HID *device, StickReader *main_stick, AxisPostProcessor *thumb_x, AxisPostProcessor *thumb_y);
        void readFromEEPROM();
        void writeToEEPROM();
        void hid_callback(uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize);

        void reconfigureAllAxis();
        bool hasPendingResponse();
        uint8_t *getPendingResponse();
        void clearPendingResponse();
        void checkAndSendPendingResponse();
};

#endif