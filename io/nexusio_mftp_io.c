#ifndef NEXUSIO_MFTP_IO_C
#define NEXUSIO_MFTP_IO_C
/*****************************************************************************/
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
/*****************************************************************************/
#include "../nexus_player.h"
#include "nexusio_mftp_io.h"
#include "nexusio_mftp_wd.h"
#include "nexusio_mftp_cs.h"
#include "nexusio_song_de.h"
/*****************************************************************************/
#ifdef FFMPEG_IO
#ifndef Malloc
#define Malloc av_malloc
#endif/*Malloc*/

#ifndef Free
#define Free av_free
#endif/*Free*/
#else
#ifndef Malloc
#define Malloc malloc
#endif/*Malloc*/

#ifndef Free
#define Free free
#endif/*Free*/
#endif
/*****************************************************************************/
/*
main()
{
	NEXUSIOProtocol *myIo;
	nexusIoContext ioContext;
	int iRet = 0;
	unsigned int iLen = 0;

	unsigned char szBuff[1024];

	nexusio_mftp_watch_dog_start_thread();
	
	myIo = nexusio_mftp_prtocol_get();

	DBG_IO(("get io protocol : %s , name = %s\n",myIo?"OK":"NULL",myIo->name));

restart:
	
	iRet =myIo->url_open(&ioContext,"mftp://1//#HD0001",0);
	DBG_IO(("iRet = %d\n",iRet));

	if(iRet < 0)
		exit(0);

	iRet = 1;
	iLen  = 0;
	while( iRet > 0 )
	{
		memset(szBuff,0x00,sizeof(szBuff));
		iRet = myIo->url_read(&ioContext,szBuff,sizeof(szBuff));
		iLen +=iRet;
		DBG_IO(("read iRet = %d \n",iLen));
	}

	DBG_IO(("read end ,goto close !\n"));

	iRet =  myIo->url_close(&ioContext);
	DBG_IO(("url_close = %d  !\n" , iRet));

	goto restart;
	
	
	exit(0);
	while(1)
		sleep(1000);
}
*/
/*****************************************************************************/
void nexusio_mftp_file_ext(char *dstr, const char *sstr)
{
	int i = 0;
	int k = 0;

	for(;;)
	{
		switch(*sstr)
		{
			case '\0':
				dstr[i] = *sstr;
				return;

			case '\\':
			case '/' :
				i = 0;
				k = 0;
				break;

			case '.' :
				i = 0;
				k = 1;
				break;

			default:
				dstr[i] = *sstr;
				i+= k;
				break;
		}

		sstr ++;
	}
}
/*****************************************************************************/
void nexusio_mftp_pack_dex(nexusio_mftp_io *io_priv_data)
{

	if(io_priv_data->dex_stat < 2)
	{
		if(!strcmp("H4XMedia", io_priv_data->pck_buff))
			io_priv_data->dex_stat = 3;

		if(!strcmp("T4HMEDIA", io_priv_data->pck_buff))
		{
			NEXUSIO_Song_M2z_Do(io_priv_data->priData.spk_text,sizeof(io_priv_data->priData.spk_text),1);
			NEXUSIO_Song_M2z_Ready(&io_priv_data->priData);
			io_priv_data->dex_stat = 3;
		}
	}

	if(io_priv_data->dex_stat < 1)
		return;

	if(io_priv_data->dex_stat < 2)
	{		
		if(strncasecmp("LYDLHLCLXZXWCZHW", io_priv_data->pck_buff, 16) == 0
		|| strncasecmp("MZYLJZQZYHHLXHSB", io_priv_data->pck_buff, 16) == 0)
		{
			int i;
			int k;

			io_priv_data->dex_stat = 2;

			for(k = i = 0; i < 16; i ++)
			{
				if(io_priv_data->pck_buff[i] >= 'A'
				&& io_priv_data->pck_buff[i] <= 'Z')
				{
					k *= 2;
					k += 1;
				}
				else
					k *= 2;
			}

			io_priv_data->pck_posi = 16;

			io_priv_data->dex_code = ((k & 0x0000ff) ^ 170);
			io_priv_data->dex_step = ((k & 0x000f00) >>  8) + 512;
			io_priv_data->dex_head = ((k & 0x00f000) >> 12) + 256;

			return;
		}
			
	}

	if(io_priv_data->dex_stat == 3 
	||io_priv_data->dex_stat == 4)
	{
		NEXUSIO_Song_M2z_Dec( io_priv_data);
		return ;
	}

	
	unsigned long i;
	unsigned long h = io_priv_data->dex_head;
	unsigned long s = io_priv_data->dex_step;
	unsigned char c = io_priv_data->dex_code;

	for(i = h; i < 49152; i += s)
	{
		io_priv_data->pck_buff[i] ^= c;
	}
	
}

