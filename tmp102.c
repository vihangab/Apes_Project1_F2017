/****************************************************
Name - tmp102.c
Author - Virag Gada and Vihanga Bare
Description - Source file for out TMP 102 library
*****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "i2c_lib.h"
#include "tmp106.h"

i2c_state setup_tmp102_registers(uint32_t *file)
{
  if(write_tmp102_config(file,INTERRUPT_MODE,EXTENDED_MODE|CONVERSION_RATE_8)==ERROR_WRITE){
    return ERROR_WRITE;
	}else if(set_low_threshold_c(file,20.0)==ERROR_WRITE){
		return ERROR_WRITE;
	}else if(set_high_threshold_c(file,22.0)==ERROR_WRITE){
		return ERROR_WRITE;
	}
	return SUCCESS;
}

i2c_state write_tmp102_config(uint32_t *file, uint8_t msb, uint8_t lsb)
{
	if(file == NULL){
		return NULL_POINTER;
	}
	uint8_t reg_value[2];
	/* Check previous values */
  if(read_tmp102_config(file,reg_value) == ERROR_READ){
		return ERROR_READ;
	}
  reg_value[0] |= msb;
	reg_value[1] |= lsb;
  return writeI2CWord(file,reg_value);
}

i2c_state read_tmp102_config(uint32_t *file, uint8_t* data)
{
	if(file == NULL){
		return NULL_POINTER;
	}
	if(write_tmp102_pointer_reg(file,SELECT_CONFIG) == ERROR_WRITE){
		return ERROR_WRITE;
	}else if(readI2CWord(file,data) == ERROR_READ){
		return ERROR_READ;
	}
	return SUCCESS;
}

i2c_state write_tmp102_pointer_reg(uint32_t *file, uint8_t reg_addr)
{
	if(file == NULL){
		return NULL_POINTER;
	}
	return writeI2CByte(file, &reg_addr);
}

i2c_state read_tmp102_temp_reg_c(uint32_t *file,double *digitalTemp)
{
	if(file == NULL){
		return NULL_POINTER;
	}
	//read from Temperature register which is read-only
  uint8_t reg_value[2];
	int16_t intTemp;
	if(write_tmp102_pointer_reg(file,SELECT_TEMP)==ERROR_WRITE){
		return ERROR_WRITE;
	}else if(readI2CWord(file,reg_value)==ERROR_READ){
		return ERROR_READ;
	}
	/* Check resolution and find value */
	if(reg_value[1]&0x01){//13 bit mode
    intTemp = (reg_value[0]<<5)|(reg_value[1]>>3);
		/* Take 2's compliment if negative*/
		if(intTemp > 0xFFF){
			intTemp |= 0xE000;
		}
	}else{ // 12 bit mode
		intTemp = (reg_value[0]<<4)|(reg_value[1]>>4);
		/* Take 2's compliment if negative*/
		if(intTemp > 0x7FF){
			intTemp |= 0xF000;
		}
	}
  *digitalTemp = 0.0625*intTemp;
	return SUCCESS;
}

i2c_state read_tmp102_temp_reg_f(uint32_t *file,double *digitalTemp){
  return read_tmp102_temp_reg_c(file,digitalTemp)*9.0/5.0+32.0;
}

i2c_state read_tmp102_temp_reg_k(uint32_t *file,double *digitalTemp){
  return read_tmp102_temp_reg_c(file,digitalTemp)+273.15;
}

i2c_state pwr_up_tmp102(uint32_t *file)
{
	if(file == NULL){
		return NULL_POINTER;
	}
	uint8_t prev_value;
	if(write_tmp102_pointer_reg(file,SELECT_CONFIG)==ERROR_WRITE){
		return ERROR_WRITE;
	}else if(readI2CByte(file,&prev_value)==ERROR_READ){
		return ERROR_READ;
	}
  prev_value = prev_value & (!ENABLE_SHUTDOWN);
	if(writeI2CByte(file,&prev_value)==ERROR_WRITE){
		return ERROR_WRITE;
	}
  return SUCCESS;
}

