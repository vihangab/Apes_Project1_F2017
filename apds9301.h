#include <stdio.h>
#include <stdint.h>

#ifndef APDS9301_H_
#define APDS9301_H_

#define CMDbit          (0x80)
#define Word_mode       (0xC0)

// Address of all the registers
#define I2C_CTRL_ADD    (0x00)
#define I2C_TIMING      (0x01)
#define I2C_TLL_ADD     (0x02)
#define I2C_TLH_ADD     (0x03)
#define I2C_THL_ADD     (0x04)
#define I2C_THH_ADD     (0x05)
#define I2C_INT_ADD     (0x06)
#define ADC0_DLOW_ADD   (0x0C)
#define ADC1_DLOW_ADD   (0x0E)
#define ADC0_DHIGH_ADD  (0x0D)
#define ADC1_DHIGH_ADD  (0x0F)

#define POWER           (0x03)
#define TIMING          (0x01)      //Low gain

// Low-High threshold values
#define TLL             (0x0F)
#define TLH             (0x00)
#define THL             (0x00)
#define THH             (0x08)

#define PERSIST         (0x04)
#define INTR_FIELD      (0x01)

typedef enum{
  LIGHT,
  DARK
}apds_state;

typedef struct light_data
{
  uint16_t lumen;
  Light_State state;
}Light_Data;

i2c_state write_registers();

i2c_state read_registers();

i2c_state write_config();

i2c_state read_config();

i2c_state config_integration(uint8_t integrationPeriod);

i2c_state enable_apds_interrupt();

i2c_state disable_apds_interrupt();

i2c_state read_ID_reg(uint8_t *id_val);

i2c_state read_lux_values(uint16_t *ch0, uint16_t *ch1);

apds_state read_light_state();

#endif /* APDS9301_H_ */
