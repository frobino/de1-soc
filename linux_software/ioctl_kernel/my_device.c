// http://people.ee.ethz.ch/~arkeller/linux/multi/kernel_user_space_howto-4.html

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/types.h>
#include <linux/device.h> // class_create
#include <linux/ioctl.h> // to define _IOR and _IOW, see Documentation/ioctl/ioctl-number.txt
// #include <linux/cdev.h> // to include if cdev_init is needed

// #define MY_MACIG 'G'
#define MY_MACIG ']'
#define READ_IOCTL _IOR(MY_MACIG, 0, int)
#define WRITE_IOCTL _IOW(MY_MACIG, 1, int)
 
static int major; 
static char msg[200];

// ADDED
// Trying to get access from user
/*
static int my_dev_uevent(struct device *dev, struct kobj_uevent_env *env)
{
    add_uevent_var(env, "DEVMODE=%#o", 0777);
    return 0;
}
*/
// Trying to get access from user
/*
static struct cdev c_dev; // Global variable for the character device structure
*/

static struct class *device_class;
static dev_t devn;
static struct device *ptr_error;
//

static ssize_t device_read(struct file *filp, char __user *buffer, size_t length, loff_t *offset)
{
  	return simple_read_from_buffer(buffer, length, offset, msg, 200);
}


static ssize_t device_write(struct file *filp, const char __user *buff, size_t len, loff_t *off)
{
	if (len > 199)
		return -EINVAL;
	copy_from_user(msg, buff, len);
	msg[len] = '\0';
	return len;
}

char buf[200];
// Version used with mknod /dev/memory c 60 0
// int device_ioctl(struct inode *inode, struct file *filep, unsigned int cmd, unsigned long arg) {
int device_ioctl(struct file *filep, unsigned int cmd, unsigned long arg) {
	int len = 200;
	switch(cmd) {
	case READ_IOCTL:	
		copy_to_user((char *)arg, buf, 200);
		break;
	
	case WRITE_IOCTL:
		copy_from_user(buf, (char *)arg, len);
		break;

	default:
		printk(KERN_INFO "Switch ioctl failed\n");
		printk(KERN_INFO "READ_IOCTL: %d\n",READ_IOCTL);
		printk(KERN_INFO "WRITE_IOCTL: %d\n",WRITE_IOCTL);
		printk(KERN_INFO "cmd: %d\n",cmd);

		return -ENOTTY;
	}
	return len;

}
static struct file_operations fops = {
	.read = device_read, 
	.write = device_write,
	.unlocked_ioctl = device_ioctl,
};

static DRIVER_ATTR(my_device, S_IWOTH, device_read, device_write);
MODULE_LICENSE("Dual BSD/GPL");

static int __init cdevexample_module_init(void)
{
	/* register_chrdev */
	// Using 0 as first parameter, the kernel will dinamically assign a major number
	major = register_chrdev(0, "my_device", &fops);
	if (major < 0) {
     		printk ("Registering the character device failed with %d\n", major);
	     	return major;
	}
	printk("cdev example: assigned major: %d\n", major);
	printk("create node with mknod /dev/my_device c %d 0\n", major);

	// http://stackoverflow.com/questions/11765395/arm-char-device-driver-initialization-isnt-creating-dev-file

	// We fix the minor number to 0, because my driver only drives this module
	devn = MKDEV(major,0);
	// http://stackoverflow.com/questions/12047946/trouble-calling-ioctl-from-user-space-c

	/* class_create */
    	device_class = class_create(THIS_MODULE, "my_device");
    	if (IS_ERR(device_class))
    	{
    		unregister_chrdev(major, "my_device");
    		printk(KERN_INFO "Class creation failed\n");
    		return PTR_ERR(device_class);
    	}

	// Trying to get access from user:
	/*
	// Instead of using udev rules, this line gives the module access from user space
	device_class->dev_uevent = my_dev_uevent;
	*/

	/* device_create */
    	ptr_error = device_create(device_class, NULL, devn, NULL, "my_device");
    	if (IS_ERR(ptr_error))
    	{
    		class_destroy(device_class);
    		unregister_chrdev(major, "my_device");
    		printk(KERN_INFO "Device creation failed\n");
    		return PTR_ERR(ptr_error);
    	}

	/*added to test Udev*/
	/*
	int ret;
	cdev_init(&c_dev, &fops);
	if ((ret = cdev_add(&c_dev, major, 1)) < 0)
	{
		device_destroy(device_class, major);
		class_destroy(device_class);
		unregister_chrdev_region(major, 1);
		return ret;
	}
	*/

	return 0;
}

static void __exit cdevexample_module_exit(void)
{
	device_destroy(device_class, devn);
    	class_destroy(device_class);
	unregister_chrdev(major, "my_device");
    	//unregister_chrdev_region(DEV_T, 1);

}  

module_init(cdevexample_module_init);
module_exit(cdevexample_module_exit);