/*****************************************************************************/

static int nexusio_mftp_data_read(nexusio_mftp_io *io_priv_data, unsigned char *buf, int len)
{

	if(io_priv_data == NULL)
		return(-1);

	if(io_priv_data->tcp_sock < 0)
		return(-1);

	if(io_priv_data->rmt_file < 1)
		return(-1);

	if(io_priv_data->err_stat > 0)
		return(0);

	int res = 0;


READ_DATA:

	if(len < io_priv_data->pck_size)
	{
		memcpy(buf, io_priv_data->pck_buff +io_priv_data->pck_posi, len);
		io_priv_data->pck_posi += len;
		io_priv_data->pck_size -= len;

		return(res + len);
	}
	else
	if(len > io_priv_data->pck_size)
	{	
		memcpy(buf, io_priv_data->pck_buff + io_priv_data->pck_posi , io_priv_data->pck_size);

		buf += io_priv_data->pck_size;
		len -= io_priv_data->pck_size;
		res += io_priv_data->pck_size;

		nexusio_mftp_watch_dog_enter(io_priv_data->mwd_numb);

		io_priv_data->pck_posi = 0;
		io_priv_data->pck_size = nexusio_mftp_tcp_file_read(
		            io_priv_data->tcp_sock,
		            io_priv_data->rmt_file,
		            io_priv_data->pck_buff);	

		//DBG_IO(("read pkt size = %d , res = %d \n",(int)io_priv_data->pck_size , res));
			
		nexusio_mftp_watch_dog_leave(io_priv_data->mwd_numb);

		nexusio_mftp_pack_dex(io_priv_data);

		if(io_priv_data->pck_size < 0)
		{	
			   io_priv_data->err_stat = 1;
			   return 0;
		}

		if(io_priv_data->pck_size < 1)
		{
			if( res > 0)
				return res;
			else
				return -1;
		}

		goto READ_DATA;
	}
	else
	{
		memcpy(buf, io_priv_data->pck_buff +
		            io_priv_data->pck_posi, len);

		nexusio_mftp_watch_dog_enter(io_priv_data->mwd_numb);

		io_priv_data->pck_posi = 0;
		io_priv_data->pck_size = nexusio_mftp_tcp_file_read(
		            io_priv_data->tcp_sock,
		            io_priv_data->rmt_file,
		            io_priv_data->pck_buff);

		nexusio_mftp_watch_dog_leave(io_priv_data->mwd_numb);

		nexusio_mftp_pack_dex(io_priv_data);

		if(io_priv_data->pck_size < 0)
		{
			io_priv_data->err_stat = 1;
			return 0;
		}

		return(res + len);
	}
}

