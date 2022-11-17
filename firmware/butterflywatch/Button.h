#ifndef BUTTON_H
#define BUTTON_H

#include <stdio.h>
#include <stdbool.h>

struct Button {
    uint8_t pin;
    volatile bool pressed;
    volatile unsigned long debounce_start;
    volatile bool status;
    volatile bool prev_status;
    volatile uint32_t clicks;
    volatile bool trig;
};

void initButton(struct Button *b, uint8_t pin);
void updateButton(struct Button *b);

#endif