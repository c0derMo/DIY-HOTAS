#ifndef HOTASMESSAGE_H
#define HOTASMESSAGE_H

#include "Arduino.h"
#include "config.h"

typedef enum {
    UP = 0,
    DOWN = 1,
    LEFT = 2,
    RIGHT = 3,
    PUSH = 4
} FiveWayDirection;

typedef struct {
    bool input_five_way[INPUT_5WAY_COUNT][5];
    bool input_button[INPUT_BUTTON_COUNT];
    int16_t input_joystick[INPUT_JOYSTICK_COUNT][2];
} HOTASPayload;

class HOTASMessage {
    private:
        HOTASPayload payload;
        
        static uint8_t crc8(const uint8_t* data, size_t len);
        void encodePayload(uint8_t* buffer);
        void decodePayload(uint8_t* buffer);
        static size_t encodeCobs(const uint8_t* data, size_t length, uint8_t* output);
        static size_t decodeCobs(const uint8_t* data, size_t length, uint8_t* output);

    public:
        static constexpr uint8_t PACKET_DELIMITER = 0x00;
        static constexpr uint16_t FIVEWAY_BITS = INPUT_5WAY_COUNT * 5;
        static constexpr uint16_t BUTTON_BITS = INPUT_BUTTON_COUNT;
        static constexpr uint16_t BOOL_BITS = FIVEWAY_BITS + BUTTON_BITS;
        static constexpr uint16_t BOOL_BYTES = (BOOL_BITS + 7) / 8; // fancy way to just ceil
        static constexpr uint16_t JOYSTICK_VALUES = INPUT_JOYSTICK_COUNT * 2;
        static constexpr uint16_t JOYSTICK_BYTES = JOYSTICK_VALUES * sizeof(int16_t);
        static constexpr uint16_t PAYLOAD_SIZE = BOOL_BYTES + JOYSTICK_BYTES;
        static constexpr uint16_t RAW_FRAME_SIZE = 1 + PAYLOAD_SIZE + 1;
        static constexpr uint16_t MAX_ENCODED_SIZE = RAW_FRAME_SIZE + RAW_FRAME_SIZE / 254 + 1;

        HOTASMessage();

        void setFiveWay(int fiveWayIndex, FiveWayDirection dir, bool value);
        void setButton(int buttonIndex, bool value);
        void setJoystick(int joystickIndex, int16_t x, int16_t y);

        bool getFiveWay(int fiveWayIndex, FiveWayDirection dir);
        bool getButton(int buttonIndex);
        int16_t getJoystickX(int joystickIndex);
        int16_t getJoystickY(int joystickIndex);

        size_t encode(uint8_t* buffer);
        bool decode(uint8_t *buffer, size_t len);
};

#endif