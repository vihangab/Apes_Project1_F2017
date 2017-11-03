#include "msg_queue.h"
#include "logger.h"

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
	exit(0);
}

void timer_handler(int signum)
{
	static uint32_t count = 0;
	printf("timer expired %d\n",count++);
	int32_t ret;
	printf("inside	timer thread \n");
	flag_cond = 2;
	ret = pthread_cond_broadcast(&condvar);
}

void create_timer()
{
	struct sigaction sa;
	struct itimerval timer;

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

	uint32_t temp_data;
	//uint32_t light_data;
	uint32_t ret;
	uint32_t bytes_read;
	LogMsg *logmsg3 = (LogMsg *)malloc(sizeof(LogMsg));
	if(create_log_file(&fdesc,"some.txt")==OPEN_ERROR){
		printf("%s\n","Error Opening file");
	}
	//printf("inside logger before its while\n");
	while(1)
	{
		//printf("inside	while of logger thread \n");
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
						printf ("[LoggerThread] Log ID: %d \n", logmsg3->sourceId);
						printf ("[LoggerThread] Timestamp: %s \n", logmsg3->timestamp);
						printf ("[LoggerThread] Log Level: %d \n", logmsg3->level);
						printf ("[LoggerThread] Payload: %s \n", logmsg3->payload);
            log_item(fdesc,logmsg3);
						// Get the attributes
						mq_getattr(tempqueue_handle, &attr);
						printf("[LoggerThread] Queue %s currently holds %ld messages\n",QTemp,attr.mq_curmsgs);

						//memset(buffer, 0, MAX_SEND_BUFFER);
						memset(logmsg3, 0, sizeof(logmsg3));

					}
				}
				pthread_mutex_unlock(&tempq_mutex);
			}

			if(flag_cond == 1)
			{
				fclose(fdesc);
				mq_close(tempqueue_handle);
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
		logmsg1->sourceId = 1;
		logmsg1->level = INFO;

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
		logmsg2->sourceId = 2;
		logmsg2->level = ALERT;

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
	initialize_queue();
	signal(SIGINT, sighandler_sigint);

	pthread_create(&tempThread,NULL,&TempThread,NULL);
	pthread_create(&lightThread,NULL,&LightThread,NULL);
	pthread_create(&loggerThread,NULL,&LoggerThread,NULL);

	create_timer();

	while(1);

	return 0;
}
