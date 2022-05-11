#ifndef _IO_LED_H_
#define _IO_LED_H_

typedef enum {
    INDICATOR_OFF,
    INDICATOR_POWERING_ON,
    INDICATOR_POWERING_OFF,
    INDICATOR_IDLE,
} t_indicator;

void io_led_set_indicator(t_indicator _indicator);

#endif
