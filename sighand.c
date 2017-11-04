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
				printf("break from while sighand thread \n");
				break;
            }
		}
		else
		{
			pthread_cond_wait(&condvar,&dataQ_mutex);
		}
	}
	pthread_mutex_unlock(&dataQ_mutex);
	printf("return from sighand thread \n");
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
	printf("return from sigint sighandler \n");
}