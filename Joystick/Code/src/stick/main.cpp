#include <Arduino.h>
#include "MatrixReader.h"
#include "shared/HOTASMessage.h"
#include "shared/config.h"

const uint8_t fiveWay_offset[] = {
    0, 5, 10, 0
};

const FiveWayDirection directionOrder[][5] = {
    { UP, DOWN, LEFT, RIGHT, PUSH },
    { UP, DOWN, LEFT, RIGHT, PUSH },
    { UP, DOWN, LEFT, RIGHT, PUSH },
    { UP, DOWN, LEFT, RIGHT, PUSH }
};

uint8_t rowPins[] = {
    7, 8, 9
};
uint8_t colPins[] = {
    2, 3, 4, 5, 6
};
ButtonMatrixReader matrixReader = ButtonMatrixReader(
    3,
    5,
    rowPins,
    colPins
);

void setup() {
    Serial.begin(9600);
    matrixReader.begin();
}

void loop() {
    matrixReader.scan();
    bool* state = matrixReader.getArray();

    HOTASMessage m = HOTASMessage();
    for (int i = 0; i < INPUT_5WAY_COUNT; i++) {
        for (int j = 0; j < 5; j++) {
            if (state[fiveWay_offset[i] + j]) {
                m.setFiveWay(i, directionOrder[i][j], true);
            }
        }
    }

    uint8_t buffer[HOTASMessage::MAX_ENCODED_SIZE];
    size_t len = m.encode(buffer);
    Serial.write(buffer, len);
    Serial.write(HOTASMessage::PACKET_DELIMITER);

    delay(DELAY_TIME);
}