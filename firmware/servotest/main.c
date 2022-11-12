#include <stdio.h>
#include "pico/stdlib.h"
#include "servo.h"

bool direction = true;
int currentMillis = 400;
int servoPin = 7;

int main()
{

    const uint led_pin = 25;

    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);

    stdio_init_all();


    setServo(servoPin, currentMillis);
    while (true)
    {
        currentMillis += (direction)?5:-5;
        if (currentMillis >= 2400) {
            direction = false;
            gpio_put(led_pin, false);
        }
        if (currentMillis <= 400) {
            direction = true;
            gpio_put(led_pin, true);
        }
        setMillis(servoPin, currentMillis);
        sleep_ms(10);
    }
}

