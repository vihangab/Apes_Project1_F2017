#include <stdio.h>
#include <stdlib.h>
#include "i2c_lib.h"
#include "tmp106.h"

i2c_state write_tmp106_registers()
{
	// set THigh register threshold limits
	// set TLow register threshold limits
	
	//call to write_tmp106_config()
	//call to write_tmp106_pointer_reg()
}

i2c_state read_tmp106_registers()
{
	//call to read_tmp106_config()
	
	// read THigh register
	// read TLow register
	
	// this is required as we need to read T LOW and T High and perform necessary calculations for *C, *F and *K values using these values
}

i2c_state write_tmp106_config()
{

}

i2c_state read_tmp106_config()
{

}
i2c_state write_tmp106_pointer_reg()
{
	
}

i2c_state read_tmp106_Temp_reg()
{
	//read from Temperature register which is read--only
}

i2c_state pwr_up_sequence_tmp106()
{
	//call to i2c_state write_tmp106_registers() to write appropriate registers 
}

i2c_state pwr_down_sequence_tmp106()
{
	//call to i2c_state write_tmp106_registers() to write appropriate registers 
}
i2c_state set_resolution_tmp106()
{
	//set the resolution for tmp 106
}


