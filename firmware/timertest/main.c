#include <stdio.h>
#include "pico/stdlib.h"

// some modifications
// based on: hello_timer.c 
// from Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
// SPDX-License-Identifier: BSD-3-Clause

volatile bool timer_fired = false;
volatile uint32_t count = 0;

int64_t alarm_callback(alarm_id_t id, void *user_data) {
    printf("Timer %d fired!\n", (int) id);
    timer_fired = true;
    // Can return a value here in us to fire in the future
    return 0;
}

bool repeating_timer_callback(struct repeating_timer *t) {
    printf("%d Repeat at %lld\n", count, time_us_64());
    count++;
    return true;
}

int main() {
    stdio_init_all();


    printf("Hello Timer!\n");

    // Call alarm_callback in 200 ms
    add_alarm_in_ms(200, alarm_callback, NULL, false);


    while(true) {

        // Wait for alarm callback to set timer_fired
        if(timer_fired) {

            timer_fired = false; // reset the alarm

            // Create a repeating timer that calls repeating_timer_callback.
            // • timer (50 ms) is the time between the 
            //   previous callback ending and the next starting.
            struct repeating_timer timer;
            add_repeating_timer_ms(50, repeating_timer_callback, NULL, &timer);
            sleep_ms(3000); // 20 Hz * 3 s, count should be = 60-1
            bool cancelled = cancel_repeating_timer(&timer);
            printf("cancelled... %d\n", cancelled);
            count = 0;
            sleep_ms(2000);

            printf("Done\n");

            // Call alarm_callback in 200 ms
            add_alarm_in_ms(200, alarm_callback, NULL, false);

        }

    }

    return 0;
}


