#ifndef NEXUS_XPIZ_C
#define NEXUS_XPIZ_C
/***********************************************************************************************/
#include "nexus_xpiz.h"
#include "nexus_bmz.h"
/***********************************************************************************************/
void NEXUSAPP_Xxpiz_Bmzimage_Reset(struct bmzimage_t *axbmp)
{
	int x_min , x_max;
	int y_min , y_max;
	int psize , y;
	
	const int BMZ_PIXEL_SIZEOF[8] = { 2, 2, 2, 2, 3, 3, 4, 4};

	x_min = 0;
	x_max = axbmp->wx - 1;
	y_min = 0;
	y_max = axbmp->hy - 1;

	psize = BMZ_PIXEL_SIZEOF[axbmp->pf];

	for(y = y_min; y <= y_max; y ++)
	{
		memset(axbmp->rs[y], 0x00 , axbmp->wx * psize );
	}
}
/***********************************************************************************************/
long NEXUSAPP_Xxpiz_Load_From_File(nexus_xxpiz *axpiz , char *bname)
{
	FILE *fp;
	short hz,ft,ii,wx,hy;
	short fi,lx,ty,rx,by;
	long  zsize = 0 , ssize = 4;
	char *zbuff = NULL;

	fp = fopen(bname, "rb");
	if(fp == NULL)
		 return 0;

	fseek(fp, 4, 0);

	fread(&hz, 1, sizeof(short), fp);
	fread(&ft, 1, sizeof(short), fp);

	if(fread(&wx, 1, sizeof(short), fp) < sizeof(short))
		goto fail;

	if(fread(&hy, 1, sizeof(short), fp) < sizeof(short))
		goto fail;

	if(wx < 1 || wx > 1920)
		goto fail;

	if(hy < 1 || hy > 1080)
		goto fail;

	NEXUSAPP_Xxpiz_Clean_Frame(axpiz);
	
	axpiz->frame_array = (nexus_xxpif *)malloc(sizeof(nexus_xxpif) * ft);

	for(ii = fi = 0; fi < ft; ii ++)
	{
		fread(&fi, 1, sizeof(short), fp);
		fread(&lx, 1, sizeof(short), fp);
		fread(&ty, 1, sizeof(short), fp);
		fread(&rx, 1, sizeof(short), fp);
		fread(&by, 1, sizeof(short), fp);

		if(fread(&zsize, 1, ssize, fp) != (size_t)ssize)
			break;

		zbuff = (char *)malloc(zsize);

		if(fread( zbuff, 1, zsize, fp) != (size_t)zsize)
			break;

		axpiz->frame_array[ii].fi = fi;
		axpiz->frame_array[ii].lx = lx;
		axpiz->frame_array[ii].ty = ty;
		axpiz->frame_array[ii].rx = rx;
		axpiz->frame_array[ii].by = by;

		axpiz->frame_array[ii].zf_size = zsize;
		axpiz->frame_array[ii].zf_data = zbuff;

		zsize = 0;
		zbuff = 0;
	}

	if(NULL != zbuff)
		free(zbuff);

	axpiz->logic_count = ft;
	axpiz->logic_index = 0;

	axpiz->frame_count = ii;
	axpiz->frame_index = 0;
	axpiz->frame_point = 0;

	axpiz->frame_axbmp.pf = 6;
	axpiz->frame_bxbmp.pf = 6;

	axpiz->frame_axbmp.am = 67;
	axpiz->frame_bxbmp.am = 67;

	axpiz->frame_axbmp.wx = wx;
	axpiz->frame_bxbmp.wx = wx;

	axpiz->frame_axbmp.hy = hy;
	axpiz->frame_bxbmp.hy = hy;

	axpiz->frame_axbmp.rs = (unsigned char **)malloc(hy << 2);
	axpiz->frame_bxbmp.rs = (unsigned char **)malloc(hy << 2);

	wx <<= 2;

	*axpiz->frame_axbmp.rs = (unsigned char * )calloc(hy , wx);
	*axpiz->frame_bxbmp.rs = (unsigned char * )calloc(hy , wx);

	for(ii = 1; ii < hy; ii ++)
	{
		axpiz->frame_axbmp.rs[ii] =
			axpiz->frame_axbmp.rs[0] + (wx * ii);

		axpiz->frame_bxbmp.rs[ii] =
			axpiz->frame_bxbmp.rs[0] + (wx * ii);
	}

	axpiz->frame_cache = (char *)malloc(hy * wx);

	fclose(fp);
	return(1);
fail:
	fclose(fp);
	return(0);

}
/***********************************************************************************************/
void NEXUSAPP_Xxpiz_Clean_Frame(nexus_xxpiz *axpiz)
{
	int i;
	
	for(i = 0; i < axpiz->frame_count; i ++)
	{
		if(axpiz->frame_array[i].zf_data)
			free(axpiz->frame_array[i].zf_data);
	}

	if( i > 0 )
	{
		if(axpiz->frame_array)
			free(axpiz->frame_array);
		
		if(axpiz->frame_axbmp.rs[0])
			free(axpiz->frame_axbmp.rs[0]);

		if(axpiz->frame_bxbmp.rs[0])
			free(axpiz->frame_bxbmp.rs[0]);
		
		if(axpiz->frame_cache)
			free(axpiz->frame_cache);
	}

	axpiz->logic_count = 0;
	axpiz->logic_index = 0;

	axpiz->frame_count = 0;
	axpiz->frame_index = 0;
	axpiz->frame_point = 0;
}
/***********************************************************************************************/
void NEXUSAPP_Xxpiz_Reset_Frame(nexus_xxpiz *axpiz)
{
	axpiz->logic_index = 0;
	axpiz->frame_logic = 0;
	axpiz->frame_index = 0;
	axpiz->frame_point = 0;

	NEXUSAPP_Xxpiz_Bmzimage_Reset(&axpiz->frame_axbmp);
	NEXUSAPP_Xxpiz_Bmzimage_Reset(&axpiz->frame_bxbmp);
}
/***********************************************************************************************/
void NEXUSAPP_Xxpiz_Decode_Frame(nexus_xxpiz *axpiz)
{
	unsigned long size = 0 ;
/*
	size = (axpiz->frame_axbmp.wx * axpiz->frame_axbmp.hy) * 4;
*/
	size = (axpiz->frame_axbmp.wx * axpiz->frame_axbmp.hy) << 2;

	uncompress(	(unsigned char *)axpiz->frame_cache, &size ,
				(unsigned char *)axpiz->frame_point->zf_data ,
				axpiz->frame_point->zf_size
	);
}
/***********************************************************************************************/
void NEXUSAPP_Xxpiz_Render_Frame(nexus_xxpiz *axpiz)
{
	int ty = axpiz->frame_point->ty;
	int by = axpiz->frame_point->by;
	int lx = axpiz->frame_point->lx;
	int rx = axpiz->frame_point->rx;

	int wx = rx - lx;
	int ll = lx << 2;

	int xx;
	int yy;


	unsigned char *a_point;
	unsigned char *b_point;
	unsigned char *c_point;

	c_point = (unsigned char *)axpiz->frame_cache;

	for(yy = ty; yy <= by; yy ++)
	{
		a_point = axpiz->frame_axbmp.rs[yy] + ll;
		b_point = axpiz->frame_bxbmp.rs[yy] + ll;

		for(xx = wx; xx >= 0; xx --)
		{
			b_point[0] += c_point[0];
			b_point[1] += c_point[1];
			b_point[2] += c_point[2];
			b_point[3] += c_point[3];

			*((long *)a_point) = *((long *)b_point) << 1;

			a_point += 4;
			b_point += 4;
			c_point += 4;
		}
	}


}

