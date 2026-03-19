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

        static const uint32_t minX = 570;
        static const uint32_t maxX = 930;
        static const uint32_t centerX = 780;
        static const bool invertX = false;

        static const uint32_t minY = 660;
        static const uint32_t maxY = 805;
        static const uint32_t centerY = 725;
        static const bool invertY = true;

        static constexpr double alpha = 0.2;
        static constexpr double deadzone = 0.03;

    public:
        StickReader(uint32_t xPin, uint32_t yPin);
        void begin();
        void read();
        double getX();
        double getY();
        double getRawX();
        double getRawY();
};

#endif