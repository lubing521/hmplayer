#ifndef NEXUSIO_MFTP_IO_H
#define NEXUSIO_MFTP_IO_H
/*****************************************************************************/
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <semaphore.h>
#include "nexusio_cryp_ec.h"
/*****************************************************************************/
#define H4X_SONG_NAME "H4X"
#define V8_SONG_NAME "T4AH"
#define  mftp_music_gethh "Select Enc "
/*****************************************************************************/
//#define DBG_IO(x)  {if(IODEBUG)printf x ;}
/*****************************************************************************/
typedef struct nexusIoContext_t {
    int flags;
    int is_streamed;  		/**< true if streamed (no seek possible), default = false */
    int max_packet_size;  /**< if non zero, the stream is packetized with this max packet size */
    void *priv_data;
    char *fullname;		
    char *filename; 		
} nexusIoContext;
/*****************************************************************************/
typedef struct
{
	mp_int gx;
	mp_int gy;

	mp_int aa;
	mp_int bb;
	mp_int pp;

	mp_int sk;
	mp_int rx;
	mp_int ry;
	mp_int rc;
	int chip_num;
	char spk_text[256];
	unsigned long xor_head[30];
	unsigned long xor_step[30];
	unsigned long xor_code[30];

}   mxz_t;
/*****************************************************************************/
#define MFTP_BUFFER_COUNT_MAX 16
#define MFTP_BUFFER_DATA_MAX 983040 /*2097152*/ /* 2 * 1024 *1024 */

#define MFTP_BUFFER_DATA_STATE_NULL			0
#define MFTP_BUFFER_DATA_STATE_READEND		1
#define MFTP_BUFFER_DATA_STATE_READY		2
/*****************************************************************************/
typedef struct mftp_data
{
	unsigned int uiState; 		/* 0: buffer max , 1:buffer max , 2:buffer read now*/
	unsigned long ulPack_pos;
	unsigned long ulPack_size;
	unsigned char szBuff[MFTP_BUFFER_DATA_MAX];
	pthread_mutex_t     v_mutex_lock;
}mftp_data;
/*****************************************************************************/
typedef struct mftp_buffer
{
	mftp_data data[MFTP_BUFFER_COUNT_MAX];
}mftp_buffer;
/*****************************************************************************/
typedef struct
{
	int tcp_sock;
	int rmt_file;
	int err_stat;
	int mwd_numb;
	int thread_state;

	long pck_posi;
	long pck_size;
	unsigned int uiBufferIndex;
	int uiBufferTotal;
	mftp_buffer buffer;
	char pck_buff[49152];

	unsigned long dex_stat;
	unsigned long dex_head;
	unsigned long dex_step;
	unsigned char dex_code;
	mxz_t priData;
	pthread_t			v_thread_ident;
	pthread_mutex_t     v_thread_mutex;
	sem_t tBufferWaitSem;
}   nexusio_mftp_io;
/*****************************************************************************/
typedef struct NEXUSIOProtocol_t {
	const char *name;
	int (*url_open)(nexusIoContext *h, const char *url, int flags);
	int (*url_read)(nexusIoContext *h, unsigned char *buf, int size);
	int (*url_write)(nexusIoContext *h, unsigned char *buf, int size);
	int64_t (*url_seek)(nexusIoContext *h, int64_t pos, int whence);
	int (*url_close)(nexusIoContext *h);
	struct URLProtocol *next;
	int (*url_read_pause)(nexusIoContext *h, int pause);
	int64_t (*url_read_seek)(nexusIoContext *h, int stream_index,int64_t timestamp, int flags);
	int (*url_get_file_handle)(nexusIoContext *h);
} NEXUSIOProtocol;
/*****************************************************************************/
NEXUSIOProtocol *nexusio_mftp_prtocol_get(void);
/*****************************************************************************/
#endif
