#include <zephyr/init.h>
#include <zephyr/kernel.h>

int from_scratch_init(void)
{
	printk("Board Initialized\n");
	return 0;
}

SYS_INIT(from_scratch_init, APPLICATION, 10);
