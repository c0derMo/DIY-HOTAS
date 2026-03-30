#include "ConfigManager.h" 
#include "AxisPostProcessor.h"
#include "EEPROM.h"
#include "SerialLogger.h"

void packDouble(double value, uint8_t* buffer) {
  union {
    double d;
    uint8_t bytes[8];
  } converter;

  converter.d = value;
  memcpy(buffer, converter.bytes, 8);
}

double unpackDouble(const uint8_t* buffer) {
  union {
    double d;
    uint8_t bytes[8];
  } converter;

  memcpy(converter.bytes, buffer, 8);
  return converter.d;
}

void packUInt16(uint16_t value, uint8_t* buffer) {
    union {
        uint16_t v;
        uint8_t bytes[2];
    } converter;

    converter.v = value;
    memcpy(buffer, converter.bytes, 2);
}

uint16_t unpackUInt16(const uint8_t* buffer) {
    union {
        uint16_t v;
        uint8_t bytes[2];
    } converter;

    memcpy(converter.bytes, buffer, 2);
    return converter.v;
}

ConfigManager::ConfigManager(Adafruit_USBD_HID *device, StickReader *main_stick, AxisPostProcessor *thumb_x, AxisPostProcessor *thumb_y) {
    this->device = device;
    this->main_stick = main_stick;
    this->thumb_x = thumb_x;
    this->thumb_y = thumb_y;
    this->set_defaults();
    this->is_pending = false;
}

void ConfigManager::set_defaults() {
    this->config = {
        .main_x = {
            .min = STICK_X_MIN,
            .center = STICK_X_CENTER,
            .max = STICK_X_MAX,
            .invert = STICK_X_INVERT,
            .curve_exp = STICK_CURVE_EXP,
        },
        .main_y = {
            .min = STICK_Y_MIN,
            .center = STICK_Y_CENTER,
            .max = STICK_Y_MAX,
            .invert = STICK_Y_INVERT,
            .curve_exp = STICK_CURVE_EXP,
        },
        .thumb_x = {
            .min = THUMBSTICK_MIN,
            .center = THUMBSTICK_CENTER,
            .max = THUMBSTICK_MAX,
            .invert = THUMBSTICK_X_INVERT,
            .curve_exp = THUMBSTICK_CURVE_EXP,
        },
        .thumb_y = {
            .min = THUMBSTICK_MIN,
            .center = THUMBSTICK_CENTER,
            .max = THUMBSTICK_MAX,
            .invert = THUMBSTICK_Y_INVERT,
            .curve_exp = THUMBSTICK_CURVE_EXP,
        },
        .main_alpha = STICK_ALPHA,
        .main_deadzone = STICK_DEADZONE,
        .thumb_alpha = THUMBSTICK_ALPHA,
        .thumb_deadzone = THUMBSTICK_DEADZONE,
    };
    this->reconfigureAllAxis();
}

void ConfigManager::hid_callback(uint8_t report_id, hid_report_type_t report_type, const uint8_t *buffer, uint16_t bufsize) {
    if (report_type != HID_REPORT_TYPE_OUTPUT) {
        return;
    }
    if (buffer[0] != 3) {
        return;
    }
    LOGF("ConfigManager", "Received HID report with report ID: %d, internal: %u, size: %u", report_id, buffer[0], bufsize);

    memset(this->pending_response, 0, sizeof(this->pending_response));
    switch (buffer[1]) {
        case COMMAND_WRITE:
            this->writeToEEPROM();
            this->set_success_response(COMMAND_WRITE);
            break;
        case COMMAND_READ:
            this->readFromEEPROM();
            this->set_success_response(COMMAND_READ);
            break;
        case COMMAND_RESET:
            this->set_defaults();
            this->set_success_response(COMMAND_RESET);
            break;
        case COMMAND_TOGGLE_INVERT:
            this->toggle_invert(static_cast<JoystickAxis>(buffer[2]));
            this->set_success_response(COMMAND_TOGGLE_INVERT);
            break;
        case COMMAND_RECORD_BOUNDARY:
            this->record_value(static_cast<JoystickAxis>(buffer[2]), static_cast<JoystickConfigValue>(buffer[3]));
            break;
        case COMMAND_SET_ALPHA:
            this->set_axis_alpha(static_cast<JoystickAxis>(buffer[2]), unpackDouble(&buffer[3]));
            break;
        case COMMAND_SET_DEADZONE:
            this->set_axis_deadzone(static_cast<JoystickAxis>(buffer[2]), unpackDouble(&buffer[3]));
            break;
        case COMMAND_SET_CURVE_EXP:
            this->set_axis_curve_exp(static_cast<JoystickAxis>(buffer[2]), unpackDouble(&buffer[3]));
            break;
        case COMMAND_SET_VALUE:
            this->set_uint16_value(static_cast<JoystickAxis>(buffer[2]), static_cast<JoystickConfigValue>(buffer[3]), unpackUInt16(&buffer[4]));
            break;
        case COMMAND_READ_AXIS:
            this->read_axis(static_cast<JoystickAxis>(buffer[2]));
            break;
    }
}

