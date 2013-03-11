#ifndef SONG_DEC_C
#define SONG_DEC_C
/*****************************************************************************/
#include "nexusio_mftp_io.h"
#include "nexusio_file_io.h"
#include "nexusio_cryp_ec.h"
/*****************************************************************************/
#define KEY_FOR_ENC_KEY "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define KEY_FOR_DEC_KEY "12345haimei67890"
#define XKTV_MUSIC_HM "752 Hai Mei V4 ENC"
#define XKTV_VERSION_V8 "V8_K3 USE_BY_HH_USE AAC MEDIA"
/*****************************************************************************/
int NEXUSIO_Song_Mftp_Default(char *str , int iLen)
{
	int i = 0 , iCount = 0;
	char szbuf[512];
	char *pBuf = NULL;
	char *pStr = NULL;
	
	pBuf = szbuf ;
	
	memset(szbuf,0x00,sizeof(szbuf));
	
	pStr = str;
	
	for (i = 0 ; i<iLen ;i++,pStr++)
	{
		if(*pStr == 0x5f || *pStr == 0x2e || *pStr == 0x20)
		{	
			continue;
		}
		iCount ++;
		if (iCount%2)
			*pBuf = *pStr + 1;
		else
			*pBuf = *pStr - 1;
		
		if ( *pBuf == 0x60)
		{
			*pBuf = 0x38;
		}
		pBuf++;
	}
	
	memset(str,0x00,30);
	memcpy(str,szbuf,iCount);
	return iCount ;
}
/*****************************************************************************/
int NEXUSIO_Song_M2z_Ready(mxz_t *mxz)
{	
	if(!mxz)
		return 0;
	
	mp_init(&mxz->gx);
	mp_init(&mxz->gy);

	mp_init(&mxz->aa);
	mp_init(&mxz->bb);
	mp_init(&mxz->pp);

	mp_init(&mxz->sk);
	mp_init(&mxz->rx);
	mp_init(&mxz->ry);
	mp_init(&mxz->rc);
	mp_read_radix(&mxz->gx, "188DA80EB03090F67CBF20EB43A18800F4FF0AFD82FF1012", 16);
	mp_read_radix(&mxz->gy, "07192B95FFC8DA78631011ED6B24CDD573F977A11E794811", 16);

	mp_read_radix(&mxz->aa, "-3", 10);
	mp_read_radix(&mxz->bb, "64210519E59C80E70FA7E9AB72243049FEB8DEECC146B9B1", 16);
	mp_read_radix(&mxz->pp, "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFF", 16);
	mp_read_radix(&mxz->sk, mxz->spk_text, 36);

	return 1;
}
/*****************************************************************************/

int NEXUSIO_Song_M2z_Check(char *spk_text , int iLen)
{
	char spk_key[256];	
	char *qq_sptr = spk_key;
	char *pk_sptr = spk_text;
	int iCount = 0 , iError =0;

	memset(spk_text,0x00,iLen);
	memset(spk_key,0x00,sizeof(spk_key));
	strcpy(spk_key,KEY_FOR_ENC_KEY);

	strcpy(spk_key,XKTV_MUSIC_HM);
	iError += NEXUSIO_Song_Mftp_Default(spk_key,13);

	memcpy(spk_key+iError,H4X_SONG_NAME,3);
	iError += NEXUSIO_Song_Mftp_Default(spk_key+iError,3);

	memcpy(spk_key+iError,mftp_music_gethh,11);
	iError += NEXUSIO_Song_Mftp_Default(spk_key+iError,11);

	memcpy(spk_key+iError,"V32H",4);
	iError +=NEXUSIO_Song_Mftp_Default(spk_key+iError,4);

	while(*qq_sptr && iCount < iLen)
	{
		iCount ++ ;
		if(*qq_sptr >= '0' && *qq_sptr <= '9')
		{
			*pk_sptr = *qq_sptr;
		}
		else
		if(*qq_sptr >= 'A' && *qq_sptr <= 'Z')
		{
			*pk_sptr = *qq_sptr;
		}
		else
		if(*qq_sptr >= 'a' && *qq_sptr <= 'z')
		{
			*pk_sptr = *qq_sptr - ('a' - 'A');
		}
		else
		{
			return 0;
		}
		
		qq_sptr ++;
		pk_sptr ++;
	};
	
	return 1;
}