long NEXUSAPP_Xxpiz_Stepp_Frame(nexus_xxpiz *bxpiz)
{
	
	if(bxpiz->frame_point != NULL)
	{
		bxpiz->logic_index ++;

		if(bxpiz->logic_index >= bxpiz->logic_count)
			return 3;
render:
		if(bxpiz->logic_index <  bxpiz->frame_logic)
			return 2;

		if(bxpiz->logic_index == bxpiz->frame_logic)
		{
			NEXUSAPP_Xxpiz_Render_Frame(bxpiz);
			return 1;
		}

		bxpiz->frame_index ++;

		if(bxpiz->frame_index >= bxpiz->frame_count)
			return 2;

		bxpiz->frame_point ++;

		bxpiz->frame_logic = bxpiz->frame_point->fi;

		NEXUSAPP_Xxpiz_Decode_Frame(bxpiz);

		goto render;
		
	}
	else
	{
		if(bxpiz->frame_count > 0)
		{
			bxpiz->logic_index = 0;
			bxpiz->frame_logic = 0;
			bxpiz->frame_index = 0;
			bxpiz->frame_point = bxpiz->frame_array;

			NEXUSAPP_Xxpiz_Decode_Frame(bxpiz);
			NEXUSAPP_Xxpiz_Render_Frame(bxpiz);

			return 1;
		}
	}

	return 0;
}

#endif
