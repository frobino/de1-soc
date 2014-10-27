CCPREFIX=arm-linux-gnueabihf-
PATH_TO_KERNEL_SOURCE=/home/frobino/isomount/Linux_tools/altera_soc/kernel/linux-socfpga

obj-m += my_device.o

all:
	make ARCH=arm CROSS_COMPILE=${CCPREFIX} -C ${PATH_TO_KERNEL_SOURCE} M=$(PWD) modules
clean:
	make -C ${PATH_TO_KERNEL_SOURCE} M=$(PWD) clean
