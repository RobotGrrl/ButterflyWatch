#include "pico/stdlib.h"
#include "UltrasonicSensor.h"
#include "Board.h"

void initUltrasonicSensor(struct UltrasonicSensor *s) {

    s->warmedup = false;
    s->ready = false;
    s->active = false;
    s->raw = 0;
    s->sample_count = 0;
    s->sum = 0;
    s->val = 0;
    s->cm = 0.0f; // Q: is this the correct way to set a float to 0?

    // 4096*20 = 81,920
    // max of uint16_t = 65,535
    // max of uint32_t = 4,294,967,295

}

void updateUltrasonicSensor(struct UltrasonicSensor *s) {

    // ready to average
    if(s->sample_count >= ULTRASONIC_SAMPLES) { // -1) {  // yikes

        // calculate average
        s->val = s->sum / s->sample_count;

        // 12-bit conversion, assume max value == ADC_VREF == 3.3 V
        const float conversion_factor = 3.3f / (1 << 12);
        uint16_t result = s->val;
        float adc_volts = result*conversion_factor;
        float adc_mv = adc_volts*1000; // in mV
        float scale_factor = 6.4; // mV per in
        float distance = adc_mv * scale_factor / 100.0f;
        s->cm = distance*2.54f;
        printf("%.0f distance: %.2f sum: %d\r\n", (get_absolute_time()/1000.0f), s->cm, s->sum);

        // reset everything
        s->sum = 0;
        s->sample_count = 0;
    }

    // reset everything
    s->raw = 0;
    s->ready = false;
}


