#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>

int main(void)
{
    const struct device *sensor_dev = DEVICE_DT_GET(DT_NODELABEL(our_driver));

    if (!device_is_ready(sensor_dev)) {
        return -ENODEV;
    }

    struct sensor_value dummy_val;

    while (1) {
        sensor_sample_fetch(sensor_dev);
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);

        sensor_channel_get(sensor_dev, SENSOR_CHAN_ALL, &dummy_val);
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);
    }
}