int NEXUSIO_Song_V8_Check(char *spk_text , int iLen)
{
	char *p = spk_text;
	int my = iLen ;

	(void *)p;
	(void *)my;
	
	return 1;
}

/*****************************************************************************/
int NEXUSIO_Song_M2z_Do(char *spk_text , int iLen , int mode)
{
	char spk_key[256];	
	char *qq_sptr = spk_key;
	char *pk_sptr = spk_text;
	int iCount = 0 , iError =0;
	int iMymode = mode ;

	(void *)iMymode;
	
	memset(spk_text,0x00,iLen);
	memset(spk_key,0x00,sizeof(spk_key));
	strcpy(spk_key,KEY_FOR_ENC_KEY);

	strcpy(spk_key,XKTV_VERSION_V8);
	iError += NEXUSIO_Song_Mftp_Default(spk_key,13);

	memcpy(spk_key+iError,V8_SONG_NAME,4);
	iError += NEXUSIO_Song_Mftp_Default(spk_key+iError,4);

	memcpy(spk_key+iError,mftp_music_gethh,11);
	iError += NEXUSIO_Song_Mftp_Default(spk_key+iError,11);

	memcpy(spk_key+iError,"V8K3VERSION",6);
	iError +=NEXUSIO_Song_Mftp_Default(spk_key+iError,6);

	while(*qq_sptr && iCount < iLen)
	{
		iCount ++ ;
		if(*qq_sptr >= '0' && *qq_sptr <= '9')
		{
			*pk_sptr = *qq_sptr;
		}
		else
		if(*qq_sptr >= 'A' && *qq_sptr <= 'Z')
		{
			*pk_sptr = *qq_sptr;
		}
		else
		if(*qq_sptr >= 'a' && *qq_sptr <= 'z')
		{
			*pk_sptr = *qq_sptr - ('a' - 'A');
		}
		else
		{
			return 0;
		}
		
		qq_sptr ++;
		pk_sptr ++;
	};
	
	return 1;
}
/*****************************************************************************/
int NEXUSIO_Song_M2z_Dec_Data(mxz_t *mxz, char *srcBuf , unsigned int iLen)
{
	char *buff_mem;
	char *buff_ptr;
	int pos = 0 , i = 0 ;
	unsigned int z;
	unsigned char *head_ptr = NULL;
	unsigned char *step_ptr =NULL;
	unsigned char *code_ptr =NULL;
	
	if(!mxz || !srcBuf)
		return -4;
	
	head_ptr = (unsigned char *)mxz->xor_head;
	step_ptr = (unsigned char *)mxz->xor_step;
	code_ptr = (unsigned char *)mxz->xor_code;

	if(!head_ptr || !step_ptr || !code_ptr)
		return -4;

	buff_mem = buff_ptr = srcBuf;
	
	if(strcmp(buff_ptr, "H4XMedia") != 0)
	{
		if(strcmp(buff_ptr, "T4HMEDIA") != 0)
			return -5;
	}

	buff_ptr += 9;

	z = strtoul(buff_ptr, 0, 16);

	if(iLen < (z+pos))
		return -5;

	buff_ptr += 9;
	
	/* raw_name **************************************/

	buff_ptr += (strlen(buff_ptr) + 1);

	for(i = 0; i < 120; i += 20)
	{
		mp_read_radix(&mxz->rx, buff_ptr, 36);
		buff_ptr += (strlen(buff_ptr) + 1);

		mp_read_radix(&mxz->ry, buff_ptr, 36);
		buff_ptr += (strlen(buff_ptr) + 1);

		mp_read_radix(&mxz->rc, buff_ptr, 36);
		buff_ptr += (strlen(buff_ptr) + 1);

		if(NEXUSIO_Ecc_Decrypt(&mxz->gx, &mxz->gy, &mxz->aa, 
			&mxz->bb, &mxz->pp, &mxz->sk, head_ptr, 
			&mxz->rx, &mxz->ry, &mxz->rc) == 0)
		{
			return -6;
		}

		mp_read_radix(&mxz->rx, buff_ptr, 36);
		buff_ptr += (strlen(buff_ptr) + 1);

		mp_read_radix(&mxz->ry, buff_ptr, 36);
		buff_ptr += (strlen(buff_ptr) + 1);

		mp_read_radix(&mxz->rc, buff_ptr, 36);
		buff_ptr += (strlen(buff_ptr) + 1);

		if(NEXUSIO_Ecc_Decrypt(&mxz->gx, &mxz->gy, &mxz->aa,
			&mxz->bb,&mxz->pp, &mxz->sk, step_ptr,
			&mxz->rx, &mxz->ry, &mxz->rc) == 0)
		{
			return -6;
		}
		
		mp_read_radix(&mxz->rx, buff_ptr, 36);
		buff_ptr += (strlen(buff_ptr) + 1);

		mp_read_radix(&mxz->ry, buff_ptr, 36);
		buff_ptr += (strlen(buff_ptr) + 1);

		mp_read_radix(&mxz->rc, buff_ptr, 36);
		buff_ptr += (strlen(buff_ptr) + 1);

		if(NEXUSIO_Ecc_Decrypt(&mxz->gx, &mxz->gy, &mxz->aa,
			&mxz->bb, &mxz->pp, &mxz->sk, code_ptr,
			&mxz->rx, &mxz->ry, &mxz->rc) == 0)
		{
			return -6;
		}

		head_ptr += 20;
		step_ptr += 20;
		code_ptr += 20;
	}

	/* cpy_data **************************************/
	z += 9;     /* mark_len */
	z += 9;     /* size_len */
	z %= 49152; /* head_dat */

	if(z > 0)
		return iLen - z;
	else
		return 0;
	
}
/*****************************************************************************/

