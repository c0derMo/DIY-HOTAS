#ifndef STICKREADER_H
#define STICKREADER_H

#include "AxisPostProcessor.h"
#include "Wire.h"
#include <Arduino.h>

#define AS5600_ADDR 0x36
#define ANGLE_REG 0x0E

class StickReader {
    private:
        AxisPostProcessor *xAxisProcessor;
        AxisPostProcessor *yAxisProcessor;
        TwoWire *xAxis;
        TwoWire *yAxis;

        static uint16_t readRawAngle(TwoWire *wire);
    public:
        StickReader(uint32_t xSDA, uint32_t xSCL, uint32_t ySDA, uint32_t ySCL);
        void begin();
        void read();
        double getX();
        double getY();
        uint32_t getRawX();
        uint32_t getRawY();
        AxisPostProcessor *getXAxis();
        AxisPostProcessor *getYAxis();
};

#endif