#include "msg_queue.h"

void *TempThread(void *args)
{

	char buffer[MAX_SEND_BUFFER];
	LogMsg *logmsg1 = (LogMsg *)malloc(sizeof(LogMsg));
	memset(logmsg1, 0, sizeof(logmsg1));
	unsigned int msgprio = 1;
	uint32_t bytes_sent=0;
	//printf("inside logger before its while\n");

	while(1)
	{
		//printf("inside	while of temp thread \n");
		//generate timestamp string
		time_t curr_time = time(NULL);
		logmsg1->timestamp = ctime(&curr_time);
		logmsg1->logId = 0;
		logmsg1->level = WARNING;
		
		snprintf (buffer, MAX_SEND_BUFFER, "Message from Temp thread");
		
		
		logmsg1->payload = buffer;
		pthread_mutex_lock(&dataQ_mutex);
		
		
		//keep waiting for timer signal 
		printf("before cond wait  temp thread \n");
		pthread_cond_wait(&condvar,&dataQ_mutex);
		printf("after cond wait  logger thread \n");
		
		
		if(flag_mask_copy == TIMER_EVENT)
		{
			
			if ((bytes_sent = mq_send (data_queue_handle,(const char*)&logmsg1, sizeof(logmsg1), msgprio)) != 0)
			{
				perror ("[TempThread] Sending:");
			}
			pthread_mutex_unlock(&dataQ_mutex);
			
		}
		
		if(flag_mask_copy == SIGINT_EVENT)
		{
			printf("closing and breaking from temp thread after sigint \n");
			mq_close (data_queue_handle);
			pthread_mutex_unlock(&dataQ_mutex);
			free(logmsg1);
			printf("break from while temp thread \n");
			break;
		}
	} 
	printf("return from temp thread \n");
}