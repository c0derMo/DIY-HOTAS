#include "Arduino.h"
#include "MatrixReader.h"

ButtonMatrixReader::ButtonMatrixReader(uint8_t rowNum, uint8_t colNum, uint8_t *rowPins, uint8_t *colPins) {
    this->rowNum = rowNum;
    this->colNum = colNum;
    this->rowPins = rowPins;
    this->colPins = colPins;

    this->state = new bool[this->rowNum * this->colNum];
}

void ButtonMatrixReader::begin() {
    for (int i = 0; i < this->rowNum; i++) {
        pinMode(this->rowPins[i], OUTPUT);
    }
    for (int i = 0; i < this->colNum; i++) {
        pinMode(this->colPins[i], INPUT);
    }
}

uint16_t ButtonMatrixReader::size() {
    return this->rowNum * this->colNum;
}

void ButtonMatrixReader::scan() {
    for (int x = 0; x < this->rowNum; x++) {
        digitalWrite(this->rowPins[x], HIGH);
        for (int y = 0; y < this->colNum; y++) {
            bool pressed = (digitalRead(this->colPins[y]) == HIGH);
            this->state[x * this->colNum + y] = pressed;
        }
        digitalWrite(this->rowPins[x], LOW);
    }
}

bool* ButtonMatrixReader::getArray() {
    return state;
}