void ConfigManager::set_success_response(hid_config_command inputCommand) {
    this->pending_response[0] = 0xFF;
    this->pending_response[1] = inputCommand;
    this->is_pending = true;
}

bool ConfigManager::hasPendingResponse() {
    return this->is_pending;
}

uint8_t *ConfigManager::getPendingResponse() {
    return this->pending_response;
}

void ConfigManager::clearPendingResponse() {
    this->is_pending = false;
}

void ConfigManager::readFromEEPROM() {
    LOG("ConfigManager", "Reading from EEPROM");
    uint8_t magicByte = EEPROM.read(0x0);
    LOGF("ConfigManager", "Magic byte: %02X", magicByte);
    if (magicByte == 0xDE) {
        EEPROM.get(0x1, this->config);
    } else {
        LOG("ConfigManager", "Aborting read, EEPROM is unwritten");
    }
}

void ConfigManager::writeToEEPROM() {
    LOG("ConfigManager", "Writing to EEPROM");
    EEPROM.put(0x1, this->config);
    EEPROM.update(0x0, 0xDE);
}

void ConfigManager::reconfigureAllAxis() {
    this->reconfigure_axis(MAIN_X);
    this->reconfigure_axis(MAIN_Y);
    this->reconfigure_axis(THUMB_X);
    this->reconfigure_axis(THUMB_Y);
}

axis_config ConfigManager::get_axis_config(JoystickAxis axis) {
    switch (axis) {
        case MAIN_X:
            return this->config.main_x;
        case MAIN_Y:
            return this->config.main_y;
        case THUMB_X:
            return this->config.thumb_x;
        case THUMB_Y:
            return this->config.thumb_y;
    }
    return {};
}

void ConfigManager::set_axis_config(JoystickAxis axis, axis_config config) {
    switch (axis) {
        case MAIN_X:
            this->config.main_x = config;
            break;
        case MAIN_Y:
            this->config.main_y = config;
            break;
        case THUMB_X:
            this->config.thumb_x = config;
            break;
        case THUMB_Y:
            this->config.thumb_y = config;
            break;
    }
    this->reconfigure_axis(axis);
}

double ConfigManager::get_axis_alpha(JoystickAxis axis) {
    switch (axis) {
        case MAIN_X:
        case MAIN_Y:
            return this->config.main_alpha;
        case THUMB_X:
        case THUMB_Y:
            return this->config.thumb_alpha;
    }
    return 0.0;
}

double ConfigManager::get_axis_deadzone(JoystickAxis axis) {
    switch (axis) {
        case MAIN_X:
        case MAIN_Y:
            return this->config.main_deadzone;
        case THUMB_X:
        case THUMB_Y:
            return this->config.thumb_deadzone;
    }
    return 0.0;
}

void ConfigManager::toggle_invert(JoystickAxis axis) {
    axis_config config = this->get_axis_config(axis);
    config.invert = !config.invert;
    this->set_axis_config(axis, config);
}

void ConfigManager::record_value(JoystickAxis axis, JoystickConfigValue value) {
    AxisPostProcessor *processor = this->get_axis_processor(axis);
    uint16_t rawValue = processor->getRawValue();
    axis_config config = this->get_axis_config(axis);
    switch (value) {
        case MIN:
            config.min = rawValue;
            break;
        case CENTER:
            config.center = rawValue;
            break;
        case MAX:
            config.max = rawValue;
            break;
    }
    this->set_axis_config(axis, config);
    this->set_success_response(COMMAND_RECORD_BOUNDARY);
    this->pending_response[2] = axis;
    this->pending_response[3] = value;
    packUInt16(rawValue, &this->pending_response[4]);
}

