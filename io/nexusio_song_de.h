#ifndef SONG_DEC_H
#define SONG_DEC_H
/*****************************************************************************/
#include "nexusio_mftp_io.h"
#include "nexusio_file_io.h"
#include "nexusio_cryp_ec.h"
/*****************************************************************************/
int NEXUSIO_Song_Mftp_Default(char *str , int iLen);
/*****************************************************************************/
int NEXUSIO_Song_M2z_Ready(mxz_t *mxz);
/*****************************************************************************/
int NEXUSIO_Song_M2z_Check(char *spk_text , int iLen);
/*****************************************************************************/
int NEXUSIO_Song_M2z_Do(char *spk_text , int iLen , int mode);
int NEXUSIO_Song_V8_Check(char *spk_text , int iLen);
/*****************************************************************************/
int NEXUSIO_Song_M2z_Dec(nexusio_mftp_io *io_priv_data);
/*****************************************************************************/
int NEXUSIO_Song_M2z_File_Dec(nexusio_file_io *io_priv_data);
/*****************************************************************************/
#endif