/*****************************************************************************/
static void *nexusio_mftp_buff_recv(void *argp)
{
	nexusio_mftp_io *io_priv_data = (nexusio_mftp_io *)argp;
	int i , iRet ;
	
	if(io_priv_data == NULL)
		return NULL;

	pthread_mutex_lock(&io_priv_data->v_thread_mutex);
	io_priv_data->thread_state = 1;
	pthread_mutex_unlock(&io_priv_data->v_thread_mutex);

	DBG_IO(("nexusio_mftp_buff_recv up \n"));
	
	while(io_priv_data != NULL && io_priv_data->thread_state > 0)
	{
		for(i=0;i<MFTP_BUFFER_COUNT_MAX && io_priv_data->thread_state > 0 ;i++)
		{
			usleep(1000);

			if(io_priv_data->err_stat)
				goto buff_recv_end;
			
			pthread_mutex_lock(&io_priv_data->buffer.data[i].v_mutex_lock);
			
			if(io_priv_data->buffer.data[i].uiState < MFTP_BUFFER_DATA_STATE_READY)
			{
				// must be lock 
				io_priv_data->buffer.data[i].uiState = MFTP_BUFFER_DATA_STATE_NULL;

				pthread_mutex_unlock(&io_priv_data->buffer.data[i].v_mutex_lock);
				
				memset(&io_priv_data->buffer.data[i],0x00,sizeof(mftp_data));
				
				iRet = nexusio_mftp_data_read(io_priv_data,io_priv_data->buffer.data[i].szBuff,MFTP_BUFFER_DATA_MAX);
				//DBG_IO(("nexusio_mftp_data_read : %d , size = %d , index = %d , err_stat = %d\n",uiReadLen,(int)io_priv_data->pck_size,io_priv_data->uiBufferIndex,io_priv_data->err_stat));
				if(iRet > 0 )
				{
					io_priv_data->buffer.data[i].ulPack_size = iRet;
					io_priv_data->buffer.data[i].ulPack_pos = 0;
					pthread_mutex_lock(&io_priv_data->buffer.data[i].v_mutex_lock);
					io_priv_data->buffer.data[i].uiState = MFTP_BUFFER_DATA_STATE_READY;
					io_priv_data->uiBufferTotal += iRet;
					pthread_mutex_unlock(&io_priv_data->buffer.data[i].v_mutex_lock);
				}
				else
				{
					if(iRet <= 0 || io_priv_data->err_stat > 0)
						goto buff_recv_end;
					
				}
								
			}
			else
				pthread_mutex_unlock(&io_priv_data->buffer.data[i].v_mutex_lock);
		}

		if(io_priv_data->err_stat)
			goto buff_recv_end;
		
		sem_wait(&io_priv_data->tBufferWaitSem);
		
	}
	
buff_recv_end:
	pthread_mutex_lock(&io_priv_data->v_thread_mutex);
	io_priv_data->thread_state = -1;
	pthread_mutex_unlock(&io_priv_data->v_thread_mutex);
	DBG_IO(("nexusio :nexusio_mftp_buff_recv thread out \n"));
	return NULL;
	
}
/*****************************************************************************/
static int nexusio_mftp_buff_start(nexusio_mftp_io *io_priv_data)
{
	int i  , iState = -1;;
	pthread_attr_t attr;
	pthread_t recvbuffthread;

	if(io_priv_data == NULL)
		return -1;

	pthread_mutex_init(&io_priv_data->v_thread_mutex,NULL);
	sem_init(&io_priv_data->tBufferWaitSem,0,0);

	for(i = 0 ; i < MFTP_BUFFER_COUNT_MAX ; i++)
	{
		pthread_mutex_init(&io_priv_data->buffer.data[i].v_mutex_lock,NULL);
	}

	
	pthread_attr_init (&attr);
	pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&recvbuffthread,&attr, nexusio_mftp_buff_recv,io_priv_data);
	pthread_attr_destroy (&attr);

	i = 0 ;
	
	while(1)
	{
		pthread_mutex_lock(&io_priv_data->v_thread_mutex);
		iState = io_priv_data->thread_state ;
		pthread_mutex_unlock(&io_priv_data->v_thread_mutex);

		if(iState > 0 )
			break;

		if( i > 100)
			break;

		i++;

		usleep(10000);
	}

	if(iState<= 0)
		return -1;
	else
		return 0;
}

