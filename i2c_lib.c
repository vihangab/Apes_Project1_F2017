/****************************************************
Name - i2c_lib.c
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
i2c_state setupI2CDevice (uint32_t * file, char * bus, uint8_t devAddr)
{
  if ((*file = open(bus, O_RDWR)) < 0)
  {
    printf("Failed to open the i2c bus\n");
    return ERROR_OPEN;
  }
  else if (ioctl(*file, I2C_SLAVE, devAddr) < 0)
    {
      printf("Failed to acquire bus access and/or talk to slave.\n");
      /* ERROR HANDLING; you can check errno to see what went wrong */
      return ERROR_ADDRESS;
    }
  return SUCCESS;
}

// Function to write a byte to an I2C device
i2c_state writeI2CByte (uint32_t * file, uint8_t* data)
{
  if (write(*file,data,1) != 1)
  {
    /* ERROR HANDLING: i2c transaction failed */
    printf("Failed to write to the i2c bus.\n");
    return ERROR_WRITE;
  }
  return SUCCESS;
}

// Function to read a byte from an I2C device
i2c_state readI2CByte (uint32_t * file, uint8_t * data)
{
  if(read(*file, data, 1)!=1)
  {
    printf("Failed to read in the buffer\n");
    return ERROR_READ;
  }
  return SUCCESS;
}

// Function to write a word to an I2C device
i2c_state writeI2CWord(uint32_t * file, uint8_t* data)
{
  if (write(*file,data,2) != 2)
  {
    /* ERROR HANDLING: i2c transaction failed */
    printf("Failed to write to the i2c bus.\n");
    return ERROR_WRITE;
  }
  return SUCCESS;
}

// Function to read a word from an I2C device
i2c_state readI2CWord (uint32_t * file, uint8_t * data)
{
  if(read(*file, data, 2)!=2)
  {
    printf("Failed to read in the buffer\n");
    return ERROR_READ;
  }
  return SUCCESS;
}

// Function to read a word from an I2C device
i2c_state readI2CDWord (uint32_t * file, uint8_t * data)
{
  if(read(*file, data, 4)!=4)
  {
    printf("Failed to read in the buffer\n");
    return ERROR_READ;
  }
  return SUCCESS;
}
