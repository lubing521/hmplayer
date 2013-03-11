#ifndef NEXUS_BMZ_H
#define NEXUS_BMZ_H
/***********************************************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
/***********************************************************************************************/
typedef struct bmzimage_t
{
	int lx;
	int ty;
	int wx;
	int hy;

	unsigned char   am;
	unsigned char   pf;
	unsigned char **rs;
} bmzimage;
/***********************************************************************************************/
typedef struct
{
	int buffSize;
	short rect_lx;
	short rect_ty;
	short rect_rx;
	short rect_by;

	short lx;
	short ty;
	short wx;
	short hy;

	short fc;
	short fi;
	char *ps;
}   t_osd_picture_ragb;
/***********************************************************************************************/
void NEXUSAPP_Surface_Bmzimage_Clean(struct bmzimage_t  *axbmp);
int 	NEXUSAPP_Surface_Bmzimage_Alloc(struct bmzimage_t *axbmp, int pf, int wx, int hy);
long NEXUSAPP_Surface_Load_Bmz(struct bmzimage_t *axbmp, int pf, char *bbuff, int csize);
/***********************************************************************************************/
int NEXUSAPP_Suface_Osd_Alloc(t_osd_picture_ragb *osd, int w, int h,int size);
int NEXUSAPP_Suface_Osd_Fill(t_osd_picture_ragb *osd, int c);
int NEXUSAPP_Suface_Osd_Free(t_osd_picture_ragb *osd);
/***********************************************************************************************/
#endif