/*****************************************************************************/
static int nexusio_mftp_file_open(nexusIoContext *h, const char *fname, int mode)
{
	SADDR saddr[32];
	int count = 0;
	int iLen;

	char scode[256];
	char ccode[256];

	int udp_sock = -1;
	int tcp_sock = -1;
	int rmt_file = -1;
	int mwd_numb;
	
	/* mode check only , no use now*/
	if(mode == -100)
		return -1;
	
	if(sscanf(fname, "mftp://%c//%31s", ccode, scode) < 2)
		return(-1);
	
	nexusio_mftp_udp_ready(&udp_sock);

	DBG_IO(("nexusio_mftp_file_open --> code = %s \n ",scode));
	
	if(scode[0] == '#' || scode[0] == '@')
		nexusio_mftp_udp_find_song(udp_sock, saddr, &count, scode);
	else
		nexusio_mftp_udp_find_srvr(udp_sock, saddr, &count);

	DBG_IO(("nexusio_mftp_file_open -->get server , server count = %d\n ",count));
	
	nexusio_mftp_udp_clean(&udp_sock);

	DBG_IO(("nexusio_mftp_file_open -->nexusio_mftp_udp_clean \n"));
	
	if(nexusio_mftp_tcp_auto_cnnt(&tcp_sock, saddr, count, ccode[0]) <= 0)
		return(-1);

	DBG_IO(("nexusio_mftp_file_open -->nexusio_mftp_tcp_auto_cnnt \n"));
	
	mwd_numb = nexusio_mftp_watch_dog_alloc(tcp_sock);
	
	nexusio_mftp_watch_dog_enter(mwd_numb);

	DBG_IO(("nexusio_mftp_file_open -->nexusio_mftp_watch_dog_alloc = %d ,scodec = %s\n",mwd_numb,scode));
	
	rmt_file = nexusio_mftp_tcp_file_open(tcp_sock, scode,sizeof(scode), NEXUSIO_MFTP_ORDP);

	DBG_IO(("nexusio_mftp_file_open -->nexusio_mftp_tcp_file_open , handle = %d\n",rmt_file));

	nexusio_mftp_watch_dog_leave(mwd_numb);	

	if(rmt_file > 0)
	{
		nexusio_mftp_io *io_priv_data;

		h->priv_data = Malloc(sizeof(nexusio_mftp_io));
		if(h->priv_data == NULL)
			goto error_out;
		
		io_priv_data = (nexusio_mftp_io *)h->priv_data;
		memset( io_priv_data , 0x00 , sizeof(nexusio_mftp_io));
	

		io_priv_data->tcp_sock = tcp_sock;
		io_priv_data->rmt_file = rmt_file;
		io_priv_data->mwd_numb = mwd_numb;
		io_priv_data->err_stat = 0;
		io_priv_data->pck_posi = 0;
		io_priv_data->pck_size = 0;

		io_priv_data->thread_state = 0;
		io_priv_data->uiBufferIndex = 0;
		io_priv_data->uiBufferTotal = 0;

		nexusio_mftp_file_ext(ccode, scode);

		DBG_IO(("nexusio_mftp_file_open -->nexusio_mftp_file_ext , ccdoe = %s , len = %d\n",ccode,strlen(ccode)));


/*	add  copy song type to nexusIoContext->filename , must be free in close */
		iLen = strlen(ccode) + 1;
		h->filename = Malloc(iLen);
		if(h->filename == NULL)
			goto error_out;

		memset(h->filename,0x00,iLen);
		strcpy(h->filename,ccode);

		iLen = strlen(scode)+1;
		h->fullname = Malloc(iLen);
		if(h->fullname == NULL)
			goto error_out;
		
		memset(h->fullname,0x00,iLen);
		strcpy(h->fullname,scode);


/* 	end add */

		DBG_IO(("nexusio_mftp_file_open -->filename= %s \n",h->filename));

		memset(&io_priv_data->priData,0x00,sizeof(mxz_t));
		NEXUSIO_Song_M2z_Check(io_priv_data->priData.spk_text,sizeof(io_priv_data->priData.spk_text));
		NEXUSIO_Song_M2z_Ready(&io_priv_data->priData);
		
		if(strcasecmp("M1X", ccode) == 0
		|| strcasecmp("M2X", ccode) == 0
		|| strcasecmp("M4X", ccode) == 0
		|| strcasecmp("MPX", ccode) == 0
		|| strcasecmp("VOX", ccode) == 0)
		{
			io_priv_data->dex_stat = 1;
			io_priv_data->dex_head = 0;
			io_priv_data->dex_step = 512;
			io_priv_data->dex_code = 0x13;
		}
		else if(strcasecmp(H4X_SONG_NAME, ccode) == 0)	
		{
			io_priv_data->dex_stat = 3;
		}
		else
		{
			io_priv_data->dex_stat = 0;
			io_priv_data->dex_head = 65536;
			io_priv_data->dex_step = 65536;
			io_priv_data->dex_code = 0;
		}
		
		DBG_IO(("nexusio_mftp_file_open -->dex set\n"));

		nexusio_mftp_tcp_file_prefetch_start(tcp_sock, rmt_file);

		DBG_IO(("nexusio_mftp_file_open --> nexusio_mftp_tcp_file_prefetch_start\n"));


		h->is_streamed = 1;

		io_priv_data->thread_state = -1;

		if(nexusio_mftp_buff_start(io_priv_data))
			goto error_out;

		return(0);
	}

	nexusio_mftp_watch_dog_clean(mwd_numb);

error_out:
	if(h->priv_data)
	{
		Free(h->priv_data);
		h->priv_data = NULL;
	}

	if(h->filename)
	{
		Free(h->filename);
		h->filename = NULL;
	}
	
	if(h->fullname)
	{
		Free(h->fullname);
		h->fullname = NULL;
	}
	return(-1);
}
/*****************************************************************************/
static int nexusio_mftp_file_read(nexusIoContext *h, unsigned char *buf, int len)
{
	int pos , size ;
	int readlen = 0 , readpos = 0 , copylen = 0 , state;
	unsigned char *pData = NULL;
	nexusio_mftp_io *io_priv_data = (nexusio_mftp_io *)h->priv_data;
	
	if(io_priv_data == NULL || buf == NULL)
		return(-1);
	
	if(io_priv_data->tcp_sock < 0)
		return(-1);

	if(io_priv_data->rmt_file < 1)
		return(-1);

	if(io_priv_data->err_stat > 0)
		return(0);

	if(len <= 0)
		return 0;

	readlen = len ;

	//DBG_IO(("nexusio_mftp_file_read uiBufferTotal = %d  thread state = %d \n" , io_priv_data->uiBufferTotal , io_priv_data->thread_state));
	// must be set thread_state , when pthread start 
	while(io_priv_data->thread_state > 0 || io_priv_data->uiBufferTotal > 0)
	{
		//DBG_IO(("nexusio_mftp_file_read Total =  %d , thread_state = %d\n",io_priv_data->uiBufferTotal , io_priv_data->thread_state));
		
		if(io_priv_data->err_stat )
			break;
		
		pthread_mutex_lock(&io_priv_data->v_thread_mutex);
		state = io_priv_data->buffer.data[io_priv_data->uiBufferIndex].uiState;
		pthread_mutex_unlock(&io_priv_data->v_thread_mutex);

		//DBG_IO(("nexusio_mftp_file_read index = %d , state = %d \n",io_priv_data->uiBufferIndex, state));
		
		if( state != MFTP_BUFFER_DATA_STATE_READY)
		{
			sem_post(&io_priv_data->tBufferWaitSem);
			usleep(10000);
			continue;
		}
		pos = io_priv_data->buffer.data[io_priv_data->uiBufferIndex].ulPack_pos;
		size = io_priv_data->buffer.data[io_priv_data->uiBufferIndex].ulPack_size;
		
		if(size <= 0)
		{
			sem_post(&io_priv_data->tBufferWaitSem);
			usleep(10000);
			continue;
		}
		
		pData = io_priv_data->buffer.data[io_priv_data->uiBufferIndex].szBuff + pos;

		if(pData == NULL)
			return(-1); 

		copylen = readlen > size ?size:readlen;

		memcpy(buf + readpos, pData, copylen);

		io_priv_data->buffer.data[io_priv_data->uiBufferIndex].ulPack_pos += copylen;
		io_priv_data->buffer.data[io_priv_data->uiBufferIndex].ulPack_size -= copylen;
		readpos += copylen;
		readlen -= copylen;

		pthread_mutex_lock(&io_priv_data->v_thread_mutex);
		io_priv_data->uiBufferTotal -= readpos;
		pthread_mutex_unlock(&io_priv_data->v_thread_mutex);

		//DBG_IO(("nexusio_mftp_file_read Total =  %d \n",io_priv_data->uiBufferTotal));
		
		// check this page data size left 
		if( io_priv_data->buffer.data[io_priv_data->uiBufferIndex].ulPack_size == 0)
		{
			pthread_mutex_lock(&io_priv_data->v_thread_mutex);
			io_priv_data->buffer.data[io_priv_data->uiBufferIndex].uiState = MFTP_BUFFER_DATA_STATE_READEND;
			pthread_mutex_unlock(&io_priv_data->v_thread_mutex);

			io_priv_data->uiBufferIndex ++;
			//DBG_IO(("nexusio_mftp_file_read index up = %d , state = %d \n",io_priv_data->uiBufferIndex, state));
			if(io_priv_data->uiBufferIndex >= MFTP_BUFFER_COUNT_MAX)
				io_priv_data->uiBufferIndex = 0;	

			sem_post(&io_priv_data->tBufferWaitSem);
		}

		if(readlen == 0)
			return readpos;

	}

	DBG_IO(("nexusio_mftp_file_read len = %d , get =%d \n",len,readpos ));

	if(io_priv_data->err_stat)
		return 0;
	
	return -1;
	
}

