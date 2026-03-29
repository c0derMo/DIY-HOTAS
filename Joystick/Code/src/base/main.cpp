#include <Arduino.h>
#include "AxisPostProcessor.h"
#include "StickReader.h"
#include "arduino/Adafruit_USBD_Device.h"
#include "shared/config.h"
#include "shared/HOTASMessage.h"
#include "HIDDescriptor.h"
#include <Adafruit_TinyUSB.h>
// #include <EEPROM.h>

HardwareSerial StickSerial(PA3, PA2);
StickReader StickReader(PB0, PB1);

AxisPostProcessor* ThumbstickProcessors[] = {
    new AxisPostProcessor(0.15, JOYSTICK_CENTER, JOYSTICK_MIN, JOYSTICK_MAX, 0.03),
    new AxisPostProcessor(0.15, JOYSTICK_CENTER, JOYSTICK_MIN, JOYSTICK_MAX, 0.03),
};

Adafruit_USBD_HID usb_hid;

hid_report_t gp;

uint8_t messageIndex = 0;
uint8_t messageBuffer[HOTASMessage::MAX_ENCODED_SIZE];

void setup() {
    SerialTinyUSB.begin(9600);
    StickSerial.begin(9600);
    StickReader.begin();

    TinyUSBDevice.setManufacturerDescriptor("CurryMaker");
    TinyUSBDevice.setProductDescriptor("Stick Mk2");
    TinyUSBDevice.setID(0x1209, 0x0001);

    if (!TinyUSBDevice.isInitialized()) {
        TinyUSBDevice.begin(0);
    }

    usb_hid.setPollInterval(2);
    usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
    usb_hid.setStringDescriptor("HOTAS Stick");
    usb_hid.begin();

    if (TinyUSBDevice.mounted()) {
        TinyUSBDevice.detach();
        delay(10);
        TinyUSBDevice.attach();
    }

    // EEPROM.read(0x0);

    // while (!SerialTinyUSB);

    SerialTinyUSB.println("Stick ready :D");
}

String handleMessage() {
    HOTASMessage message = HOTASMessage();
    if (!message.decode(messageBuffer, messageIndex)) {
        return "Invalid message received!";
    }

    String out = "";
    memset(gp.buttons, 0, (NUM_BUTTONS + 7) / 8);
    int bit = 0;
    for (int i = 0; i < INPUT_5WAY_COUNT; i++) {
        for (int j = 0; j < 5; j++) {
            if (message.getFiveWay(i, (FiveWayDirection) j)) {
                gp.buttons[bit / 8] |= (1U << (bit % 8));
                out += "5-Way " + String(i) + " Button " + String(j) + " ";
            }
            bit += 1;
        }
    }
    for (int i = 0; i < INPUT_BUTTON_COUNT; i++) {
        if (message.getButton(i)) {
            gp.buttons[bit / 8] |= (1U << (bit % 8));
            out += "Button " + String(i) + " ";
        }
        bit += 1;
    }
    for (int i = 0; i < INPUT_JOYSTICK_COUNT; i++) {
        ThumbstickProcessors[i*2]->process(message.getJoystickX(i));
        ThumbstickProcessors[i*2 + 1]->process(message.getJoystickY(i));
        gp.axes[2 + 2 * i] = ThumbstickProcessors[i*2]->getValue() * -127;
        gp.axes[3 + 2 * i] = ThumbstickProcessors[i*2 + 1]->getValue() * 127;
        out += "Joystick " + String(i) + " X: " + String(ThumbstickProcessors[i*2]->getValue()) + "(" + String(ThumbstickProcessors[i*2]->getRawValue()) + ") Y: " + String(ThumbstickProcessors[i*2+1]->getValue()) + "(" + String(ThumbstickProcessors[i*2+1]->getRawValue()) + ") || ";
    }

    return out;
}

void loop() {
    String msg = "";
    while (StickSerial.available()) {
        uint8_t b = StickSerial.read();

        if (b == HOTASMessage::PACKET_DELIMITER) {
            msg += handleMessage();
            messageIndex = 0;
        } else {
            messageBuffer[messageIndex++] = b;
        }
    }

    StickReader.read();
    if (msg != "") {
        SerialTinyUSB.println(msg + " X: " + String(StickReader.getX()) + " (" + String(StickReader.getRawX()) + ") Y: " + String(StickReader.getY()) + " (" + String(StickReader.getRawY()) + ")");
    }

    // Main stick
    gp.axes[0] = StickReader.getX() * 127;
    gp.axes[1] = StickReader.getY() * 127;

    if (TinyUSBDevice.mounted() && usb_hid.ready()) {
        usb_hid.sendReport(0, &gp, sizeof(gp));
    }

    delay(DELAY_TIME);
}