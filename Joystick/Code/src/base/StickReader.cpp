#include <Arduino.h>
#include "StickReader.h"
#include "AxisPostProcessor.h"
#include "Wire.h"
#include "shared/config.h"

StickReader::StickReader(uint32_t xSDA, uint32_t xSCL, uint32_t ySDA, uint32_t ySCL) {
    this->xAxis = new TwoWire(xSDA, xSCL);
    this->yAxis = new TwoWire(ySDA, ySCL);

    this->xAxisProcessor = new AxisPostProcessor(STICK_ALPHA, STICK_X_CENTER, STICK_X_MIN, STICK_X_MAX, STICK_DEADZONE, STICK_X_INVERT, STICK_CURVE_EXP);
    this->yAxisProcessor = new AxisPostProcessor(STICK_ALPHA, STICK_Y_CENTER, STICK_Y_MIN, STICK_Y_MAX, STICK_DEADZONE, STICK_Y_INVERT, STICK_CURVE_EXP);
}

void StickReader::begin() {
    this->xAxis->begin();
    this->yAxis->begin();
}

uint16_t StickReader::readRawAngle(TwoWire *wire) {
    wire->beginTransmission(AS5600_ADDR);
    wire->write(ANGLE_REG);
    wire->endTransmission();
    wire->requestFrom(AS5600_ADDR, 2);
    if (wire->available() == 2) {
        uint8_t highByte = wire->read();
        uint8_t lowByte = wire->read();
        return (highByte << 8) | lowByte;
    }
    return 0;
}

void StickReader::read() {
    this->xAxisProcessor->process(StickReader::readRawAngle(this->xAxis));
    this->yAxisProcessor->process(StickReader::readRawAngle(this->yAxis));
}

double StickReader::getX() {
    return this->xAxisProcessor->getValue();
}

double StickReader::getY() {
    return this->yAxisProcessor->getValue();
}

uint32_t StickReader::getRawX() {
    return this->xAxisProcessor->getRawValue();
}

uint32_t StickReader::getRawY() {
    return this->yAxisProcessor->getRawValue();
}

AxisPostProcessor *StickReader::getXAxis() {
    return this->xAxisProcessor;
}

AxisPostProcessor *StickReader::getYAxis() {
    return this->yAxisProcessor;
}