/*****************************************************************************/
static int64_t  nexusio_mftp_file_seek(nexusIoContext *h, int64_t pos, int whence)
{
	int64_t offset = 0;
	
	nexusio_mftp_io *io_priv_data = (nexusio_mftp_io *)h->priv_data;
	if(io_priv_data == NULL)
		return(-1);
	if(io_priv_data->tcp_sock < 0)
		return(-1);
	if(io_priv_data->rmt_file < 1)
		return(-1);
	if(io_priv_data->err_stat > 0)
		return(0);

	if(pos < 49152 )
		offset = 49152;
	else
		offset = (pos /49152 + 1) * 49152;

	io_priv_data->pck_posi = 0;
	io_priv_data->pck_size = 0; 
		
	return nexusio_mftp_tcp_file_seek(
					io_priv_data->tcp_sock,
					io_priv_data->rmt_file,
					offset,
					whence
					);
}	

/*****************************************************************************
static int nexusio_mftp_file_read(nexusIoContext *h, unsigned char *buf, int len)
{
	int res = 0;

	nexusio_mftp_io *io_priv_data = (nexusio_mftp_io *)h->priv_data;
	
	if(io_priv_data == NULL)
		return(-1);
	if(io_priv_data->tcp_sock < 0)
		return(-1);
	if(io_priv_data->rmt_file < 1)
		return(-1);
	if(io_priv_data->err_stat > 0)
		return(0);

READ_DATA:

	if(len < io_priv_data->pck_size)
	{
		memcpy(buf, io_priv_data->pck_buff + io_priv_data->pck_posi, len);
		io_priv_data->pck_posi += len;
		io_priv_data->pck_size -= len;
		
		return(res + len);
	}
	else if(len > io_priv_data->pck_size)
	{
		memcpy(buf, io_priv_data->pck_buff + io_priv_data->pck_posi, io_priv_data->pck_size);
		buf += io_priv_data->pck_size;
		len -= io_priv_data->pck_size;
		res += io_priv_data->pck_size;

		nexusio_mftp_watch_dog_enter(io_priv_data->mwd_numb);

		io_priv_data->pck_posi = 0;
		io_priv_data->pck_size = nexusio_mftp_tcp_file_read(
								io_priv_data->tcp_sock,
								io_priv_data->rmt_file,
								io_priv_data->pck_buff);
		
		nexusio_mftp_watch_dog_leave(io_priv_data->mwd_numb);
		nexusio_mftp_pack_dex(io_priv_data);
		
		if(io_priv_data->pck_size < 0)
		{
			io_priv_data->err_stat = 1;
			return 0;
		}

		if(io_priv_data->pck_size < 1)
			return(res);

		goto READ_DATA;
	}
	else    
	{
		memcpy(buf, io_priv_data->pck_buff + io_priv_data->pck_posi, len);
		
		nexusio_mftp_watch_dog_enter(io_priv_data->mwd_numb);

		io_priv_data->pck_posi = 0;
		io_priv_data->pck_size = nexusio_mftp_tcp_file_read(
								io_priv_data->tcp_sock,
								io_priv_data->rmt_file,
								io_priv_data->pck_buff);
		
		nexusio_mftp_watch_dog_leave(io_priv_data->mwd_numb);
		
		nexusio_mftp_pack_dex(io_priv_data);

		if(io_priv_data->pck_size < 0)
		{
			io_priv_data->err_stat = 1;
			return 0;
		}

		return(res + len);
	}
}
*/
/*****************************************************************************/
static int nexusio_mftp_file_writ(nexusIoContext *h, unsigned char *buf, int len)
{
	unsigned char *writeBuff;
	int iRet ;
	nexusio_mftp_io *io_priv_data;
		
	io_priv_data = (nexusio_mftp_io *)h->priv_data;
	writeBuff = buf ;
	iRet = len;
	return(-1);
}
/*****************************************************************************/
static int nexusio_mftp_file_shut(nexusIoContext *h)
{
	
	int iCount = 0 ;
	nexusio_mftp_io * io_priv_data = (nexusio_mftp_io *)h->priv_data;

	if(io_priv_data == NULL)
		return(-1);
/*
	if(io_priv_data->tcp_sock < 0)
		return(-1);

	if(io_priv_data->rmt_file < 1)
		return(-1);

	if(io_priv_data->err_stat > 0)
		goto SHUT_DOWN;
*/
	io_priv_data->err_stat = 1;

	pthread_mutex_lock(&io_priv_data->v_thread_mutex);
	if( io_priv_data->thread_state > 0)
	{
		io_priv_data->thread_state = 0;
		pthread_mutex_unlock(&io_priv_data->v_thread_mutex);
		
		DBG_IO(("nexusio :nexusio_mftp_file_shut wait \n"));
		
		while(io_priv_data->thread_state != -1 && iCount < 2000)
		{
			sem_post(&io_priv_data->tBufferWaitSem);
			usleep(1000);
			iCount++;
		}
	}
	else
		pthread_mutex_unlock(&io_priv_data->v_thread_mutex);

	nexusio_mftp_watch_dog_enter(io_priv_data->mwd_numb);

	DBG_IO(("nexusio :nexusio_mftp_file_shut nexusio_mftp_watch_dog_enter \n"));
	
	nexusio_mftp_tcp_file_prefetch_clean(io_priv_data->tcp_sock);

	DBG_IO(("nexusio :nexusio_mftp_file_shut nexusio_mftp_tcp_file_prefetch_clean \n"));

	nexusio_mftp_tcp_file_shut(io_priv_data->tcp_sock,io_priv_data->rmt_file);

	DBG_IO(("nexusio :nexusio_mftp_file_shut nexusio_mftp_tcp_file_shut \n"));

	nexusio_mftp_watch_dog_leave(io_priv_data->mwd_numb);

//SHUT_DOWN:

	nexusio_mftp_tcp_shut_down(&io_priv_data->tcp_sock);

	DBG_IO(("nexusio :nexusio_mftp_file_shut nexusio_mftp_tcp_shut_down \n"));

	nexusio_mftp_watch_dog_clean(io_priv_data->mwd_numb);

	DBG_IO(("nexusio :nexusio_mftp_file_shut nexusio_mftp_watch_dog_clean \n"));

	
	if(io_priv_data)
		Free(io_priv_data);
	
	h->priv_data = NULL;

	if(h->filename)
		Free(h->filename);

	h->filename = NULL;

	DBG_IO(("nexusio :nexusio_mftp_file_shut succ \n"));
	
	return(0);
}
/*****************************************************************************/
NEXUSIOProtocol nexusio_mftp_protocol=
{
	"nexusio_mftp_io",
	nexusio_mftp_file_open,
	nexusio_mftp_file_read,
	nexusio_mftp_file_writ,
	nexusio_mftp_file_seek, 
	nexusio_mftp_file_shut,
	NULL,
	NULL,
	NULL,
	NULL
};
/*****************************************************************************/
NEXUSIOProtocol *nexusio_mftp_prtocol_get(void)
{
	return & nexusio_mftp_protocol;
}
/*****************************************************************************/
#endif/*MFTP_IO_H*/
