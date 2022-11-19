#include <stdio.h>
#include "pico/stdlib.h"
#include "pico_sleep/sleep.h"

#define LED_PIN 25
#define WAKE_PIN 3

unsigned long sleep_start = 0;

// based on these examples:
// https://www.youtube.com/watch?app=desktop&v=05o5QOZkpwg
// https://github.com/HappyDonuts/Pico_Examples/tree/b5bb28dbb6a20442e655a6603d9fd9db6b333728/Pico_Low_Power

// useful: https://raspberrypi.github.io/pico-sdk-doxygen/group__pico__stdlib.html#gab3a273e837ba1947bb5fd8fc97cf47e5

int main() {

    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(WAKE_PIN, GPIO_IN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    
    // uncomment this block for additional power testing
    /*
    // startup
    gpio_put(LED_PIN,1);
    sleep_ms(3000);
    gpio_put(LED_PIN,0);

    // default
    sleep_ms(3000);

    // 48 MHz
    set_sys_clock_48mhz(); // system clock to 48 MHz
    sleep_ms(3000);

    // 10 MHz
    set_sys_clock_khz(10000, false); // system clock to 10 MHz
    sleep_ms(3000);
    */
    
    // dormant
    gpio_put(LED_PIN,0);
    sleep_start = get_absolute_time();
    sleep_run_from_xosc();
    sleep_goto_dormant_until_edge_high(WAKE_PIN);

    // continue from here
    while(true) {
        //tight_loop_contents();
        gpio_put(LED_PIN,1);
        sleep_ms(100);
        gpio_put(LED_PIN,0);
        sleep_ms(100);

        // go to dormant again after 3 s
        if(absolute_time_diff_us(sleep_start, get_absolute_time()) >= 3000*1000) {
            gpio_put(LED_PIN,0);
            sleep_start = get_absolute_time();
            sleep_run_from_xosc();
            sleep_goto_dormant_until_edge_high(WAKE_PIN);
        }

    }

    return 0;
}

