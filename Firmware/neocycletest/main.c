#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"

#include "Button.h"
#include "Board.h"

// peripheral related
struct Button btn;

// button related
int64_t debounce_callback(alarm_id_t id, void *user_data);
void button_callback(uint gpio, uint32_t events);

// neopixel related
static inline void put_pixel(uint32_t pixel_grb);
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b);
uint8_t colour_sel = 0;
absolute_time_t neo_update = 0;


int main() {

    stdio_init_all();

    // led related
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // neopixel related
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, NEO_PIN, 800000, false);

    // button related
    initButton(&btn, BTN_PIN);
    gpio_set_irq_enabled_with_callback(BTN_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &button_callback);

    printf("Neopixel cycle test\r\n");

    while(true) {

        updateButton(&btn);

        // Q: 1) which method of checking the button is better? 
        //  event driven (calling just once) or looping (called each time)?
        // in this case, the rationale for the looping method is in case in the future 
        //  there's a need to update the colours of the neopixels - which would be 
        //  dependent on the button state, rather than just the button event.
        // Q: 2) also, is that the right terminology for 'looping'?
        //  was thinking it's not polling, since it's not reading the button each time,
        //  as it's based on the state...

        if(btn.status == true) {

            gpio_put(LED_PIN, true);

            switch(colour_sel) {
                case 0: {
                    if(absolute_time_diff_us(neo_update, get_absolute_time()) >= 10*1000) {
                        for(uint i=0; i<NUM_PIXELS; ++i) {
                            put_pixel(urgb_u32(0x11, 0, 0));
                        }
                        neo_update = get_absolute_time();
                    }
                }
                break;
                case 1: {
                    if(absolute_time_diff_us(neo_update, get_absolute_time()) >= 10*1000) {
                        for(uint i=0; i<NUM_PIXELS; ++i) {
                            put_pixel(urgb_u32(0, 0x11, 0));
                        }
                        neo_update = get_absolute_time();
                    }
                }
                break;
                case 2: {
                    if(absolute_time_diff_us(neo_update, get_absolute_time()) >= 10*1000) {
                        for(uint i=0; i<NUM_PIXELS; ++i) {
                            put_pixel(urgb_u32(0, 0, 0x11));
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

    }

    return 0;
}


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

