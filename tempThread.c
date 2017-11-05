#include "msg_queue.h"

void *TempThread(void *args)
{

	char buffer[MAX_SEND_BUFFER],buffer1[MAX_SEND_BUFFER];
	LogMsg *logmsg1 = (LogMsg *)malloc(MAX_SEND_BUFFER+10);
	LogMsg *logmsg10 = (LogMsg *)malloc(MAX_SEND_BUFFER+10);
	memset(logmsg1, 0, MAX_SEND_BUFFER+10);
	memset(logmsg10, 0, MAX_SEND_BUFFER+10);
	time_t curr_time;
	
	//unsigned int msgprio = 1;
	uint32_t bytes_sent=0;
	//printf("inside logger before its while\n");

	while(1)
	{
		
		//printf("inside	while of temp thread \n");
		//generate timestamp string
		
		pthread_mutex_lock(&dataQ_mutex);
		//keep waiting for timer signal 
		printf("before cond wait  temp thread \n");
		
		pthread_cond_wait(&condvar,&dataQ_mutex);
		printf("after cond wait  after sigint temp thread \n");
		pthread_mutex_unlock(&dataQ_mutex);
		
		if(flag_mask_copy & TIMER_EVENT)
		{
			curr_time = time(NULL);
			logmsg1->timestamp = ctime(&curr_time);
			logmsg1->sourceId = TEMP;
			logmsg1->level = WARNING;
			snprintf(buffer, strlen("Message from Temp thread")+1, "Message from Temp thread");
			logmsg1->payload = buffer;
			
			pthread_mutex_lock(&logQ_mutex);
			if ((bytes_sent = mq_send (data_queue_handle,(const char*)&logmsg1, MAX_SEND_BUFFER, 3)) != 0)
			{
				perror ("[TempThread] Sending:");
			}
			pthread_mutex_unlock(&logQ_mutex);

			curr_time = time(NULL);
			logmsg10->timestamp = ctime(&curr_time);
			logmsg10->sourceId = TEMP;
			logmsg10->level = WARNING;
			snprintf (buffer1, strlen("Temp thread Alive")+1, "Temp thread Alive");
			logmsg10->payload = buffer1;
			
			pthread_mutex_lock(&mainQ_mutex);
			if ((bytes_sent = mq_send (main_queue_handle,(const char*)&logmsg10, MAX_SEND_BUFFER, 4)) != 0)
			{
				perror ("[TempThread] Sending alive error:");
			}
			pthread_mutex_unlock(&mainQ_mutex);
			
			pthread_mutex_lock(&dataQ_mutex);
			flag_mask |= ASYNC_EVENT;
			pthread_mutex_unlock(&dataQ_mutex);
		}
		
		printf("flag mask in temp %d\n",flag_mask);
		printf("flag mask copy in temp %d\n",flag_mask_copy);
		if(flag_mask_copy & SIGINT_EVENT)
		{
			printf("closing and breaking from temp thread after sigint \n");
			//pthread_mutex_unlock(&dataQ_mutex);
			free(logmsg1);
			free(logmsg10);
			printf("break from while temp thread \n");
			break;
		}
		
	} 
	printf("return from temp thread \n");
	//pthread_exit(NULL);
}