#include "msg_queue.h"
#include "apds9301.h"
#include "i2c_lib.h"

static struct mq_attr attr;

char *stateNames[]={"LIGHT","DARK"};

i2c_state setupApdsSensor(uint32_t * fd, char *bus, uint8_t devAddr){
	i2c_state state;
	if((state=setupI2CDevice(fd,bus,devAddr))!=SUCCESS){
		return state;
	}else if((state=setup_apds9301_registers(fd))!=SUCCESS){
		return state;
	}
	return SUCCESS;
}

void *LightThread(void *args)
{ printf("%s\n","Light created");

  i2c_state sensorStatus=SUCCESS;
	double luxValue;
	uint32_t fileDesc;
  apds_state lightState,prevState=LIGHT;

	unsigned int msgprio = 0;
	uint32_t bytes_sent = 0;
	uint32_t bytes_read = 0;
  uint32_t data_to_log;
	//time_t curr_time;
	LogMsg *logmsg2, *logmsg20, *logmsg200;
	if(create_log_struct(&logmsg2)!=DONE){
		printf("%s\n","Error creating struct");
	}
	if(create_log_struct(&logmsg20)!=DONE){
		printf("%s\n","Error creating struct");
	}
	if(create_log_struct(&logmsg200)!=DONE){
		printf("%s\n","Error creating struct");
	}


	logmsg200->sourceId = LIGHT_TASK;
	logmsg200->level = INITIALIZATION;
	logmsg200->requestID = LOG_DATA;
	logmsg200->timestamp = time(NULL);

	sensorStatus = setupApdsSensor(&fileDesc,"/dev/i2c-2",0x39);

	if(sensorStatus == SUCCESS){
    sprintf(logmsg200->payload,"%s","APDS-9301 sensor initialized");
	}else{
		sprintf(logmsg200->payload,"%s","APDS-9301 sensor initialization failed");
	}
	pthread_mutex_lock(&logQ_mutex);
	if ((bytes_sent = mq_send (data_queue_handle,(const char*)&logmsg200, sizeof(LogMsg), 1)) != 0) //can be changed later to light queue handle
	{
		printf("[LightThread] Error Sending");
	}
	pthread_mutex_unlock(&logQ_mutex);

	usleep(100);

	flag_mask |= ASYNC_EVENT;
	int32_t retval = pthread_cond_broadcast(&condvar);

	if(retval != 0)
	{
			printf("condition signal failed, error code - %d\n", retval);
	}
	while(1)
	{
		//generate timestamp string
    pthread_mutex_lock(&dataQ_mutex);
		//keep waiting for timer signal
		pthread_cond_wait(&condvar,&dataQ_mutex);
    pthread_mutex_unlock(&dataQ_mutex);

		if(flag_mask & SIGINT_EVENT)
		{
			printf("%s\n","Light got SIGINT");
			free(logmsg20);
			free(logmsg2);
			free(logmsg200);
			close(fileDesc);
			break;
		}

		if(flag_mask_copy & TIMER_EVENT)
		{
			logmsg2->sourceId = LIGHT_TASK;
			logmsg2->level = INFO;
			logmsg2->requestID = LOG_DATA;
			logmsg2->timestamp = time(NULL);
			sensorStatus = read_lux_values(&fileDesc,&luxValue);
			if(sensorStatus == SUCCESS){
			  sprintf(logmsg2->payload,"Luminosity Value - %f",luxValue);
			}else{
				sprintf(logmsg2->payload,"%s","Error reading luminosity value from the sensor");
			}
		  pthread_mutex_lock(&logQ_mutex);
			if ((bytes_sent = mq_send (data_queue_handle,(const char*)&logmsg2, sizeof(LogMsg), 2)) != 0) //can be changed later to light queue handle
			{
				perror ("[LightThread] Sending:");
			}
      pthread_mutex_unlock(&logQ_mutex);

      /* Check for change in light state */
      sensorStatus = read_light_state(&fileDesc,&lightState);
      if(prevState != lightState){
				prevState = lightState;
        logmsg20->level = WARNING;
				logmsg20->timestamp = time(NULL);
				logmsg20->sourceId = LIGHT_TASK;
				logmsg20->requestID = LOG_DATA;
				sprintf(logmsg20->payload,"Light state changed to - %s",stateNames[lightState]);
				pthread_mutex_lock(&logQ_mutex);
				if ((bytes_sent = mq_send (data_queue_handle,(const char*)&logmsg20, sizeof(LogMsg), 3)) != 0) //can be changed later to light queue handle
				{
					perror ("[LightThread] Sending:");
				}
				pthread_mutex_unlock(&logQ_mutex);
			}

			/* Send heartbeat to Main task */

			logmsg200->sourceId = LIGHT_TASK;
			logmsg200->level = HEART_BEAT;
			logmsg200->requestID - HEARTBEAT;
			logmsg200->timestamp = time(NULL);
			sprintf(logmsg200->payload,"%s","Light Thread - Alive");
			pthread_mutex_lock(&mainQ_mutex);
			if ((bytes_sent = mq_send (main_queue_handle,(const char*)&logmsg200, sizeof(LogMsg), 4)) != 0) //can be changed later to light queue handle
			{
				perror ("[LightThread] Sending:");
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
      mq_getattr(lightreq_queue_handle, &attr);
      while(attr.mq_curmsgs > 0)
			{
				pthread_mutex_lock(&lightQ_mutex);
				bytes_read = mq_receive(lightreq_queue_handle, (char *)&logmsg2, MAX_SEND_BUFFER, &data_to_log);
				pthread_mutex_unlock(&lightQ_mutex);
				if (bytes_read == -1)
				{
					printf("[LightThread] Failed to recieve\n");
				}
				else
				{
					printf ("[LightThread] Source: %d ", logmsg20->sourceId);
					printf ("Log Level: %d ", logmsg20->level);
					printf ("Payload: %s \n", logmsg20->payload);
					// Get the attributes
					mq_getattr(lightreq_queue_handle, &attr);
					printf("[LightThread] Queue %s currently holds %ld messages\n",QLight_Request,attr.mq_curmsgs);
					printf ("Timestamp: %s ", ctime(&logmsg20->timestamp));
					switch(logmsg20->requestID)
					{
            case GET_LUX:
									logmsg20->sourceId = LIGHT_TASK;
									logmsg20->level = INFO;
									logmsg20->requestID = LOG_DATA;
									logmsg20->timestamp = time(NULL);
									logmsg20->payload;// = buffer;
									sensorStatus = read_lux_values(&fileDesc,&luxValue);
									if(sensorStatus == SUCCESS){
										sprintf(logmsg20->payload,"Luminosity Value - %f",luxValue);
									}else{
										sprintf(logmsg20->payload,"%s","Error reading luminosity value from the sensor");
									}
									pthread_mutex_lock(&logQ_mutex);
									if ((bytes_sent = mq_send (data_queue_handle,(const char*)&logmsg20, sizeof(LogMsg), 5)) != 0) //can be changed later to light queue handle
									{
										perror ("[LightThread] Sending:");
									}
									pthread_mutex_unlock(&logQ_mutex);
									break;

						case GET_LIGHT_STATE:
									logmsg20->sourceId = LIGHT_TASK;
									logmsg20->level = INFO;
									logmsg20->requestID = LOG_DATA;
									logmsg20->timestamp = time(NULL);
									sensorStatus = read_light_state(&fileDesc,&lightState);;
									if(sensorStatus == SUCCESS){
										sprintf(logmsg20->payload,"Light State - %s",stateNames[lightState]);
									}else{
										sprintf(logmsg20->payload,"%s","Error determining light state");
									}
									pthread_mutex_lock(&logQ_mutex);
									if ((bytes_sent = mq_send (data_queue_handle,(const char*)&logmsg20, sizeof(LogMsg), 6)) != 0) //can be changed later to light queue handle
									{
										perror ("[LightThread] Sending:");
									}
									pthread_mutex_unlock(&logQ_mutex);
									break;

						default:
						     break;
					}
						// Clear buffer

				}
				flag_mask_copy |= ASYNC_EVENT;
			}
		}
	//sleep(0.75);
	memset(logmsg2, (uint8_t)'\0', sizeof(LogMsg));
	memset(logmsg20, (uint8_t)'\0', sizeof(LogMsg));
	memset(logmsg200, (uint8_t)'\0', sizeof(LogMsg));
	}
  printf("%s\n","Outside Light Thread");
	pthread_exit(NULL);
}
