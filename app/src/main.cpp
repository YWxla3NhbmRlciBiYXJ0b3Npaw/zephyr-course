#include <zephyr/drivers/led_strip.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#define SLEEP_TIME_MS 1000
#define LED_NODE DT_ALIAS(app_led)
#define STRIP_NUM_PIXELS 1

static const struct device *const led = DEVICE_DT_GET(LED_NODE);

//structs for rgb led strip: state on with purple color, state off with turned off subpixels
static struct led_rgb on = {.r = 0xFF, .g = 0x00, .b = 0xFF};
static struct led_rgb off = {.r = 0x00, .g = 0x00, .b = 0x00};

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
    bool led_state = true;

    if (!device_is_ready(led)) return 0;

    while (1) {
        //updates led strip with given state based on variable led_state
        led_strip_update_rgb(led, led_state ? &on : &off, STRIP_NUM_PIXELS);

        led_state = !led_state;
        LOG_INF("LED state: %s", led_state ? "ON" : "OFF");
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);
    }
    return 0;
}