void ConfigManager::set_axis_alpha(JoystickAxis axis, double alpha) {
    switch (axis) {
        case MAIN_X:
        case MAIN_Y:
            this->config.main_alpha = alpha;
            this->reconfigure_axis(MAIN_X);
            this->reconfigure_axis(MAIN_Y);
            break;
        case THUMB_X:
        case THUMB_Y:
            this->config.thumb_alpha = alpha;
            this->reconfigure_axis(THUMB_X);
            this->reconfigure_axis(THUMB_Y);
    }
    this->set_success_response(COMMAND_SET_ALPHA);
    this->pending_response[2] = axis;
    packDouble(alpha, &this->pending_response[3]);
}

void ConfigManager::set_axis_deadzone(JoystickAxis axis, double deadzone) {
    switch (axis) {
        case MAIN_X:
        case MAIN_Y:
            this->config.main_deadzone = deadzone;
            this->reconfigure_axis(MAIN_X);
            this->reconfigure_axis(MAIN_Y);
            break;
        case THUMB_X:
        case THUMB_Y:
            this->config.thumb_deadzone = deadzone;
            this->reconfigure_axis(THUMB_X);
            this->reconfigure_axis(THUMB_Y);
    }
    this->set_success_response(COMMAND_SET_DEADZONE);
    this->pending_response[2] = axis;
    packDouble(deadzone, &this->pending_response[3]);
}

void ConfigManager::set_axis_curve_exp(JoystickAxis axis, double exp) {
    axis_config config = this->get_axis_config(axis);
    config.curve_exp = exp;
    this->set_axis_config(axis, config);
    this->set_success_response(COMMAND_SET_CURVE_EXP);
    this->pending_response[2] = axis;
    packDouble(exp, &this->pending_response[3]);
}

void ConfigManager::set_uint16_value(JoystickAxis axis, JoystickConfigValue valueToSet, uint16_t value) {
    axis_config config = this->get_axis_config(axis);
    switch (valueToSet) {
        case MIN:
            config.min = value;
            break;
        case MAX:
            config.max = value;
            break;
        case CENTER:
            config.center = value;
            break;
    }
    this->set_axis_config(axis, config);
    this->set_success_response(COMMAND_SET_VALUE);
    this->pending_response[2] = axis;
    this->pending_response[3] = valueToSet;
    packUInt16(value, &this->pending_response[4]);
}

void ConfigManager::reconfigure_axis(JoystickAxis axis) {
    AxisPostProcessor *processor = this->get_axis_processor(axis);
    axis_config config = this->get_axis_config(axis);
    double alpha = this->get_axis_alpha(axis);
    double deadzone = this->get_axis_deadzone(axis);

    processor->reconfigure(
        alpha,
        config.center,
        config.min,
        config.max,
        deadzone,
        config.invert,
        config.curve_exp
    );
    LOGF("ConfigManager", "Reconfiguring %d with a=%.2f, dz=%.2f, axis_config{min=%u, center=%u, max=%u, invert=%s, curve_exp=%.2f}", axis, alpha, deadzone, config.min, config.center, config.max, config.invert ? "T" : "F", config.curve_exp);
}

AxisPostProcessor *ConfigManager::get_axis_processor(JoystickAxis axis) {
    switch (axis) {
        case MAIN_X:
            return this->main_stick->getXAxis();
        case MAIN_Y:
            return this->main_stick->getYAxis();
        case THUMB_X:
            return this->thumb_x;
        case THUMB_Y:
            return this->thumb_y;
    }
    return NULL;
}

void ConfigManager::read_axis(JoystickAxis axis) {
    axis_config config = this->get_axis_config(axis);
    double alpha = this->get_axis_alpha(axis);
    double deadzone = this->get_axis_deadzone(axis);
    this->set_success_response(COMMAND_READ_AXIS);
    this->pending_response[2] = axis;
    packUInt16(config.min, &this->pending_response[3]);
    packUInt16(config.center, &this->pending_response[5]);
    packUInt16(config.max, &this->pending_response[7]);
    this->pending_response[7] = config.invert ? 0xFF : 0x00;
    packDouble(alpha, &this->pending_response[10]);
    packDouble(deadzone, &this->pending_response[18]);
    packDouble(config.curve_exp, &this->pending_response[26]);
}