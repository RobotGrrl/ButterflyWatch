#ifndef SERVO_H
#define SERVO_H

#include <stdio.h>
#include <stdbool.h>
#include "Board.h"

struct Servo {
    uint8_t pin;
    volatile bool update;
    volatile uint16_t pulse;
    volatile bool direction;
    uint16_t max;
    uint16_t min;
    uint16_t home;
    uint8_t speed; // 1 = slow, 5 = regular, 10 = fast
};

// parameters: servo, pin, home position, speed
void initServo(struct Servo *s, uint8_t p, uint16_t h, uint8_t v);

// parameters: servo
void updateServo(struct Servo *s);

// parameters: servo, speed (max = 10, min = 1)
void setServoSpeed(struct Servo *s, uint8_t v);

// parameters: servo, position (must be within min and max)
void setServoPosition(struct Servo *s, uint16_t pos);


void setMillis(int servoPin, float millis);
void setServo(int servoPin, float startMillis);


#endif