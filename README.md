A simple test of accessing GPIO pins on BeagleBone Black.

Based on https://github.com/chiragnagpal/beaglebone_mmap

This example will simplify things for those who are still unclear
on determinining addresses of GPIO banks.

Improvements are:
 - cleanup on exit (CTrl+C handling), return port to original state;
 - access any GPIO pin available on P8 and P9, not only GPIO60;
 - GPIO number can be set from command line;
 - more debug output regarding real registers state and bit masks.

The test will send pulses (set high and low) into chosen BeagleBone Black 
(TI Sitara based SoC) GPIO pin.
Usage: gpi <gpio_num>
Example: ./gpi 60
If gpio_num is omitted the test will send output into GPIO_60 (P9.12)
GPIO must be connected in device tree (use "config-pin -q P9_12" to query)
 
Just build the file with your favourite C/C++ compiler, e.g.:
	gcc -o gpi gpi.c
and enjoy. 