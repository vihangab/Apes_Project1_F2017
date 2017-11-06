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
	printf("condition broadcast\n");
	if(ret != 0)
	{
		printf("condition signal failed - %d\n", ret);
	}

	if(flag_mask_copy & SIGINT_EVENT)
	{
		printf("%s\n","Clear timer");
		create_interval_timer(0); //stop timer
		//sleep(1);
		//pthread_cond_broadcast(&condvar);
		//printf("%s\n","After timer interval set to 0");
	}
}

void create_interval_timer(float timer_val)
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
}

void heartbeat_handler(){
	//if it reaches here then we did not get heartbeat, set error led and log message
}

void create_timer(float timer_val, timer_t *timer_id)
{
  int status;
  struct itimerspec ts;
  struct sigevent se;
	uint32_t int_part = timer_val;
	uint32_t decimal_part = (timer_val - (float)int_part)*1000000000;
  /*
   * Set the sigevent structure to cause the signal to be
   * delivered by creating a new thread.
   */
  se.sigev_notify = SIGEV_THREAD;
  se.sigev_value.sival_ptr = timer_id;
  se.sigev_notify_function = heartbeat_handler;
  se.sigev_notify_attributes = NULL;

  ts.it_value.tv_sec = int_part;
  ts.it_value.tv_nsec = decimal_part;
  ts.it_interval.tv_sec = 0;
  ts.it_interval.tv_nsec = 0;

  status = timer_create(CLOCK_REALTIME, &se, timer_id);
  if (status == -1)
    printf("%s\n","Create timer error");

  // TODO maybe we'll need to have an array of itimerspec
  status = timer_settime(*timer_id, 0, &ts, 0);
  if (status == -1)
    printf("%s\n","Set timer error");
}
