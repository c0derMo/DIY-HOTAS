#include "SerialLogger.h"

void SerialLogger::begin() {
    #ifdef ENABLE_SERIAL
        SerialTinyUSB.begin(9600);
    #endif
}

void SerialLogger::wait() {
    #ifdef ENABLE_SERIAL
        while (!SerialTinyUSB);
        delay(100);
    #endif
}

void SerialLogger::log(String message) {
    #ifdef ENABLE_SERIAL
        SerialTinyUSB.println(message);
    #endif
}

void SerialLogger::log(String prefix, String message) {
    #ifdef ENABLE_SERIAL
        SerialTinyUSB.println("[" + prefix + "] " + message);
    #endif
}

void SerialLogger::logf(String prefix, String message, ...) {
    #ifdef ENABLE_SERIAL
        char buf[128];
        va_list args;
        va_start(args, message);
        vsnprintf(buf, sizeof(buf), message.c_str(), args);
        va_end(args);
        SerialLogger::log(prefix, buf);
    #endif
}