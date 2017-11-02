/*************************************************************************
* Authors : Vishal Vishnani, Virag Gada
* Date : 03/12/2017
*
* File : logger.c
* Description : Source file for wrapper logger functions
*								-logger_state create_log_file(FILE *fptr, char *fileName)
*								-logger_state log_string(FILE *fptr, uint8_t * data_TX)
*								-logger_state log_integer(FILE *fptr,uint32_t val)
* 							-logger_state log_float(FILE *fptr,double val)
***************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "logger.h"

/* Function to create a log file */
logger_state create_log_file(FILE *fptr, char *fileName){
  if((fptr=fopen(fileName,"w+"))==NULL){
		return OPEN_ERROR;
	}
  return DONE;
}

/* This function is used to take a string and then write it to a log file */
logger_state log_string(FILE *fptr, uint8_t * data_TX){
	size_t length=strlen(data_TX);
	printf("Length of string - %ld\n",length);
	if(fwrite(data_TX,length,1,fptr)<0){
		return WRITE_ERROR;
	}
  return DONE;
}


/* This function is used to convert 