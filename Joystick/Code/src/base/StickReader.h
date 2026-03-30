#ifndef STICKREADER_H
#define STICKREADER_H

#include "AxisPostProcessor.h"
#include <Arduino.h>

class StickReader {
    private:
        AxisPostProcessor *xAxis;
        AxisPostProcessor *yAxis;
        uint32_t xPin;
        uint32_t yPin;
    public:
        StickReader(uint32_t xPin, uint32_t yPin);
        void begin();
        void read();
        double getX();
        double getY();
        double getRawX();
        double getRawY();
        AxisPostProcessor *getXAxis();
        AxisPostProcessor *getYAxis();
};

#endif