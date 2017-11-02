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
#include<sys/time.h>
#include<time.h>


/*definitions */

#define MAX_SEND_BUFFER 4096
#define TICKS 1000000
#define QTemp "/temp"
#define QLight "/light"

/*synchronisation variables */
pthread_mutex_t tempq_mutex;
pthread_mutex_t timer_mutex;
pthread_cond_t condvar;
//pthread_mutex_t lightq_mutex = PTHREAD_MUTEX_INITIALIZER;
mqd_t tempqueue_handle;
//mqd_t lightqueue_handle;
struct mq_attr attr;


/*global variables */
sig_atomic_t flag_cond = 0;
pthread_t tempThread, lightThread,loggerThread;
//uint32_t timer_count = 10;

/*Structure definitions*/

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


/* Function declarations */
void *TempThread (void *);
void *LightThread (void *);
void *LoggerThread (void *);
void create_timer(void);
void initialize_queue();