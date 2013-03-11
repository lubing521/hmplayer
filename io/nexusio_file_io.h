#ifndef NEXUSIO_FILE_IO_H
#define NEXUSIO_FILE_IO_H
/*****************************************************************************/
#include "nexusio_mftp_io.h"
#include "nexusio_cryp_ec.h"
/*****************************************************************************/
typedef struct
{
	FILE *rmt_file;
	int err_stat;
	int mwd_numb;
	int thread_state;

	long pck_posi;
	long pck_size;
	char pck_buff[49152];

	unsigned long dex_stat;
	unsigned long dex_head;
	unsigned long dex_step;
	unsigned char dex_code;
	mxz_t priData;
}   nexusio_file_io;
/*****************************************************************************/
NEXUSIOProtocol *nexusio_file_prtocol_get(void);
/*****************************************************************************/
#endif
