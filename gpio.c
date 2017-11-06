/*****************************************************
Name - led.c
Author - Virag Gada and Vihanga Bare
Description - Source file for our LED library
*****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "gpio.h"

gpioState pinSet(char * devAddr){
  FILE *fHandle = NULL;
  if((fHandle = fopen(devAddr,"w"))==NULL){
    return ERR_OPEN;
  }else if(fwrite("1",1,1,fHandle)<0){
    return ERR_WRITE;
  }else if(fclose(fHandle)!=0){
    return ERR_CLOSE;
  }
  return SET;
}

gpioState pinReset(char * devAddr){
  FILE *fHandle = NULL;
  if((fHandle = fopen(devAddr,"w"))==NULL){
    return ERR_OPEN;
  }else if(fwrite("0",1,1,fHandle)<0){
    return ERR_WRITE;
  }else if(fclose(fHandle)!=0){
    return ERR_CLOSE;
  }
  return RESET;
}
