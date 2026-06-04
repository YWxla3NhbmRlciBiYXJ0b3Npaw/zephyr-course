#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <our_driver.h>

int main(void)
{
    const struct device *sensor_dev = DEVICE_DT_GET(DT_NODELABEL(our_driver));

    if (!device_is_ready(sensor_dev)) {
        return -ENODEV;
    }

    struct sensor_value dummy_val;

    uint8_t current_brightness = 32;

    while (1) {
        our_driver_set_brightness(sensor_dev, current_brightness);

        sensor_sample_fetch(sensor_dev);
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);

        sensor_channel_get(sensor_dev, SENSOR_CHAN_ALL, &dummy_val);
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);

        current_brightness += 32;
        if (current_brightness > 160) {
            current_brightness = 32;
        }
    }
}
