#include <stdio.h>
#include <stdint.h>

#ifndef I2C_LIB_H_
#define I2C_LIB_H_

/*Enum used for returning the i2c state*/
typedef enum i2c_states
{
  SUCCESS,
  ERROR_READ,
  ERROR_WRITE,
  ERROR_OPEN,
  ERROR_ADDRESS,
  ERROR_VALUE,
  NULL_POINTER,
}i2c_state;

i2c_state setupI2CDevice (uint32_t * file, char * bus , uint8_t devAddr);

i2c_state writeI2CByte (uint32_t * file, uint8_t * data);

i2c_state readI2CByte (uint32_t * file, uint8_t * data);

i2c_state writeI2CWord (uint32_t * file, uint8_t * data);

i2c_state readI2CWord (uint32_t * file, uint8_t * data);

i2c_state readI2CDWord (uint32_t * file, uint8_t * data);

#endif /* I2C_LIB_H_ */
