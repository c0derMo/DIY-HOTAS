#ifndef SERIALLOGGER_H
#define SERIALLOGGER_H

#include "shared/config.h"
#include <Arduino.h>

#ifdef ENABLE_SERIAL
#include "arduino/Adafruit_USBD_Device.h"
#include "arduino/Adafruit_USBD_CDC.h"
#define CFG_TUD_CDC 1
#else
#define CFG_TUD_CDC 0

#define LOGF(pref, mesg, ...) ((void)0)
#define LOG(pref, mesg) ((void)0)
#define PRINT(mesg) ((void)0)

#endif

class SerialLogger {
    public:
        static void log(String message);
        static void log(String prefix, String message);
        static void logf(String prefix, String message, ...);
        static void begin();
        static void wait();
};

#ifdef ENABLE_SERIAL
#define LOGF(pref, mesg, ...) SerialLogger::logf(pref, mesg, ##__VA_ARGS__)
#define LOG(pref, mesg) SerialLogger::log(pref, mesg)
#define PRINT(mesg) SerialLogger::log(mesg)
#endif

#endif