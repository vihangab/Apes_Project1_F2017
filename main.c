#include "msg_queue.h"

int main()
{
	flag_mask = 0;
	flag_mask_copy = 0;
	heartbeat_flag = 0x0;
	int32_t retval = 0;
  int32_t alive;
	char threadName[20];
	pthread_mutex_init(&dataQ_mutex,NULL);
	pthread_mutex_init(&logQ_mutex,NULL);
	pthread_mutex_init(&mainQ_mutex,NULL);
	pthread_mutex_init(&lightQ_mutex,NULL);

	pthread_cond_init(&condvar,NULL);

	initialize_queue(QLog,&data_queue_handle);
  initialize_queue(QTemp_Request,&tempreq_queue_handle);
  initialize_queue(QMain,&main_queue_handle);
  initialize_queue(QLight_Request,&lightreq_queue_handle);

	signal(SIGINT, sighandler_sigint);
	signal(SIGUSR1, sighandler_sigusr1);
  retval = pthread_create(&loggerThread,NULL,&LoggerThread,NULL);
  //retval = 1;
	if(retval != 0)
	{
		printf("Thread Creation failed, error code - %d\n", retval);
		pinSet(led_path);
	}
	sleep(1);
	retval = pthread_create(&tempThread,NULL,&TempThread,NULL);
	if(retval != 0)
	{
		printf("Thread Creation failed, error code - %d\n", retval);
		pinSet(led_path);
	}
	retval = pthread_create(&lightThread,NULL,&LightThread,NULL);
	if(retval != 0)
	{
		printf("Thread Creation failed, error code - %d\n", retval);
		pinSet(led_path);
	}
  retval =	pthread_create(&sighandThread,NULL,&SighandThread,NULL); //common signal handler for all events
	if(retval != 0)
	{
		printf("Thread Creation failed, error code - %d\n", retval);
		pinSet(led_path);

	}
	create_interval_timer(2);

	char buffer[MAX_SEND_BUFFER];
	unsigned int msgprio = 2;
	uint32_t bytes_sent=0;
  struct mq_attr attr;
	uint32_t bytes_read = 0;
	uint32_t data_to_log=0;

	LogMsg *logmsg0,*logmsg00;
	if(create_log_struct(&logmsg0)!=DONE){
		printf("%s\n","Error creating struct");
	}
	if(create_log_struct(&logmsg00)!=DONE){
		printf("%s\n","Error creating struct");
	}
  timer_t heartTimer;
	logmsg0->requestID=100;
  //create_timer(6,heartTimer);
	while(1)
	{
		pthread_mutex_lock(&dataQ_mutex);
		//keep waiting for timer signal
		pthread_cond_wait(&condvar,&dataQ_mutex);
		pthread_mutex_unlock(&dataQ_mutex);
		//Assuming that we are getting the condition signal

		if(flag_mask & SIGINT_EVENT)
		{
			printf("%s\n","Main got SIGINT");
			mq_close(main_queue_handle);
			//free(logmsg0);
			break;
		}

    if(flag_mask_copy & TIMER_EVENT){
      if((alive = pthread_kill(tempThread, 0))==3) //Check with ERSCH flag
			{
				printf("%s\n","Temp Thread Dead");
				strcpy(threadName,taskNames[1]);
				pinSet(led_path);
			}else if((alive = pthread_kill(loggerThread, 0))==3)
			{
				printf("%s\n","Logger Thread Dead");
				strcpy(threadName,taskNames[3]);
				pinSet(led_path);
			}else if((alive = pthread_kill(lightThread, 0))==3)
			{
				printf("%s\n","Light Thread Dead");
				strcpy(threadName,taskNames[2]);
				pinSet(led_path);
			}

			if(alive == 3){
				logmsg00->sourceId = MAIN_TASK;
				logmsg00->level = ALERT;
				logmsg00->requestID = LOG_DATA;
				logmsg00->timestamp = time(NULL);
				sprintf(logmsg00->payload,"%s thread dead",threadName);
				pthread_mutex_lock(&logQ_mutex);
				if ((bytes_sent = mq_send (data_queue_handle,(const char*)&logmsg00, sizeof(LogMsg), 2)) != 0) //can be changed later to light queue handle
				{
					perror ("[MainThread] Sending:");
				}
				pthread_mutex_unlock(&logQ_mutex);
			}
		}

		if(flag_mask_copy & ASYNC_EVENT)
		{
			mq_getattr(main_queue_handle, &attr);
			while(attr.mq_curmsgs > 0)
			{
				pthread_mutex_lock(&mainQ_mutex);
				bytes_read = mq_receive(main_queue_handle, (char *)&logmsg0, MAX_SEND_BUFFER+1, &data_to_log);
				pthread_mutex_unlock(&mainQ_mutex);
				if (bytes_read == -1)
				{
					perror("[MainThread] Failed to recieve:");
				}
				else
				{
					mq_getattr(main_queue_handle, &attr);

					printf("[MainThread] Queue %s currently holds %ld messages\n",QMain,attr.mq_curmsgs);
					if(logmsg0->requestID!=0)
					{
						printf ("[MainThread]Source: %d \n", logmsg0->sourceId);
						printf ("[MainThread]Log Level: %d \n", logmsg0->level);
						printf ("[MainThread]Payload: %s \n", logmsg0->payload);
						// Get the attributes

						printf ("[MainThread]Timestamp: %s \n", ctime(&logmsg0->timestamp));
							// Clear buffer

							switch (logmsg0->requestID)
							{
		          	case HEARTBEAT:
								    break;

                case DECIDE:
								    break;
								default:
								    break;
		          }
          }

					//memset(logmsg0, (uint8_t)'\0', sizeof(LogMsg));
				}

			}
		}
	}

  printf("%s\n","Main while ends");
	retval = pthread_join(tempThread,NULL);
	if(retval != 0)
	{
		printf("pthread join failed, error code - %d\n", retval);
	}
  printf("%s\n","Temp nikala");
	retval = pthread_join(lightThread,NULL);
	if(retval != 0)
	{
		printf("pthread join failed, error code - %d\n", retval);
	}
  printf("%s\n","Light nikala");
	retval = pthread_join(loggerThread,NULL);
	if(retval != 0)
	{
		printf("pthread join failed, error code - %d\n", retval);
	}
  printf("%s\n","Logger nikala");
	retval = pthread_join(sighandThread,NULL);
	if(retval != 0)
	{
		printf("pthread join failed, error code - %d\n", retval);
	}
  printf("%s\n","Sab nikla nikala");
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

	retval = pthread_mutex_destroy(&logQ_mutex);
	if(retval != 0)
	{
		printf("mutex destroy failed, error code - %d\n", retval);
	}

	retval =  pthread_cond_destroy(&condvar);
	if(retval != 0)
	{
		printf("cond destroy failed, error code - %d\n", retval);
	}

	printf("Joined all threads, destroyed mutexes and condition vars...\n");
	printf("Return zero from main()...\n");
	return 0;
}
