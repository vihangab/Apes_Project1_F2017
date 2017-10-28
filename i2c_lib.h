#include <stdio.h>
#include <stdint.h>

#ifndef I2C_LIB_H_
#define I2C_LIB_H_

/*Enum used for returning the i2c state*/
typedef enum i2c_states
{
  SUCCESS,
  ERROR,
  ACK,
  NACK,
  NULL_POINTER,
  AVAILABLE
}i2c_state;

i2c_state setupI2CDevice (uint32_t * file, char * device , uint8_t devAddr);

i2c_state writeI2CByte (uint32_t * file, int8_t * data);

i2c_state readI2CByte (uint32_t * file, int8_t * data);



#endif /* I2C_LIB_H_ */
