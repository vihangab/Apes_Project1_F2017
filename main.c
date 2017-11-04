#include "msg_queue.h"

int main()
{
	flag_mask = 0;
	flag_mask_copy = 0;
	int32_t retval = 0;
	initialize_queue();
	signal(SIGINT, sighandler_sigint);

	pthread_create(&tempThread,NULL,&TempThread,NULL);
	pthread_create(&lightThread,NULL,&LightThread,NULL);
	pthread_create(&loggerThread,NULL,&LoggerThread,NULL);
	pthread_create(&sighandThread,NULL,&SighandThread,NULL); //common signal handler for all events

	create_timer(1.5);
	
	retval = pthread_join(tempThread,NULL);
	if(retval != 0)
	{
		printf("pthread join failed, error code - %d\n", retval);
	}
	printf("joined temp thread \n");
	
	retval = pthread_join(lightThread,NULL);
	if(retval != 0)
	{
		printf("pthread join failed, error code - %d\n", retval);
	}
	
	printf("joined light thread \n");
	
	retval = pthread_join(loggerThread,NULL);
	if(retval != 0)
	{
		printf("pthread join failed, error code - %d\n", retval);
	}
	
	printf("joined logger thread \n");
	
	
	retval = pthread_join(sighandThread,NULL);
	if(retval != 0)
	{
		printf("pthread join failed, error code - %d\n", retval);
	}
	
	printf("joined sighand thread \n");
	
	/*retval = pthread_mutex_destroy(&dataQ_mutex);
	if(retval != 0)
	{
		printf("mutex destroy failed, error code - %d\n", retval);
	}
	else
		printf("Mutex destroyed \n");
	*/
	retval =  pthread_cond_destroy(&condvar);
	if(retval != 0)
	{
		printf("cond destroy failed, error code - %d\n", retval);
	}
	printf("Convar destroyed \n");
	
	printf("Joined all threads, destroyed mutexes and condition vars...\n");
	printf("Return zero from main()...\n");
	return 0;
}