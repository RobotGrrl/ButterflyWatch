#include "Button.h"

void initButton(struct Button *b, uint8_t p) { // params: Button, pin
    b->pin = p;
    b->pressed = false;
    b->debounce_start = 0;
    b->status = false;
    b->prev_status = false;
    b->clicks = 0;
    b->trig = false;
}

void updateButton(struct Button *b) {

    if(b->pressed == true) { // set by the debounce callback (timer interrupt)
        if(b->status) {
            printf("[+] activated\r\n");
        } else {
            printf("[-] deactivated\r\n");
        }
        b->pressed = false;
        b->trig = false;
    }

}

