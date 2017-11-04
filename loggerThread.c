#include "msg_queue.h"

void *LoggerThread(void *args)
{
	uint32_t data_to_log;
	uint32_t ret; 
	uint32_t bytes_read;
	LogMsg *logmsg3 = (LogMsg *)malloc(sizeof(LogMsg));
	
	//printf("inside logger before its while\n");
	while(1)
	{	
		//printf("inside	while of logger thread \n");
			pthread_mutex_lock(&dataQ_mutex);
			//keep waiting for timer signal 
			printf("before cond wait  logger thread \n");
			pthread_cond_wait(&condvar,&dataQ_mutex);
			printf("after cond wait  logger thread \n");
			
			printf("after cond wait in logger thread\n");

			if(flag_mask_copy == TIMER_EVENT)
			{

				mq_getattr(data_queue_handle, &attr);
				while(attr.mq_curmsgs > 0)
				{
					bytes_read = mq_receive(data_queue_handle, (char *)&logmsg3, MAX_SEND_BUFFER, &data_to_log);
				
					if (bytes_read == -1)
					{
						perror("[LoggerThread] Failed to recieve:");
					}	
					else
					{
						printf ("[LoggerThread] Log ID: %d \n", logmsg3->logId);
						printf ("[LoggerThread] Timestamp: %s \n", logmsg3->timestamp);
						printf ("[LoggerThread] Log Level: %d \n", logmsg3->level);
						printf ("[LoggerThread] Payload: %s \n", logmsg3->payload);
						
						// Get the attributes
						mq_getattr(data_queue_handle, &attr);
						printf("[LoggerThread] Queue %s currently holds %ld messages\n",QTemp,attr.mq_curmsgs);   

							// Clear buffer
						memset(logmsg3, 0, sizeof(logmsg3));
					}
				}
				pthread_mutex_unlock(&dataQ_mutex);	
			}
			if(flag_mask_copy == SIGINT_EVENT)
			{
				printf("closing and breaking from logger thread after sigint \n");
				mq_close(data_queue_handle);
				pthread_mutex_unlock(&dataQ_mutex);
				printf("break from while logger thread \n");
				//free(logmsg3);
				break;
			}
	}
	printf("return from logger thread \n");	
}