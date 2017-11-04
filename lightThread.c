#include "msg_queue.h"

void *LightThread(void *args)
{
	char buffer[MAX_SEND_BUFFER];
	unsigned int msgprio = 2;
	uint32_t bytes_sent=0;

	LogMsg *logmsg2 = (LogMsg *)malloc(sizeof(LogMsg));
	memset(logmsg2, 0, sizeof(logmsg2));

	//printf("inside logger before its while\n");

	while(1)
	{
		printf("inside while of light thread \n");
		
		//generate timestamp string
		time_t curr_time = time(NULL);
		logmsg2->timestamp = ctime(&curr_time);
		logmsg2->logId = 1;
		logmsg2->level = WARNING;

		snprintf (buffer, MAX_SEND_BUFFER, "Message from Light thread");
		
		logmsg2->payload = buffer;
		
		pthread_mutex_lock(&dataQ_mutex);
		
		//keep waiting for timer signal 
		printf("before cond wait  light thread \n");
		pthread_cond_wait(&condvar,&dataQ_mutex);
		printf("after cond wait  light thread \n");
		if(flag_mask_copy == TIMER_EVENT)
		{
			
			if ((bytes_sent = mq_send (data_queue_handle,(const char*)&logmsg2, sizeof(logmsg2), msgprio)) != 0) //can be changed later to light queue handle
			{
				perror ("[LightThread] Sending:");
			}
			pthread_mutex_unlock(&dataQ_mutex);
		}
		
		if(flag_mask_copy == SIGINT_EVENT)
		{
			printf("closing and breaking from light thread after sigint \n");
			pthread_mutex_unlock(&dataQ_mutex);
			free(logmsg2);
			printf("break from while light thread \n");
			break;
		}
	}
	printf("return light thread \n");
}