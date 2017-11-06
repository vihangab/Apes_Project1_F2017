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
#include "logger.h"

/*Macro definitions */
#define MAX_SEND_BUFFER 4096
#define QLog            "/logq"
#define QMain           "/mainq"
#define QTemp_Request   "/tempreq"
#define QLight_Request  "/lightreq"
#define SIGINT_EVENT    0x01
#define TIMER_EVENT     0x02
#define ASYNC_EVENT     0x04

/*synchronisation variables */
pthread_mutex_t dataQ_mutex;
pthread_mutex_t logQ_mutex;
pthread_mutex_t tempQ_mutex;
pthread_mutex_t mainQ_mutex;
pthread_mutex_t lightQ_mutex;

pthread_cond_t condvar;
mqd_t data_queue_handle;
mqd_t tempreq_queue_handle;
mqd_t lightreq_queue_handle;
mqd_t main_queue_handle;

/*global variables and flags*/
sig_atomic_t flag_mask;
sig_atomic_t flag_mask_copy;
sig_atomic_t heartbeat_flag;

LogMsg *logmsg_req;

/* Function declarations */
void *TempThread(void *);
void *LightThread(void *);
void *LoggerThread(void *);
void *SighandThread(void *);
void create_interval_timer(float timerval);
void create_timer(float timer_val, timer_t *timer_id);
void initialize_queue(char * qName, mqd_t *msgHandle);
void sighandler_sigint(int signum);
void sighandler_sigusr1(int signum);
pthread_t tempThread;
pthread_t lightThread;
pthread_t loggerThread;
pthread_t sighandThread;
