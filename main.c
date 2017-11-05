#include "msg_queue.h"

int main()
{
	flag_mask = 0;
	flag_mask_copy = 0;
	int32_t retval = 0;
	uint32_t data_to_log = 0;
	uint32_t bytes_read = 0;
	initialize_queue();
	signal(SIGINT, sighandler_sigint);

	pthread_create(&tempThread,NULL,&TempThread,NULL);
	pthread_create(&lightThread,NULL,&LightThread,NULL);
	pthread_create(&loggerThread,NULL,&LoggerThread,NULL);
	pthread_create(&sighandThread,NULL,&SighandThread,NULL); //common signal handler for all events

	create_timer(1.5);
	LogMsg *logmsg0 = (LogMsg *)malloc(MAX_SEND_BUFFER+10);
	
	while(1)
	{
			
			//printf("inside while of main thread \n");
			pthread_mutex_lock(&dataQ_mutex);
			//keep waiting for timer signal 
			printf("before cond wait  main thread \n");
			pthread_cond_wait(&condvar,&dataQ_mutex);
			printf("after cond wait after sigint main thread \n");
			pthread_mutex_unlock(&dataQ_mutex);	
			
			printf("after cond wait in main thread\n");

			if(flag_mask_copy & ASYNC_EVENT)
			{
					
				mq_getattr(data_queue_handle, &attr);
				while(attr.mq_curmsgs > 0)
				{
					
					pthread_mutex_lock(&mainQ_mutex);
					bytes_read = mq_receive(main_queue_handle, (char *)&logmsg0, MAX_SEND_BUFFER, &data_to_log);
					
					if (bytes_read == -1)
					{
						perror("[MainThread] Failed to recieve:");
						break;
					}	
					else
					{
						printf ("[MainThread] Log ID: %d \n", logmsg0->sourceId);
						printf ("[MainThread] Timestamp: %s \n", logmsg0->timestamp);
						printf ("[MainThread] Log Level: %d \n", logmsg0->level);
						printf ("[MainThread] Payload: %s \n", logmsg0->payload);
						
						// Get the attributes
						mq_getattr(data_queue_handle, &attr);
						printf("[LoggerThread] Queue %s currently holds %ld messages\n",QMain,attr.mq_curmsgs);   

							// Clear buffer
						memset(logmsg0, 0, sizeof(LogMsg));
					}
					pthread_mutex_unlock(&mainQ_mutex);
				}
			}
			
			printf("flag mask in main %d\n",flag_mask);
			printf("flag mask copy in main%d\n",flag_mask_copy);
			if(flag_mask_copy & SIGINT_EVENT)
			{
				printf("closing and breaking from main thread after sigint \n");
				//pthread_mutex_unlock(&dataQ_mutex);
				mq_close(main_queue_handle);
				printf("break from while main thread \n");
				//break;
			}
			
			
			
			
			retval = pthread_join(sighandThread,NULL);
    if(retval != 0)
    {
        printf("pthread join failed, error code - %d\n", retval);
    }
	printf("%s\n","sighand joined");
	
	retval = pthread_join(lightThread,NULL);
    if(retval != 0)
    {
        printf("pthread join failed, error code - %d\n", retval);
    }
	printf("%s\n","Light joined");
	
	retval = pthread_join(tempThread,NULL);
    if(retval != 0)
    {
        printf("pthread join failed, error code - %d\n", retval);
    }
	printf("%s\n","Temp joined");
	
	retval = pthread_join(loggerThread,NULL);
    if(retval != 0)
    {
        printf("pthread join failed, error code - %d\n", retval);
    }
	printf("%s\n","Logger joined");
			
		break;

		
	}
	printf("main while ends \n");
	
	
	retval = pthread_mutex_destroy(&dataQ_mutex);
	if(retval != 0)
	{
		printf("mutex destroy failed, error code - %d\n", retval);
	}
	retval = pthread_mutex_destroy(&mainQ_mutex);
	if(retval != 0)
	{
		printf("mutex destroy failed, error code - %d\n", retval);
	}
	else
		printf("Mutex destroyed \n");
	
	retval = pthread_mutex_destroy(&logQ_mutex);
	if(retval != 0)
	{
		printf("mutex destroy failed, error code - %d\n", retval);
	}
	else
		printf("Mutex destroyed \n");
	
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