#ifndef ULTRASONICSENSOR_H
#define ULTRASONICSENSOR_H

#include <stdio.h>
#include <stdbool.h>

struct UltrasonicSensor {
    volatile bool warmedup;
    volatile bool ready;
    volatile bool active;
    volatile uint8_t sample_count;
    volatile uint32_t sum; // all raw added
    uint16_t val; // after avg
    float cm; // after avg
    volatile float raw_cm; // raw value converted
};

void initUltrasonicSensor(struct UltrasonicSensor *s);
void updateUltrasonicSensor(struct UltrasonicSensor *s);
float convertValue(uint16_t result);

#endif
