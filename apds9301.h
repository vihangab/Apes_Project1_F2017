#include <stdio.h>
#include <stdint.h>
#include "i2c_lib.h"

#ifndef APDS9301_H_
#define APDS9301_H_

#include "i2c_lib.h"

#define LIGHT_STATE_THRESHOLD (2.75)

#define CMDbit           (0x80)
#define Word_mode        (0xC0)

// Address of all the registers
#define CTRL_ADDR        (0x00)
#define TIMING_ADDR      (0x01)
#define TLL_ADDR         (0x02)
#define TLH_ADDR         (0x03)
#define THL_ADDR         (0x04)
#define THH_ADDR         (0x05)
#define INT_ADDR         (0x06)
#define ID_ADDR          (0x0A)
#define ADC0_DLOW_ADDR   (0x0C)
#define ADC1_DLOW_ADDR   (0x0E)
#define ADC0_DHIGH_ADDR  (0x0D)
#define ADC1_DHIGH_ADDR  (0x0F)

#define POWER_ON         (0x03)
#define POWER_OFF        (0x00)

/* Timing Register definitions */
#define HIGH_GAIN        (0x10)
#define LOW_GAIN         (0x00)

#define MANUAL_ON        (0x08)
#define MANUAL_OFF       (0x00)

#define INTEG_TIME_13    (0x00)
#define INTEG_TIME_101   (0x01)
#define INTEG_TIME_402   (0x02)
#define INTEG_TIME_NA    (0x03)

/* Intrerrupt Threshold Registers */
// Low-High threshold values
#define TLL              (0x0F)
#define TLH              (0x00)
#define THL              (0x00)
#define THH              (0x08)

/* Interrupt Control Register definitions */
#define PERSIST          (0x04)

#define INTERRUPT_ENABLE  (0x20)
#define INTERRUPT_DISABLE (0x00)

typedef enum{
  LIGHT,
  DARK
}apds_state;

typedef struct light_data
{
  uint16_t lumen;
  apds_state state;
}Light_Data;

i2c_state setup_apds9301_registers(uint32_t * file);

i2c_state write_control(uint32_t * file, uint8_t powerState);

i2c_state read_control(uint32_t * file, uint8_t * powerState);

i2c_state config_integration(uint32_t * file, uint8_t integrationPeriod);

i2c_state enable_apds_interrupt();

i2c_state disable_apds_interrupt();

i2c_state read_ID_reg(uint32_t * file,uint8_t *id_val);

i2c_state read_lux_values(uint32_t * file, double * lux_value);

i2c_state read_light_state(uint32_t * file, apds_state *state);

#endif /* APDS9301_H_ */
