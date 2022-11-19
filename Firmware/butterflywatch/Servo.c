#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "Servo.h"


// ---------------------------------------------------------------------------------
// Servo Movements Test Program
// ---------------------------------------------------------------------------------
// Source: Copyright 2022 Erin Kennedy. Own work.
// ---------------------------------------------------------------------------------

// servo, pin, home position, speed
void initServo(struct Servo *s, uint8_t p, uint16_t h, uint8_t v) {
    s->pin = p;
    s->update = false;
    s->max = SERVO_MAX;
    s->min = SERVO_MIN;
    s->home = h;
    s->speed = v;
    s->pulse = h; // init to home pos

    // set up the pwm
    setServo(s->pin, s->home);
}

void updateServo(struct Servo *s) {
    if(s->update) { // set from repeating timer interrupt

        // increment pulse by step amount (speed)
        if(s->direction) {
            s->pulse += s->speed;
        } else {
            s->pulse -= s->speed;
        }

        // check it's within bounds and change direction
        if(s->pulse >= s->max) {
            s->pulse = s->max;
            s->direction = false;
        }
        if(s->pulse <= s->min) {
            s->pulse = s->min;
            s->direction = true;
        }

        // update the position
        setMillis(s->pin, s->pulse);

        // reset the flag
        s->update = false;
    }
}

// speed max = 10, min = 1
void setServoSpeed(struct Servo *s, uint8_t v) {
    if(v > 10 || v == 0) return;
    s->speed = v;
}

// position must be within min and max
void setServoPosition(struct Servo *s, uint16_t pos) {
    if(pos > s->max || pos < s->min) return;
    s->pulse = pos;
    setMillis(s->pin, s->pulse);
}



// ---------------------------------------------------------------------------------
// Pico Servo Library
// ---------------------------------------------------------------------------------
// Source:  Pico Servo Library
//          https://github.com/KleistRobotics/Pico-Servo/blob/main/servo/servo.c
// ---------------------------------------------------------------------------------
// License: (None given)
// ---------------------------------------------------------------------------------

float clockDiv = 64;
float wrap = 39062;

void setMillis(int servoPin, float millis)
{
    pwm_set_gpio_level(servoPin, (millis/20000.f)*wrap);
}

void setServo(int servoPin, float startMillis)
{
    gpio_set_function(servoPin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(servoPin);

    pwm_config config = pwm_get_default_config();
    
    uint64_t clockspeed = clock_get_hz(5);
    clockDiv = 64;
    wrap = 39062;

    while (clockspeed/clockDiv/50 > 65535 && clockDiv < 256) clockDiv += 64; 
    wrap = clockspeed/clockDiv/50;

    pwm_config_set_clkdiv(&config, clockDiv);
    pwm_config_set_wrap(&config, wrap);

    pwm_init(slice_num, &config, true);

    setMillis(servoPin, startMillis);
}

