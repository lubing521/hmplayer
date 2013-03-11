#ifndef NEXUS_XPIZ_H
#define NEXUS_XPIZ_H
/***********************************************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include "nexus_bmz.h"
/***********************************************************************************************/
typedef struct
{
	short fi;
	short lx;
	short ty;
	short rx;
	short by;
	long  zf_size;
	char *zf_data;
} nexus_xxpif;
/***********************************************************************************************/
typedef struct nexus_xxpiz_t
{
	short  logic_count;
	short  logic_index;

	short  frame_logic;

	short  frame_count;
	short  frame_index;

	nexus_xxpif *frame_point;
	nexus_xxpif *frame_array;

	bmzimage  frame_axbmp;
	bmzimage  frame_bxbmp;

	char  *frame_cache;

} nexus_xxpiz;
/***********************************************************************************************/
long NEXUSAPP_Xxpiz_Load_From_File(nexus_xxpiz *axpiz, char *bname);
void NEXUSAPP_Xxpiz_Clean_Frame(nexus_xxpiz *axpiz);
void NEXUSAPP_Xxpiz_Reset_Frame(nexus_xxpiz *axpiz);
long NEXUSAPP_Xxpiz_Stepp_Frame(nexus_xxpiz *bxpiz);
/***********************************************************************************************/
#endif
