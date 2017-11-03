#include <stdio.h>
#include <stdint.h>

#ifndef TMP106_H_
#define TMP106_H_

#define ALERTbit          (0x00) // Comparator Mode = 00
#define POWER_VCC         (0x49) //VCC
#define SDA_PIN           (0x4A) //SDA_PIN
#define SDL_PIN           (0x4B) //SDL_PIN

//P1,P0 bit in Pointer Register of TMP 106 to select registers

#define TEMP_SELECT_ROTEMP	(0x00)
#define TEMP_SELECT_CONFIG	(0x01)
#define TEMP_SELECT_TLOW	(0x20)
#define TEMP_SELECT_THIGH	(0x03)


struct temp_data
{
	uint16_t t_low;
	uint16_t t_high;
	double temp_celsius;
	double temp_farenh;
	double temp_kelvin;
	
}Temp_Data;

i2c_state write_tmp106_registers();

i2c_state read_tmp106_registers();

i2c_state write_tmp106_config();

i2c_state read_tmp106_config();

i2c_state write_tmp106_pointer_reg();

i2c_state read_tmp106_Temp_reg();

i2c_state pwr_up_sequence_tmp106();

i2c_state pwr_down_sequence_tmp106();

i2c_state set_resolution_tmp106();

#endif /* TMP106_H_ */
