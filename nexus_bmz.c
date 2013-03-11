#ifndef NEXUS_BMZ_C
#define NEXUS_BMZ_C
/***********************************************************************************************/
#include "nexus_bmz.h"
#include "nexus_osd.h"
/*****************************************************************************/
void NEXUSAPP_Suface_Osd_Ready(t_osd_picture_ragb *osd, int w, int h)
{
	osd->rect_lx = 0;
	osd->rect_ty = 0;
	osd->rect_rx = w - 1;
	osd->rect_by = h - 1;

	osd->fi = 0;
	osd->fc = 1;

	osd->lx = 0;
	osd->ty = 0;
	osd->wx = w;
	osd->hy = h;
}

/*****************************************************************************/
int NEXUSAPP_Suface_Osd_Alloc(t_osd_picture_ragb *osd, int w, int h,int size)
{
	NEXUSAPP_Suface_Osd_Free(osd);

	NEXUSAPP_Suface_Osd_Ready(osd,w,h);
	
	if(size)
	{
		osd->buffSize = size;
		osd->ps = malloc(osd->buffSize);
	}
	else
	{	
		osd->buffSize = w * h * sizeof(int);
		osd->ps = malloc(osd->buffSize);
	}
	return 1;
}
/*****************************************************************************/
int NEXUSAPP_Suface_Osd_Fill(t_osd_picture_ragb *osd, int c)
{
	if(osd->ps != NULL)
	{
		memset(osd->ps, c, osd->buffSize);
		return 0;
	}

	return -1;
}
/*****************************************************************************/
int NEXUSAPP_Suface_Osd_Free(t_osd_picture_ragb *osd)
{
	if(osd->ps != NULL)
	{
		free(osd->ps);

		osd->ps = NULL;
		osd->wx = 0;
		osd->hy = 0;

		return 1;
	}

	return 0;
}
/***********************************************************************************************/
void NEXUSAPP_Surface_Bmzimage_Clean(struct bmzimage_t  *axbmp)
{
	int i;

	if(!axbmp || !axbmp->rs)
		return;
	
	if(axbmp->wx <= 0 || axbmp->hy <= 0
	|| axbmp->pf >= 8 || axbmp->am != 66)
		return;

	for(i = 0; i < axbmp->hy; i ++)
	{
		if(axbmp->rs[i])
			free(axbmp->rs[i]);
	}
	
	if(axbmp->rs)
		free(axbmp->rs);

	axbmp->am = 0;
	axbmp->wx = 0;
	axbmp->hy = 0;
}

int NEXUSAPP_Surface_Bmzimage_Alloc(struct bmzimage_t *axbmp, int pf, int wx, int hy)
{
	int i;
	const int BMZ_PIXEL_SIZEOF[8] = { 2, 2, 2, 2, 3, 3, 4, 4};
	
	if(!axbmp)
		return -1;
		
	if(axbmp->wx == wx
	&& axbmp->hy == hy
	&& axbmp->pf == pf)
		return -1;

	if(axbmp->am >= 67)
		return -1;

	if(wx < 1 || hy < 1
	|| pf < 0 || pf > 7)
		return -1;

	NEXUSAPP_Surface_Bmzimage_Clean(axbmp);

	axbmp->wx = wx;
	axbmp->hy = hy;
	axbmp->pf = pf;
	axbmp->am = 66;

	wx *= BMZ_PIXEL_SIZEOF[pf];
	wx += 0x00000004;
	wx &= 0x7ffffffc;
	
	axbmp->rs = (unsigned char ** )malloc(hy << 2);
	
	if(!axbmp)
		return -1;
	
	for(i = 0; i < hy; i ++)
	{
		axbmp->rs[i] = (unsigned char *)malloc(wx);
		if(!axbmp->rs[i])
		{
			goto fail;
		}
	}

	return 0;

fail:	
	for(i=i-1;i>=0;i--)
		free(axbmp->rs[i]);

	free(axbmp->rs);
		
	return -1;
	
}
long NEXUSAPP_Surface_Load_Bmz(struct bmzimage_t *axbmp, int pf, char *bbuff, int csize)
{
	unsigned char *bmp_buff = NULL;
	unsigned char *bmp_line;
	unsigned char *zip_buff;

	unsigned long  bmp_size , zip_size , int_size =4;

	short pic_w ,pic_h , zip_h;
	short pic_y , pic_i , pic_x;

	if(!axbmp || !bbuff)
		return -1;
		
	if(axbmp->am <= 66)
	{
		if(csize < 16)
			goto fail;

		/*skip head flag*/
		bbuff += 4;
		csize -= 4;

		memcpy(&pic_w, bbuff, sizeof(pic_w));
		bbuff += sizeof(pic_w);
		csize -= sizeof(pic_w);

		memcpy(&pic_h, bbuff, sizeof(pic_h));
		bbuff += sizeof(pic_h);
		csize -= sizeof(pic_h);

		memcpy(&zip_h, bbuff, sizeof(zip_h));
		bbuff += sizeof(zip_h);
		csize -= sizeof(zip_h);

		if(NEXUSAPP_Surface_Bmzimage_Alloc(axbmp, pf, pic_w, pic_h))
			goto fail;
	
		bmp_size = zip_h * pic_w * 4;
		bmp_buff = (unsigned char *)malloc(bmp_size);

		if(!bmp_buff)
			goto fail;
		
		for(pic_y = 0; pic_y < pic_h;)
		{
			if(csize < (long)int_size)
				goto fail;

			memcpy(&zip_size, bbuff, int_size);
			bbuff += int_size;
			csize -= int_size;

			if(csize < (long)zip_size)
				goto fail;

			zip_buff = (unsigned char *)bbuff;
			bbuff += zip_size;
			csize -= zip_size;

			if(uncompress(bmp_buff, &bmp_size,zip_buff, zip_size) != Z_OK)
				goto fail;

			if((pic_i = pic_y + zip_h) > pic_h)
				pic_i = pic_h;

			switch(pf)
			{
				case BMZ_COLOR_ARGB32:
					for(bmp_line = bmp_buff; pic_y < pic_i; ++ pic_y)
					{
						memcpy(axbmp->rs[pic_y], bmp_line, 4 * pic_w);

						bmp_line += (4 * pic_w);
					}
					break;
				case BMZ_COLOR_BGRA32:
					for(; pic_y < pic_i; ++ pic_y)
					{
						bmp_line = axbmp->rs[pic_y];

						for(pic_x = 0; pic_x < pic_w; pic_x ++)
						{
							*bmp_line ++ = bmp_buff[3];
							*bmp_line ++ = bmp_buff[2];
							*bmp_line ++ = bmp_buff[1];
							*bmp_line ++ = bmp_buff[0];

							bmp_buff += 4;
						}
					}
					break;
				default:
					goto fail;
			}
		}

		if(bmp_buff != NULL)
			free(bmp_buff);

		return 0;
fail:
		if(bmp_buff != NULL)
			free(bmp_buff);
	}

	return -1;
}
/***********************************************************************************************/
#endif
