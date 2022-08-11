#ifndef _IO_LED_H_
#define _IO_LED_H_

typedef enum {
    INDICATOR_OFF,
    INDICATOR_POWERING_ON,
    INDICATOR_POWERING_OFF,
    INDICATOR_IDLE,
    INDICATOR_ERROR_3,
    INDICATOR_ERROR_4,
} t_indicator;

void io_led_set_indicator(t_indicator _indicator);
t_indicator io_led_get_indicator();

#endif
