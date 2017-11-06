#include "msg_queue.h"
#include "logger.h"

struct mq_attr attr;
FILE *logFile;

void *LoggerThread(void *args)
{ printf("%s\n","Logger created");
	uint32_t data_to_log;
	uint32_t ret;
	uint32_t bytes_read;
	LogMsg *logmsg3;

	printf("Log file create status - %d,\n", create_log_file(&logFile, "some"));

	if(create_log_struct(&logmsg3)!=DONE){
		printf("%s\n","Error creating struct");
	}
  logmsg3->requestID=100;
	//printf("inside logger before its while\n");
	while(1)
	{
		//printf("inside	while of logger thread \n");
			pthread_mutex_lock(&dataQ_mutex);
			//keep waiting for timer signal
			pthread_cond_wait(&condvar,&dataQ_mutex);
			pthread_mutex_unlock(&dataQ_mutex);

			if(flag_mask & SIGINT_EVENT)
			{
				printf("%s\n","Logger got SIGINT");
				mq_close(data_queue_handle);
				//pthread_mutex_unlock(&dataQ_mutex);
				//free(logmsg3);
				fclose(logFile);
				break;
			}

			if((flag_mask_copy & TIMER_EVENT) || (flag_mask_copy & ASYNC_EVENT))
			{

				mq_getattr(data_queue_handle, &attr);

				while(attr.mq_curmsgs > 0)
				{
					pthread_mutex_lock(&logQ_mutex);
					bytes_read = mq_receive(data_queue_handle, (char *)&logmsg3, MAX_SEND_BUFFER+1, &data_to_log);

					if (bytes_read == -1)
					{
						perror("[LoggerThread] Failed to recieve:");
					}
					else
					{
						printf("[LoggerThread] Queue %s currently holds %ld messages\n",QLog,attr.mq_curmsgs);
						mq_getattr(data_queue_handle, &attr);
						if(logmsg3->requestID == LOG_DATA)
						{
							printf("Logging status -%d\n",log_item(logFile,logmsg3) );
							printf ("[LoggerThread] source ID: %d \n", logmsg3->sourceId);
							printf ("[LoggerThread] Log Level: %d \n", logmsg3->level);
							printf ("[LoggerThread] Payload: %s \n\n", logmsg3->payload);
							// Get the attributes
							printf ("[LoggerThread] Timestamp: %s \n", ctime(&logmsg3->timestamp));
					  }// Clear buffer

						//memset(logmsg3, (uint8_t)'\0', sizeof(LogMsg));
						//memset(logmsg3->payload,0,1024);
					}
					pthread_mutex_unlock(&logQ_mutex);
				}
				//pthread_mutex_unlock(&logQ_mutex);
			}

	}
  printf("%s\n","Outside Logger Thread");
	pthread_exit(NULL);
}
