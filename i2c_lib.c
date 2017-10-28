/****************************************************
Name - i2c_lib.cab
Author - Virag Gada and Vihanga Bare
Description - Source file for out I2C library
*****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include "i2c_lib.h"

// Function to setup an I2C device
i2c_state setupI2CDevice (uint32_t * file, char * device, uint8_t devAddr)
{

}

// Function to write to an I2C device
i2c_state writeI2CByte (uint32_t * file, int8_t* data)
{

}

// Function to read from an I2C device
i2c_state readI2CByte (uint32_t * file, int8_t * data)
{

}
