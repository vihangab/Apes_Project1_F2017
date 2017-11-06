/****************************************************
Name - tmp102.h
Author - Virag Gada and Vihanga Bare
Description - Header file for out TMP 102 library
*****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "i2c_lib.h"
#include "apds9301.h"

// Function to write all registers
i2c_state setup_apds9301_registers(uint32_t * file)
{
  if(file == NULL){
    return NULL_POINTER;
  }
  //Calls control and timing register
  i2c_state status;
  if((status = write_control(file,POWER_ON)) == ERROR_WRITE){
    return status;
  }else if((status = config_integration(file,INTEG_TIME_101)) == ERROR_WRITE){
    return status;
  }else {
    uint8_t data[2]={0};
    data[0] = CMDbit | TLL_ADDR;
    data[1] = TLL;
    if((status = writeI2CByte(file,data)) == ERROR_WRITE){
      return ERROR_WRITE;
    }
    data[0] = CMDbit | TLH_ADDR;
    data[1] = TLH;
    if((status = writeI2CByte(file,data)) == ERROR_WRITE){
      return ERROR_WRITE;
    }
    data[0] = CMDbit | THL_ADDR;
    data[1] = THL;
    if((status = writeI2CByte(file,data)) == ERROR_WRITE){
      return ERROR_WRITE;
    }
    data[0] = CMDbit | THH_ADDR;
    data[1] = THH;
    if((status = writeI2CByte(file,data)) == ERROR_WRITE){
      return ERROR_WRITE;
    }
  }
  return SUCCESS;
}

// Function to write to control register
i2c_state write_control(uint32_t * file, uint8_t powerState)
{
  if(file == NULL){
    return NULL_POINTER;
  }
  uint8_t data[2]={0};
  data[0] = CTRL_ADDR | CMDbit;
  data[1] = powerState;
  return writeI2CWord(file,data);
}

// Function to read to control register
i2c_state read_control(uint32_t * file, uint8_t * powerState)
{
  if(file == NULL){
    return NULL_POINTER;
  }
  uint8_t addr = CTRL_ADDR | CMDbit;
  if(writeI2CByte(file,&addr)==ERROR_WRITE){
    return ERROR_WRITE;
  }else{
    return readI2CByte(file,powerState);
  }
  return SUCCESS;
}

// Function to set integration times
i2c_state config_integration(uint32_t * file, uint8_t integrationPeriod)
{
  if(file == NULL){
    return NULL_POINTER;
  }
  uint8_t data[2]={0};
  data[0] = TIMING_ADDR | CMDbit;
  data[1] = integrationPeriod;
  return writeI2CWord(file,data);
}

// Function to enable the APDS sensor interrupt
i2c_state enable_apds_interrupt(uint32_t * file)
{
  if(file == NULL){
    return NULL_POINTER;
  }
  uint8_t data[2]={0};
  data[0] = INT_ADDR | CMDbit;
  data[1] = INTERRUPT_ENABLE | PERSIST;
  return writeI2CWord(file,data);
}

//Function to disable APDS sensor interrupts
i2c_state disable_apds_interrupt(uint32_t * file)
{
  if(file == NULL){
    return NULL_POINTER;
  }
  uint8_t data[2]={0};
  data[0] = INT_ADDR | CMDbit;
  data[1] = INTERRUPT_DISABLE;
  return writeI2CWord(file,data);
}

//Function to read the ID register
i2c_state read_ID_reg(uint32_t * file,uint8_t *id_val)
{
  if(file == NULL){
    return NULL_POINTER;
  }
  uint8_t addr = ID_ADDR | CMDbit;
  if(writeI2CByte(file,&addr)==ERROR_WRITE){
    return ERROR_WRITE;
  }else
    return readI2CByte(file,id_val);
  return SUCCESS;
}

// Function to read LUX values
i2c_state read_lux_values(uint32_t * file, double * lux_value)
{
  if(file == NULL){
    return NULL_POINTER;
  }
  uint8_t ch0_data[2] = {0}, ch1_data[2] = {0};
  double ch0, ch1, ratio;
  uint8_t addr_0 =  ADC0_DLOW_ADDR | CMDbit | Word_mode;
  uint8_t addr_1;
  //addr_1[0] = 0x39;
  addr_1 =  ADC1_DLOW_ADDR | CMDbit | Word_mode;
  if(writeI2CByte(file,&addr_0)==ERROR_WRITE){
    return ERROR_WRITE;
  }else if(readI2CWord(file,ch0_data)==ERROR_READ){
    return ERROR_READ;
  }else if(writeI2CByte(file, &addr_1)==ERROR_WRITE){
    return ERROR_WRITE;
  }else if(readI2CWord(file,ch1_data)==ERROR_READ){
    return ERROR_READ;
  }
  ch0 = ch0_data[1]<<8 | ch0_data[0];
  ch1 = ch1_data[1]<<8 | ch1_data[0];
  //printf("CH0 - %f, CH1 - %f\n", ch0,ch1);
  ratio = ch1/ch0;
  //printf("Ratio - %f\n", ratio);
  if((ratio)>0 && (ratio<=0.5)){
    *lux_value = (0.0304*ch0)-(0.062*ch0*pow(ratio,1.4));
  }else if(ratio<=0.61){
    *lux_value = (0.0224*ch0)-(0.031*ch1);
  }else if(ratio<=0.80){
    *lux_value = (0.0128*ch0)-(0.0153*ch1);
  }else if(ratio<=1.3){
    *lux_value = (0.00146*ch0)-(0.00112*ch1);
  }else {
    *lux_value = 0;
  }
  return SUCCESS;
}

//Function to find light state based on luc values
i2c_state read_light_state(uint32_t * file, apds_state *light_state)
{
  if(file == NULL){
    return NULL_POINTER;
  }
  double lux_value;
  i2c_state state;
  if((state = read_lux_values(file,&lux_value))!=SUCCESS){
    return ERROR_VALUE;
  }
  if(lux_value > LIGHT_STATE_THRESHOLD){
    *light_state = LIGHT;
  }else{
    *light_state = DARK;
  }
  return SUCCESS;
}
