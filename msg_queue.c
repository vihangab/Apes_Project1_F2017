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
#include<sys/time.h>


#define BUFFER_SIZE 10000
#define MAX_SEND_BUFFER 10000
#define TICKS 10

/*struct to get temperature data*/

/*typedef enum
{
	GET_TEMP_C,
	GET_LIGHT,
	LOG_DATA	
}CMDS;

typedef struct msg_queue
{
	enum user_request_id;
}Msg_Queue;
*/


/* Threads */
static void *MainThread (void *);
static void *AnotherThread (void *);
static void *OneMoreThread (void *);
void create_timer(void);

/* Defines */
#define MAIN_QNAME "/MainQueue"
pthread_mutex_t wait_mutex;
pthread_mutex_t timer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t timer_cond = PTHREAD_COND_INITIALIZER;

//timer_val = 10;



void timer_thread(union sigval arg)
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

	status = timer_create(CLOCK_REALTIME, &se, &timer_id);
	if (status == -1)
		printf("Create timer error");

	
	status = timer_settime(timer_id, 0, &ts, 0);
	if (status == -1)
		printf("Set timer error");
}



int main()
{
	pthread_t mainThread, anotherThread,onemoreThread;
	int i = 10;
	pthread_mutex_init (&wait_mutex, NULL);
	printf ("Creating threads .. \n");
	
	pthread_create (&mainThread,    NULL, &MainThread,    NULL);
	sleep(1);
	pthread_create (&anotherThread, NULL, &AnotherThread, NULL);
	pthread_create (&onemoreThread, NULL, &OneMoreThread, NULL);
	
	create_timer();

	while(i-- > 0)
	{
		printf("waiting for timer signal condtion wait\n");
		pthread_cond_wait(&timer_cond, &timer_mutex);
	}
	
	printf("Receiver timer signal\n");
	pthread_join (mainThread,    NULL);
	pthread_join (anotherThread, NULL);
	pthread_join (onemoreThread, NULL);
	return 0;
}


/* Main thread .. Waiting for messages */
void *MainThread (void *args)
{

	mqd_t queue_handle;
	char buffer[BUFFER_SIZE];
	int bytes_read;

	struct mq_attr msgq_attr;
	unsigned int sender;

	printf ("[MainThread] Inside main thread \n");
	// Let the other thread wait till I am ready! 
	pthread_mutex_lock (&wait_mutex);

	// Clear the buffer
	memset (buffer, 0, BUFFER_SIZE);

	// unlink the queue if it exisits - debug
	mq_unlink (MAIN_QNAME);


	printf ("[MainThread]Opening MQ \n"); 
	queue_handle=  mq_open(MAIN_QNAME, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG, NULL);

	if (queue_handle == -1)
	{
		perror ("[MainThread] Error Opening MQ: ");
		return 0;
	} 

	printf ("[MainThread] Waiting for messages ... \n"); 
	pthread_mutex_unlock (&wait_mutex);


	for (; ;)
	{
		bytes_read = mq_receive(queue_handle, buffer, BUFFER_SIZE, &sender);
		//printf ("Received %s \n", buffer);

		if (bytes_read == -1)
		{
			perror("[MainThread] Failed to recieve:");
			return 0;
		}

		else
		{
			printf ("[MainThread] Data: %s \n", buffer);   
			// Get the MQ attributes
			mq_getattr(queue_handle, &msgq_attr);
			printf("[MainThread] Queue \"%s\":\n"
					"\t- currently holds %ld messages\n",
			   MAIN_QNAME,  
			   msgq_attr.mq_curmsgs);   

				// Clear buffer and sleep to block for some time t and see 
				// if you get all the messages!	

				memset(buffer, 0, BUFFER_SIZE); 
				//sleep(1);
		}  
	} 
	mq_close (queue_handle);
}



void *AnotherThread (void *args)
{

	mqd_t queue_handle;
	char buffer[MAX_SEND_BUFFER];
	unsigned int msgprio = 1;

	int count;

	printf ("[AnotherThread] Inside Another thread \n");
	
	//sleep(1);
	
	queue_handle=  mq_open(MAIN_QNAME, O_RDWR);
	if (queue_handle == -1)
	{
		perror ("[AnotherThread] Error Opening MQ:");
		return 0;
	}

	for (count = 1; count <= 5; count++)
	{

		snprintf (buffer, MAX_SEND_BUFFER, "Message %d from Another thread", count);
		
		pthread_mutex_lock (&wait_mutex);
		
		if (0 != mq_send (queue_handle, buffer, strlen(buffer)+1, msgprio))
		{

			perror ("[AnotherThread] Sending:");
			mq_close (queue_handle); 
			return 0;
		}
		pthread_mutex_unlock (&wait_mutex); 
		sleep(2);
	} 

	
	mq_close (queue_handle); 

	return 0;
}


void *OneMoreThread (void *args)
{

	mqd_t queue_handle;
	char buffer[MAX_SEND_BUFFER];
	unsigned int msgprio = 2;

	int count;

	printf ("[OneMoreThread] Inside One More thread \n");
	//sleep(1);
	

	queue_handle=  mq_open(MAIN_QNAME, O_RDWR);
	if (queue_handle == -1)
	{

		perror ("[AnotherThread] Error Opening MQ:");
		return 0;
	}

	for (count = 1; count <= 5; count++)
	{

		snprintf (buffer, MAX_SEND_BUFFER, "Message %d from One more thread", count+100);
		
		pthread_mutex_lock (&wait_mutex);
		if (0 != mq_send (queue_handle, buffer, strlen(buffer)+1, msgprio))
		{

			perror ("[AnotherThread] Sending:");
			mq_close (queue_handle); 

			return 0;
		}
		pthread_mutex_unlock (&wait_mutex); 
		sleep(1);
	} 

	
	mq_close (queue_handle); 

	return 0;
}
