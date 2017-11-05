#include "msg_queue.h"

void *LightThread(void *args)
{
	char buffer[MAX_SEND_BUFFER];
	uint32_t bytes_sent=0;
	LogMsg *logmsg2 = (LogMsg *)malloc(MAX_SEND_BUFFER+10);
	memset(logmsg2, 0, sizeof(LogMsg));

	//printf("inside logger before its while\n");

	while(1)
	{
		
		printf("inside while of light thread \n");
		
		//generate timestamp string
		time_t curr_time = time(NULL);
		logmsg2->timestamp = ctime(&curr_time);
		logmsg2->sourceId = LIGHT;
		logmsg2->level = WARNING;

		snprintf (buffer, strlen("Message from Light thread")+1, "Message from Light thread");
		logmsg2->payload = buffer;
		
		pthread_mutex_lock(&dataQ_mutex);
		//keep waiting for timer signal 
		printf("before cond wait  light thread \n");
		pthread_cond_wait(&condvar,&dataQ_mutex);
		printf("after cond wait  after sigint light thread \n");
		pthread_mutex_unlock(&dataQ_mutex);
		printf("flag mask %d\n",flag_mask);
		if(flag_mask_copy & TIMER_EVENT)
		{
			
			pthread_mutex_lock(&logQ_mutex);
			if ((bytes_sent = mq_send (data_queue_handle,(const char*)&logmsg2, MAX_SEND_BUFFER, 5)) != 0) //can be changed later to light queue handle
			{
				perror ("[LightThread] Sending:");
			}
			pthread_mutex_unlock(&logQ_mutex);
		}
		
		printf("flag mask %d\n",flag_mask);
		printf("flag mask copy %d\n",flag_mask_copy);
		if(flag_mask_copy & SIGINT_EVENT)
		{
			printf("closing and breaking from light thread after sigint \n");
			//pthread_mutex_unlock(&dataQ_mutex);
			free(logmsg2);
			printf("break from while light thread \n");
			
			break;
		}
		
	}
	printf("return light thread \n");
	//pthread_exit(NULL);
}