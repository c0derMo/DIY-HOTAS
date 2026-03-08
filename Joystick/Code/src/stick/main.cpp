#include <Arduino.h>
#include "shared/config.h"

int i = 0;

void setup() {
    Serial.begin(9600);
}

void loop() {
    if (Serial.available()) {
        String in = Serial.readStringUntil('\n');
        String out = String(i) + " Arduino | " + in.substring(0, 6);
        i += 1;
        Serial.println(out);
    }
    delay(DELAY_TIME);
}