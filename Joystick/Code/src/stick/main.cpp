#include <Arduino.h>
#include "MatrixReader.h"
#include "shared/HOTASMessage.h"
#include "shared/config.h"


const FiveWayDirection directionOrder[][5] = {
    { LEFT, UP, DOWN, RIGHT, PUSH },
    { PUSH, LEFT, RIGHT, DOWN, UP },
    { DOWN, UP, LEFT, RIGHT, PUSH },
    { DOWN, UP, LEFT, RIGHT, PUSH }
};

uint8_t fiveWayRowPins[] = {
    7, 9, 10, 11
};
uint8_t buttonRowPins[] = {
    8, 12
};
uint8_t colPins[] = {
    2, 3, 4, 5, 6
};
ButtonMatrixReader fiveWayMatrixReader = ButtonMatrixReader(
    INPUT_5WAY_COUNT,
    5,
    fiveWayRowPins,
    colPins
);
ButtonMatrixReader buttonMatrixReader = ButtonMatrixReader(
    2,
    5,
    buttonRowPins,
    colPins
);

uint8_t joystickPins[][2] = {
    { A2, A1 }
};

void setup() {
    Serial.begin(9600);
    fiveWayMatrixReader.begin();
    buttonMatrixReader.begin();
    pinMode(A1, INPUT);
    pinMode(A2, INPUT);
}

void loop() {
    fiveWayMatrixReader.scan();
    buttonMatrixReader.scan();

    bool* fiveWays = fiveWayMatrixReader.getArray();
    bool* buttons = buttonMatrixReader.getArray();

    HOTASMessage m = HOTASMessage();
    for (int i = 0; i < INPUT_5WAY_COUNT; i++) {
        for (int j = 0; j < 5; j++) {
            if (fiveWays[i * 5 + j]) {
                m.setFiveWay(i, directionOrder[i][j], true);
            }
        }
    }
    for (int i = 0; i < INPUT_BUTTON_COUNT; i++) {
        if (buttons[i]) {
            m.setButton(i, true);
        }
    }
    m.setJoystick(0, analogRead(joystickPins[0][0]), analogRead(joystickPins[0][1]));

    uint8_t buffer[HOTASMessage::MAX_ENCODED_SIZE];
    size_t len = m.encode(buffer);
    Serial.write(buffer, len);
    Serial.write(HOTASMessage::PACKET_DELIMITER);

    delay(DELAY_TIME);
}