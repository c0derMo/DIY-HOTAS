#include <Arduino.h>
#include "StickReader.h"
#include "arduino/Adafruit_USBD_Device.h"
#include "shared/config.h"
#include "shared/HOTASMessage.h"
#include <Adafruit_TinyUSB.h>

HardwareSerial StickSerial(PA3, PA2);
StickReader StickReader(PB0, PB1);

uint8_t const desc_hid_report[] = {
    TUD_HID_REPORT_DESC_GAMEPAD()
};

Adafruit_USBD_HID usb_hid;

hid_gamepad_report_t gp;

uint8_t messageIndex = 0;
uint8_t messageBuffer[HOTASMessage::MAX_ENCODED_SIZE];

void setup() {
    SerialTinyUSB.begin(9600);
    StickSerial.begin(9600);
    StickReader.begin();
    if (!TinyUSBDevice.isInitialized()) {
        TinyUSBDevice.begin(0);
    }
    
    usb_hid.setPollInterval(2);
    usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
    usb_hid.begin();

    if (TinyUSBDevice.mounted()) {
        TinyUSBDevice.detach();
        delay(10);
        TinyUSBDevice.attach();
    }

    while (!SerialTinyUSB);

    SerialTinyUSB.println("Ready to relay :D");
}

String handleMessage() {
    HOTASMessage message = HOTASMessage();
    if (!message.decode(messageBuffer, messageIndex)) {
        return "Invalid message received!";
    }

    String out = "";
    gp.buttons = 0;
    for (int i = 0; i < INPUT_5WAY_COUNT; i++) {
        for (int j = 0; j < 5; j++) {
            if (message.getFiveWay(i, (FiveWayDirection) j)) {
                gp.buttons |= (1U << (i*5+j));
                out += "5-Way " + String(i) + " Button " + String(j) + " ";
            }
        }
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
    SerialTinyUSB.println(msg + " X: " + String(StickReader.getX()) + " (" + String(StickReader.getRawX()) + ") Y: " + String(StickReader.getY()) + " (" + String(StickReader.getRawY()) + ")");
    gp.x = (StickReader.getX() * 254) - 127;
    gp.y = (StickReader.getY() * 254) - 127;

    if (TinyUSBDevice.mounted() && usb_hid.ready()) {
        usb_hid.sendReport(0, &gp, sizeof(gp));
    }

    delay(DELAY_TIME / 2);
}