#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#include "AxisPostProcessor.h"

AxisPostProcessor::AxisPostProcessor(double alpha, double center, double min, double max, double deadzone) {
    this->alpha = alpha;
    this->center = center;
    this->min = min;
    this->max = max;
    this->deadzone = deadzone;
    this->rawValue = center;
    this->processedValue = 0.5;
}

double AxisPostProcessor::filter(int value) {
    return this->rawValue + this->alpha * (value - this->rawValue);
}

double AxisPostProcessor::normalize(double value) {
    double normalized = value - this->center;
    if (normalized > 0) {
        return normalized / (this->max - this->center);
    } else {
        return normalized / (this->center - this->min);
    }
}

double AxisPostProcessor::clamp(double value) {
    return constrain(value, -1.0, 1.0);
}

double AxisPostProcessor::applyDeadzone(double value) {
    if (abs(value) < this->deadzone) {
        return 0.0;
    }
    return value;
}

double AxisPostProcessor::linearize(double value) {
    return pow(value, 3);
}

void AxisPostProcessor::process(int value) {
    this->rawValue = value;
    double filtered = this->filter(value);

    double normalized = this->normalize(filtered);
    double clamped = this->clamp(normalized);
    double deadzoned = this->applyDeadzone(clamped);
    double linearized = this->linearize(deadzoned);
    this->processedValue = linearized;
}

double AxisPostProcessor::getValue() {
    return this->processedValue;
}

double AxisPostProcessor::getRawValue() {
    return this->rawValue;
}