#include "msg_queue.h"

void *SighandThread(void *args)
{	
	while(1)
	{
		if(flag_mask)
		{
			flag_mask_copy = flag_mask;
			if (flag_mask & TIMER_EVENT)
            {
                flag_mask ^= TIMER_EVENT;		
            }
			
			if (flag_mask & SIGINT_EVENT)
            {
                flag_mask ^= SIGINT_EVENT;
				break;
            }
		}
		else
		{
			pthread_cond_wait(&condvar,&dataQ_mutex);
		}
		
	}
}

void sighandler_sigint(int signum)
{
	printf("Caught signal sigint, coming out...\n");
	flag_mask |= SIGINT_EVENT;	//set flag for SIGINT event
	int32_t retval = pthread_cond_broadcast(&condvar);
	if(retval != 0)
	{
		printf("condition signal failed, error code - %d\n", retval);
	}
}

void timer_handler(int signum)
{
	static uint32_t count = 0;
	printf("timer expired %d\n",count++);
	int32_t ret;
	//printf("inside timer thread \n");
	flag_mask |= TIMER_EVENT;
	ret = pthread_cond_broadcast(&condvar); //use broadcast as I need to unblock all threads waiting on this condvar
	printf("condition broadcast\n");
	if(ret != 0)
	{
		printf("condition signal failed - %d\n", ret);
	}
	
	if(flag_mask_copy == SIGINT_EVENT)
	{
		timer.it_value.tv_sec = 0;
		timer.it_value.tv_usec = 0;

		timer.it_interval.tv_sec = 0;
		timer.it_interval.tv_usec = 0;
		setitimer(ITIMER_REAL, &timer, NULL);
		//exit(0);
	}
}

void create_timer()
{
	struct sigaction sa;
	memset(&sa,0,sizeof(sa));
	sa.sa_handler = &timer_handler;
	sigaction(SIGALRM, &sa, NULL);

	timer.it_value.tv_sec = 0;
	timer.it_value.tv_usec = 500000;

	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = 500000;

	setitimer(ITIMER_REAL, &timer, NULL);
}


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
			pthread_cond_wait(&condvar,&dataQ_mutex);
			
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
			}
			if(flag_mask_copy == SIGINT_EVENT)
			{
				
				mq_close(data_queue_handle);
				pthread_mutex_unlock(&dataQ_mutex);
				//free(logmsg3);
				break;
			}
			pthread_mutex_unlock(&dataQ_mutex);	
	}	
	
}

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
		pthread_mutex_lock (&dataQ_mutex);
		
		
		//keep waiting for timer signal 
		
		pthread_cond_wait(&condvar,&dataQ_mutex);
		
		
		if(flag_mask_copy == TIMER_EVENT)
		{
			
			if ((bytes_sent = mq_send (data_queue_handle,(const char*)&logmsg1, sizeof(logmsg1), msgprio)) != 0)
			{
				perror ("[TempThread] Sending:");
			}
			
			pthread_mutex_unlock (&dataQ_mutex);
			
		}
		
		if(flag_mask_copy == SIGINT_EVENT)
		{
			mq_close (data_queue_handle);
			pthread_mutex_unlock(&dataQ_mutex);
			free(logmsg1);
			break;
		}
	} 
}


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
		//printf("inside	while of light thread \n");
		
		//generate timestamp string
		time_t curr_time = time(NULL);
		logmsg2->timestamp = ctime(&curr_time);
		logmsg2->logId = 1;
		logmsg2->level = WARNING;

		snprintf (buffer, MAX_SEND_BUFFER, "Message from Light thread");
		
		logmsg2->payload = buffer;
		
		
		pthread_mutex_lock(&dataQ_mutex);
		
		//keep waiting for timer signal 
		
		pthread_cond_wait(&condvar,&dataQ_mutex);
		
		if(flag_mask_copy == TIMER_EVENT)
		{
			
			if ((bytes_sent = mq_send (data_queue_handle,(const char*)&logmsg2, sizeof(logmsg2), msgprio)) != 0) //can be changed later to light queue handle
			{
				perror ("[LightThread] Sending:");
			}
			pthread_mutex_unlock (&dataQ_mutex);
		}
		
		if(flag_mask_copy == SIGINT_EVENT)
		{
			pthread_mutex_unlock(&dataQ_mutex);
			free(logmsg2);
			break;
		}
	} 
	
}

void initialize_queue()
{
	pthread_mutex_init(&dataQ_mutex,NULL);
	pthread_cond_init(&condvar,NULL);
	
	// unlink the queue if it exists - debug
	mq_unlink (QTemp);
	mq_unlink (QTemp_Request);
	//mq_unlink (QLight_Request);
	
	attr.mq_maxmsg = 20;
	attr.mq_msgsize = MAX_SEND_BUFFER;
	attr.mq_flags = 0;

	
	data_queue_handle=  mq_open(QTemp,O_RDWR | O_CREAT,0666,&attr);
	tempreq_queue_handle = mq_open(QTemp_Request,O_RDWR | O_CREAT,0666,&attr);
	//lightreq_queue_handle = mq_open(QLight_Request,O_RDWR | O_CREAT,0666,&attr);

	if (data_queue_handle == -1)
	{
		perror ("[TempThread] Error Opening TempQ:");
	} 

	if (tempreq_queue_handle == -1)
	{
		perror ("[TempThread] Error Opening Temp Request Q: ");
	}
	
	if (lightreq_queue_handle == -1)
	{
		perror ("[LightThread] Error Opening Light Request Q: ");
	}
}


int main()
{
	int32_t retval = 0;
	initialize_queue();
	signal(SIGINT, sighandler_sigint);

	pthread_create(&tempThread,NULL,&TempThread,NULL);
	pthread_create(&lightThread,NULL,&LightThread,NULL);
	pthread_create(&loggerThread,NULL,&LoggerThread,NULL);
	pthread_create(&sighandThread,NULL,&SighandThread,NULL); //common signal handler for all events

	create_timer();
	
	retval = pthread_join(tempThread,NULL);
	if(retval != 0)
	{
		printf("pthread join failed, error code - %d\n", retval);
	}
	
	retval = pthread_join(lightThread,NULL);
	if(retval != 0)
	{
		printf("pthread join failed, error code - %d\n", retval);
	}
	
	retval = pthread_join(loggerThread,NULL);
	if(retval != 0)
	{
		printf("pthread join failed, error code - %d\n", retval);
	}
	
	retval = pthread_join(sighandThread,NULL);
	if(retval != 0)
	{
		printf("pthread join failed, error code - %d\n", retval);
	}
	
	retval = pthread_mutex_destroy(&dataQ_mutex);
	if(retval != 0)
	{
		printf("mutex destroy failed, error code - %d\n", retval);
	}
	
	retval =  pthread_cond_destroy(&condvar);
	if(retval != 0)
	{
		printf("cond destroy failed, error code - %d\n", retval);
	}
	
	printf("Joined all threads, destroyed mutexes and condition vars...\n");
	printf("Return zero from main()...\n");
	return 0;
}