#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "Button.h"
#include "Board.h"

// peripheral related
struct Button btn;

// callbacks
int64_t debounce_callback(alarm_id_t id, void *user_data);
void button_callback(uint gpio, uint32_t events);


int main() {

    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    initButton(&btn, BTN_PIN);

    printf("Debouncing test\r\n");
    gpio_set_irq_enabled_with_callback(BTN_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &button_callback);

    while(true) {

        updateButton(&btn);

        if(btn.status) {
            gpio_put(LED_PIN, true);
        } else {
            gpio_put(LED_PIN, false);
        }

    }

    return 0;
}


// --- button related ---
int64_t debounce_callback(alarm_id_t id, void *user_data) {
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

