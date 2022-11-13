// based on: https://github.com/KleistRobotics/Pico-Servo

#include <stdio.h>
#include "pico/stdlib.h"
#include "servo.h"

const int min = 600;
const int max = 2400;

bool direction1 = true;
bool direction2 = false;
int currentMillis1 = min;
int currentMillis2 = max;
int servoPin1 = 7;
int servoPin2 = 6;

int main()
{

    stdio_init_all();

    const uint led_pin = 25;

    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);

    
    setServo(servoPin1, currentMillis1);
    setServo(servoPin2, currentMillis2);

    while (true)
    {
        currentMillis1 += (direction1)?5:-5;
        currentMillis2 += (direction2)?5:-5;

        setMillis(servoPin1, currentMillis1);
        setMillis(servoPin2, currentMillis2);

        printf("1: %d 2: %d\r\n", currentMillis1, currentMillis2);

        sleep_ms(10);

        if (currentMillis1 >= max) {
            direction1 = false;
            gpio_put(led_pin, false);
        }
        if (currentMillis1 <= min) {
            direction1 = true;
            gpio_put(led_pin, true);
        }

        if (currentMillis2 >= max) {
            direction2 = false;
        }
        if (currentMillis2 <= min) {
            direction2 = true;
        }

    }
}

