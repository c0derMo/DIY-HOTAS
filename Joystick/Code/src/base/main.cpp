#include <Arduino.h>
#include "AxisPostProcessor.h"
#include "ConfigManager.h"
#include "SerialLogger.h"
#include "StickReader.h"
#include "shared/config.h"
#include "shared/HOTASMessage.h"
#include "HIDDescriptor.h"
#include <Adafruit_TinyUSB.h>

HardwareSerial StickSerial(PA3, PA2);
StickReader StickReader(PB0, PB1);

AxisPostProcessor* ThumbstickProcessors[] = {
    new AxisPostProcessor(THUMBSTICK_ALPHA, THUMBSTICK_CENTER, THUMBSTICK_MIN, THUMBSTICK_MAX, THUMBSTICK_DEADZONE, THUMBSTICK_X_INVERT, THUMBSTICK_CURVE_EXP),
    new AxisPostProcessor(THUMBSTICK_ALPHA, THUMBSTICK_CENTER, THUMBSTICK_MIN, THUMBSTICK_MAX, THUMBSTICK_DEADZONE, THUMBSTICK_Y_INVERT, THUMBSTICK_CURVE_EXP),
};

Adafruit_USBD_HID usb_hid;
joystick_hid_report_t joystick_report;

ConfigManager Config(&usb_hid, &StickReader, ThumbstickProcessors[0], ThumbstickProcessors[1]);

uint8_t messageIndex = 0;
uint8_t messageBuffer[HOTASMessage::MAX_ENCODED_SIZE];

void set_report_callback(uint8_t report_id, hid_report_type_t report_type, const uint8_t *buffer, uint16_t bufsize) {
    Config.hid_callback(report_id, report_type, buffer, bufsize);
    usb_hid.sendReport(0, buffer, bufsize);
}

uint16_t get_report_callback(uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen) {
    if (report_id == 2 && Config.hasPendingResponse()) {
        uint16_t to_copy = reqlen;
        if (to_copy > 64) {
            to_copy = 64;
        }
        memcpy(buffer, Config.getPendingResponse(), to_copy);
        Config.clearPendingResponse();
        LOG("main", "Returning ConfigManager response");
        return to_copy;
    }
    return 0;
}

void setup() {
    #ifndef ENABLE_SERIAL
        TinyUSBDevice.clearConfiguration();
    #endif
    
    TinyUSBDevice.setManufacturerDescriptor("CurryMaker");
    TinyUSBDevice.setProductDescriptor("Stick Mk2");
    TinyUSBDevice.setID(0x1209, 0x0001);

    if (!TinyUSBDevice.isInitialized()) {
        TinyUSBDevice.begin(0);
    }

    StickSerial.begin(9600);
    StickReader.begin();
    SerialLogger::begin();

    usb_hid.enableOutEndpoint(true);
    usb_hid.setPollInterval(POLL_INTERVAL_MS);
    usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
    usb_hid.setStringDescriptor("HOTAS Stick");
    usb_hid.setReportCallback(get_report_callback, set_report_callback);
    usb_hid.begin();

    if (TinyUSBDevice.mounted()) {
        TinyUSBDevice.detach();
        delay(10);
        TinyUSBDevice.attach();
    }

    SerialLogger::wait();

    SerialLogger::log("main", "Stick started");
    Config.readFromEEPROM();
    Config.reconfigureAllAxis();
}

void handleMessage() {
    HOTASMessage message = HOTASMessage();
    if (!message.decode(messageBuffer, messageIndex)) {
        LOG("main", "Invalid stick message received");
    }

    String out = "";
    memset(joystick_report.buttons, 0, (NUM_BUTTONS + 7) / 8);
    int bit = 0;
    for (int i = 0; i < INPUT_5WAY_COUNT; i++) {
        for (int j = 0; j < 5; j++) {
            if (message.getFiveWay(i, (FiveWayDirection) j)) {
                joystick_report.buttons[bit / 8] |= (1U << (bit % 8));
            }
            bit += 1;
        }
    }
    for (int i = 0; i < INPUT_BUTTON_COUNT; i++) {
        if (message.getButton(i)) {
            joystick_report.buttons[bit / 8] |= (1U << (bit % 8));
        }
        bit += 1;
    }
    for (int i = 0; i < INPUT_JOYSTICK_COUNT; i++) {
        ThumbstickProcessors[i*2]->process(message.getJoystickX(i));
        ThumbstickProcessors[i*2 + 1]->process(message.getJoystickY(i));
        joystick_report.axes[2 + 2 * i] = ThumbstickProcessors[i*2]->getValue() * 127;
        joystick_report.axes[3 + 2 * i] = ThumbstickProcessors[i*2 + 1]->getValue() * 127;
    }
}

void loop() {
    while (StickSerial.available()) {
        uint8_t b = StickSerial.read();

        if (b == HOTASMessage::PACKET_DELIMITER) {
            handleMessage();
            messageIndex = 0;
        } else {
            messageBuffer[messageIndex++] = b;
        }
    }

    StickReader.read();

    // Main stick
    joystick_report.axes[0] = StickReader.getX() * 127;
    joystick_report.axes[1] = StickReader.getY() * 127;

    if (TinyUSBDevice.mounted() && usb_hid.ready()) {
        bool joystick_send = usb_hid.sendReport(1, &joystick_report, sizeof(joystick_report));
        if (!joystick_send) {
            LOG("main", "Joystick sendReport failed!");
        }
    }

    delay(DELAY_TIME);
}