#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

#include "Board.h"
#include "UltrasonicSensor.h"

#define DEBUG false

// -- ultrasonic sensor related --
struct UltrasonicSensor sen;
int64_t ultrasonic_warmup_callback(alarm_id_t id, void *user_data);
struct repeating_timer ultrasonic_timer;
bool ultrasonic_daq_callback(struct repeating_timer *t);
// -------------

int main() {
    stdio_init_all();
    
    adc_init();

    adc_gpio_init(ULTRASONIC_PIN); // make sure GPIO is high-impedance, no pullups etc
    adc_select_input(ULTRASONIC_ADC); // select ADC input

    printf("Ultrasonic sensor test\r\n");

    initUltrasonicSensor(&sen);

    // ultrasonic sensor warm up time
    add_alarm_in_ms(ULTRASONIC_WARMUP, ultrasonic_warmup_callback, NULL, false);

    printf("Sensor initialised\r\n");

    while(true) {
        
        if(DEBUG) {
            if(sen.ready) {
                printf("%d raw val: %d\r\n", sen.sample_count, sen.raw);
            }
        }
        
        updateUltrasonicSensor(&sen);
    }

    return 0;
}


int64_t ultrasonic_warmup_callback(alarm_id_t id, void *user_data) {
    sen.warmedup = true;
    sen.active = true;
    // once the sensor is warmed up, start the timer interrupt 20 Hz
    // TODO: does calling this from a callback work?
    add_repeating_timer_ms(ULTRASONIC_RATE, ultrasonic_daq_callback, NULL, &ultrasonic_timer);
    return 0;
}

bool ultrasonic_daq_callback(struct repeating_timer *t) {
    // acquire the raw value
    uint16_t result = adc_read();
    sen.raw = result;
    // sen.raw = 1.0;
    sen.sum += sen.raw;
    sen.sample_count++;

    // flag it's ready for processing
    sen.ready = true;
    return true;
}

// Q: ^ is this an unwise design?
//  it gets the value in the interrupt, and will process it in the main loop.
//  what if the timing of the main loop is delayed more than the sampling rate?
//  though, the value average is based on the count not the time...
//  but if the sensor was moving, wouldn't a missing value throw off
//    the measurements, if it was velocity?
//  does this mean all the sensor values should have a timestamp to ensure that
//    none have been skipped?
//  if velocity is being calculated it needs time... so yes... if going the main
//    loop route. otherwise, can depend on the interrupt firing at the exact
//    intervals.
//  conclusion: getting the raw sensor reading in the interrupt so that these are
//    acquired at the same time interval, which is needed if calculating velocity.