i2c_state pwr_down_tmp102(uint32_t *file)
{
	if(file == NULL){
		return NULL_POINTER;
	}
	uint8_t prev_value;
	if(write_tmp102_pointer_reg(file,SELECT_CONFIG)==ERROR_WRITE){
		return ERROR_WRITE;
	}else if(readI2CByte(file,&prev_value)==ERROR_READ){
		return ERROR_READ;
	}
  prev_value = prev_value | ENABLE_SHUTDOWN;
	if(writeI2CByte(file,&prev_value)==ERROR_WRITE){
		return ERROR_WRITE;
	}
  return SUCCESS;
}

i2c_state set_resolution_tmp102(uint32_t *file, uint8_t mode)
{
	if(file == NULL){
		return NULL_POINTER;
	}
	//set the resolution for tmp 106
	uint8_t prev_value[2];
	if(read_tmp102_config(file,prev_value)==ERROR_READ){
		return ERROR_READ;
	}
  prev_value[1] = prev_value[1] & (!EXTENDED_MODE); //Clear previous mode
	prev_value[1] = prev_value[1] | mode;
	if(writeI2CWord(file,prev_value)==ERROR_WRITE){
		return ERROR_WRITE;
	}
	return SUCCESS;
}

i2c_state set_low_threshold_c(uint32_t *file, float temp){
	if(file == NULL){
		return NULL_POINTER;
	}
	uint8_t extendedMode;
	uint8_t reg_value[2];

	if(temp > 150.0f)
	{
		temp = 150.0f;
	}
	if(temp < -55.0)
	{
		temp = -55.0f;
	}
	if(read_tmp102_config(file,reg_value)==ERROR_READ){
		return ERROR_READ;
	}
	extendedMode = EXTENDED_MODE & reg_value[1];
	temp = temp / 0.0625;

	if(extendedMode){ //13 bit mode
		reg_value[0] = (uint32_t)(temp)>>5;
		reg_value[1] = (uint32_t)(temp)<<3;
	}else{ //12 bit mode
		reg_value[0] = (uint32_t)(temp)>>4;
		reg_value[1] = (uint32_t)(temp)<<4;
	}
	if(write_tmp102_pointer_reg(file,SELECT_TLOW) == ERROR_WRITE){
		return ERROR_WRITE;
	}else if(writeI2CWord(file,reg_value) == ERROR_WRITE){
		return ERROR_WRITE;
	}
	return SUCCESS;
}


i2c_state set_high_threshold_c(uint32_t *file, float temp){
	if(file == NULL){
		return NULL_POINTER;
	}
	uint8_t extendedMode;
	uint8_t reg_value[2];

	if(temp > 150.0f)
	{
		temp = 150.0f;
	}
	if(temp < -55.0)
	{
		temp = -55.0f;
	}
	if(read_tmp102_config(file,reg_value)==ERROR_READ){
		return ERROR_READ;
	}
	extendedMode = EXTENDED_MODE & reg_value[1]; //0x10 - 13 bit mode
	                                             //0x00 - 12 bit mode
	temp = temp / 0.0625;

	if(extendedMode){ //13 bit mode
		reg_value[0] = (uint32_t)(temp)>>5;
		reg_value[1] = (uint32_t)(temp)<<3;
	}else{ //12 bit mode
		reg_value[0] = (uint32_t)(temp)>>4;
		reg_value[1] = (uint32_t)(temp)<<4;
	}
	if(write_tmp102_pointer_reg(file,SELECT_THIGH) == ERROR_WRITE){
		return ERROR_WRITE;
	}else if(writeI2CWord(file,reg_value) == ERROR_WRITE){
		return ERROR_WRITE;
	}
	return SUCCESS;
}


i2c_state set_conversion_rate(uint32_t *file,uint32_t value)
{
	if(file == NULL){
		return NULL_POINTER;
	}
	uint8_t prev_value[2];
	if(write_tmp102_pointer_reg(file,SELECT_CONFIG)==ERROR_WRITE){
		return ERROR_WRITE;
	}else if(readI2CWord(file,prev_value)==ERROR_READ){
		return ERROR_READ;
	}
  prev_value[1] = prev_value[1] & 0xC0;
  prev_value[1] = prev_value[1] | value;
	if(writeI2CWord(file,prev_value)==ERROR_WRITE){
		return ERROR_WRITE;
	}
  return SUCCESS;
}
