#include "HOTASMessage.h"
#include "config.h"

void writeBit(uint8_t* buffer, uint16_t bitIndex, bool value) {
    uint16_t byteIndex = bitIndex >> 3;
    uint8_t bit = bitIndex & 7;

    if (value) {
        buffer[byteIndex] |= (1 << bit);
    }
    // We never really set a true bit to false, so we can ignore this for now... might need in the future tho
    // } else {
    //     buffer[byteIndex] &= ~(1 << bit);
    // }
}

bool readBit(uint8_t* buffer, uint16_t bitIndex) {
    uint16_t byteIndex = bitIndex >> 3;
    uint8_t bit = bitIndex & 7;

    return (buffer[byteIndex] >> bit) & 1;
}

HOTASMessage::HOTASMessage() {
    this->payload = HOTASPayload();
}

void HOTASMessage::setFiveWay(int fiveWayIndex, FiveWayDirection dir, bool value) {
    this->payload.input_five_way[fiveWayIndex][dir] = value;
}

void HOTASMessage::setButton(int buttonIndex, bool value) {
    this->payload.input_button[buttonIndex] = value;
}

void HOTASMessage::setJoystick(int joystickIndex, int16_t x, int16_t y) {
    this->payload.input_joystick[joystickIndex][0] = x;
    this->payload.input_joystick[joystickIndex][1] = y;
}

bool HOTASMessage::getFiveWay(int fiveWayIndex, FiveWayDirection dir) {
    return this->payload.input_five_way[fiveWayIndex][dir];
}

bool HOTASMessage::getButton(int buttonIndex) {
    return this->payload.input_button[buttonIndex];
}

int16_t HOTASMessage::getJoystickX(int joystickIndex) {
    return this->payload.input_joystick[joystickIndex][0];
}

int16_t HOTASMessage::getJoystickY(int joystickIndex) {
    return this->payload.input_joystick[joystickIndex][1];
}

void HOTASMessage::encodePayload(uint8_t* buffer) {
    memset(buffer, 0, PAYLOAD_SIZE);

    uint16_t bitIndex = 0;

    // First: Encode Buttons
    for (int i = 0; i < INPUT_BUTTON_COUNT; i++) {
        writeBit(buffer, bitIndex++, this->payload.input_button[i]);
    }

    // Second: Encode 5way
    for (int i = 0; i < INPUT_5WAY_COUNT; i++) {
        for (int j = 0; j < 5; j++) {
            writeBit(buffer, bitIndex++, this->payload.input_five_way[i][j]);
        }
    }

    uint16_t byteIndex = BOOL_BYTES;

    // Third: Encode joystick
    for (int i = 0; i < INPUT_JOYSTICK_COUNT; i++) {
        int16_t x = this->payload.input_joystick[i][0];
        int16_t y = this->payload.input_joystick[i][1];

        buffer[byteIndex++] = (uint8_t)(x & 0xFF);
        buffer[byteIndex++] = (uint8_t)((x >> 8) & 0xFF);
        buffer[byteIndex++] = (uint8_t)(y & 0xFF);
        buffer[byteIndex++] = (uint8_t)((y >> 8) & 0xFF);
    }
}

void HOTASMessage::decodePayload(uint8_t* buffer) {
    uint16_t bitIndex = 0;

    // First: Decode Buttons
    for (int i = 0; i < INPUT_BUTTON_COUNT; i++) {
        this->payload.input_button[i] = readBit(buffer, bitIndex++);
    }

    // Second: Decode 5way
    for (int i = 0; i < INPUT_5WAY_COUNT; i++) {
        for (int j = 0; j < 5; j++) {
            this->payload.input_five_way[i][j] = readBit(buffer, bitIndex++);
        }
    }

    uint16_t byteIndex = BOOL_BYTES;

    // Third: Decode joystick
    for (int i = 0; i < INPUT_JOYSTICK_COUNT; i++) {

        int16_t x = (int16_t) buffer[byteIndex] | ((int16_t) buffer[byteIndex+1] << 8);
        int16_t y = (int16_t) buffer[byteIndex+2] | ((int16_t) buffer[byteIndex+3] << 8);
        byteIndex += 4;

        this->payload.input_joystick[i][0] = x;
        this->payload.input_joystick[i][1] = y;
    }
}

size_t HOTASMessage::encode(uint8_t* buffer) {
    uint8_t raw[RAW_FRAME_SIZE];

    raw[0] = PAYLOAD_SIZE;
    this->encodePayload(&raw[1]);

    // Negating every byte, since a lot of them would be 0x00 which need to be encoded during COBS
    for (int i = 1; i < PAYLOAD_SIZE + 1; i++) {
        raw[i] = ~raw[i];
    }

    raw[1 + PAYLOAD_SIZE] = crc8(raw, PAYLOAD_SIZE + 1);

    return encodeCobs(raw, RAW_FRAME_SIZE, buffer);
}

bool HOTASMessage::decode(uint8_t* buffer, size_t len) {
    uint8_t decoded[RAW_FRAME_SIZE];

    size_t length = decodeCobs(buffer, len, decoded);
    if (length < 2) {
        return false;
    }

    uint8_t payloadLen = decoded[0];
    if (payloadLen != PAYLOAD_SIZE) {
        return false;
    }

    uint8_t crc = decoded[1 + payloadLen];

    if (crc8(decoded, payloadLen + 1) != crc) {
        return false;
    }

    // Negating every byte, since a lot of them would be 0x00 which need to be encoded during COBS
    for (int i = 1; i < payloadLen + 1; i++) {
        decoded[i] = ~decoded[i];
    }

    this->decodePayload(&decoded[1]);

    return true;
}

uint8_t HOTASMessage::crc8(const uint8_t* data, size_t len) {
    uint8_t crc = 0;
    while (len--) {
        crc ^= *data++;

        for (uint8_t i = 0; i < 8; i++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x07;
            } else {
                crc <<= 1;
            }
        }
    }

    return crc;
}

size_t HOTASMessage::encodeCobs(const uint8_t* data, size_t length, uint8_t* output) {
    const uint8_t* end = data + length;

    uint8_t* start = output;
    uint8_t* code_ptr = output++;
    uint8_t code = 1;

    while (data < end) {
        if (*data == 0) {
            *code_ptr = code;
            code_ptr = output++;
            code = 1;
        } else {
            *output++ = *data;
            code++;
            if (code == 0xFF) {
                *code_ptr = code;
                code_ptr = output++;
                code = 1;
            }
        }

        data++;
    }

    *code_ptr = code;

    return output - start;
}

size_t HOTASMessage::decodeCobs(const uint8_t* data, size_t length, uint8_t* output) {
    const uint8_t* end = data + length;

    uint8_t* start = output;

    while (data < end) {
        uint8_t code = *data++;

        if (code == 0 || data + code - 1 > end) {
            return 0;
        }

        for (uint8_t i = 1; i < code; i++) {
            *output++ = *data++;
        }
        if (code != 0xFF && data < end) {
            *output++ = 0;
        }
    }
    
    return output - start;
}