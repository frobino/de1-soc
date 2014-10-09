de1-soc
=======

experiments with de1-soc: custom digital hardware driven from ARM running linux (custom device drivers)

folder structure
================

- rtl: contains hdl files describing the custom component we want to drive from the ARM

- my_first_HPS: the files needed to generate a system containing ARM and the custom component (using Altera qsys)

- linux_utilities: an image file of the linux bootloader for ARM Cortex-A9, and a script which
		   can be used to program the FPGA directly from the ARM running Linux (requires .rbf file)

- linux_software: contains C code which can be compiled for the ARM running Linux, together with 
		  a device driver enabling a task from user space to access the custom component

- bare_metal_software: contains C code accessing the custom component from the ARM without the Linux OS

instruction to setup linux
==========================

look in the file README_CONFIG_LINUX
