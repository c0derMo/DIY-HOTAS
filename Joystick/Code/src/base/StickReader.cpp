#include <Arduino.h>
#include "StickReader.h"
#include "AxisPostProcessor.h"
#include "shared/config.h"

StickReader::StickReader(uint32_t xPin, uint32_t yPin) {
    this->xPin = xPin;
    this->yPin = yPin;

    this->xAxis = new AxisPostProcessor(STICK_ALPHA, STICK_X_CENTER, STICK_X_MIN, STICK_X_MAX, STICK_DEADZONE, STICK_X_INVERT, STICK_CURVE_EXP);
    this->yAxis = new AxisPostProcessor(STICK_ALPHA, STICK_Y_CENTER, STICK_Y_MIN, STICK_Y_MAX, STICK_DEADZONE, STICK_Y_INVERT, STICK_CURVE_EXP);
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
    return this->xAxis->getValue();
}

double StickReader::getY() {
    return this->yAxis->getValue();
}

double StickReader::getRawX() {
    return this->xAxis->getRawValue();
}

double StickReader::getRawY() {
    return this->yAxis->getRawValue();
}

AxisPostProcessor *StickReader::getXAxis() {
    return this->xAxis;
}

AxisPostProcessor *StickReader::getYAxis() {
    return this->yAxis;
}