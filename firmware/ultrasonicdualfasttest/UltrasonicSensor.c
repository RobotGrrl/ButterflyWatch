#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "UltrasonicSensor.h"
#include "Board.h"

void initUltrasonicSensor(struct UltrasonicSensor *s) {

    s->warmedup = false;
    s->ready = false;
    s->active = false;
    s->sample_count = 0;
    s->sum = 0;
    s->val = 0;
    s->cm = 0.0f; // Q: is this the correct way to set a float to 0?
    s->raw_cm = 0.0f;

    // 4096*20 = 81,920
    // max of uint16_t = 65,535
    // max of uint32_t = 4,294,967,295

}


float convertValue(uint16_t result) {
    // 12-bit conversion, assume max value == ADC_VREF == 3.3 V
    const float conversion_factor = 3.3f / (1 << 12);
    float adc_volts = result*conversion_factor;
    float adc_mv = adc_volts*1000; // in mV
    float scale_factor = 6.4; // mV per in
    float distance = adc_mv * scale_factor / 100.0f;
    float return_val = distance*2.54f;
    return return_val;
}


void updateUltrasonicSensor(struct UltrasonicSensor *s) {

    // acquire the sensor reading
    adc_select_input(ULTRASONIC_ADC); // select ADC input
    s->val = adc_read();

    // convert and add to avg
    s->raw_cm = convertValue(s->val);
    s->sum += s->val;
    s->sample_count++;
    
    // ready to average
    if(s->sample_count >= ULTRASONIC_SAMPLES) {

        // calculate average
        s->val = s->sum / s->sample_count;
        s->cm = convertValue(s->val);
        //printf("%.0f distance: %.2f sum: %d\r\n", (get_absolute_time()/1000.0f), s->cm, s->sum);

        // reset everything
        s->sum = 0;
        s->sample_count = 0;
    }

    // reset
    s->ready = false;
}

