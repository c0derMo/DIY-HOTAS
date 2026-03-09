#ifndef MATRIXREADER_H
#define MATRIXREADER_H

#include <Arduino.h>

class ButtonMatrixReader {
    private:
        uint8_t rowNum;
        uint8_t colNum;
        uint8_t *rowPins;
        uint8_t *colPins;
        bool *state;
    
    public:
        ButtonMatrixReader(uint8_t rowNum, uint8_t colNum, uint8_t *rowPins, uint8_t *colPins);
        uint16_t size();
        void begin();
        void scan();
        bool* getArray();
        void read(bool *output);
};

#endif