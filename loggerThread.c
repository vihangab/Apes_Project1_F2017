#include "msg_queue.h"

void *LoggerThread(void *args)
{
	uint32_t data_to_log;
	uint32_t ret; 
	uint32_t bytes_read;
	LogMsg *logmsg3 = (LogMsg *)malloc(MAX_SEND_BUFFER+10);
	
	//printf("inside logger before its while\n");
	while(1)
	{	
			//printf("inside	while of logger thread \n");
			pthread_mutex_lock(&dataQ_mutex);
			//keep waiting for timer signal 
			printf("before cond wait  logger thread \n");
			pthread_cond_wait(&condvar,&dataQ_mutex);
			printf("after cond wait  logger thread \n");
			pthread_mutex_unlock(&dataQ_mutex);
			
			printf("after cond wait in logger thread\n");

			if(flag_mask_copy & TIMER_EVENT)
			{

				
				mq_getattr(data_queue_handle, &attr);
				while(attr.mq_curmsgs > 0)
				{
					pthread_mutex_lock(&logQ_mutex);
					bytes_read = mq_receive(data_queue_handle, (char *)&logmsg3, MAX_SEND_BUFFER, &data_to_log);
					
				
					if (bytes_read == -1)
					{
						perror("[LoggerThread] Failed to recieve:");
					}	
					else
					{
						printf ("[LoggerThread] Log ID: %d \n", logmsg3->sourceId);
						printf ("[LoggerThread] Timestamp: %s \n", logmsg3->timestamp);
						printf ("[LoggerThread] Log Level: %d \n", logmsg3->level);
						printf ("[LoggerThread] Payload: %s \n", logmsg3->payload);
						
						// Get the attributes
						mq_getattr(data_queue_handle, &attr);
						printf("[LoggerThread] Queue %s currently holds %ld messages\n",QLog,attr.mq_curmsgs);   

							// Clear buffer
						memset(logmsg3, 0, sizeof(LogMsg));
					}
					pthread_mutex_unlock(&logQ_mutex);
				}
			}
			printf("flag mask in logger sigint %d\n",flag_mask);
			printf("flag mask copy in logger sigint %d\n",flag_mask_copy);
			if(flag_mask & SIGINT_EVENT)
			{
				printf("closing and breaking from logger thread after sigint \n");
				//free(logmsg3);
				mq_close(data_queue_handle);
				//pthread_mutex_unlock(&dataQ_mutex);
				
				printf("break from while logger thread \n");
				
				break;
			}
	}
	printf("return from logger thread \n");	
	//pthread_exit(NULL);
}