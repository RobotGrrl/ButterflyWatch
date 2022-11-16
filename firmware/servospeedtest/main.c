#include <stdio.h>
#include "pico/stdlib.h"

#include "Board.h"
#include "Servo.h"

// servo related
struct Servo wing_l;
struct Servo wing_r;
struct repeating_timer servo_timer;
bool servo_timer_callback(struct repeating_timer *t);
void stopServoTimer();
void startServoTimer();

// led related
absolute_time_t last_blink = 0;
bool blink = false;
void blinkLed();

// test related
uint8_t speed_num = 0;
absolute_time_t last_speed = 0;
void servoPosTest();


int main() {

    stdio_init_all();

    // led related
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    printf("Servo speed test\r\n");

    // init the servos
    initServo(&wing_l, SERVO_L_PIN, SERVO_MAX, 5);
    initServo(&wing_r, SERVO_R_PIN, SERVO_MIN, 5);
    wing_l.direction = false;
    wing_r.direction = true;
    
    servoPosTest();

    startServoTimer();
    printf("Servos started\r\n");

    while(true) {

        // update the servos
        updateServo(&wing_l);
        updateServo(&wing_r);

        // blinky slice
        blinkLed();

        // change the speed
        if( absolute_time_diff_us(last_speed, get_absolute_time()) >= 2000*1000 ) {
            speed_num++;
            uint8_t speed_val = 0;
            switch(speed_num) {
                case 0:
                    speed_val = 1;
                break;
                case 1:
                    speed_val = 3;
                break;
                case 2:
                    speed_val = 5;
                break;
                case 3:
                    speed_val = 7;
                break;
                case 4:
                    speed_val = 10;
                    speed_num = 0;
                break;
            }
            wing_l.speed = speed_val;
            wing_r.speed = speed_val;
            last_speed = get_absolute_time();
        }

    }

    return 0;
}


// --- servo timer related ---
bool servo_timer_callback(struct repeating_timer *t) {
    wing_l.update = true;
    wing_r.update = true;
    return true;
}

void stopServoTimer() {
    bool cancelled = cancel_repeating_timer(&servo_timer);
}

void startServoTimer() {
    add_repeating_timer_ms(10, servo_timer_callback, NULL, &servo_timer); // 10 ms
}
// --------------------------


// --- led related ---
void blinkLed() {
    if( absolute_time_diff_us(last_blink, get_absolute_time()) >= 1000*1000 ) {
        gpio_put(LED_PIN, blink);
        blink = !blink;
        last_blink = get_absolute_time();
    }
}
// --------------------------


// --- test related ---
void servoPosTest() {
    setServoPosition(&wing_l, SERVO_MAX);
    setServoPosition(&wing_r, SERVO_MIN);
    sleep_ms(5000);

    setServoPosition(&wing_l, SERVO_MIN);
    setServoPosition(&wing_r, SERVO_MAX);
    sleep_ms(5000);
}
// --------------------------

