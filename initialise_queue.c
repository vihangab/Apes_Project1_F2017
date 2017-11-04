#include "msg_queue.h"

void initialize_queue()
{
	pthread_mutex_init(&dataQ_mutex,NULL);
	pthread_cond_init(&condvar,NULL);
	
	// unlink the queue if it exists - debug
	mq_unlink (QTemp);
	//mq_unlink (QTemp_Request);
	//mq_unlink (QLight_Request);
	
	attr.mq_maxmsg = 20;
	attr.mq_msgsize = MAX_SEND_BUFFER;
	attr.mq_flags = 0;

	
	data_queue_handle=  mq_open(QTemp,O_RDWR | O_CREAT,0666,&attr);
	//tempreq_queue_handle = mq_open(QTemp_Request,O_RDWR | O_CREAT,0666,&attr);
	//lightreq_queue_handle = mq_open(QLight_Request,O_RDWR | O_CREAT,0666,&attr);

	if (data_queue_handle == -1)
	{
		perror ("[TempThread] Error Opening TempQ:");
	} 

	if (tempreq_queue_handle == -1)
	{
		perror ("[TempThread] Error Opening Temp Request Q: ");
	}
	
	if (lightreq_queue_handle == -1)
	{
		perror ("[LightThread] Error Opening Light Request Q: ");
	}
	printf("init queue return \n");
}