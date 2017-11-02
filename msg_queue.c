#include"msg_queue.h"

void sighandler_sigint(int signum)
{
	printf("Caught signal %d, coming out...\n", signum);
	
	flag_cond = 1;	//set exit flag for all threads
	
	int32_t retval = pthread_cond_broadcast(&condvar);
	if(retval != 0)
	{
		printf("condition signal failed, error code - %d\n", retval);
	}
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
	
	retval = pthread_mutex_destroy(&timer_mutex);
	if(retval != 0)
	{
		printf("mutex destroy failed, error code - %d\n", retval);
	}
	
	retval = pthread_mutex_destroy(&tempq_mutex);
	if(retval != 0)
	{
		printf("mutex destroy failed, error code - %d\n", retval);
	}
	retval =  pthread_cond_destroy(&condvar);
	if(retval != 0)
	{
		printf("cond destroy failed, error code - %d\n", retval);
	}

	//exit(0);
}

void timer_thread(union sigval arg)
{
	
	int32_t ret;
	if(flag_cond == 0)
	{
		ret = pthread_mutex_lock(&timer_mutex);
		if (ret != 0)
			printf("Mutex timer lock mutex error\n");

		printf("Timer expire signal to main thread \n");
		
		flag_cond = 2;
		ret = pthread_cond_broadcast(&condvar);
		if (ret != 0)
		{
		  printf("Signal condition error\n");
		}

		ret = pthread_mutex_unlock(&timer_mutex);
		if (ret != 0)
				printf("Timer mutex Unlock mutex error\n");
			
		
		create_timer(); //change this value
	}
	else if(flag_cond == 1)
	{
		printf("Timer stopped\n");
		pthread_mutex_unlock(&timer_mutex);
	}
}

void create_timer(void)
{
	timer_t timer_id;
	int32_t ret;
	struct itimerspec ts;
	struct sigevent se;
	long long nanosecs = TICKS * 100;


	se.sigev_notify = SIGEV_THREAD;
	se.sigev_value.sival_ptr = &timer_id;
	se.sigev_notify_function = timer_thread;
	se.sigev_notify_attributes = NULL;

	ts.it_value.tv_sec = nanosecs / 1000000000;
	ts.it_value.tv_nsec = nanosecs % 1000000000;//TICKS * 100 * 100 ;
	ts.it_interval.tv_sec = 0;
	ts.it_interval.tv_nsec = 0;//usecs % (100 * 100);

	ret = timer_create(ITIMER_REAL, &se, &timer_id);
	if (ret== -1)
		printf("Create timer error");

	
	ret = timer_settime(timer_id, 0, &ts, 0);
	if (ret == -1)
		printf("Set timer error");
}


void *LoggerThread(void *args)
{

	uint32_t temp_data;
	//uint32_t light_data;
	uint32_t ret; 
	uint32_t bytes_read;
	LogMsg *logmsg3 = (LogMsg *)malloc(sizeof(LogMsg));

	while(1)
	{	
		ret = pthread_mutex_trylock(&tempq_mutex);

		if(ret == 0)
		{
			//keep waiting for timer signal 
			pthread_cond_wait(&condvar,&tempq_mutex);
			
			if(flag_cond == 2)
			{
				mq_getattr(tempqueue_handle, &attr);
				while(attr.mq_curmsgs > 0)
				{
					bytes_read = mq_receive(tempqueue_handle, (char *)&logmsg3, MAX_SEND_BUFFER, &temp_data);
					//printf("Bytes received = %d\n",bytes_read);
					//printf("Size of  = %ld\n",sizeof(LogMsg));
					if (bytes_read == -1)
					{
						perror("[LoggerThread] Failed to recieve:");
					}	
					else
					{
						printf ("[LoggerThread] Log ID: %d \n", logmsg3->logId);
						printf ("[LoggerThread] Timestamp: %d \n", logmsg3->timestamp);
						printf ("[LoggerThread] Log Level: %d \n", logmsg3->level);
						printf ("[LoggerThread] Payload: %s \n", logmsg3->payload);
						
						// Get the attributes
						mq_getattr(tempqueue_handle, &attr);
						printf("[LoggerThread] Queue %s currently holds %ld messages\n",QTemp,attr.mq_curmsgs);   

							// Clear buffer
							
						//memset(buffer, 0, MAX_SEND_BUFFER); 
						memset(logmsg3, 0, sizeof(logmsg3));
						
					}
				}
				pthread_mutex_unlock(&tempq_mutex);
			}
			
			if(flag_cond == 1)
			{
				mq_close (tempqueue_handle);
				pthread_mutex_unlock(&tempq_mutex);
				break;
			}
			
		}
		
		/*
		ret = pthread_mutex_trylock(&lightq_mutex);
		if(ret == 0)
		{
			mq_getattr(lightqueue_handle, &attr);
			while(attr.mq_curmsgs > 0)
			{
				bytes_read = mq_receive(lightqueue_handle, buffer, MAX_SEND_BUFFER, &light_data);
				if (bytes_read == -1)
				{
					perror("[LoggerThread] Failed to recieve:");
					return 0;
				}	
				else
				{
					printf ("[LoggerThread] Data: %s \n", buffer);   
					// Get the attributes
					mq_getattr(lightqueue_handle, &attr);
					printf("[LoggerThread] Queue %s currently holds %ld messages\n",QLight,attr.mq_curmsgs);   

						// Clear buffer
						
					memset(buffer, 0, MAX_SEND_BUFFER); 
				}
			}
			pthread_mutex_unlock(&lightq_mutex);
			sleep(1);
		}*/	
	}
	
}

