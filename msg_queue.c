#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<mqueue.h>
#include<sys/stat.h>
#include<errno.h>
#include<string.h>
#include<sys/signal.h>
#include<sys/time.h>
#include<stdint.h>


#define MAX_SEND_BUFFER 4096
#define TICKS 10
#define QTemp "/temp"
#define QLight "/light"

pthread_mutex_t tempq_mutex = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_t lightq_mutex = PTHREAD_MUTEX_INITIALIZER;

/*struct to get temperature data*/

/*typedef enum
{
	GET_TEMP_C,
	GET_LIGHT,
	LOG_DATA	
}CMDS;
*/
typedef enum loglevel
{
	INFO,
	WARNING,
	ALERT,
	HEART_BEAT
}LogLevel; 

typedef struct logger
{
	uint32_t timestamp;
	//uint32_t longlength;
	uint8_t logId;
	LogLevel level;
	uint8_t *payload;
}LogMsg;



/* Threads */
void *TempThread (void *);
void *LightThread (void *);
void *LoggerThread (void *);

mqd_t tempqueue_handle;
//mqd_t lightqueue_handle;

struct mq_attr attr;


//void create_timer(void);

//pthread_mutex_t timer_mutex = PTHREAD_MUTEX_INITIALIZER;
//pthread_cond_t timer_cond = PTHREAD_COND_INITIALIZER;

//timer_val = 10;



/*void timer_thread(union sigval arg)
{
	int status;

	status = pthread_mutex_lock(&timer_mutex);
	if (status != 0)
		printf("Lock mutex error\n");

	printf("Timer expire signal to main thread \n");
	
	status = pthread_cond_signal(&timer_cond);
	if (status != 0)
	{
	  printf("Signal condition error\n");
	}

	status = pthread_mutex_unlock(&timer_mutex);
	if (status != 0)
			printf("Unlock mutex error\n");
	
	create_timer(); //change this value
}

void create_timer(void)
{
	timer_t timer_id;
	int status;
	struct itimerspec ts;
	struct sigevent se;
	long long nanosecs = TICKS * 100;


	se.sigev_notify = SIGEV_THREAD;
	se.sigev_value.sival_ptr = &timer_id;
	se.sigev_notify_function = timer_thread;
	se.sigev_notify_attributes = NULL;

	ts.it_value.tv_sec = nanosecs / 1000000000;
	ts.it_value.tv_nsec = nanosecs % 1000000000;
	ts.it_interval.tv_sec = 0;
	ts.it_interval.tv_nsec = 0;

	status = timer_create(ITIMER_REAL, &se, &timer_id);
	if (status == -1)
		printf("Create timer error");

	
	status = timer_settime(timer_id, 0, &ts, 0);
	if (status == -1)
		printf("Set timer error");
}
*/
void initialize_queue();

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
			
			sleep(1);
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
		//printf("Size of  sent logmsg 1= %ld\n",sizeof(logmsg1));
		
		if ((bytes_sent = mq_send (tempqueue_handle,(const char*)&logmsg1, sizeof(logmsg1), msgprio)) != 0)
		{
			perror ("[TempThread] Sending:");
		}
		//printf("Bytes sent = %d\n",bytes_sent);
		pthread_mutex_unlock (&tempq_mutex);
		//count++;
		sleep(1);
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
		pthread_mutex_lock (&tempq_mutex);
		if ((bytes_sent = mq_send (tempqueue_handle,(const char*)&logmsg2, sizeof(logmsg2), msgprio)) != 0) //can be changed later to light queue handle
		{

			perror ("[LightThread] Sending:");
		}
		pthread_mutex_unlock (&tempq_mutex);
		//pthread_mutex_unlock (&lightq_mutex); 
		//count++;
		sleep(1);
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
	pthread_t tempThread, lightThread,loggerThread;
	
	initialize_queue();
	
	sleep(1);
	
	pthread_create(&tempThread,NULL,&TempThread,NULL);
	pthread_create(&lightThread,NULL,&LightThread,NULL);
	pthread_create(&loggerThread,NULL,&LoggerThread,NULL);
	
	//create_timer();

	/*while(i-- > 0)
	{
		printf("waiting for timer signal condtion wait\n");
		pthread_cond_wait(&timer_cond, &timer_mutex);
	}*/
	
	pthread_join (tempThread,NULL);
	mq_close (tempqueue_handle); 
	
	pthread_join (lightThread, NULL);
	//mq_close (lightqueue_handle);
	
	pthread_join (loggerThread, NULL);
	return 0;
}
