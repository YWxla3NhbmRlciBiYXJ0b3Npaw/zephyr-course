#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/led_strip.h>
#include <zephyr/logging/log.h>
#include <our_driver.h>

LOG_MODULE_REGISTER(our_driver, LOG_LEVEL_INF);

#undef DT_DRV_COMPAT
#define DT_DRV_COMPAT our_driver

struct led_sensor_config {
    const struct device *led_strip_dev;
};

struct led_sensor_data {
    uint8_t brightness;
};

int our_driver_set_brightness(const struct device *dev, uint8_t brightness)
{
    struct led_sensor_data *data = dev->data;

    if (data == NULL) {
        return -EINVAL;
    }

    data->brightness = brightness;
    return 0;
}

static int led_sensor_sample_fetch(const struct device *dev, enum sensor_channel chan)
{
    const struct led_sensor_config *config = dev->config;
    const struct led_sensor_data *data = dev->data;

    if (!device_is_ready(config->led_strip_dev)) {
        return -ENODEV;
    }

    struct led_rgb color = { .r = 0, .g = 0, .b = data->brightness };

    LOG_INF("LED ON");

    return led_strip_update_rgb(config->led_strip_dev, &color, 1);
}

static int led_sensor_channel_get(const struct device *dev, enum sensor_channel chan,
                                  struct sensor_value *val)
{
    const struct led_sensor_config *config = dev->config;

    if (!device_is_ready(config->led_strip_dev)) {
        return -ENODEV;
    }

    struct led_rgb color = { .r = 0, .g = 0, .b = 0 };

    LOG_INF("LED OFF");

    int ret = led_strip_update_rgb(config->led_strip_dev, &color, 1);
    if (ret < 0) {
        return ret;
    }

    if (val != NULL) {
        val->val1 = 0;
        val->val2 = 0;
    }

    return 0;
}

static const struct sensor_driver_api led_sensor_api = {
    .sample_fetch = led_sensor_sample_fetch,
    .channel_get = led_sensor_channel_get,
};

static int led_sensor_init(const struct device *dev)
{
    struct led_sensor_data *data = dev->data;
    data->brightness = 32;

    LOG_INF("DRIVER INITIALIZED");
    return 0;
}

#undef LED_SENSOR_INIT
#define LED_SENSOR_INIT(inst)                                                       \
    static const struct led_sensor_config led_sensor_config_##inst = {              \
        .led_strip_dev = DEVICE_DT_GET(DT_INST_PHANDLE(inst, led_strip))            \
    };                                                                              \
    static struct led_sensor_data led_sensor_data_##inst;                           \
    DEVICE_DT_INST_DEFINE(inst,                                                     \
                          led_sensor_init,                                          \
                          NULL,                                                     \
                          &led_sensor_data_##inst,                                  \
                          &led_sensor_config_##inst,                                \
                          POST_KERNEL,                                              \
                          CONFIG_SENSOR_INIT_PRIORITY,                              \
                          &led_sensor_api)

DT_INST_FOREACH_STATUS_OKAY(LED_SENSOR_INIT)
