#include "led.h"
// #include <zephyr.h>
#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <drivers/gpio.h>

struct k_thread led_thread;

static const struct gpio_dt_spec int_led = GPIO_DT_SPEC_GET(DT_NODELABEL(int_led), gpios);

#define LED_STACK_SIZE 512
static K_KERNEL_STACK_DEFINE(io_led_stack, LED_STACK_SIZE);

// TODO: Use kernel mechanic
volatile t_indicator indicator = INDICATOR_OFF;

static void io_led_thread(void *p1, void *p2, void *p3)
{
    uint32_t counter = 0;
    for(;;) {
        counter++;
        switch(indicator) {
            case INDICATOR_OFF:
                gpio_pin_set_dt(&int_led, 0);
                break;
            case INDICATOR_POWERING_ON:
                gpio_pin_set_dt(&int_led, 1);
                break;
            case INDICATOR_POWERING_OFF:
                gpio_pin_set_dt(&int_led, (counter & 0x08) == 0);
                break;
            case INDICATOR_IDLE:
                gpio_pin_set_dt(&int_led, (counter & 0x0F) == 0);
                break;
        }
        k_sleep(K_MSEC(64));
    }
}

static int io_led_init(const struct device *dev)
{
    ARG_UNUSED(dev);

    // Internal LED
    gpio_pin_configure_dt(&int_led, GPIO_OUTPUT);
    gpio_pin_set_dt(&int_led, 0);

    k_thread_create(&led_thread,
        io_led_stack, LED_STACK_SIZE,
        io_led_thread, NULL, NULL, NULL,
        -1, K_USER, K_NO_WAIT);
    k_thread_name_set(&led_thread, "IO/LED");

    return 0;
}

SYS_INIT(io_led_init, APPLICATION, CONFIG_KERNEL_INIT_PRIORITY_DEVICE);

// Public API
void io_led_set_indicator(t_indicator _indicator)
{
    indicator = _indicator;
}
