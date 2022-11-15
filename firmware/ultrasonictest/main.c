#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

#include "Board.h"

// based on: hello_adc.c 
// from Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
// SPDX-License-Identifier: BSD-3-Clause

int main() {
    stdio_init_all();
    
    adc_init();

    adc_gpio_init(ULTRASONIC_PIN); // make sure GPIO is high-impedance, no pullups etc
    adc_select_input(ULTRASONIC_ADC); // select ADC input

    printf("Ultrasonic sensor test\r\n");

    while(true) {
        // 12-bit conversion, assume max value == ADC_VREF == 3.3 V
        const float conversion_factor = 3.3f / (1 << 12);
        uint16_t result = adc_read();
        float adc_volts = result*conversion_factor;
        float adc_mv = adc_volts*1000; // in mV
        float scale_factor = 6.4; // mV per in
        float distance = adc_mv * scale_factor / 100.0f;
        float cm = distance*2.54f;
        printf("Distance: %.2f in, %.2f cm \tRaw value: 0x%03x, voltage: %f V\n", distance, cm, result, result * conversion_factor);
        sleep_ms(100);
    }

    return 0;
}

