#ifndef NEXUSIO_MFTP_WD_C
#define NEXUSIO_MFTP_WD_C
/*****************************************************************************/
#include "nexusio_mftp_wd.h"
#include "nexusio_mftp_cs.h"
/*****************************************************************************/
static struct
{
	int flags;
	int state;

	struct
	{
		int flag;
		int sock;
		int time;

	}   items[8];

	pthread_t       thrd;
	pthread_mutex_t lock;

}   _nexusio_mftp_watch_dog_zone;
/*****************************************************************************/
void * _nexusio_mftp_watch_dog_thrd_func(void *argp)
{
	int f;
	int i;
	void *p ;

	/*  unused parameter */ 
	p = argp;
	
	for(f = 0; f < 1;)
	{
		usleep(750000);

		pthread_mutex_lock(&_nexusio_mftp_watch_dog_zone.lock);

		for(i = 0; i < 8; i ++)
		{
			if(_nexusio_mftp_watch_dog_zone.items[i].flag < 0)
				continue;

			if(_nexusio_mftp_watch_dog_zone.items[i].sock < 0)
				continue;

			if(_nexusio_mftp_watch_dog_zone.items[i].time < 9)
			{
				_nexusio_mftp_watch_dog_zone.items[i].time ++;
			}
			else
			if(_nexusio_mftp_watch_dog_zone.items[i].flag < 1)
			{
				nexusio_mftp_tcp_ctrl_null(_nexusio_mftp_watch_dog_zone.items[i].sock);

				_nexusio_mftp_watch_dog_zone.items[i].time = 0;
			}
			else
			{
#ifdef  MFTP_WD_AUTO_SHUT

			nexusio_mftp_tcp_shut_down(
				&_nexusio_mftp_watch_dog_zone.items[i].sock);

				_nexusio_mftp_watch_dog_zone.items[i].flag = 0;
				_nexusio_mftp_watch_dog_zone.items[i].time = 0;

#endif/*MFTP_WD_AUTO_SHUT*/
			}
		}

		f = _nexusio_mftp_watch_dog_zone.flags;

		pthread_mutex_unlock(&_nexusio_mftp_watch_dog_zone.lock);
	};

	return NULL;
}
/*****************************************************************************/
int nexusio_mftp_watch_dog_start_thread(void)
{
	int i;
	pthread_attr_t attr;

	for(i = 0; i < 8; i ++)
	{
		_nexusio_mftp_watch_dog_zone.items[i].flag = -1;
		_nexusio_mftp_watch_dog_zone.items[i].sock = -1;
		_nexusio_mftp_watch_dog_zone.items[i].time =  0;
	}

	_nexusio_mftp_watch_dog_zone.flags = 0;
	_nexusio_mftp_watch_dog_zone.state = 1;

	pthread_mutex_init(&_nexusio_mftp_watch_dog_zone.lock, NULL);

	pthread_attr_init (&attr);
	pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&_nexusio_mftp_watch_dog_zone.thrd, &attr,
	     _nexusio_mftp_watch_dog_thrd_func, NULL);

	pthread_attr_destroy (&attr);

	return 1 ;
}
/*****************************************************************************/
int nexusio_mftp_watch_dog_close_thread(void)
{
	pthread_mutex_lock(&_nexusio_mftp_watch_dog_zone.lock);

	_nexusio_mftp_watch_dog_zone.flags = 1;

	pthread_mutex_unlock(&_nexusio_mftp_watch_dog_zone.lock);

	pthread_join(_nexusio_mftp_watch_dog_zone.thrd, NULL);

	return(1);
}
/*****************************************************************************/
int nexusio_mftp_watch_dog_alloc(int s)
{
	int i;

	pthread_mutex_lock(&_nexusio_mftp_watch_dog_zone.lock);

	for(i = 7; i >= 0; i --)
	{
		if(_nexusio_mftp_watch_dog_zone.items[i].flag < 0)
		{
			_nexusio_mftp_watch_dog_zone.items[i].sock = s;
			_nexusio_mftp_watch_dog_zone.items[i].flag = 0;
			_nexusio_mftp_watch_dog_zone.items[i].time = 0;

			break;
		}
	}

	pthread_mutex_unlock(&_nexusio_mftp_watch_dog_zone.lock);

	return i;
}
/*****************************************************************************/
int nexusio_mftp_watch_dog_enter(int n)
{
	if(n >= 0 && n <= 7)
	{
		pthread_mutex_lock(&_nexusio_mftp_watch_dog_zone.lock);

		_nexusio_mftp_watch_dog_zone.items[n].flag = 1;
		_nexusio_mftp_watch_dog_zone.items[n].time = 0;

		pthread_mutex_unlock(&_nexusio_mftp_watch_dog_zone.lock);

		return 1;
	}

	return 0;
}
/*****************************************************************************/
int nexusio_mftp_watch_dog_leave(int n)
{
	if(n >= 0 && n <= 7)
	{
		pthread_mutex_lock(&_nexusio_mftp_watch_dog_zone.lock);

		_nexusio_mftp_watch_dog_zone.items[n].flag = 0;
		_nexusio_mftp_watch_dog_zone.items[n].time = 0;

		pthread_mutex_unlock(&_nexusio_mftp_watch_dog_zone.lock);

		return 1;
	}

	return 0;
}
/*****************************************************************************/
int nexusio_mftp_watch_dog_clean(int n)
{
	if(n >= 0 && n <= 7)
	{
		pthread_mutex_lock(&_nexusio_mftp_watch_dog_zone.lock);

		_nexusio_mftp_watch_dog_zone.items[n].sock = -1;
		_nexusio_mftp_watch_dog_zone.items[n].flag = -1;
		_nexusio_mftp_watch_dog_zone.items[n].time =  0;

		pthread_mutex_unlock(&_nexusio_mftp_watch_dog_zone.lock);

		return 1;
	}

	return 0;
}
/*****************************************************************************/
#endif/*MFTP_WD_C*/
