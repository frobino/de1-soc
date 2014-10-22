#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
// check difference between registering device and platform device
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/fcntl.h>
//#include <asm/system.h>
#include <linux/uaccess.h>
#include <linux/ioport.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/types.h>


// - chardevs ?? 
struct params
{
  int op;
  char pname[42];
};

// real (non virtual) memory address of VHDL custom component 
// (see QSYS, 0x0, plus 0xff200000 where HPS starts seeing the FPGA logic)
#define BLINKER_BASE 0xff200000
#define BLINKER_SIZE PAGE_SIZE

// used to store the virtual address of the VHDL custom component
void *blink_mem;

#define IOCTL_SET_MSG 1066
//--
#define SUCCESS 0
int pkt_drop_flag=0;     /*By default, it will not drop the packets */
int memory_major = 60;

int device_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param) ;

/*The operations our driver knows how to do*/
static const struct file_operations blinker_driver_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl=device_ioctl,
};

static struct miscdevice blinker_driver = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "blinker",
	//.bus = &platform_bus_type,

	.fops = &blinker_driver_fops,
};
/*
struct file_operations memory_fops = {
 ioctl: device_ioctl
};
*/

int device_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param)
{
  int rc;
  struct params *obj= kmalloc(sizeof(struct params), GFP_DMA);

  switch (ioctl_num)
    {
    case IOCTL_SET_MSG:
      rc = copy_from_user(obj, (void *)ioctl_param, sizeof(struct params));
      printk(KERN_INFO "\ncopy_from_user() = %d.\n", rc);
      pkt_drop_flag = obj->op;
      printk(KERN_INFO "\npname=\"%s\"\n", obj->pname);
      break;
    default:
      printk(KERN_INFO "\ndefault case\n");
      break;
    }

  if(pkt_drop_flag == 1)
    {
      printk(KERN_INFO "\nPackets are set to DROP\n");
    }
  else
    {
      printk(KERN_INFO "\nPackets are set to ACCEPT\n");
    }

  return SUCCESS;
}

// This function is called every time a process tries to read the driver file (blinker)
ssize_t blinker_show(struct device_driver *drv, char *buf)
{
	return 0;
}

int blinker_store(struct inode *inode, struct file *file, unsigned int ioctl_num, unsigned long ioctl_param)
{
  int rc;
  struct params *obj= kmalloc(sizeof(struct params), GFP_DMA);

  switch (ioctl_num)
    {
    case IOCTL_SET_MSG:
      rc = copy_from_user(obj, (void *)ioctl_param, sizeof(struct params));
      printk(KERN_INFO "\ncopy_from_user() = %d.\n", rc);
      pkt_drop_flag = obj->op;
      printk(KERN_INFO "\npname=\"%s\"\n", obj->pname);
      break;
    default:
      printk(KERN_INFO "\ndefault case\n");
      break;
    }

  if(pkt_drop_flag == 1)
    {
      printk(KERN_INFO "\nPackets are set to DROP\n");
    }
  else
    {
      printk(KERN_INFO "\nPackets are set to ACCEPT\n");
    }

  return SUCCESS;
}

static DRIVER_ATTR(blinker, S_IWOTH, blinker_show, blinker_store);
MODULE_LICENSE("Dual BSD/GPL");

static int __init blinker_init(void)
{
	int ret;
	struct resource *res;

	ret = misc_register(&blinker_driver);
        if (ret < 0)
		return ret;

	// create the file in /sys/.../blinker, as specified by device_driver struct
	/*
	ret = driver_create_file(&blinker_driver, &driver_attr_blinker);
	if (ret < 0) {
		driver_unregister(&blinker_driver);
		return ret;
	}
	*/

	res = request_mem_region(BLINKER_BASE, BLINKER_SIZE, "blinker");
	if (res == NULL) {
		//driver_remove_file(&blinker_driver, &driver_attr_blinker);
		misc_deregister(&blinker_driver);
		return -EBUSY;
	}

	// assign to blink_mem the virtual memory address representing the BLINKER_BASE 0xff200000 
	blink_mem = ioremap(BLINKER_BASE, BLINKER_SIZE);
	if (blink_mem == NULL) {
		//driver_remove_file(&blinker_driver, &driver_attr_blinker);
		misc_deregister(&blinker_driver);
		release_mem_region(BLINKER_BASE, BLINKER_SIZE);
		return -EFAULT;
	}

	return 0;
}


static void __exit blinker_exit(void)
{
	//driver_remove_file(&blinker_driver, &driver_attr_blinker);
	misc_deregister(&blinker_driver);
	release_mem_region(BLINKER_BASE, BLINKER_SIZE);
	iounmap(blink_mem);
}

module_init(blinker_init);
module_exit(blinker_exit);
