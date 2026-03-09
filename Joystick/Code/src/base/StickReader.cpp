#include <Arduino.h>
#include "StickReader.h"
#include "wiring_analog.h"

StickReader::StickReader(uint32_t xPin, uint32_t yPin) {
    this->xPin = xPin;
    this->yPin = yPin;
    
    this->rawX = (StickReader::maxX - StickReader::minX) / 2;
    this->rawY = (StickReader::maxY - StickReader::minY) / 2;
}

void StickReader::begin() {
    pinMode(this->xPin, INPUT);
    pinMode(this->yPin, INPUT);
}

void StickReader::read() {
    this->rawX = analogRead(this->xPin);
    this->rawY = analogRead(this->yPin);
}

double StickReader::getX() {
    return
        StickReader::mix(0, 0.5, StickReader::minX, StickReader::centerX, this->rawX) +
        StickReader::mix(0, 0.5, StickReader::centerX, StickReader::maxX, this->rawX);
}

double StickReader::getY() {
    return
        StickReader::mix(0, 0.5, StickReader::minY, StickReader::centerY, this->rawY) +
        StickReader::mix(0, 0.5, StickReader::centerY, StickReader::maxY, this->rawY);
}

uint32_t StickReader::getRawX() {
    return this->rawX;
}

uint32_t StickReader::getRawY() {
    return this->rawY;
}

double StickReader::mix(double y0, double y1, double x0, double x1, double x) {
    double f = constrain((x - x0) / (x1 - x0), 0.0, 1.0);
    return y0 * (1.0 - f) + y1 * f;
}