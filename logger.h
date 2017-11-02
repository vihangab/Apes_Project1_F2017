/*************************************************************************
* Authors : Vishal Vishnani, Virag Gada
* Date : 03/12/2017
*
* File : logger.h
* Description : Header file for wrapper logger functions
*               -void log_data(uint8_t * data_TX,uint8_t length_TX)
*               -void log_flush(void)
*               -void log_data_BBB(uint8_t * data_TX,uint8_t length_TX)
*		-void log_string(uint8_t * data_TX)
*               -void log_string_BBB(uint8_t * data_TX)
*		-void log_integer(uint32_t test3,uint8_t length)
*		-void log_integer_BBB(uint32_t test3,uint8_t length)
***************************************************************************/
#ifndef LOGGER_H_
#define LOGGER_H_

typedef enum{
  DONE,
  OPEN_ERROR,
  WRITE_ERROR
}logger_state;


logger_state create_log_file(FILE *fptr, char *fileName);

/****************************************************************************
* This function is used to add string to circular buffer and then display
* using printf for BBB and host machine
* uint8_t * data_TX - This pointer points to the string to be transmitted
****************************************************************************/
logger_state log_string(FILE *fptr, uint8_t * data_TX);


/***************************************************************************
* This function is used to convert integer to ascii and then display
* using printf for BBB and host machine
* uint32_t test3 - This variable has the integer variable which is to be
 convert to ascii
* uint8_t length - This variable gives the length to be converted
**************************************************************************/
logger_state log_integer(FILE *fptr,uint32_t val);


logger_state log_float(FILE *fptr,double val);

#endif /* LOGGER_H_ */
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   