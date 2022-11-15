#ifndef ULTRASONICSENSOR_H
#define ULTRASONICSENSOR_H

#include <stdio.h>
#include <stdbool.h>

struct UltrasonicSensor {
    volatile bool warmedup;
    volatile bool ready;
    bool active;
    volatile uint16_t raw;
    volatile uint8_t sample_count;
};

s->warmedup = false;
s->ready = false;
s->active = false;
s->raw = 0;
s->sample_count = 0;

#endif
