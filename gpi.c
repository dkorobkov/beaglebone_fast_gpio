/*
 * Extended test for fast accessing Beaglebone Black GPIO over mmap().
 *
 * Code based on https://github.com/chiragnagpal/beaglebone_mmap
 *
 * Improvements are:
 * - cleanup on exit (CTrl+C handling);
 * - access any GPIO pin available on P8 and P9, not only GPIO60
 * - more debug output regarding real registers state and bit masks
 *
 * github.com/dkorobkov
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <signal.h>

#include "beaglebone_gpio.h"

int IsValidGpio(int gpio)
{
	int arKnownGpios[] = {
			30,31,48,5,3,49,117,115,
			111,110,20,60,50,51,4,2,15,14,113,112,
			7,38,39,34,35,66,67,69,68,45,44,23,26,
			47,46,27,65,22,63,62,37,36,33,32,61,86,
			88,87,89,10,11,9,81,8,80,78,79,76,77,
			74,75,72,73,70,71, 0
	};
	if(gpio <= 0 || gpio > 127)
		return 0;
	// Find GPIO in array. GPIOs are taken from Beaglebone Black pinout
	// at http://beagleboard.org/Support/bone101
	int* p = arKnownGpios;
	while(*p != 0)
	{
		if(gpio == *p++)
			return 1;
	}
	return 0;
}

int GetGpioAddresses(int gpio, uint32_t* pGpioStartAddress, uint32_t* pGpioEndAddress, uint32_t* Pin)
{
	if(gpio <= 0) return 0;
	if(gpio <= 32)
	{
		*pGpioStartAddress = GPIO0_START_ADDR;
		*pGpioEndAddress = GPIO0_END_ADDR;
		*Pin = (1<<gpio);
		return 1;
	}

	if(gpio <= 64)
	{
		*pGpioStartAddress = GPIO1_START_ADDR;
		*pGpioEndAddress = GPIO1_END_ADDR;
		*Pin = (1<<(gpio-32));
		return 1;
	}

	if(gpio <= 96)
	{
		*pGpioStartAddress = GPIO2_START_ADDR;
		*pGpioEndAddress = GPIO2_END_ADDR;
		*Pin = (1<<(gpio-64));
		return 1;
	}

	if(gpio <= 128)
	{
		*pGpioStartAddress = GPIO3_START_ADDR;
		*pGpioEndAddress = GPIO3_END_ADDR;
		*Pin = (1<<(gpio-96));
		return 1;
	}

	return 0;
}

volatile uint8_t bStop = 0;

void CtrlChandler(int s)
{
	bStop = 1;
}

void InstallCtrlChandler()
{
	struct sigaction sigIntHandler;

	sigIntHandler.sa_handler = CtrlChandler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;

	sigaction(SIGINT, &sigIntHandler, NULL);
}


int main(int argc, char *argv[])
{
    volatile void *gpio_addr = NULL;
    volatile unsigned int *gpio_oe_addr = NULL;
    volatile unsigned int *gpio_setdataout_addr = NULL;
    volatile unsigned int *gpio_cleardataout_addr = NULL;
    unsigned int reg;

    int gpio = 60; // Default will be P9.12 (GPIO_60)
  
  if(argc == 1)
	  printf("Will drive P9.12 (GPIO_60)\n");

  if(argc == 2)
  {
	  if(strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
	  {
		  printf("Send pulses into chosen BeagleBone Black (TI Sitara based SoC) GPIO pin.\n"
				  "Usage: %s <gpio_num>\n"
				  "Use Ctrl_C to stop.\n"
				  "Example: %s 60\n"
				  "If gpio_num is omitted will send output into GPIO_60 (P9.12)\n"
				  "GPIO must be connected in device tree (use \"config-pin -q P9_12\" to query)\n",
				  argv[0], argv[0]);
		  return -1;
	  }
	  else
	  {
		  gpio = strtol(argv[1], NULL, 10);
		  if(IsValidGpio(gpio) == 0)
		  {
			  printf("GPIO%d is not valid or not available\n", gpio);
			  return -1;
		  }
	  }
  }

  uint32_t GpioStartAddress, GpioEndAddress, Pin;

  if(GetGpioAddresses(gpio, &GpioStartAddress, &GpioEndAddress, &Pin) == 0)
  {
	  printf("Error getting GPIO mapping addresses\n");
	  return -1;
  }

  InstallCtrlChandler();  // Handle Ctrl+C (SIGINT)

  uint32_t GpioSize = GpioEndAddress - GpioStartAddress;

  int fd = open("/dev/mem", O_RDWR);

    printf("Mapping %X - %X (size: %X)\n", GpioStartAddress,
    		GpioEndAddress, GpioSize);

    gpio_addr = mmap(0, GpioSize, PROT_READ | PROT_WRITE,
    		MAP_SHARED, fd, GpioStartAddress);

    gpio_oe_addr = gpio_addr + GPIO_OE;
    gpio_setdataout_addr = gpio_addr + GPIO_SETDATAOUT;
    gpio_cleardataout_addr = gpio_addr + GPIO_CLEARDATAOUT;

    if(gpio_addr == MAP_FAILED)
    {
        printf("Unable to map GPIO\n");
        return -2;
    }

    printf("GPIO mapped to %p\n", gpio_addr);
    printf("GPIO OE mapped to %p\n", gpio_oe_addr);
    printf("GPIO SETDATAOUTADDR mapped to %p\n", gpio_setdataout_addr);
    printf("GPIO CLEARDATAOUT mapped to %p\n", gpio_cleardataout_addr);

    printf("Pin mask: %08X\n", Pin);

    reg = *gpio_oe_addr;
    printf("GPIO configuration (OE): %08X\n", reg);

    uint32_t OldDir = *gpio_oe_addr & Pin;
    printf("Direction bit value: %08X\n", OldDir);

    reg = reg & ~Pin;
    *gpio_oe_addr = reg;
    printf("GPIO configuration (OE) should become: %08X\n", reg);

    printf("Real GPIO configuration (OE): %08X\n", *gpio_oe_addr);

    uint32_t NewDir = *gpio_oe_addr & Pin;
    printf("Now Direction bit value: %08X\n", NewDir);

    printf("Start toggling GPIO%d \n", gpio);
    while(bStop == 0)
    {
        
        *gpio_setdataout_addr = Pin;
        *gpio_cleardataout_addr = Pin;
      }

    // restore initial port setup
    if(OldDir) // Do not touch port if no bit to set
    	*gpio_oe_addr |= OldDir;

    printf("GPIO configuration (OE) after cleanup: %08X\n", *gpio_oe_addr);

    close(fd);
    return 0;
}
