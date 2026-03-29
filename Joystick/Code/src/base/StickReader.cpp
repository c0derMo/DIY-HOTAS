#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#include "StickReader.h"
#include "AxisPostProcessor.h"
#include "wiring_analog.h"

StickReader::StickReader(uint32_t xPin, uint32_t yPin) {
    this->xPin = xPin;
    this->yPin = yPin;

    this->xAxis = new AxisPostProcessor(this->alpha, this->centerX, this->minX, this->maxX, this->deadzone);
    this->yAxis = new AxisPostProcessor(this->alpha, this->centerY, this->minY, this->maxY, this->deadzone);
}

void StickReader::begin() {
    pinMode(this->xPin, INPUT);
    pinMode(this->yPin, INPUT);
}

void StickReader::read() {
    this->xAxis->process(analogRead(this->xPin));
    this->yAxis->process(analogRead(this->yPin));
}

double StickReader::getX() {
    if (StickReader::invertX) {
        return -this->xAxis->getValue();
    } else {
        return this->xAxis->getValue();
    }
}

double StickReader::getY() {
    if (StickReader::invertY) {
        return -this->yAxis->getValue();
    } else {
        return this->yAxis->getValue();
    }
}

double StickReader::getRawX() {
    return this->xAxis->getRawValue();
}

double StickReader::getRawY() {
    return this->yAxis->getRawValue();
}