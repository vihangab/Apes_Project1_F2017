#include "msg_queue.h"
#include "i2c_lib.h"
#include "tmp102.h"

struct mq_attr attr;

i2c_state setupTmpSensor(uint32_t * fd, char *bus, uint8_t devAddr){
	i2c_state state = SUCCESS;
	if((state=setupI2CDevice(fd,bus,devAddr))!=SUCCESS){
		return state;
	}else if((state=setup_tmp102_registers(fd))!=SUCCESS){
		return state;
	}
	return SUCCESS;
}

void *TempThread(void *args)
{ printf("%s\n","Temp created");
  i2c_state sensorStatus = SUCCESS;
	uint8_t reg_val[2];
	double tempVal=0;
	uint32_t fileDesc;

	uint32_t bytes_read = 0;
	uint32_t bytes_sent = 0;
	uint32_t data_to_log;
	unsigned int msgprio = 0;

	LogMsg *logmsg1,*logmsg10,*logmsg100;
	if(create_log_struct(&logmsg1)!=DONE){
		printf("%s\n","Error creating struct");
	}
	if(create_log_struct(&logmsg10)!=DONE){
		printf("%s\n","Error creating struct");
	}

	if(create_log_struct(&logmsg100)!=DONE){
		printf("%s\n","Error creating struct");
	}

	logmsg100->sourceId = TEMP_TASK;
	logmsg100->level = INITIALIZATION;
	logmsg100->requestID = LOG_DATA;
	logmsg100->timestamp = time(NULL);
	sensorStatus = setupTmpSensor(&fileDesc,"/dev/i2c-1",0x48);
	printf("Temp sensor state - %d\n", sensorStatus);
	if(sensorStatus == SUCCESS){
		sprintf(logmsg100->payload,"%s","TMP-102 sensor initialized");
		printf("%s\n","This is happening");
	}else{
		sprintf(logmsg100->payload,"%s","TMP-102 sensor initialization failed");
	}
	pthread_mutex_lock(&logQ_mutex);
	if ((bytes_sent = mq_send (data_queue_handle,(const char*)&logmsg100, sizeof(LogMsg), 9)) != 0) //can be changed later to light queue handle
	{
		perror ("[TempThread] Sending:");
	}
	printf("Bytes sent  - %d\n", bytes_sent);
	pthread_mutex_unlock(&logQ_mutex);
	usleep(100);
	//pthread_mutex_lock(&dataQ_mutex);
	flag_mask |= ASYNC_EVENT;
	int32_t retval = pthread_cond_broadcast(&condvar);
	//pthread_mutex_unlock(&dataQ_mutex);
	if(retval != 0)
	{
			printf("condition signal failed, error code - %d\n", retval);
	}
  //sleep(1);
	while(1)
	{
		//printf("inside	while of temp thread \n");
		pthread_mutex_lock (&dataQ_mutex);
		//keep waiting for timer signal
		pthread_cond_wait(&condvar,&dataQ_mutex);
    pthread_mutex_unlock(&dataQ_mutex);

		if(flag_mask & SIGINT_EVENT)
		{
			printf("%s\n","Temp got SIGINT");
			free(logmsg1);
			//free(logmsg10);
			break;
		}

		if(flag_mask_copy & TIMER_EVENT)
		{
			//generate timestamp string
			logmsg1->sourceId = TEMP_TASK;
			logmsg1->level = WARNING;
      logmsg1->requestID = LOG_DATA;
      logmsg1->timestamp = time(NULL);
			sensorStatus = read_tmp102_temp_reg_c(&fileDesc,&tempVal);
			if(sensorStatus == SUCCESS){
			  sprintf(logmsg1->payload,"Temperature Value in timer wala'C- %f",tempVal);
			}else{
				sprintf(logmsg1->payload,"%s","Error reading temperature value from the sensor");
			}
			pthread_mutex_lock(&logQ_mutex);
			if ((bytes_sent = mq_send (data_queue_handle,(const char*)&logmsg1,sizeof(LogMsg), 10)) != 0)
			{
				perror ("[TempThread] Sending:");
			}
			pthread_mutex_unlock(&logQ_mutex);

			//memset(logmsg1, (uint8_t)'\0', sizeof(logmsg1));

			//sleep(0.75);
			logmsg10->sourceId = TEMP_TASK;
			logmsg10->level = HEART_BEAT;
			logmsg10->requestID = HEARTBEAT;
			logmsg10->timestamp = time(NULL);

			pthread_mutex_lock(&mainQ_mutex);
			if ((bytes_sent = mq_send (main_queue_handle,(const char*)&logmsg10,sizeof(LogMsg), 11)) != 0) //can be changed later to light queue handle
			{
				perror ("TempThread] Sending:");
			}
			pthread_mutex_unlock (&mainQ_mutex);

			usleep(100);
		  //pthread_mutex_lock(&dataQ_mutex);
      flag_mask |= ASYNC_EVENT;
			retval = pthread_cond_broadcast(&condvar);
			//pthread_mutex_unlock(&dataQ_mutex);
			if(retval != 0)
			{
					printf("condition signal failed, error code - %d\n", retval);
			}
		}

		if(flag_mask_copy & ASYNC_EVENT)
		{
      mq_getattr(tempreq_queue_handle, &attr);
      while(attr.mq_curmsgs > 0)
			{
				pthread_mutex_lock(&tempQ_mutex);
				bytes_read = mq_receive(tempreq_queue_handle, (char *)&logmsg10, MAX_SEND_BUFFER, &data_to_log);
				pthread_mutex_unlock(&tempQ_mutex);
				if (bytes_read == -1)
				{
					printf("[TempThread] Failed to recieve\n");
				}
				else
				{
					printf ("[TempThread] Source: %d ", logmsg10->sourceId);
					//printf ("Log Level: %d ", logmsg10->level);
					//printf ("Payload: %s \n", logmsg10->payload);
					// Get the attributes
					mq_getattr(tempreq_queue_handle, &attr);
					printf("[TempThread] Queue %s currently holds %ld messages\n",QTemp_Request,attr.mq_curmsgs);
					printf ("Timestamp: %s ", ctime(&logmsg10->timestamp));
						// Clear buffer
          switch(logmsg10->requestID)
					{
            case GET_TEMP_C:

								logmsg100->sourceId = TEMP_TASK;
								printf("%s\n","ASYNC request has come for temp in celcius");
								logmsg100->level = INFO;
								logmsg100->requestID = LOG_DATA;
								logmsg100->timestamp = time(NULL);
								sensorStatus = read_tmp102_temp_reg_c(&fileDesc,&tempVal);
								if(sensorStatus == SUCCESS){
									sprintf(logmsg100->payload,"Temperature Value in async wala 'C- %f",tempVal);
								}else{
									sprintf(logmsg100->payload,"%s","Error reading temperature value from the sensor");
								}
								pthread_mutex_lock(&logQ_mutex);
								if ((bytes_sent = mq_send (data_queue_handle,(const char*)&logmsg100,sizeof(LogMsg), 12)) != 0)
								{
									perror ("[TempThread] Sending:");
								}
								pthread_mutex_unlock(&logQ_mutex);
						    break;

						case GET_TEMP_K:

								logmsg100->sourceId = TEMP_TASK;
								printf("%s\n","ASYNC request has come for temp in kelvin");
								logmsg100->level = INFO;
								logmsg100->requestID = LOG_DATA;
								logmsg100->timestamp = time(NULL);
								sensorStatus = read_tmp102_temp_reg_k(&fileDesc,&tempVal);
								if(sensorStatus == SUCCESS){
									sprintf(logmsg100->payload,"Temperature Value in 'K- %f",tempVal);
								}else{
									sprintf(logmsg100->payload,"%s","Error reading temperature value from the sensor");
								}
								pthread_mutex_lock(&logQ_mutex);
								if ((bytes_sent = mq_send (data_queue_handle,(const char*)&logmsg100,sizeof(LogMsg), 13)) != 0)
								{
									perror ("[TempThread] Sending:");
								}
								pthread_mutex_unlock(&logQ_mutex);
						    break;

						case GET_TEMP_F:

								logmsg100->sourceId = TEMP_TASK;
								printf("%s\n","ASYNC request has come for temp in fahrenheit");
								logmsg100->level = INFO;
								logmsg100->requestID = LOG_DATA;
								logmsg100->timestamp = time(NULL);
								sensorStatus = read_tmp102_temp_reg_f(&fileDesc,&tempVal);
								if(sensorStatus == SUCCESS){
									sprintf(logmsg100->payload,"Temperature Value in 'F- %f",tempVal);
								}else{
									sprintf(logmsg100->payload,"%s","Error reading temperature value from the sensor");
								}
								pthread_mutex_lock(&logQ_mutex);
								if ((bytes_sent = mq_send (data_queue_handle,(const char*)&logmsg100,sizeof(LogMsg), 14)) != 0)
								{
									perror ("[TempThread] Sending:");
								}
								pthread_mutex_unlock(&logQ_mutex);
								break;

						default:
						    break;
					}

				}
				flag_mask_copy ^= ASYNC_EVENT;
			}
		}
	//sleep(1);
	memset(logmsg1, (uint8_t)'\0', sizeof(LogMsg));
	memset(logmsg10, (uint8_t)'\0', sizeof(LogMsg));
	memset(logmsg100, (uint8_t)'\0', sizeof(LogMsg));
	}

	printf("%s\n","Outside Temperature Thread");
	pthread_exit(NULL);
}
