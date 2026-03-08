#include <Arduino.h>
#include "shared/config.h"

int i = 0;
HardwareSerial StickSerial(PA3, PA2);

void setup() {
    Serial.begin(9600);
    StickSerial.begin(9600);

    while (!Serial);

    Serial.println("Ready to relay :D");
    StickSerial.println("Pog serial 2");
}

void loop() {
    if (StickSerial.available()) {
        String in = StickSerial.readStringUntil('\n');
        String out = String(i) + " STM | " + in.substring(0, 6);
        i += 1;
        StickSerial.println(out);
        Serial.println("< " + in);
        Serial.println("> " + out);
    }
    delay(DELAY_TIME);
}