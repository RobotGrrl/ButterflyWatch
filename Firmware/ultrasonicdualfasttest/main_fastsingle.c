#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "pico/multicore.h"

#include "Board.h"
#include "UltrasonicSensor.h"

#define DEBUG false

// -- ultrasonic sensor related --
struct UltrasonicSensor sen;
int64_t ultrasonic_warmup_callback(alarm_id_t id, void *user_data);
struct repeating_timer ultrasonic_timer;
bool ultrasonic_daq_callback(struct repeating_timer *t);
// -------------

// -- dual core related --
volatile absolute_time_t last_print = 0;
void second_core_code();
long map(long x, long in_min, long in_max, long out_min, long out_max);
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

        if(sen.ready) {
            
            // acquire the sensor reading
            uint16_t result = adc_read();
            const float conversion_factor = 3.3f / (1 << 12);
            float adc_volts = result*conversion_factor;
            float adc_mv = adc_volts*1000; // in mV
            float scale_factor = 6.4; // mV per in
            float distance = adc_mv * scale_factor / 100.0f;
            sen.raw_cm = distance*2.54f;

            // map the distance to 8
            // use the raw value for better responsiveness
            uint8_t cm_val = (uint8_t)sen.raw_cm;
            if(cm_val < 7) cm_val = 7;
            if(cm_val > 40) cm_val = 40;
            uint32_t dist_val = map(sen.raw_cm, 7, 40, 0, 8);
            

            if( absolute_time_diff_us(last_print, get_absolute_time()) >= 100*1000 ) {
                for(uint8_t i=0; i<8; i++) {
                    if(i >= dist_val) {
                        printf(". ");
                    } else {
                        printf("* ");
                    }
                }
                printf(" %d (%d, %.2f)", dist_val, cm_val, sen.raw_cm);
                printf("\r\n");
                last_print = get_absolute_time();
            }

            sen.ready = false;
        }

        updateUltrasonicSensor(&sen);

        if(DEBUG) {
            if(sen.ready) {
                printf("%d raw val: %d\r\n", sen.sample_count, sen.raw);
            }
        }
        
    }

    return 0;
}


// --- dual core related ---
void second_core_code() {

}
// -------------


// --- helper related ---
// Source of map from: Arduino (https://www.arduino.cc/reference/en/language/functions/math/map/)
long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
// -------------


// --- ultrasonic sensor related ---
int64_t ultrasonic_warmup_callback(alarm_id_t id, void *user_data) {
    sen.warmedup = true;
    sen.active = true;
    // once the sensor is warmed up, start the timer interrupt 20 Hz
    add_repeating_timer_ms(ULTRASONIC_RATE, ultrasonic_daq_callback, NULL, &ultrasonic_timer);
    return 0;
}

bool ultrasonic_daq_callback(struct repeating_timer *t) {
    // flag it's ready for reading
    sen.ready = true;
    return true;
}
// -------------

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

