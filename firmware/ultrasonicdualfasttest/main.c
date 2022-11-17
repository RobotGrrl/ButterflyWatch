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
    
    multicore_launch_core1(second_core_code);

    while(true) {

        if( absolute_time_diff_us(last_print, get_absolute_time()) >= 100*1000 ) {
            
            uint32_t dist_val = 0;

            // drain the fifo first
            multicore_fifo_drain();

            // pop it, we know that there will be new data from the 2nd core every 50 ms
            multicore_fifo_pop_timeout_us(100*1000, &dist_val);

            for(uint8_t i=0; i<8; i++) {
                if(i >= dist_val) {
                    printf(". ");
                } else {
                    printf("* ");
                }
            }
            printf(" %d", dist_val);
            printf("\r\n");
            last_print = get_absolute_time();
        }

    }

    return 0;
}


// --- dual core related ---
void second_core_code() {
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
            
            // update the sensor
            updateUltrasonicSensor(&sen);
            
            // map the distance to 8
            // use the raw value for better responsiveness
            uint8_t cm_val = (uint8_t)sen.raw_cm;
            if(cm_val < 7) cm_val = 7;
            if(cm_val > 40) cm_val = 40;
            uint32_t dist_val = map(sen.raw_cm, 7, 40, 0, 8);
            
            // small timeout because the "fifo" is going to be emptied anyway
            multicore_fifo_push_timeout_us(dist_val, 10*1000);

            sen.ready = false;
        }

        if(DEBUG) {
            if(sen.ready) {
                printf("%d raw val: %d\r\n", sen.sample_count, sen.val);
            }
        }
        
    }
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
    
    // acquire the sensor reading
    adc_select_input(ULTRASONIC_ADC); // select ADC input
    sen.val = adc_read();
    
    // flag it's ready for processing
    sen.ready = true;
    return true;
}
// -------------
