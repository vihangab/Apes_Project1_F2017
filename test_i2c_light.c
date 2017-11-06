/*
	File name : test_i2c_light.c
	File Description : Source files unit tests for I2c Light
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
#include "apds9301.h"

/*Function - Unit test to check if i2c device is not opened properly*/
void test_i2c_setup_success(void **state)
{
	uint32_t fileDesc;
	i2c_state status = setupI2CDevice(&fileDesc, "/dev/i2c-1",0x39);
	
    assert_int_equal(status,SUCCESS);
}

void test_i2c_setup_fail(void **state)
{
	uint32_t fileDesc;
	i2c_state status = setupI2CDevice(&fileDesc, "/dev/i2c-1",0x39)
	
    assert_int_equal(status,ERROR_OPEN);
}

/*Function - Unit test to check if sensor registers are read properly*/
void test_i2c_write_success(void **state)
{
	i2c_state status;
	uint32_t fileDesc;
	setupI2CDevice(&fileDesc, "/dev/i2c-1",0x39);
	uint8_t controlval_read, controlval_write;
	if(status = read_control(&fileDesc,&controlval_read) == SUCCESS)
	{
		setup_apds9301_registers(&fileDesc);
		status = read_control(&fileDesc,&controlval_write);

		if(controlval_read != controlval_write)
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
