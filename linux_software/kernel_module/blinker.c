#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/uaccess.h>
#include <linux/ioport.h>
#include <linux/io.h>

// real (non virtual) memory address of VHDL custom component 
// (see QSYS, 0x0, plus 0xff200000 where HPS starts seeing the FPGA logic)
#define BLINKER_BASE 0xff200000
#define BLINKER_SIZE PAGE_SIZE

// used to store the virtual address of the VHDL custom component
void *blink_mem;

// the standard way for userspace to communicate with drivers is through file IO operations. 
// To get a driver entry in Sysfs, we need to declare and register a device_driver struct.
// With the configuration provided in this struct, when the kernel module will be loaded, 
// a file will be created at “/sys/bus/platform/drivers/blinker/blinker”.
static struct device_driver blinker_driver = {
	.name = "blinker",
	.bus = &platform_bus_type,
};

// This function is called every time a process tries to read the driver file (blinker)
ssize_t blinker_show(struct device_driver *drv, char *buf)
{
	return 0;
}

// This function is called every time a process tries to write to the driver file (blinker)
ssize_t blinker_store(struct device_driver *drv, const char *buf, size_t count)
{
	u8 delay;

	if (buf == NULL) {
		pr_err("Error, string must not be NULL\n");
		return -EINVAL;
	}

	// kstrtou8 is a kernel function transforming a string (pointed by "buf")
	// to a unsiged 8 bit, stored in "delay" 
	if (kstrtou8(buf, 10, &delay) < 0) {
		pr_err("Could not convert string to integer\n");
		return -EINVAL;
	}

	if (delay < 1 || delay > 15) {
		pr_err("Invalid delay %d\n", delay);
		return -EINVAL;
	}

	// write 1 byte to the VHDL custom component register interface
	iowrite8(delay, blink_mem);

	return count;
}

/* S_IWUSR specifies that the "blinker" driver file can be accesses in write mode only by the owner 
	  in user space (so only the user who has compiled the source); 
   S_IWOTH specifies that the "blinker" driver file can be accesses in write mode by others, 
   	  including other users in user space; 
*/
// static DRIVER_ATTR(blinker, S_IWUSR, blinker_show, blinker_store);
static DRIVER_ATTR(blinker, S_IWOTH, blinker_show, blinker_store);

MODULE_LICENSE("Dual BSD/GPL");

static int __init blinker_init(void)
{
	int ret;
	struct resource *res;

	ret = driver_register(&blinker_driver);
        if (ret < 0)
		return ret;

	// create the file in /sys/.../blinker, as specified by device_driver struct
	ret = driver_create_file(&blinker_driver, &driver_attr_blinker);
	if (ret < 0) {
		driver_unregister(&blinker_driver);
		return ret;
	}

	res = request_mem_region(BLINKER_BASE, BLINKER_SIZE, "blinker");
	if (res == NULL) {
		driver_remove_file(&blinker_driver, &driver_attr_blinker);
		driver_unregister(&blinker_driver);
		return -EBUSY;
	}

	// assign to blink_mem the virtual memory address representing the BLINKER_BASE 0xff200000 
	blink_mem = ioremap(BLINKER_BASE, BLINKER_SIZE);
	if (blink_mem == NULL) {
		driver_remove_file(&blinker_driver, &driver_attr_blinker);
		driver_unregister(&blinker_driver);
		release_mem_region(BLINKER_BASE, BLINKER_SIZE);
		return -EFAULT;
	}

	return 0;
}

static void __exit blinker_exit(void)
{
	driver_remove_file(&blinker_driver, &driver_attr_blinker);
	driver_unregister(&blinker_driver);
	release_mem_region(BLINKER_BASE, BLINKER_SIZE);
	iounmap(blink_mem);
}

module_init(blinker_init);
module_exit(blinker_exit);
