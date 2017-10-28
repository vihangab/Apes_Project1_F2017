#include <stdio.h>
#include <stdlib.h>
#include "i2c_lib.h"
#include "apds9301.h"

// Function to write all registers
i2c_state write_registers()
{
  //Also calls config register
}

//Function to read all registers
i2c_state read_registers()
{

}

// Function to write to config register
i2c_state write_config()
{

}

// Function to read to config register
i2c_state read_config()
{

}

// Function to set integration times
i2c_state config_integration(uint8_t integrationPeriod)
{

}

// Function to enable the APDS sensor interrupt
i2c_state enable_apds_interrupt()
{

}

//Function to disable APDS sensor interrupts
i2c_state disable_apds_interrupt()
{

}

//Function to read the ID register
i2c_state read_ID_reg(uint8_t *id_val)
{

}

// Function to read LUX values
i2c_state read_lux_values(uint16_t *ch0, uint16_t *ch1)
{

}

//Function to find light state based on luc values
apds_state read_light_state()
{

}
