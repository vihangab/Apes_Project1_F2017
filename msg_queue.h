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
#include<sys/types.h>
#include<time.h>

/*Macro definitions */
#define MAX_SEND_BUFFER 4096
#define QTemp "/temp"
#define QTemp_Request "/tempreq"
#define QLight_Request "/lightreq"
#define TIMER_EVENT 0x02
#define SIGINT_EVENT 0x01


/*synchronisation variables */
pthread_mutex_t dataQ_mutex;
pthread_mutex_t sighand_mutex;
pthread_cond_t condvar;
mqd_t data_queue_handle;
mqd_t tempreq_queue_handle;
mqd_t lightreq_queue_handle;
struct mq_attr attr;
struct itimerval timer;

/*global variables and flags*/
sig_atomic_t flag_mask = 0x00;
sig_atomic_t flag_mask_copy = 0x00;



/*Structure definitions*/
typedef enum loglevel
{
	INFO,
	WARNING,
	ALERT,
	HEART_BEAT
}LogLevel; 

typedef struct logger
{
	uint8_t *timestamp;
	uint8_t logId;
	uint8_t *payload;
	LogLevel level;
}LogMsg;


/* Function declarations */
void *TempThread(void *);
void *LightThread(void *);
void *LoggerThread(void *);
void *SighandThread(void *);
void create_timer();
void initialize_queue();
void sighandler_sigint(int signum);
pthread_t tempThread;
pthread_t lightThread;
pthread_t loggerThread;
pthread_t sighandThread;