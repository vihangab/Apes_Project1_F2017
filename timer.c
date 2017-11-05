#include "msg_queue.h"

void timer_handler(int signum)
{
	static uint32_t count = 0;
	printf("timer expired %d\n",count++);
	int32_t ret;
	pthread_mutex_lock(&dataQ_mutex);
	
	flag_mask |= TIMER_EVENT;
	ret = pthread_cond_broadcast(&condvar); //use broadcast as I need to unblock all threads waiting on this condvar
	
	pthread_mutex_unlock(&dataQ_mutex);
	if(ret != 0)
	{
		printf("condition signal failed - %d\n", ret);
	}
	if(flag_mask_copy & SIGINT_EVENT)
	{
		printf("clear timer \n");
		create_timer(0); //stop timer

	}
	printf("timer handler return \n");
}

void create_timer(float timer_val)
{
	struct sigaction sa;
    struct itimerval timer;
	uint32_t int_part = timer_val;
    uint32_t decimal_part = (timer_val - (float)int_part)*1000000;
    memset(&sa,0,sizeof(sa));
    sa.sa_handler = &timer_handler;
    sigaction(SIGALRM, &sa, NULL);

    timer.it_value.tv_sec = int_part;
    timer.it_value.tv_usec = decimal_part;

    timer.it_interval.tv_sec = int_part;
    timer.it_interval.tv_usec = decimal_part;

    setitimer(ITIMER_REAL, &timer, NULL);
	printf("Create timer function return \n");
}