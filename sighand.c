#include "msg_queue.h"

void *SighandThread(void *args)
{
	//pthread_mutex_lock(&dataQ_mutex);
	while(1)
	{
		if(flag_mask)
		{
			//pthread_mutex_unlock(&dataQ_mutex);
			//printf("Flags - %x\n", flag_mask);
			flag_mask_copy = flag_mask;

			if (flag_mask & TIMER_EVENT)
      {
        flag_mask ^= TIMER_EVENT;
      }

			if (flag_mask & ASYNC_EVENT)
      {
        flag_mask ^= ASYNC_EVENT;
      }

			if (flag_mask & SIGINT_EVENT)
			{
				printf("%s\n","Sighandler got SIGINT");
				flag_mask ^= SIGINT_EVENT;
				//pthread_mutex_lock(&dataQ_mutex);
				break;
			}
			//pthread_mutex_lock(&dataQ_mutex);
		}
		else
		{ pthread_mutex_lock(&dataQ_mutex);
			pthread_cond_wait(&condvar,&dataQ_mutex);
		  pthread_mutex_unlock(&dataQ_mutex);
		}
	}
	//pthread_mutex_unlock(&dataQ_mutex);
	//free(logmsg_req);
	printf("%s\n","Outside sighandler thread");
}

void sighandler_sigint(int signum)
{
	printf("Caught signal sigint, coming out...\n");
	flag_mask |= SIGINT_EVENT;	//set flag for SIGINT event
	//flag_mask_copy = flag_mask;
	int32_t retval = pthread_cond_broadcast(&condvar);
	if(retval != 0)
	{
		printf("condition signal failed, error code - %d\n", retval);
	}
}

void sighandler_sigusr1(int signum)
{
    printf("Caught signal sigusr, coming out...\n");
    uint8_t c;// = '0';// = '0';
    //time_t curr_time;
    uint32_t bytes_sent=0;

    memset(logmsg_req, 0, sizeof(LogMsg));
    printf("before get char in sig usr1 \n");
    //system("echo 'A'");
		//scanf("%c",c);
		srand(time(NULL));
    c = rand() % 2;
	  printf("rand value - %d\n", c);
    if(c == 0)
    {
			printf("%s char - %d\n","yaha aaya",c);
      logmsg_req->sourceId = MAIN_TASK;
      logmsg_req->requestID = GET_TEMP_C;
      logmsg_req->level = INFO;
      logmsg_req->timestamp = time(NULL);
      //logmsg_req->payload = 0;
      printf("%s\n","Struct created");
        pthread_mutex_lock(&tempQ_mutex);
        if ((bytes_sent = mq_send (tempreq_queue_handle,(const char*)&logmsg_req, sizeof(LogMsg), 7)) != 0)
        {
            perror ("[HandlerThread] Sending:");
        }
        pthread_mutex_unlock(&tempQ_mutex);
				printf("%s\n","Struct sent");
        //pthread_mutex_lock(&dataQ_mutex);
        flag_mask |= ASYNC_EVENT;   //set flag for SIGINT event
        int32_t retval = pthread_cond_broadcast(&condvar);
        //pthread_mutex_unlock(&dataQ_mutex);
        if(retval != 0)
        {
            printf("condition signal failed, error code - %d\n", retval);
        }
    }


    if(c==1)
    {   printf("%s char - %d\n","yaha pe aaya",c);
            logmsg_req->sourceId = MAIN_TASK;
            logmsg_req->requestID = GET_TEMP_K;
            logmsg_req->level = WARNING;
            logmsg_req->timestamp = time(NULL);
            //logmsg_req->payload = {0};

        pthread_mutex_lock(&tempQ_mutex);
        if ((bytes_sent = mq_send(tempreq_queue_handle,(const char*)&logmsg_req, sizeof(LogMsg), 8)) != 0)
        {
            perror ("[HandlerThread] Sending:");
        }
        pthread_mutex_unlock(&tempQ_mutex);
				usleep(100);
        //pthread_mutex_lock(&dataQ_mutex);
        flag_mask |= ASYNC_EVENT;   //set flag for SIGINT event
        int32_t retval = pthread_cond_broadcast(&condvar);
        //pthread_mutex_unlock(&dataQ_mutex);
        if(retval != 0)
        {
            printf("condition signal failed, error code - %d\n", retval);
        }
    }
		//free(logmsg_req);
    printf("return from sigusr1 sighandler \n");
}
