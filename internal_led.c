#include "internal_led.h"
// #include <zephyr.h>
#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(internal_led, CONFIG_APP_LOG_LEVEL);

struct k_thread led_thread;

static const struct gpio_dt_spec int_led = GPIO_DT_SPEC_GET(DT_NODELABEL(int_led), gpios);

// TODO: Use kernel mechanic
volatile t_indicator indicator = INDICATOR_OFF;

static inline __attribute__((always_inline)) void error3(int counter) {
    switch((counter & 0x0F)) {
        case 0:
        case 2:
        case 4:
            gpio_pin_set_dt(&int_led, 1);
            break;
        default:
            gpio_pin_set_dt(&int_led, 0);
    }
}

static inline __attribute__((always_inline)) void error4(int counter) {
    switch((counter & 0x0F)) {
        case 0:
        case 2:
        case 4:
        case 6:
            gpio_pin_set_dt(&int_led, 1);
            break;
        default:
            gpio_pin_set_dt(&int_led, 0);
    }
}

// static void io_led_thread(void *p1, void *p2, void *p3)
static void io_led_to(struct k_timer *dummy)
{
    ARG_UNUSED(dummy);
    static uint8_t counter = 0;
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
        case INDICATOR_ERROR_3:
            error3(counter & 0x0F);
            break;
        case INDICATOR_ERROR_4:
            error4(counter & 0x0F);
            break;
    }
    counter++;
}

K_TIMER_DEFINE(io_led_timer, io_led_to, io_led_to/*no realy need cancel*/);

static int io_led_init(const struct device *dev)
{
    ARG_UNUSED(dev);

    if(!device_is_ready(int_led.port)) {
		LOG_ERR("GPIO device [%s] is not ready", int_led.port->name);
		return -ENODEV;
	}

    // Internal LED
    gpio_pin_configure_dt(&int_led, GPIO_OUTPUT);
    gpio_pin_set_dt(&int_led, 0);

    k_timer_start(&io_led_timer, K_MSEC(64), K_MSEC(64));

    // k_thread_create(&led_thread,
    //     io_led_stack, LED_STACK_SIZE,
    //     io_led_thread, NULL, NULL, NULL,
    //     -1, K_USER, K_NO_WAIT);
    // k_thread_name_set(&led_thread, "IO/LED");

    return 0;
}

SYS_INIT(io_led_init, APPLICATION, CONFIG_KERNEL_INIT_PRIORITY_DEVICE);

// Public API
void io_led_set_indicator(t_indicator _indicator)
{
    indicator = _indicator;
}

t_indicator io_led_get_indicator() {
    return indicator;
}
