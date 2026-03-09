#include <Arduino.h>
#include "StickReader.h"
#include "shared/config.h"
#include "shared/HOTASMessage.h"

HardwareSerial StickSerial(PA3, PA2);
StickReader StickReader(PB0, PB1);

uint8_t messageIndex = 0;
uint8_t messageBuffer[HOTASMessage::MAX_ENCODED_SIZE];

void setup() {
    Serial.begin(9600);
    StickSerial.begin(9600);
    StickReader.begin();

    while (!Serial);

    Serial.println("Ready to relay :D");
}

void handleMessage() {
    HOTASMessage message = HOTASMessage();
    if (!message.decode(messageBuffer, messageIndex)) {
        Serial.println("Invalid message received!");
        return;
    }

    String out = "";
    for (int i = 0; i < INPUT_5WAY_COUNT; i++) {
        for (int j = 0; j < 5; j++) {
            if (message.getFiveWay(i, (FiveWayDirection) j)) {
                out += "5-Way " + String(i) + " Button " + String(j) + " ";
            }
        }
    }

    Serial.println(out);
}

void loop() {
    // while (StickSerial.available()) {
    //     uint8_t b = StickSerial.read();

    //     if (b == HOTASMessage::PACKET_DELIMITER) {
    //         handleMessage();
    //         messageIndex = 0;
    //     } else {
    //         messageBuffer[messageIndex++] = b;
    //     }
    // }

    StickReader.read();

    Serial.println("X: " + String(StickReader.getX()) + " (" + String(StickReader.getRawX()) + ") Y: " + String(StickReader.getY()) + " (" + String(StickReader.getRawY()) + ")");

    delay(DELAY_TIME / 2);
}