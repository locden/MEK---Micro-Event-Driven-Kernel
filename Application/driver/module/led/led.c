#include "led.h"

#include "main.h"

void led_init(led_t* led, pf_led_ctrl init, pf_led_ctrl on, pf_led_ctrl off ) {

    led->blink_enable = LED_BLINK_DISABLE;
    led->counter = 0U;

    led->ctrl = init;
    led->on = on;
    led->off = off;
    /* initialize hardware for led */
    if(led->ctrl) {
        led->ctrl();
    }

    /* set default led is off */
    if(led->off) {
        led->off();
    }
}
void led_on(led_t* led) {

    led->status = LED_STATUS_ON;
    if(led->on) {
        led->on();
    }
}

void led_off(led_t* led) {

    led->status = LED_STATUS_OFF;
    if(led->off) {
        led->off();
    }
}
void led_toggle(led_t* led) {
    if(led->status != LED_STATUS_ON) {
        led_on(led);
    }
    else {
        led_off(led);
    }
}
void led_blink_set(led_t* led, uint32_t freq, uint32_t duty) {
    led->freq = freq;
    led->duty = duty;

    __disable_irq();
    led->blink_enable = LED_BLINK_ENABLE;
    __enable_irq();
}

void led_blink_reset(led_t* led) {
    __disable_irq();
    led->blink_enable = LED_BLINK_DISABLE;
    __enable_irq();
}

void led_blink_polling(led_t* led) {
    if (led->blink_enable == LED_BLINK_ENABLE) {

        led->counter++;

        if (led->counter <= led->duty) {
            if (led->status != LED_STATUS_ON) {
                led_on(led);
            }
        }
        else if (led->counter < led->freq) {
            if (led->status != LED_STATUS_OFF) {
                led_off(led);
            }
        }
        else if (led->counter == led->freq) {
            led->counter = 0;
        }
    }
}
