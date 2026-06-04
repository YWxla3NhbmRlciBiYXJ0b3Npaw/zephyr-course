#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/led_strip.h>
#include <zephyr/logging/log.h>
#include <our_driver.h>
#include <zephyr/shell/shell.h>
#include <stdlib.h>

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

#define MY_SENSOR_DEVICE DEVICE_DT_GET(DT_NODELABEL(our_driver))

static int cmd_sensor_fetch(const struct shell *sh, size_t argc, char **argv)
{
    const struct device *dev = MY_SENSOR_DEVICE;

    if (!device_is_ready(dev)) {
        return -ENODEV;
    }

    int ret = sensor_sample_fetch(dev);
    if (ret < 0) {
        return ret;
    }
    shell_print(sh, "(LED ON)");
    return 0;
}

static int cmd_sensor_read(const struct shell *sh, size_t argc, char **argv)
{
    const struct device *dev = MY_SENSOR_DEVICE;
    struct sensor_value val = {0};

    if (!device_is_ready(dev)) {
        shell_error(sh, "Device not ready");
        return -ENODEV;
    }

    int ret = sensor_channel_get(dev, SENSOR_CHAN_ALL, &val);
    if (ret < 0) {
        shell_error(sh, "Read failed: %d", ret);
        return ret;
    }

    shell_print(sh, "val1=%d, val2=%d (LED OFF)", val.val1, val.val2);
    return 0;
}

static int cmd_sensor_info(const struct shell *sh, size_t argc, char **argv)
{
    const struct device *dev = MY_SENSOR_DEVICE;

    shell_print(sh, "Device name: %s", dev->name);
    shell_print(sh, "Status ready:    %s", device_is_ready(dev) ? "True" : "False");
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_sensor,
    SHELL_CMD(fetch, NULL, "Fetches sensor (LED ON)", cmd_sensor_fetch),
    SHELL_CMD(read,  NULL, "Reads sensor (LED OFF)", cmd_sensor_read),
    SHELL_CMD(info,  NULL, "Shows name and sensor status", cmd_sensor_info),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(sensor, &sub_sensor, "LED Sensor commands", NULL);
