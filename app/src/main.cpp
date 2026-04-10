#include <zephyr/drivers/led_strip.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#define SLEEP_TIME_MS 1000
#define STRIP_NODE DT_ALIAS(led_strip)
#define STRIP_NUM_PIXELS 1

static const struct device *const strip = DEVICE_DT_GET(STRIP_NODE);
static struct led_rgb on = {.r = 0x00, .g = 0x00, .b = 0xFF};
static struct led_rgb off = {.r = 0x00, .g = 0x00, .b = 0x00};

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
    bool led_state = true;

    if (!device_is_ready(strip)) return 0;

    while (1) {

        led_strip_update_rgb(strip, led_state ? &on : &off, STRIP_NUM_PIXELS);

        led_state = !led_state;
        LOG_INF("LED state: %s", led_state ? "ON" : "OFF");
        k_msleep(SLEEP_TIME_MS);
    }
    return 0;
}
