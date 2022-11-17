#include <stdio.h>
#include "pico/stdlib.h"        // ultrasonicsensorduafastltest
#include "hardware/gpio.h"      // ultrasonicsensorduafastltest, debouncetest, neocycletest
#include "hardware/adc.h"       // ultrasonicsensorduafastltest
#include "pico/multicore.h"     // ultrasonicsensorduafastltest
#include "pico_sleep/sleep.h"   // sleeptest
#include "hardware/pio.h"       // neocycletest
#include "hardware/clocks.h"    // neocycletest
#include "ws2812.pio.h"         // neocycletest

#include "Board.h"
#include "UltrasonicSensor.h"
#include "Button.h"
#include "Servo.h"

#define DEBUG false

/*
TODO:
- gotta test the ultrasonic and neopixels
- then add in servo code
- integrate sleeptest code later
*/


// -- servo related --
struct Servo wing_l;
struct Servo wing_r;
struct repeating_timer servo_timer;
bool servo_timer_callback(struct repeating_timer *t);
void stopServoTimer();
void startServoTimer();
uint8_t speed_num = 0;
absolute_time_t last_speed = 0;
// -------------

// -- button related --
struct Button btn;
int64_t debounce_callback(alarm_id_t id, void *user_data);
void button_callback(uint gpio, uint32_t events);
// -------------

// -- neopixel related --
static inline void put_pixel(uint32_t pixel_grb);
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b);
uint8_t colour_sel = 0;
absolute_time_t neo_update = 0;
// -------------

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

// -- app related --
uint8_t PIXELS_DISPLAYED = 0; // determined by ultrasonic sensor, sent to neopixels
// -------------


int main() {
    stdio_init_all();
    
    // sensor core
    multicore_launch_core1(second_core_code);

    // led related
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    printf("LED ready\r\n");

    // neopixel related
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, NEO_PIN, 800000, false);
    printf("Neopixel ready\r\n");

    // button related
    initButton(&btn, BTN_PIN);
    gpio_set_irq_enabled_with_callback(BTN_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &button_callback);
    printf("Button ready\r\n");
    
    // servo related
    initServo(&wing_l, SERVO_L_PIN, SERVO_MAX, 5);
    initServo(&wing_r, SERVO_R_PIN, SERVO_MIN, 5);
    wing_l.direction = false;
    wing_r.direction = true;

    startServoTimer();
    printf("Servos ready\r\n");



    while(true) {


        // update the servos
        updateServo(&wing_l);
        updateServo(&wing_r);

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




        // read from sensor core fifo
        if( absolute_time_diff_us(last_print, get_absolute_time()) >= 100*1000 ) {
            
            uint32_t dist_val = 0;

            // drain the fifo first
            multicore_fifo_drain();

            // pop it, we know that there will be new data from the 2nd core every 50 ms
            multicore_fifo_pop_timeout_us(100*1000, &dist_val);

            PIXELS_DISPLAYED = dist_val;

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
        // --




        // -- neopixel button related --
        updateButton(&btn);

        if(btn.status == true) {

            gpio_put(LED_PIN, true);

            switch(colour_sel) {
                case 0: {
                    if(absolute_time_diff_us(neo_update, get_absolute_time()) >= 10*1000) {
                        for(uint i=0; i<NUM_PIXELS; ++i) {
                            if(i<PIXELS_DISPLAYED) {
                                put_pixel(urgb_u32(0x11, 0, 0));
                            } else {
                                put_pixel(urgb_u32(0, 0, 0));
                            }
                        }
                        neo_update = get_absolute_time();
                    }
                }
                break;
                case 1: {
                    if(absolute_time_diff_us(neo_update, get_absolute_time()) >= 10*1000) {
                        for(uint i=0; i<NUM_PIXELS; ++i) {
                            if(i<PIXELS_DISPLAYED) {
                                put_pixel(urgb_u32(0, 0x11, 0));
                            } else {
                                put_pixel(urgb_u32(0, 0, 0));
                            }
                        }
                        neo_update = get_absolute_time();
                    }
                }
                break;
                case 2: {
                    if(absolute_time_diff_us(neo_update, get_absolute_time()) >= 10*1000) {
                        for(uint i=0; i<NUM_PIXELS; ++i) {
                            if(i<PIXELS_DISPLAYED) {
                                put_pixel(urgb_u32(0, 0, 0x11));
                            } else {
                                put_pixel(urgb_u32(0, 0, 0));
                            }
                        }
                        neo_update = get_absolute_time();
                    }
                }
                break;
            }
            
        } else {

            gpio_put(LED_PIN, false);
            
            // turn all off
            if(absolute_time_diff_us(neo_update, get_absolute_time()) >= 10*1000) {
                for(uint i=0; i<NUM_PIXELS; ++i) {
                    put_pixel(urgb_u32(0, 0, 0));
                }
                neo_update = get_absolute_time();
            }

            // do this only once when the button changes from on to off
            if(btn.prev_status == true) {
                printf("colour_sel = %d\r\n", colour_sel);
                colour_sel++;
                if(colour_sel > 2) colour_sel = 0;
            }
            btn.prev_status = false;

            // Q: 3) is the above an acceptable way of mixing event driven inside of looping?
            // Q: 4) should my Button.c code be handling this more than here in main? is it messy?
            
        }
        // --



    } // end of while

    return 0;
}





// --- servo related ---
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
// -------------


// --- dual core related ---
void second_core_code() {
    adc_init();

    adc_gpio_init(ULTRASONIC_PIN); // make sure GPIO is high-impedance, no pullups etc
    adc_select_input(ULTRASONIC_ADC); // select ADC input

    initUltrasonicSensor(&sen);

    // ultrasonic sensor warm up time
    add_alarm_in_ms(ULTRASONIC_WARMUP, ultrasonic_warmup_callback, NULL, false);

    printf("Sensor ready (warming up)\r\n");

    while(true) {

        if(sen.ready) {
            
            // update the sensor
            updateUltrasonicSensor(&sen);
            
            // map the distance to 8
            // use the raw value for better responsiveness
            uint8_t cm_val = (uint8_t)sen.raw_cm;
            if(cm_val < ULTRASONIC_MIN) cm_val = ULTRASONIC_MIN;
            if(cm_val > ULTRASONIC_MAX) cm_val = ULTRASONIC_MAX;
            uint32_t dist_val = map(sen.raw_cm, ULTRASONIC_MIN, ULTRASONIC_MAX, 0, 8);
            
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


// --- button related ---
int64_t debounce_callback(alarm_id_t id, void *user_data) {
    btn.prev_status = btn.status;
    btn.status = !btn.status;
    btn.pressed = true;
    btn.clicks++;
    return 0;
}

void button_callback(uint gpio, uint32_t events) {
    if(events == 0x8) { // 0x8 = EDGE_RISE
        btn.debounce_start = get_absolute_time();
        if(!btn.trig) {
            add_alarm_in_ms(DEBOUNCE_DEL, debounce_callback, NULL, false);
            btn.trig = true;
        }
    }
}
// -------------


// --- neopixel related ---
static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return
            ((uint32_t) (r) << 8) |
            ((uint32_t) (g) << 16) |
            (uint32_t) (b);
}
// -------------
