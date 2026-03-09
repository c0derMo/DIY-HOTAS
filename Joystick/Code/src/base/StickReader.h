#ifndef STICKREADER_H
#define STICKREADER_H

#include <Arduino.h>

class StickReader {
    private:
        uint32_t xPin;
        uint32_t yPin;

        uint32_t rawX;
        uint32_t rawY;

        static const uint32_t minX = 640;
        static const uint32_t maxX = 860;
        static const uint32_t centerX = 800;

        static const uint32_t minY = 680;
        static const uint32_t maxY = 820;
        static const uint32_t centerY = 740;

        static double mix(double y0, double y1, double x0, double x1, double x);
    
    public:
        StickReader(uint32_t xPin, uint32_t yPin);
        void begin();
        void read();
        double getX();
        double getY();
        uint32_t getRawX();
        uint32_t getRawY();
};

#endif