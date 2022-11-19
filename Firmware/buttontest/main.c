#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

// based on: hello_gpio_irq.c 
// from Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
// SPDX-License-Identifier: BSD-3-Clause

#define DEBUG false

volatile uint8_t pressed = 0;
volatile absolute_time_t debounce_start = 0;
bool status = false;


static char event_str[128];

void gpio_event_string(char *buf, uint32_t events);

void gpio_callback(uint gpio, uint32_t events) {
    // Put the GPIO event(s) that just happened into event_str
    // so we can print it
    gpio_event_string(event_str, events);
    if(DEBUG) printf("GPIO %d %s %d\r\n", gpio, event_str, events);

    if(events == 8) { // EDGE_RISE
        pressed = 1;
        debounce_start = get_absolute_time();
    } else {
        pressed = 0;
    }

}



int main() {

    const uint led_pin = 25;

    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);

    stdio_init_all();

    printf("Hello GPIO IRQ\n");
    gpio_set_irq_enabled_with_callback(3, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    // Wait forever
    while(true) {

        if(pressed == 1) {

            if( absolute_time_diff_us(debounce_start, get_absolute_time()) >= 100*1000 ) {
                
                status = !status;
                if(status) {
                    printf("[+] activated\r\n");
                } else {
                    printf("[-] deactivated\r\n");
                }

                pressed = 0; // reset button val
                
            }

        }

    }

    return 0;


    // while(true) {
    //     printf("blink\r\n");
    //     gpio_put(led_pin, true);
    //     sleep_ms(100);
    //     gpio_put(led_pin, false);
    //     sleep_ms(100);
    // }

}



static const char *gpio_irq_str[] = {
        "LEVEL_LOW",  // 0x1
        "LEVEL_HIGH", // 0x2
        "EDGE_FALL",  // 0x4
        "EDGE_RISE"   // 0x8
};

void gpio_event_string(char *buf, uint32_t events) {
    for (uint i = 0; i < 4; i++) {
        uint mask = (1 << i);
        if (events & mask) {
            // Copy this event string into the user string
            const char *event_str = gpio_irq_str[i];
            while (*event_str != '\0') {
                *buf++ = *event_str++;
            }
            events &= ~mask;

            // If more events add ", "
            if (events) {
                *buf++ = ',';
                *buf++ = ' ';
            }
        }
    }
    *buf++ = '\0';
}


