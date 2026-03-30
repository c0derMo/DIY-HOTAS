#ifndef AXIS_POST_PROCESSOR_H
#define AXIS_POST_PROCESSOR_H

#include <Arduino.h>

class AxisPostProcessor {
    private:
        double rawValue;
        double processedValue;
        
        uint16_t center;
        uint16_t min;
        uint16_t max;
        double alpha;
        double deadzone;
        bool inverted;
        double curve_exp;

        double filter(uint32_t value);
        double normalize(double value);
        double clamp(double value);
        double applyDeadzone(double value);
        double linearize(double value);
    public:
        AxisPostProcessor(double alpha, uint16_t center, uint16_t min, uint16_t max, double deadzone, bool inverted, double curve_exp);
        void reconfigure(double alpha, uint16_t center, uint16_t min, uint16_t max, double deadzone, bool inverted, double curve_exp);
        void process(uint32_t value);
        double getValue();
        uint32_t getRawValue();
};

#endif