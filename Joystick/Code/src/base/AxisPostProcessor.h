#ifndef AXIS_POST_PROCESSOR_H
#define AXIS_POST_PROCESSOR_H

#include <Arduino.h>

class AxisPostProcessor {
    private:
        double alpha;
        double rawValue;
        double processedValue;

        double center;
        double min;
        double max;
        double deadzone;

        double filter(int value);
        double normalize(double value);
        double clamp(double value);
        double applyDeadzone(double value);
        double linearize(double value);
    public:
        AxisPostProcessor(double alpha, double center, double min, double max, double deadzone);
        void process(int value);
        double getValue();
        double getRawValue();
};

#endif