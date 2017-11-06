/*
	File name : test_i2c_temp.c
	File Description : Source files unit tests for I2c Temperature
	File Author: Vihanga Bare and Virag Gada
	Assignment Date : 11/5/2017
	Tools used : Ubuntu 16.04.2, GCC version 5.4.0, cmocka, cmake	
*/

#include <math.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdint.h>
#include <stdio.h>

#include "i2c_lib.h"
#include "tmp102.h"

/*Function - Unit test to check if i2c device is not opened properly*/
void test_i2c_setup_success(void **state)
{
	i2c_state status = setupI2CDevice(&fileDesc, "/dev/i2c-1",0x48));
	
    assert_int_equal(status,SUCCESS);
}

void test_i2c_setup_fail(void **state)
{
	i2c_state status = setupI2CDevice(&fileDesc, "/dev/i2c-1",0x48));
	
    assert_int_equal(status,ERROR_OPEN);
}

/*Function - Unit test to check if sensor registers are read properly*/
void test_i2c_write_success(void **state)
{
	i2c_state status;
	setupI2CDevice(&fileDesc, "/dev/i2c-1",0x48)
	uint8_t reg_val_read[2], reg_val_write[2];
	uint16_t var1,var2;
	if(status = read_tmp102_config(&fileDesc,reg_val_read) == SUCCESS)
	{
		setup_tmp102_registers(&fileDesc);
		status = read_tmp102_config(&fileDesc,reg_val_write);
		var1 = reg_val_read[0]<<8 | reg_val_read[1]; 
		var2 = reg_val_write[0]<<8 | reg_val_write[1];
		if(var1 != var2)
			status = SUCCESS;
	}
	else
		status = ERROR;
	
    assert_int_equal(status,SUCCESS);
}
int main(int argc, char **argv)
{
  const struct CMUnitTest tests[] = 
  {
	//unit test framework struct	
	cmocka_unit_test(test_i2c_setup_success),
	cmocka_unit_test(test_i2c_setup_fail),
	cmocka_unit_test(test_i2c_write_success),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
