#ifndef _BEAGLEBONE_GPIO_H_
#define _BEAGLEBONE_GPIO_H_

// Constants taken from ARM Cortex-A8 Memory Map table 2-3 page 181
// AM335x and AMIC110 Sitara™ Processors Technical Reference Manual
// https://www.ti.com/lit/ug/spruh73p/spruh73p.pdf
#define GPIO0_START_ADDR 0x44E07000
#define GPIO0_END_ADDR 0x44E08FFF
#define GPIO1_START_ADDR 0x4804C000
#define GPIO1_END_ADDR 0x4804DFFF
#define GPIO2_START_ADDR 0x481AC000
#define GPIO2_END_ADDR 0x481ADFFF
#define GPIO3_START_ADDR 0x481AE000
#define GPIO3_END_ADDR 0x481AFFFF
//#define GPIO1_SIZE (GPIO1_END_ADDR - GPIO1_START_ADDR)
#define GPIO_OE 0x134
#define GPIO_SETDATAOUT 0x194
#define GPIO_CLEARDATAOUT 0x190


//#define PIN (1<<28)


#endif
