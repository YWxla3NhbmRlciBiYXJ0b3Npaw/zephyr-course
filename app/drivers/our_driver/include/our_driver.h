#ifndef OUR_DRIVER_H_
#define OUR_DRIVER_H_

#include <zephyr/device.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int our_driver_set_brightness(const struct device *dev, uint8_t brightness);

#ifdef __cplusplus
}
#endif

#endif /* OUR_DRIVER_H_ */
