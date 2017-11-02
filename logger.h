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

/***********************************************************