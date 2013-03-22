#ifndef NEXUSIO_FILE_IO_C
#define NEXUSIO_FILE_IO_C
/*****************************************************************************/
#include <unistd.h>
#include <stdio.h>
/*****************************************************************************/
#include "nexusio_file_io.h"
#include "nexusio_song_de.h"
#include "nexusio_mftp_io.h"
#include "../nexus_player.h"
/*****************************************************************************/
void nexusio_file_ext(char *dstr, const char *sstr)
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
void nexusio_file_pack_dex(nexusio_file_io *io_priv_data)
{
	unsigned long i;
	unsigned long h;
	unsigned long s;
	unsigned char c;


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
		NEXUSIO_Song_M2z_File_Dec(io_priv_data);
		return ;
	}

	
	h = io_priv_data->dex_head;
	s = io_priv_data->dex_step;
	c = io_priv_data->dex_code;

	for(i = h; i < 49152; i += s)
	{
		io_priv_data->pck_buff[i] ^= c;
	}
	
}
/*****************************************************************************/
static int nexusio_file_open(nexusIoContext *h, const char *fname, int mode)
{
	FILE *rmt_file = NULL ;
	nexusio_file_io *io_priv_data = NULL;
	char ccode[256];
	int iLen = strlen(fname);

	/* mode check only , no use now*/
	if(mode == -100)
		goto error_out;

	if(iLen <= 0)
		goto error_out;
	
	rmt_file = fopen(fname , "r");
	if(rmt_file == NULL)
		goto error_out;

	fseek(rmt_file,0,0);
	/* get song type */
	nexusio_file_ext(ccode,fname);

	h->priv_data = malloc(sizeof(nexusio_file_io));
	if(h->priv_data == NULL)
		goto error_out;

	io_priv_data = (nexusio_file_io *)h->priv_data;
	io_priv_data->rmt_file = rmt_file;
	io_priv_data->err_stat = 0;
	io_priv_data->pck_posi = 0;
	io_priv_data->pck_size = 0;


	/*	add  copy song type to nexusIoContext->filename , must be free in close */
	iLen = strlen(ccode) + 1;
	h->filename = malloc(iLen);
	if(h->filename == NULL)
		goto error_out;

	memset(h->filename,0x00,iLen);
	strcpy(h->filename,ccode);

	/* 	end add */

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

	h->is_streamed = 0;

	return(0);

error_out:
	if(h->priv_data)
	{
		free(h->priv_data);
		h->priv_data = NULL;
	}

	if(h->filename)
	{
		free(h->filename);
		h->filename = NULL;
	}
	return(-1);
}

/*****************************************************************************/
static int64_t  nexusio_file_seek(nexusIoContext *h, int64_t pos, int whence)
{
	nexusio_file_io *io_priv_data = (nexusio_file_io *)h->priv_data;
	if(io_priv_data == NULL)
		return(-1);
	if(io_priv_data->rmt_file == NULL)
		return(-1);
	if(io_priv_data->err_stat > 0)
		return(0);

	io_priv_data->pck_posi = 0;
	io_priv_data->pck_size = 0;

	memset(io_priv_data->pck_buff,0x00,sizeof(io_priv_data->pck_buff));
	
	return fseek(io_priv_data->rmt_file,pos,whence);
}	

/*****************************************************************************/
static int nexusio_file_read(nexusIoContext *h, unsigned char *buf, int len)
{
	int res = 0;

	nexusio_file_io *io_priv_data = (nexusio_file_io *)h->priv_data;
	
	if(io_priv_data == NULL)
		return(-1);
	if(io_priv_data->rmt_file == NULL)
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

		io_priv_data->pck_posi = 0;
		io_priv_data->pck_size = 
			fread(io_priv_data->pck_buff,1,49152,io_priv_data->rmt_file);

		nexusio_file_pack_dex(io_priv_data);
		
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
		

		io_priv_data->pck_posi = 0;
		io_priv_data->pck_size = 
			fread(io_priv_data->pck_buff,1,49152,io_priv_data->rmt_file);	
						

		
		nexusio_file_pack_dex(io_priv_data);

		if(io_priv_data->pck_size < 0)
		{
			io_priv_data->err_stat = 1;
			return 0;
		}

		return(res + len);
	}
}
/*****************************************************************************/
static int nexusio_file_writ(nexusIoContext *h, unsigned char *buf, int len)
{
	unsigned char *writeBuff;
	int iRet ;
	nexusio_file_io *io_priv_data;
		
	io_priv_data = (nexusio_file_io *)h->priv_data;
	writeBuff = buf ;
	iRet = len;
	return(-1);
}
/*****************************************************************************/
static int nexusio_file_shut(nexusIoContext *h)
{
	nexusio_file_io * io_priv_data = (nexusio_file_io *)h->priv_data;

	if(io_priv_data == NULL)
		return(-1);

	if(io_priv_data->rmt_file == NULL)
		return(-1);

	if(io_priv_data->err_stat > 0)
		goto SHUT_DOWN;


	fclose(io_priv_data->rmt_file);
	io_priv_data->rmt_file = NULL;
	
SHUT_DOWN:

	if(io_priv_data)
		free(io_priv_data);
	
	h->priv_data = NULL;

	if(h->filename)
		free(h->filename);

	h->filename = NULL;

	return(0);
}
/*****************************************************************************/
NEXUSIOProtocol nexusio_file_protocol =
{
	"nexusio_file_io",
	nexusio_file_open,
	nexusio_file_read,
	nexusio_file_writ,
	nexusio_file_seek, 
	nexusio_file_shut,
	NULL,
	NULL,
	NULL,
	NULL
};
/*****************************************************************************/
NEXUSIOProtocol *nexusio_file_prtocol_get(void)
{
	return & nexusio_file_protocol;
}
/*****************************************************************************/
#endif/*MFTP_IO_H*/
