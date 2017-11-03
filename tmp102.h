#include <stdio.h>
#include <stdint.h>
#include <stdint.h>

#ifndef TMP102_H_
#define TMP102_H_

#define ALERTbit             (0x00) // Comparator Mode = 00
#define POWER_VCC            (0x49) //VCC
#define SDA_PIN              (0x4A) //SDA_PIN
#define SDL_PIN              (0x4B) //SDL_PIN
#define SLAVE_ADDR           (0x48)

#define POINTER_MASK			   (0x03)

//P1,P0 bit in Pointer Register of TMP 102 to select registers
#define SELECT_TEMP          (0x00)
#define SELECT_CONFIG	       (0x01)
#define SELECT_TLOW	         (0x20)
#define SELECT_THIGH	       (0x03)

/* Config register values */
#define ENABLE_SHUTDOWN      (0x01)
#define DISABLE_SHUTDOWN     (0x00)
#define COMPARATOR_MODE      (0x00)
#define INTERRUPT_MODE       (0x02)
#define POLARITY_ZERO        (0x00)
#define POLARITY_ONE         (0x04)
#define FAULTS_ONE           (0x00)
#define FAULTS_TWO           (0x08)
#define FAULTS_FOUR          (0x10)
#define FAULTS_SIX           (0x18)
#define ONE_SHOT					   (0x80)
#define NORMAL_MODE          (0x00)
#define EXTENDED_MODE        (0x10)
#define CONVERSION_RATE_0_25 (0x00)
#define CONVERSION_RATE_1    (0x40)
#define CONVERSION_RATE_4    (0x80)
#define CONVERSION_RATE_8    (0xC0)

struct temp_data
{
	uint16_t t_low;
	uint16_t t_high;
	double temp_celsius;
	double temp_farenh;
	double temp_kelvin;
}Temp_Data;

i2c_state setup_tmp102_registers(uint32_t *file);

i2c_state write_tmp102_config(uint32_t *file, uint8_t msb, uint8_t lsb);

i2c_state read_tmp102_config(uint32_t *file, uint8_t* data);

i2c_state write_tmp102_pointer_reg(uint32_t *file, uint8_t reg_addr);

i2c_state read_tmp102_temp_reg_c(uint32_t *file,double *digitalTemp);

i2c_state read_tmp102_temp_reg_f(uint32_t *file,double *digitalTemp);

i2c_state read_tmp102_temp_reg_k(uint32_t *file,double *digitalTemp);

i2c_state pwr_up_tmp102(uint32_t *file);

i2c_state pwr_down_tmp102(uint32_t *file);

i2c_state set_resolution_tmp102(uint32_t *file, uint8_t mode);

i2c_state set_low_threshold_c(uint32_t *file, float temp);

i2c_state set_high_threshold_c(uint32_t *file, float temp);

i2c_state set_conversion_rate(uint32_t *file,uint32_t value);
#endif /* TMP106_H_ */
