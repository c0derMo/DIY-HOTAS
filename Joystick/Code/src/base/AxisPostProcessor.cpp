#include <Arduino.h>
#include "AxisPostProcessor.h"
#include "SerialLogger.h"

AxisPostProcessor::AxisPostProcessor(double alpha, uint16_t center, uint16_t min, uint16_t max, double deadzone, bool invert, double curve_exp) {
    this->reconfigure(alpha, center, min, max, deadzone, invert, curve_exp);
    this->rawValue = center;
    this->processedValue = 0.5;
}

void AxisPostProcessor::reconfigure(double alpha, uint16_t center, uint16_t min, uint16_t max, double deadzone, bool invert, double curve_exp) {
    this->alpha = alpha;
    this->center = center;
    this->min = min;
    this->max = max;
    this->deadzone = deadzone;
    this->inverted = invert;
    this->curve_exp = curve_exp;
    LOGF("AxisPostProcessor", "Reconfiguring with a=%.2f, dz=%.2f, axis_config{min=%u, center=%u, max=%u, invert=%s, curve_exp=%.2f}", alpha, deadzone, min, center, max, invert ? "T" : "F", curve_exp);
}

double AxisPostProcessor::filter(uint32_t value) {
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
    if (value > 0) {
        return pow(abs(value), this->curve_exp);
    } else {
        return -pow(abs(value), this->curve_exp);
    }
}

void AxisPostProcessor::process(uint32_t value) {
    double filtered = this->filter(value);
    this->rawValue = filtered;

    double normalized = this->normalize(filtered);
    double clamped = this->clamp(normalized);
    double deadzoned = this->applyDeadzone(clamped);
    double linearized = this->linearize(deadzoned);
    if (this->inverted) {
        this->processedValue = -linearized;
    } else {
        this->processedValue = linearized;
    }
}

double AxisPostProcessor::getValue() {
    return this->processedValue;
}

uint32_t AxisPostProcessor::getRawValue() {
    return (uint32_t)this->rawValue;
}