int  NEXUSIO_Song_M2z_Dec_First(mxz_t *mxz, char *srcBuf , int iLen)
{
	long *long_ptr;
	long *long_max;
	int isPos = 0;
	int iePos = 8192;
	int iCount , i ;
	unsigned long head ;
	unsigned long step ;
	unsigned long code ;

	if(!mxz || !srcBuf)
		return 0;
	
	iCount = iLen / 8192;
	
	for(i =0 ; i < iCount ; i++)
	{
		long_ptr = (long *)(srcBuf + isPos);
		long_max = (long *)(srcBuf + iePos);

		head = mxz->xor_head[mxz->chip_num];
		step = mxz->xor_step[mxz->chip_num];
		code = mxz->xor_code[mxz->chip_num];

		for(long_ptr += head ; long_ptr <  long_max ; long_ptr += step)
		{
			*long_ptr ^= code;
		}

		mxz->chip_num = (mxz->chip_num + 1) % 30;
		isPos += 8192;
		iePos += 8192;

	}

	return 1;
}

/*****************************************************************************/
int NEXUSIO_Song_M2z_Dec(nexusio_mftp_io *io_priv_data)
{
	int iPos = 0;
	
	if(io_priv_data->dex_stat == 3)
	{
		iPos = NEXUSIO_Song_M2z_Dec_Data(&io_priv_data->priData , io_priv_data->pck_buff,49152);
		if(iPos <= 0)
			return 0;
		
		io_priv_data->pck_size = iPos;
		io_priv_data->pck_posi = 49152 - iPos;
		io_priv_data->dex_stat = 4;
	}
	else
	{
		NEXUSIO_Song_M2z_Dec_First(&io_priv_data->priData , io_priv_data->pck_buff,49152);	
	}

	return 1;
}

/*****************************************************************************/
int NEXUSIO_Song_M2z_File_Dec(nexusio_file_io *io_priv_data)
{
	int iPos = 0;
	
	if(io_priv_data->dex_stat == 3)
	{
		iPos = NEXUSIO_Song_M2z_Dec_Data(&io_priv_data->priData , io_priv_data->pck_buff,49152);
		if(iPos <= 0)
			return 0;
		
		io_priv_data->pck_size = iPos;
		io_priv_data->pck_posi = 49152 - iPos;
		io_priv_data->dex_stat = 4;
	}
	else
	{
		NEXUSIO_Song_M2z_Dec_First(&io_priv_data->priData , io_priv_data->pck_buff,49152);	
	}

	return 1;
}
/*****************************************************************************/
#endif
