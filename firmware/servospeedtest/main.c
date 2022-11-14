#include <stdio.h>
#include "pico/stdlib.h"

#include "Board.h"
#include "Servo.h"

struct Servo wing_l;
struct Servo wing_r;

volatile uint32_t count = 0;

struct repeating_timer servo_timer;
bool servo_timer_callback(struct repeating_timer *t);
void stopServoTimer();
void startServoTimer();


#define SERVO_MAX 2400
#define SERVO_MIN 600

uint16_t SERVO_HOME = 1600; // variable since we might want to change it through cli in the future

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

// servo, pin, home position, speed
void initServo(struct Servo *s, uint8_t p, uint8_t h, uint8_t v) {
    s->pin = p;
    s->update = false;
    s->max = SERVO_MAX;
    s->min = SERVO_MIN;
    s->home = h;
    s->speed = v;
    s->pulse = h; // init to home pos

    // TODO: set the pin, start the pwm
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
        if(s->pulse > s->max) {
            s->pulse = s->max;
            s->direction = false;
        }
        if(s->pulse < s->min) {
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
}



int main() {

    stdio_init_all();

    // led related
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    printf("Servo speed test\r\n");

    startServoTimer();

    while(true) {

        updateServo(&wing_l);
        updateServo(&wing_r);

    }

    return 0;
}



bool servo_timer_callback(struct repeating_timer *t) {
    
    wing_l.update = true;
    wing_r.update = true;
    
    printf("%d Repeat at %lld\n", count, time_us_64());
    count++;
    return true;
}

void stopServoTimer() {
    bool cancelled = cancel_repeating_timer(&servo_timer);
}

void startServoTimer() {
    add_repeating_timer_ms(10, servo_timer_callback, NULL, &servo_timer); // 10 ms
}