void *TempThread (void *args)
{

	char buffer[MAX_SEND_BUFFER];
	LogMsg *logmsg1 = (LogMsg *)malloc(sizeof(LogMsg));
	memset(logmsg1, 0, sizeof(logmsg1));
	unsigned int msgprio = 1;
	uint32_t bytes_sent=0;
	
	//int count=0;

	while(1)
	{

		logmsg1->timestamp = 123456789;
		logmsg1->logId = 0;
		logmsg1->level = WARNING;
		
		snprintf (buffer, MAX_SEND_BUFFER, "Message from Temp thread");
		
		//printf ("Message from Temp thread %s\n",logmsg1->payload);
		logmsg1->payload = buffer;
		pthread_mutex_lock (&tempq_mutex);
		
		//keep waiting for timer signal 
		pthread_cond_wait(&condvar,&tempq_mutex);
		if(flag_cond == 2)
		{
		
			//printf("Size of  sent logmsg 1= %ld\n",sizeof(logmsg1));
			
			if ((bytes_sent = mq_send (tempqueue_handle,(const char*)&logmsg1, sizeof(logmsg1), msgprio)) != 0)
			{
				perror ("[TempThread] Sending:");
			}
			//printf("Bytes sent = %d\n",bytes_sent);
			pthread_mutex_unlock (&tempq_mutex);
			
		}
		
		if(flag_cond == 1)
		{
			mq_close (tempqueue_handle);
			pthread_mutex_unlock(&tempq_mutex);
			break;
		}
	} 
}


void *LightThread (void *args)
{
	char buffer[MAX_SEND_BUFFER];
	unsigned int msgprio = 2;
	uint32_t bytes_sent=0;

	LogMsg *logmsg2 = (LogMsg *)malloc(sizeof(LogMsg));
	memset(logmsg2, 0, sizeof(logmsg2));

	//int count = 0;
	

	while(1)
	{
		logmsg2->timestamp = 123456790;
		logmsg2->logId = 1;
		logmsg2->level = WARNING;

		snprintf (buffer, MAX_SEND_BUFFER, "Message from Light thread");
		//printf ("Message from Light thread %s \n",logmsg2->payload);
		logmsg2->payload = buffer;
		
		//pthread_mutex_lock (&lightq_mutex);
		//printf("Size of  sent logmsg 2= %ld\n",sizeof(logmsg2));
		pthread_mutex_lock(&tempq_mutex);
		
		//keep waiting for timer signal 
		pthread_cond_wait(&condvar,&tempq_mutex);
		if(flag_cond == 2)
		{
			if ((bytes_sent = mq_send (tempqueue_handle,(const char*)&logmsg2, sizeof(logmsg2), msgprio)) != 0) //can be changed later to light queue handle
			{
				perror ("[LightThread] Sending:");
			}
			pthread_mutex_unlock (&tempq_mutex);
		}
		
		if(flag_cond == 1)
		{
			mq_close (tempqueue_handle);
			pthread_mutex_unlock(&tempq_mutex);
			break;
		}
	} 
	
}
void initialize_queue()
{
	// unlink the queue if it exists - debug
	mq_unlink (QTemp);
	mq_unlink (QLight);
	attr.mq_maxmsg = 20;
	attr.mq_msgsize = MAX_SEND_BUFFER;
	attr.mq_flags = 0;

	
	tempqueue_handle=  mq_open(QTemp,O_RDWR | O_CREAT,0666,&attr);

	//lightqueue_handle=  mq_open(QLight,O_RDWR | O_CREAT,0666,&attr);

	if (tempqueue_handle == -1)
	{
		perror ("[TempThread] Error Opening TempQ:");
	} 

	/*if (lightqueue_handle == -1)
	{
		perror ("[LightThread] Error Opening LightQ: ");
	} */
}


int main()
{
	
	pthread_mutex_init(&tempq_mutex,NULL);
	pthread_mutex_init(&timer_mutex,NULL);
	pthread_cond_init(&condvar,NULL);
	initialize_queue();
	
	pthread_create(&tempThread,NULL,&TempThread,NULL);
	pthread_create(&lightThread,NULL,&LightThread,NULL);
	pthread_create(&loggerThread,NULL,&LoggerThread,NULL);
	
	create_timer();
	
	return 0;
}
