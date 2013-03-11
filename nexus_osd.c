#ifndef NEXUS_OSD_C
#define NEXUS_OSD_C
/***********************************************************************************************/
#include "nexus_memory.h"
#include "nexus_osd.h"
#include "nexus_player.h"
#include "nexus_mw.h"
#include "nexus_ctrl.h"
#include "nexus_pizplay.h"
#include "nexus_video_decoder_extra.h"
/***********************************************************************************************/
#define LOGOPIZNAME "LOGOPIZ"
/***********************************************************************************************/
#define abs(X) ((X)>0?(X):-(X))
/***********************************************************************************************/
NEXUS_Rect syncSRect;
NEXUS_Rect syncDRect;
NEXUS_Rect syncSstRect;
NEXUS_Rect syncDstRect;
/***********************************************************************************************/
void NEXUSAPP_Surface_CallBack(void *fb, int unused)
{
	struct av_playpump_t *avp = (struct av_playpump_t*)fb;
	BSTD_UNUSED(unused);
	avp->framebuffFlag = 1;
}


void NEXUSAPP_Surface_2DCallback(void *data, int unused)
{
	BSTD_UNUSED(unused);
	BKNI_SetEvent((BKNI_EventHandle)data);
}

void NEXUSAPP_Surface_2DAsync_Checkpoint(NEXUS_Graphics2DHandle gfx, BKNI_EventHandle event)
{
	NEXUS_Error rc;

	rc=NEXUS_Graphics2D_Checkpoint(gfx, NULL);
	if(rc==NEXUS_SUCCESS) {
	    return;
	}
	if(rc==NEXUS_GRAPHICS2D_QUEUED) {
	    BKNI_WaitForEvent(event, 0xffffffff);
	
	    return;
	}
	BDBG_ASSERT(rc!=NEXUS_GRAPHICS2D_BUSY);
	return;

}

void NEXUSAPP_Surface_2DCreate( struct av_playpump_t *avp)
{
	NEXUS_Graphics2DSettings graphics2DSettings;
	NEXUS_Graphics2DOpenSettings openSettings;


	NEXUS_Graphics2D_GetDefaultOpenSettings(&openSettings);
	openSettings.preAllocPacketMemory = true;
	openSettings.maxOperations = 400;
	avp->gfx = NEXUS_Graphics2D_Open(0, &openSettings);
	
	NEXUS_Graphics2D_GetSettings(avp->gfx, &graphics2DSettings);
	graphics2DSettings.checkpointCallback.callback = NEXUSAPP_Surface_2DCallback;
	graphics2DSettings.checkpointCallback.context = avp->g2dEvent;

	NEXUS_Graphics2D_SetSettings(avp->gfx, &graphics2DSettings);
}

void NEXUSAPP_Surface_SDSurface_DfRectGet( NEXUS_Rect *rect)
{
	rect->x = 0;
	rect->y = 0;
	rect->width = 720;
	rect->height = 480;
}

void NEXUSAPP_Surface_HDSurface_DfRectGet( NEXUS_Rect *rect)
{
	rect->x = 0;
	rect->y = 0;
	rect->width = 1920;
	rect->height = 1080;
}

int NEXUSAPP_Surface_MemIndex_Get( av_playpump *avp , int width , int height)
{
	NEXUS_MemoryStatus MemStatus;

	NEXUS_Heap_GetStatus(avp->platformConfig.heap[3],&MemStatus);

	if((width * height *4) > ((int)(MemStatus.largestFreeBlock)))
		return 2;
	else
		return 3;
}

int NEXUSAPP_Surface_DefaultCreate( struct av_playpump_t *avp)
{
	NEXUS_DisplaySettings displaySettings;
	NEXUS_VideoFormatInfo videoFormatInfo;
	NEXUS_GraphicsSettings graphicsSettings;
	NEXUS_GraphicsSettings graphicsCfg;
	NEXUS_Error rc;
	int iHdmiWidth = 0;
	int iHdmiHeight = 0;;
	
	NEXUSAPP_Surface_2DCreate(avp);	
	DBG_APP(("NEXUSAPP_Surface_DefaultCreate start!-->hd display test!\n"));
/* hd surface create */

	NEXUS_Display_GetGraphicsSettings(avp->display.display_hd, &graphicsCfg);
	graphicsCfg.frameBufferCallback.callback = NEXUSAPP_Surface_CallBack;
	graphicsCfg.frameBufferCallback.context = avp;
	NEXUS_Display_SetGraphicsSettings(avp->display.display_hd, &graphicsCfg);

	NEXUS_Display_GetSettings(avp->display.display_hd,&displaySettings);
	NEXUS_VideoFormat_GetInfo(displaySettings.format, &videoFormatInfo);
	NEXUS_Surface_GetDefaultCreateSettings(&avp->framebuff[0].createSettings);
	avp->framebuff[0].createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;

	iHdmiWidth = DISPLAY_FORMAT_HD_WX;
	iHdmiHeight = DISPLAY_FORMAT_HD_HY;

	avp->framebuff[0].createSettings.width = iHdmiWidth;
	avp->framebuff[0].createSettings.height = iHdmiHeight;
	avp->framebuff[0].createSettings.heap = avp->platformConfig.heap[3];

	NEXUS_Surface_GetDefaultCreateSettings(&avp->framebuff[1].createSettings);
	avp->framebuff[1].createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
	avp->framebuff[1].createSettings.width = iHdmiWidth;
	avp->framebuff[1].createSettings.height = iHdmiHeight;
	avp->framebuff[1].createSettings.heap = avp->platformConfig.heap[3];
	
	avp->framebuff[0].surface = NEXUS_Surface_Create(&avp->framebuff[0].createSettings);
	avp->framebuff[1].surface = NEXUS_Surface_Create(&avp->framebuff[1].createSettings);
	
	NEXUS_Surface_GetMemory(avp->framebuff[0].surface, &avp->framebuff[0].mem);
	NEXUS_Surface_GetMemory(avp->framebuff[1].surface, &avp->framebuff[1].mem);

	/* fill with black */
	BKNI_Memset(avp->framebuff[0].mem.buffer, 0, avp->framebuff[0].createSettings.height * avp->framebuff[0].mem.pitch);
	BKNI_Memset(avp->framebuff[1].mem.buffer, 0, avp->framebuff[1].createSettings.height * avp->framebuff[1].mem.pitch);	

	NEXUS_Graphics2D_GetDefaultFillSettings(&avp->fillSettings);
	avp->fillSettings.surface = avp->framebuff[0].surface;
	avp->fillSettings.rect.width = avp->framebuff[0].createSettings.width;
	avp->fillSettings.rect.height = avp->framebuff[0].createSettings.height;
	avp->fillSettings.color = 0x0;
	NEXUS_Graphics2D_Fill(avp->gfx, &avp->fillSettings);
	NEXUSAPP_Surface_2DAsync_Checkpoint(avp->gfx, avp->g2dEvent);
	
	/* use first framebuff  default*/

	/* sd surface  */
	NEXUS_Surface_GetDefaultCreateSettings(&avp->framebuff[3].createSettings);
	avp->framebuff[3].createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
	avp->framebuff[3].createSettings.width = DISPLAY_FORMAT_SD_WX;
	avp->framebuff[3].createSettings.height = DISPLAY_FORMAT_SD_HY;
	avp->framebuff[3].createSettings.heap = avp->platformConfig.heap[3];
	avp->framebuff[3].surface = NEXUS_Surface_Create(&avp->framebuff[3].createSettings);
	NEXUS_Surface_GetMemory(avp->framebuff[3].surface, &avp->framebuff[3].mem);
	BKNI_Memset(avp->framebuff[3].mem.buffer, 0, avp->framebuff[3].createSettings.height * avp->framebuff[3].mem.pitch);

	NEXUS_Graphics2D_GetDefaultFillSettings(&avp->fillSettings);
	avp->fillSettings.surface = avp->framebuff[3].surface;
	avp->fillSettings.rect.width = avp->framebuff[3].createSettings.width;
	avp->fillSettings.rect.height = avp->framebuff[3].createSettings.height;
	avp->fillSettings.color = 0x0;
	NEXUS_Graphics2D_Fill(avp->gfx, &avp->fillSettings);
	NEXUSAPP_Surface_2DAsync_Checkpoint(avp->gfx, avp->g2dEvent);
	
	NEXUS_Surface_Flush(avp->framebuff[3].surface);
	
	DBG_APP((" sd surface tmp\n"));

	/* sd surface tmp */
	NEXUS_Surface_GetDefaultCreateSettings(&avp->framebuff[4].createSettings);
	avp->framebuff[4].createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
	avp->framebuff[4].createSettings.width = iHdmiWidth;
	avp->framebuff[4].createSettings.height = iHdmiHeight;
	avp->framebuff[4].createSettings.heap = avp->platformConfig.heap[3];
	avp->framebuff[4].surface = NEXUS_Surface_Create(&avp->framebuff[4].createSettings);
	NEXUS_Surface_GetMemory(avp->framebuff[4].surface, &avp->framebuff[4].mem);
	BKNI_Memset(avp->framebuff[4].mem.buffer, 0, avp->framebuff[4].createSettings.height * avp->framebuff[4].mem.pitch);

	/* mw surface palette */
	NEXUS_Surface_GetDefaultCreateSettings(&avp->framebuff[5].createSettings);
	avp->framebuff[5].createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
	avp->framebuff[5].createSettings.width = 1280;
	avp->framebuff[5].createSettings.height = 720;
	avp->framebuff[5].createSettings.heap = avp->platformConfig.heap[3];
	avp->framebuff[5].surface = NEXUS_Surface_Create(&avp->framebuff[5].createSettings);
	NEXUS_Surface_GetMemory(avp->framebuff[5].surface, &avp->framebuff[5].mem);
	BKNI_Memset(avp->framebuff[5].mem.buffer, 0, avp->framebuff[5].createSettings.height * avp->framebuff[5].mem.pitch);

	/* mw surface tmp */
	NEXUS_Surface_GetDefaultCreateSettings(&avp->framebuff[6].createSettings);
	avp->framebuff[6].createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
	avp->framebuff[6].createSettings.width = 720;
	avp->framebuff[6].createSettings.height = 480;
	avp->framebuff[6].createSettings.heap = avp->platformConfig.heap[3];
	avp->framebuff[6].surface = NEXUS_Surface_Create(&avp->framebuff[6].createSettings);
	NEXUS_Surface_GetMemory(avp->framebuff[6].surface, &avp->framebuff[6].mem);
	BKNI_Memset(avp->framebuff[6].mem.buffer, 0, avp->framebuff[6].createSettings.height * avp->framebuff[6].mem.pitch);

	if(avp->framebuffIndex)
	{
		/* graphicsSettings.position will default to the display size */
		NEXUS_Display_GetGraphicsSettings(avp->display.display_hd, &graphicsSettings);
		graphicsSettings.enabled = true;
		graphicsSettings.clip.width = avp->framebuff[3].createSettings.width;
		graphicsSettings.clip.height = avp->framebuff[3].createSettings.height;
		rc = NEXUS_Display_SetGraphicsSettings(avp->display.display_hd, &graphicsSettings);
		BDBG_ASSERT(!rc);

		rc = NEXUS_Display_SetGraphicsFramebuffer(avp->display.display_hd,avp->framebuff[3].surface);
		BDBG_ASSERT(!rc);		
	}
	else
	{
		/* graphicsSettings.position will default to the display size */
		NEXUS_Display_GetGraphicsSettings(avp->display.display_hd, &graphicsSettings);
		graphicsSettings.enabled = true;
		graphicsSettings.clip.width = avp->framebuff[0].createSettings.width;
		graphicsSettings.clip.height = avp->framebuff[0].createSettings.height;
		rc = NEXUS_Display_SetGraphicsSettings(avp->display.display_hd, &graphicsSettings);
		BDBG_ASSERT(!rc);

		rc = NEXUS_Display_SetGraphicsFramebuffer(avp->display.display_hd,avp->framebuff[0].surface);
		BDBG_ASSERT(!rc);
	}
	return 0;
}

int NEXUSAPP_Surface_MyCreate(struct framebuff_t *fb)
{	
	DBG_APP(("NEXUSAPP_Surface_MyCreate :  %d x %d \n",fb->createSettings.width , fb->createSettings.height));
	
	fb->surface = NEXUS_Surface_Create(&fb->createSettings);
	if(fb->surface ==NULL)
		return -1;

	NEXUS_Surface_GetMemory(fb->surface, &fb->mem);
	BKNI_Memset(fb->mem.buffer, 0, fb->createSettings.height * fb->mem.pitch);

	return 0;
}

int NEXUSAPP_Surface_PicFlashCreate(struct framebuff_t *fb)
{

/* pic flash surface create */	
	NEXUS_Surface_GetDefaultCreateSettings(&fb->createSettings);
	fb->createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
	fb->createSettings.width = 720;
	fb->createSettings.height = 482;

	return NEXUSAPP_Surface_MyCreate(fb);

}

void NEXUSAPP_Surface_Alpha_Set(const NEXUS_SurfaceMemory *mem, const NEXUS_Rect *rect, NEXUS_Pixel pixel)
{
	unsigned x,y;
	unsigned width = rect->width;
	uint32_t tmp;
	
	for(y=0;y<rect->height;y++) {	
		uint32_t *buf = (uint32_t *)((uint8_t *)mem->buffer + mem->pitch*(y+rect->y))+rect->x;
		for(x=0;x<width;x++) {
			
			if((buf[x]>>24) > 0)
			{
				tmp = buf[x] >> 24;
				tmp = tmp / pixel;
				tmp = tmp <<24;
				buf[x]  &=0x00FFFFFF;
				buf[x]  |= tmp;
			}
			else
			if(buf[x] <= 0xFFFFFF)
				buf[x] = 0;
		}
	}
}

void NEXUSAPP_Surface_Simple_Fill(const NEXUS_SurfaceMemory *mem, const NEXUS_Rect *rect, NEXUS_Pixel pixel)
{
    uint32_t x,y;
    uint32_t width = rect->width;
    for(y=0;y<rect->height;y++) {
        uint32_t *buf = (uint32_t *)((uint8_t *)mem->buffer + mem->pitch*(y+rect->y))+rect->x;
        for(x=0;x<width;x++) {
            buf[x] = pixel;
        }
    }
}

int NEXUSAPP_Surface_Get_Framebuff_Index( struct av_playpump_t *avp)
{
	if(avp->framebuffIndex)
		return 0;
	else
		return 1;
	
}

int NEXUSAPP_Surface_Display_Pic3( struct av_playpump_t *avp,t_osd_picture_ragb *pic, int picWeith)
{
	int16_t x,y;
	NEXUS_Rect rect;
	NEXUS_Rect resetStartRect;
	NEXUS_Pixel *pBuf;
	NEXUS_Pixel *pNextH;
	int ipos=0;
	
	pNextH =(NEXUS_Pixel *)pic->ps;
	
	rect.height = 1;
	rect.width= 1;

	DBG_APP(("NEXUSAPP_Surface_Display_pic : !!!,weith : %d , height:%d\n",picWeith,pic->hy));

	pNextH =(NEXUS_Pixel *)pic->ps + ipos;
	pBuf = pNextH;

	for(y=0; y< pic->hy; y++)
	{
		for(x=0; x<picWeith ;x++)
		{
			rect.x = x + resetStartRect.x;
			rect.y = y + resetStartRect.y;
			NEXUSAPP_Surface_Simple_Fill(&avp->framebuff[2].mem,&rect,*pBuf);
			pBuf ++;
		}
		pNextH+=pic->wx;
		pBuf = pNextH;
	}

	NEXUS_Surface_Flush(avp->framebuff[2].surface);	
	return 1;
}

int NEXUSAPP_Surface_Display_Pic( struct av_playpump_t *avp,t_osd_picture_ragb *pic,NEXUS_Rect *startPoint , int picWeith ,int iIndex)
{
	int16_t x,y;
	NEXUS_Rect rect;
	NEXUS_Pixel *pBuf;
	NEXUS_Pixel *pNextH;
	
	pNextH = (NEXUS_Pixel *)pic->ps;
	pBuf = pNextH;
	
	rect.height = 1;
	rect.width= 1;

	MSG_APP(("NEXUSAPP_Surface_Display_pic : !!!,weith : %d , height:%d\n",picWeith,pic->hy));

	for(y=0; y< pic->hy; y++)
	{
		for(x=0; x<picWeith ;x++)
		{
			
			rect.x = x + startPoint->x;
			rect.y = y + startPoint->y;
			NEXUSAPP_Surface_Simple_Fill(&avp->framebuff[iIndex].mem,&rect,*pBuf);
			pBuf ++;
		}
		pNextH += pic->wx;
		pBuf = pNextH;
	}

	NEXUS_Surface_Flush(avp->framebuff[iIndex].surface);

	return 1;
}

int NEXUSAPP_Surface_Display_Bmz( struct bmzimage_t *axbmp,NEXUS_Rect *startPoint , struct framebuff_t *fb)
{

	int16_t x,y;
	NEXUS_Rect rect;
	NEXUS_Pixel *pBuf;
	NEXUS_Pixel *pNext;
	pixel_t color;
	pixel_t nextColor;
	NEXUS_Pixel pixelColor;

	pBuf = (NEXUS_Pixel *)axbmp->rs;
	rect.height = 1;
	rect.width= 1;
	for(y=0; y< axbmp->hy; y++)
	{
		pBuf = (NEXUS_Pixel *)axbmp->rs[y];
		for(x=0; x<axbmp->wx ;x++)
		{
			pNext = pBuf;
			pNext ++;
			
			rect.x = x + startPoint->x;
			rect.y = y + startPoint->y;

			memcpy(&color, pBuf,4);
			memcpy(&nextColor, pNext,4);
		
			memcpy(&pixelColor,&color,4);
			NEXUSAPP_Surface_Simple_Fill(&fb->mem,&rect,pixelColor);

			pBuf ++;
		}
	}

	NEXUS_Surface_Flush(fb->surface);

	return 0;

}

int NEXUSAPP_Surface_Display_Img(bmzimage *axbmp , NEXUS_Rect *startPoint , fbuff *fb)
{

	int16_t x,y;
	NEXUS_Rect rect;
	NEXUS_Pixel *pBuf;
	
	pBuf = (NEXUS_Pixel *)axbmp->rs;
	
	rect.height = 1;
	rect.width= 1;	
	
	for(y=0; y< axbmp->hy; y++)
	{
		pBuf = (NEXUS_Pixel *)axbmp->rs[y];
		for(x=0; x<axbmp->wx ;x++)
		{
			rect.x = x + startPoint->x;
			rect.y = y + startPoint->y;
			NEXUSAPP_Surface_Simple_Fill(&fb->mem,&rect,*pBuf);
			pBuf ++;
		}
	}

	NEXUS_Surface_Flush(fb->surface);
	return 0;

}

void NEXUSAPP_Surface_Video_Close(av_playpump *avp , int playerid)
{
	if(playerid == PLAYER_PLAYPUMP_SD)
		avp->framebuff[5].createSettings.width = 0;
}

void NEXUSAPP_Surface_From_Video(av_playpump *avp , int playerid)
{

	NEXUS_SurfaceHandle surface;
	NEXUS_StripedSurfaceHandle stripedSurface;
	NEXUS_Graphics2DBlitSettings blitSettings;
	NEXUS_VideoDecoderHandle decoder;

	if(playerid == PLAYER_PLAYPUMP_HD)
		decoder = avp->hdPlayer.videoDecoder;
	else
		decoder = avp->sdPlayer.videoDecoder;
	
	/* Create a Nexus striped surface for the most recent picture reported by VideoDecoder */
        stripedSurface = NEXUS_VideoDecoder_CreateStripedSurface(decoder);

	if(stripedSurface)
	{
		surface = NEXUS_Graphics2D_Destripe(avp->gfx , stripedSurface);
		NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
		blitSettings.source.surface = surface;
		blitSettings.output.surface = avp->framebuff[5].surface;
		NEXUSAPP_Osd_Graphics2D_Blit(avp,&blitSettings);
		NEXUS_Surface_Destroy(surface);
		NEXUS_VideoDecoder_DestroyStripedSurface(decoder,stripedSurface);
		NEXUS_Surface_Flush(avp->framebuff[5].surface);
		avp->framebuff[5].createSettings.width = 1280;
	}
	else
		avp->framebuff[5].createSettings.width = 0;
	
}

void NEXUSAPP_Osd_Graphics2D_Blit(struct av_playpump_t *avp,NEXUS_Graphics2DBlitSettings *pblitSettings)
{
	pthread_mutex_lock(&avp->g2dFillLock);
	NEXUS_Graphics2D_Blit(avp->gfx,pblitSettings);
	NEXUSAPP_Surface_2DAsync_Checkpoint(avp->gfx,avp->g2dEvent);
	pthread_mutex_unlock(&avp->g2dFillLock);
}


void NEXUSAPP_Osd_Surface_Fill(struct av_playpump_t *avp,NEXUS_Rect *rect ,NEXUS_Pixel color,NEXUS_SurfaceHandle surface)
{

	pthread_mutex_lock(&avp->g2dFillLock);
	
	NEXUS_Graphics2D_GetDefaultFillSettings(&avp->fillSettings);
	avp->fillSettings.surface = surface;
	avp->fillSettings.rect.x = rect->x;
	avp->fillSettings.rect.y = rect->y;
	avp->fillSettings.rect.width = rect->width;
	avp->fillSettings.rect.height = rect->height;
	avp->fillSettings.color = color;
	NEXUS_Graphics2D_Fill(avp->gfx, &avp->fillSettings);
	
	NEXUSAPP_Surface_2DAsync_Checkpoint(avp->gfx, avp->g2dEvent);
	
	pthread_mutex_unlock(&avp->g2dFillLock);
	
}

void NEXUSAPP_Osd_Framebuff_Fill(struct av_playpump_t *avp,NEXUS_Rect *rect ,NEXUS_Pixel color,int framebuffId)
{
	NEXUSAPP_Osd_Surface_Fill(avp,rect,color,avp->framebuff[framebuffId].surface);	
}

void NEXUSAPP_Osd_Framebuff_Blit(struct av_playpump_t *avp,NEXUS_Rect *SourceRect,NEXUS_Rect *DestRect,int SourceSurfaceIndex,int destSuerfaceIndex)
{		
	
	avp->blitSettings.source.surface = avp->framebuff[SourceSurfaceIndex].surface;
	avp->blitSettings.source.rect.x = SourceRect->x;
	avp->blitSettings.source.rect.y = SourceRect->y; 
	avp->blitSettings.source.rect.height = SourceRect->height;
	avp->blitSettings.source.rect.width = SourceRect->width;


	avp->blitSettings.output.surface = avp->framebuff[destSuerfaceIndex].surface;
	avp->blitSettings.output.rect.x =	DestRect->x;
	avp->blitSettings.output.rect.y =	DestRect->y;
	avp->blitSettings.output.rect.height = DestRect->height;
	avp->blitSettings.output.rect.width = DestRect->width;

	pthread_mutex_lock(&avp->g2dFillLock);
	if(NEXUS_Graphics2D_Blit(avp->gfx, &avp->blitSettings))
	{
		pthread_mutex_unlock(&avp->g2dFillLock);
		return;
	}

	NEXUSAPP_Surface_2DAsync_Checkpoint(avp->gfx, avp->g2dEvent);

/*add by hh 2012-04-29 */
	NEXUS_Surface_Flush(avp->framebuff[destSuerfaceIndex].surface);

	pthread_mutex_unlock(&avp->g2dFillLock);
}

void NEXUSAPP_Osd_PicAlpha_Set(struct framebuff_t *fb,NEXUS_Rect *rect, NEXUS_Pixel pixel)
{

	NEXUSAPP_Surface_Alpha_Set(&fb->mem,rect,pixel);
	NEXUS_Surface_Flush(fb->surface);
}

void NEXUSAPP_Osd_PicHL_Set(struct framebuff_t *psfb,struct framebuff_t *pdfb,NEXUS_Rect *rect, NEXUS_Pixel pixel)
{

	unsigned x,y;
	uint32_t *psBuf , *pDbuf;
	
	for(y=rect->height-1;y>0;y--) {	
		psBuf = (uint32_t *)((uint8_t *)psfb->mem.buffer + psfb->mem.pitch*(y+rect->y))+rect->x;
		pDbuf = (uint32_t *)((uint8_t *)pdfb->mem.buffer + pdfb->mem.pitch*(y+rect->y))+rect->x;
		for(x = rect->width;x>0;x--) 
		{
			if((psBuf[x]>>24) == 0)
			{
				pDbuf[x] = pixel;
			}
			else
			{
				pDbuf[x] = psBuf[x];
				pDbuf[x+1] = 0xA1FFFFFF;
			}	
		}
	}

	NEXUS_Surface_Flush(pdfb->surface);
	
}

void NEXUSAPP_Osd_PicFlash_ReturnPic(struct  osdFlash *flash)
{
	struct framebuff_t tmpfb;
	NEXUS_Graphics2DBlitSettings blitSettings;

	DBG_OSD(("NEXUSAPP_Osd_PicFlash_HLButtion\n"));

	DBG_OSD(("position : %d : %d , %d * %d\n",
		flash->startPoint.x,
		flash->startPoint.y,
		flash->startPoint.width,
		flash->startPoint.height));

/* copy source surface to tmp surface*/
	NEXUSAPP_Surface_PicFlashCreate(&tmpfb);

	NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
	blitSettings.source.surface = flash->pSfb->surface;
	blitSettings.source.rect.x = flash->startPoint.x;
	blitSettings.source.rect.y = flash->startPoint.y; 
	blitSettings.source.rect.height= flash->startPoint.height;
	blitSettings.source.rect.width = flash->startPoint.width;

	blitSettings.dest.surface = flash->pDfb->surface;
	blitSettings.dest.rect.x = flash->startPoint.x;
	blitSettings.dest.rect.y = flash->startPoint.y;
	blitSettings.dest.rect.height = flash->startPoint.height;
	blitSettings.dest.rect.width  = flash->startPoint.width;
	
	blitSettings.output.surface =  tmpfb.surface;
	blitSettings.output.rect.x = flash->startPoint.x - 3;
	blitSettings.output.rect.y = flash->startPoint.y - 3;
	blitSettings.output.rect.height = flash->startPoint.height + 6;
	blitSettings.output.rect.width  = flash->startPoint.width + 6;

	blitSettings.colorOp =  NEXUS_BlitColorOp_eUseSourceAlpha;
	blitSettings.alphaOp = NEXUS_BlitAlphaOp_eCombine;

	NEXUSAPP_Osd_Graphics2D_Blit(flash->avp,&blitSettings);
	NEXUS_Surface_Flush(flash->pDfb->surface);

/*	
	NEXUSAPP_Osd_PicHL_Set(flash->pDfb,&tmpfb,&blitSettings.source.rect,ulApercent);
*/

/* init tmp surface blit setting */

	blitSettings.source.surface = tmpfb.surface;
	blitSettings.output.surface = flash->pDfb->surface;


	NEXUSAPP_Osd_Graphics2D_Blit(flash->avp,&blitSettings);

	NEXUS_Surface_Flush(flash->pDfb->surface);		

	NEXUS_Surface_Destroy(tmpfb.surface);
	return ;
	
}

void NEXUSAPP_Osd_PicFlash_HLButtion(struct  osdFlash *flash)
{
	struct framebuff_t tmpfb;
	NEXUS_Graphics2DBlitSettings blitSettings;

	DBG_OSD(("NEXUSAPP_Osd_PicFlash_HLButtion\n"));

	DBG_OSD(("position : %d : %d , %d * %d\n",
		flash->startPoint.x,
		flash->startPoint.y,
		flash->startPoint.width,
		flash->startPoint.height));

/* copy source surface to tmp surface*/
	NEXUSAPP_Surface_PicFlashCreate(&tmpfb);

	NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
	blitSettings.source.surface = flash->pSfb->surface;
	blitSettings.source.rect.x = flash->startPoint.x;
	blitSettings.source.rect.y = flash->startPoint.y; 
	blitSettings.source.rect.height= flash->startPoint.height;
	blitSettings.source.rect.width = flash->startPoint.width;

	blitSettings.dest.surface = flash->pDfb->surface;
	blitSettings.dest.rect.x = flash->startPoint.x;
	blitSettings.dest.rect.y = flash->startPoint.y;
	blitSettings.dest.rect.height = flash->startPoint.height;
	blitSettings.dest.rect.width  = flash->startPoint.width;
	
	blitSettings.output.surface =  tmpfb.surface;
	blitSettings.output.rect.x = flash->startPoint.x - 3;
	blitSettings.output.rect.y = flash->startPoint.y - 3;
	blitSettings.output.rect.height = flash->startPoint.height + 6;
	blitSettings.output.rect.width  = flash->startPoint.width + 6;

	blitSettings.colorOp =  NEXUS_BlitColorOp_eUseSourceAlpha;
	blitSettings.alphaOp = NEXUS_BlitAlphaOp_eCombine;

	NEXUSAPP_Osd_Graphics2D_Blit(flash->avp,&blitSettings);
	NEXUS_Surface_Flush(flash->pDfb->surface);

/*	
	NEXUSAPP_Osd_PicHL_Set(flash->pDfb,&tmpfb,&blitSettings.source.rect,ulApercent);
*/

/* init tmp surface blit setting */

	blitSettings.source.surface = tmpfb.surface;
	blitSettings.output.surface = flash->pDfb->surface;


	NEXUSAPP_Osd_Graphics2D_Blit(flash->avp,&blitSettings);

	NEXUS_Surface_Flush(flash->pDfb->surface);		

	NEXUS_Surface_Destroy(tmpfb.surface);
	return ;
	
}

void NEXUSAPP_Osd_PicFlash_FadeOut(struct  osdFlash *flash)
{
	int iFrameRate = 8;
	unsigned long ulApercent = 0;
	int i , iwh ,iMwState = 0;
	NEXUS_Rect rect_tmp;
	struct framebuff_t tmpfb;
	NEXUS_Graphics2DBlitSettings blitSettings;

	DBG_OSD(("NEXUSAPP_Osd_PicFlash_Fade out\n"));

/* copy source surface to tmp surface*/
	NEXUSAPP_Surface_PicFlashCreate(&tmpfb);

	NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
	blitSettings.source.surface = flash->pSfb->surface;
	blitSettings.source.rect.x = flash->startPoint.x;
	blitSettings.source.rect.y = flash->startPoint.y; 
	blitSettings.source.rect.height= flash->startPoint.height;
	blitSettings.source.rect.width = flash->startPoint.width;

	blitSettings.output.surface = tmpfb.surface;
	blitSettings.output.rect.x = flash->startPoint.x;
	blitSettings.output.rect.y = flash->startPoint.y;
	blitSettings.output.rect.height = flash->startPoint.height;
	blitSettings.output.rect.width  = flash->startPoint.width;

	NEXUSAPP_Osd_Graphics2D_Blit(flash->avp,&blitSettings);
	NEXUS_Surface_Flush(tmpfb.surface);
	
/* init tmp surface blit setting */
	NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
	blitSettings.source.surface = tmpfb.surface;
	blitSettings.source.rect.x  =  flash->startPoint.x;
	blitSettings.source.rect.y  =  flash->startPoint.y;
	blitSettings.source.rect.height =  flash->startPoint.height;
	blitSettings.source.rect.width  =  flash->startPoint.width;

	blitSettings.output.surface = flash->pDfb->surface;
	
/* check pic size , deal of fullscrean*/
	if(	flash->ifmw == 1
		&&(flash->startPoint.width == 720 || flash->startPoint.height == 480)
	  )
	{
		memcpy(&rect_tmp,&flash->startPoint,sizeof(NEXUS_Rect));

		NEXUSAPP_Osd_Framebuff_Blit(flash->avp,&rect_tmp,&rect_tmp,SURFACE_SD_DISPLAY,SURFACE_MW_BACKUP);
		NEXUSAPP_Osd_Framebuff_Fill(flash->avp,&rect_tmp,0,SURFACE_SD_DISPLAY);

		rect_tmp.x +=4;
		rect_tmp.y +=4;
		rect_tmp.width -=8;
		rect_tmp.width -=8;

		NEXUSAPP_Osd_Framebuff_Blit(flash->avp,&rect_tmp,&rect_tmp,SURFACE_MW_BACKUP,SURFACE_SD_DISPLAY);
	}

	NEXUS_Surface_Flush(flash->pDfb->surface);		

	usleep(20000);

/* fade out pic */

	for(i = - 2, ulApercent = 1; i< iFrameRate ; i++ ,ulApercent++)
	{
		if(flash->ifmw)
		{
			NEXUSAPP_Mwall_State_Get(flash->avp,&iMwState);
			if(iMwState != OSD_MW_STATE_CLEANINT)
				break;
		}

		/*  change tmp surface alpha to output surface*/
		iwh = i * 2 ;
		
		/* lastet frame set alpha min 255/100 = 2*/
		if(i == iFrameRate -1)
			ulApercent = 100;
		
		blitSettings.output.rect.x =	flash->startPoint.x + iwh;
		blitSettings.output.rect.y =	flash->startPoint.y + iwh;
		blitSettings.output.rect.height =  flash->startPoint.height - iwh*2;
		blitSettings.output.rect.width  =   flash->startPoint.width  - iwh*2;
		
		if(i == - 2)
		{
			memcpy(&rect_tmp,&blitSettings.output.rect,sizeof(NEXUS_Rect));	
			rect_tmp.x -=2;
			rect_tmp.y -=2;
			rect_tmp.width   -=4;
			rect_tmp.height -=4;
		}
		NEXUSAPP_Osd_PicAlpha_Set(&tmpfb,&blitSettings.source.rect,ulApercent);
		
		NEXUSAPP_Osd_Graphics2D_Blit(flash->avp,&blitSettings);

		usleep(10000);
		
		NEXUS_Surface_Flush(flash->pDfb->surface);		

	}
	
	NEXUSAPP_Osd_Surface_Fill(flash->avp,&rect_tmp,0x00,flash->pDfb->surface);
	NEXUS_Surface_Flush(flash->pDfb->surface);
	
	NEXUS_Surface_Destroy(tmpfb.surface);
	return ;
	
}

void NEXUSAPP_Osd_PicFlash_FadeIn(struct  osdFlash *flash)
{
	int iFrameRate = 5;
	unsigned long ulApercent = 0;
	int i , iwh ,iMwState;
	struct framebuff_t tmpfb;
	NEXUS_Graphics2DBlitSettings blitSettings;
	NEXUS_Graphics2DBlitSettings blitSettings_1;

	DBG_OSD(("NEXUSAPP_Osd_PicFlash_Fade In\n"));
	
	/* create tmp surface and copy source surface to tmp surface */ 
	
	NEXUSAPP_Surface_PicFlashCreate(&tmpfb);

	DBG_OSD(("source setting  %d : %d\n",flash->pSfb->createSettings.width,flash->pSfb->createSettings.height));
	DBG_OSD(("startpoint setting  %d : %d\n", flash->startPoint.width,flash->startPoint.height));
/* tmp surface */
	NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
	blitSettings.source.surface = flash->pSfb->surface;
	blitSettings.source.rect.x = flash->startPoint.x;
	blitSettings.source.rect.y = flash->startPoint.y; 
	blitSettings.source.rect.height= flash->startPoint.height;
	blitSettings.source.rect.width = flash->startPoint.width;

	blitSettings.output.surface = tmpfb.surface;
	blitSettings.output.rect.x = flash->startPoint.x;
	blitSettings.output.rect.y = flash->startPoint.y;
	blitSettings.output.rect.height = flash->startPoint.height;
	blitSettings.output.rect.width  = flash->startPoint.width;
	
	NEXUSAPP_Osd_Graphics2D_Blit(flash->avp,&blitSettings);
	NEXUS_Surface_Flush(tmpfb.surface);
	
	/* start change tmp surface alpha , and blit to output surface */
	
	NEXUSAPP_Osd_Framebuff_Fill(flash->avp,&flash->startPoint,0x0,3);

	NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings_1);
	blitSettings_1.source.surface = tmpfb.surface;
	blitSettings_1.source.rect.x  =  flash->startPoint.x;
	blitSettings_1.source.rect.y  =  flash->startPoint.y;
	blitSettings_1.source.rect.height =  flash->startPoint.height;
	blitSettings_1.source.rect.width  =  flash->startPoint.width;

	blitSettings_1.output.surface = flash->pDfb->surface;

	for(i = iFrameRate , ulApercent = iFrameRate; i > 0; i--,ulApercent--)
	{
		if(flash->ifmw)
		{
			NEXUSAPP_Mwall_State_Get(flash->avp,&iMwState);
			if(iMwState != OSD_MW_STATE_DISPLAYING)
				break;
		}

		/*  change tmp surface alpha to output surface*/
		iwh = (i -1) * 2 ;	

		NEXUSAPP_Osd_Graphics2D_Blit(flash->avp,&blitSettings);

		usleep(1000);
		
		blitSettings_1.output.rect.x =	flash->startPoint.x +  iwh;
		blitSettings_1.output.rect.y =	flash->startPoint.y +  iwh;
		blitSettings_1.output.rect.height =  flash->startPoint.height - iwh*2;
		blitSettings_1.output.rect.width  =   flash->startPoint.width  - iwh*2;

		NEXUSAPP_Osd_PicAlpha_Set(&tmpfb,&blitSettings_1.source.rect,ulApercent);

		NEXUSAPP_Osd_Graphics2D_Blit(flash->avp,&blitSettings_1);
		
		NEXUS_Surface_Flush(flash->pDfb->surface);		

	}

	NEXUS_Surface_Destroy(tmpfb.surface);

	return ;
	
}

static void *NEXUSAPP_Osd_Flash_Pthread(void *context)
{
	struct  osdFlash *flash = (struct  osdFlash *)context;
		
	PARA_ASSERT_VALUE(flash,NULL); 
	
	DBG_OSD(("NEXUSAPP_Osd_Flash_Pthread, mode : %d  \n",flash->mode));
	
	switch(flash->mode)
	{
		case FLASH_FADEIN:	/* fade in */
			NEXUSAPP_Osd_PicFlash_FadeIn(flash);
			break;
		case FLASH_FADEOUT:	/*fade out */
			NEXUSAPP_Osd_PicFlash_FadeOut(flash);
			break;
		case FLSHA_HIGHLIGHT_BUTTION:
			NEXUSAPP_Osd_PicFlash_HLButtion(flash);
			break;	
		default:
			break;
	}

	NEXUSAPP_Ctrl_Respond_Send(flash->avp->socketCtrlFd,
	CTRL_OSD_MESSAGE_INITIATIVE,CTRL_OSD_INITIATIVE_PICFLASH_COMPLETE);

	if(flash->ifmw)
	{
		DBG_OSD(("\nmw flash , unlock and set idle"));
		pthread_mutex_unlock(&flash->avp->osd.mw.mwActionLock);
		NEXUSAPP_Mwall_State_Set(flash->avp , OSD_MW_STATE_IDLE);
	}
	
	free(flash);
	
	pthread_detach(pthread_self());
	
	return NULL;
}

int NEXUSAPP_Osd_PicFlash_Action(struct  osdFlash *flash)
{
	pthread_t threadFlashPic;
	pthread_attr_t attr;
	int iRet = 0;
	
	pthread_attr_init (&attr);
	pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);
	iRet = pthread_create(&threadFlashPic,&attr,NEXUSAPP_Osd_Flash_Pthread,flash);
	pthread_attr_destroy (&attr);
	DBG_OSD(("pthread_create : return :%d\r\n",iRet));

	return iRet;
}

void NEXUSAPP_Osd_Piz_Close(struct  xxpiz_info_t *info)
{	
	if(info->axpiz.frame_count)
	{
		NEXUSAPP_Xxpiz_Clean_Frame(&info->axpiz);
	}	
}

int NEXUSAPP_Osd_PizBuffer_Malloc(struct  xxpiz_info_t *xxpizInfo)
{
	
	PARA_ASSERT_VALUE(xxpizInfo,-1); 
		
	if(xxpizInfo->pizSize<=0)
		return -1;

	xxpizInfo->pizBuf = malloc(xxpizInfo->pizSize * sizeof(char *));
		
	PARA_ASSERT_VALUE(xxpizInfo->pizBuf,-1);

	return 0;
}

int NEXUSAPP_Osd_PizBuffer_Free(struct  xxpiz_info_t *xxpizInfo)
{	
	PARA_ASSERT_VALUE(xxpizInfo,-1);

	if(xxpizInfo->pizBuf)
		free(xxpizInfo->pizBuf);

	xxpizInfo->pizBuf = NULL;
	xxpizInfo->pizSize =0;
	xxpizInfo->pizRecSize =0;

	return 0;
}

int NEXUSAPP_Osd_Piz_Save(char *pizBuf,unsigned int iLen,char *szPath)
{
	FILE *fp;

	if(!access(szPath,F_OK))
		unlink(szPath);

	fp = fopen(szPath,"ab+");
	if(fp==NULL)
		return -1;
	
	fseek(fp,0,0);
	fwrite(pizBuf,1,iLen,fp);
	fclose(fp);
	return 0;	
}

int NEXUSAPP_Osd_PizDisplay_Clean(av_playpump *avp,xxpiz_info *xxpizInfo,int fbId)
{
	NEXUSAPP_PizPlayer_Stop(avp,LOGOPIZNAME);	
	NEXUSAPP_PizPlayer_Destroy(avp,LOGOPIZNAME);
	
	if(*xxpizInfo->pizPath)	
		NEXUSAPP_Osd_Framebuff_Fill(avp,&xxpizInfo->position,0,fbId);

	return 0;
}

int NEXUSAPP_Osd_Piz_Ready(av_playpump *avp , xxpiz_info *xxpizInfo,char *path)
{
	PizPlayerSettings pizSettings;

	PARA_ASSERT_VALUE(xxpizInfo,-1);
		
	memcpy(&xxpizInfo->position,&xxpizInfo->newPosition,sizeof(NEXUS_Rect));
	
	if(strcmp(xxpizInfo->pizPath,path))
	{
		memset(xxpizInfo->pizPath,0x00,sizeof(xxpizInfo->pizPath));
		strcpy(xxpizInfo->pizPath,path);	
	}

	if(NEXUSAPP_Osd_Piz_Save(xxpizInfo->pizBuf,xxpizInfo->pizSize,xxpizInfo->pizPath))
		goto error_out;

	NEXUSAPP_Osd_PizBuffer_Free(xxpizInfo);	

	NEXUSAPP_Osd_Piz_Close(xxpizInfo);
	
/* 	add by hh ,piz used pizplayer , 2012_02_20*/
	memset(&pizSettings,0x00,sizeof(PizPlayerSettings));
	pizSettings.iLoop = -1;
	strcpy(pizSettings.szAppName , LOGOPIZNAME);
	strcpy(pizSettings.szPath, xxpizInfo->pizPath);
	memcpy(&pizSettings.dest_rect,&xxpizInfo->position,sizeof(NEXUS_Rect));

	if(NEXUSAPP_PizPlayer_Init(avp ,&pizSettings))
	{
		DBG_OSD(("NEXUSAPP_PizPlayer_Init error!\n"));
		NEXUSAPP_Osd_Piz_Close(xxpizInfo);
		goto error_out;
	}

	DBG_OSD(("NEXUSAPP_PizPlayer_Init ok !\n "));
	
	if(NEXUSAPP_PizPlayer_Start(avp, LOGOPIZNAME))
	{
		DBG_OSD(("NEXUSAPP_PizPlayer_Start error!\n"));
		NEXUSAPP_Osd_Piz_Close(xxpizInfo);
		goto error_out;
	}

	DBG_OSD(("NEXUSAPP_PizPlayer_Start ok !\n "));
	
/* 	add end*/
/*	
	if(!NEXUSAPP_Xxpiz_Load_From_File(&xxpizInfo->axpiz,xxpizInfo->pizPath))
	{
		DBG_OSD(("load piz file error!\n"));
		NEXUSAPP_Osd_Piz_Close(xxpizInfo);
		return -1;
	}
*/	
	DBG_OSD(("load piz succ out!\n"));
	return 0;
error_out:
	DBG_OSD(("load piz save error out!\n"));
	NEXUSAPP_Osd_PizBuffer_Free(xxpizInfo);
	return-1;
	
}

void NEXUSAPP_Osd_Piz_Display(av_playpump *avp , int fbId)
{
	int iRet =0;
	int state;
	int distFramebuff =0 ;
	xxpiz_info *pizInfo = NULL;
	NEXUS_Rect position;

	/* used pizplayer */
	return ;
	
	PARA_ASSERT(avp);

	if(fbId == FRAMEBUFF_HD)
	{
		pizInfo = &avp->osd.piz.pizHd;
		memcpy(&position,&pizInfo->position,sizeof(NEXUS_Rect));
		distFramebuff = 0;
		NEXUSAPP_Ctrl_OsdState_Get(avp,CTRL_STATE_ITEM_ePIZHD,&state);
	}
	else
	{
		pizInfo = &avp->osd.piz.pizSd;
		memcpy(&position,&pizInfo->position,sizeof(NEXUS_Rect));
		distFramebuff = 3;
		position.x = 500;
		NEXUSAPP_Ctrl_OsdState_Get(avp,CTRL_STATE_ITEM_ePIZSD,&state);
	}	
	
	if(state!=CTRL_OSD_STATE_PLAY)
	{
		return;
	}

	
	if(pizInfo->axpiz.frame_count)
	{
		iRet = NEXUSAPP_Xxpiz_Stepp_Frame(&pizInfo->axpiz);
		
		if(iRet==3)
			NEXUSAPP_Xxpiz_Reset_Frame(&pizInfo->axpiz);
		else if(iRet ==2)
			return ;

		NEXUSAPP_Surface_Display_Img(&pizInfo->axpiz.frame_axbmp,&position,&avp->framebuff[1]);

		position.width = 135 ;
		position.height = 85 ;
	
		NEXUSAPP_Osd_Framebuff_Blit(avp,&pizInfo->position,&position,1,distFramebuff);
	}

	return ;
}

int NEXUSAPP_Osd_StaticText_Malloc(struct av_playpump_t *avp,struct osd_addContext_info_t *info)
{
	struct av_osd_text_t *text_t;

	PARA_ASSERT_VALUE(avp,-1);
	PARA_ASSERT_VALUE(info,-1);

	text_t = &avp->osd.textStatic;

	PARA_ASSERT_VALUE(text_t,-1);
	
	text_t->textWidth = info->picWidth;
	text_t->textHeight = info->picHeight;

	text_t->compressType = info->contextType;

	if(text_t->picSize<=0)
		return -1;

	NEXUSAPP_Suface_Osd_Alloc(&text_t->pic,info->picWidth,info->picHeight,text_t->picSize);

	PARA_ASSERT_VALUE(text_t->pic.ps,-1);
	
	NEXUSAPP_Suface_Osd_Fill(&text_t->pic, 0);		

	return 0;
}

int NEXUSAPP_Osd_StaticText_Free(struct av_playpump_t *avp)
{
	struct av_osd_text_t *text_t ;

	PARA_ASSERT_VALUE(avp,-1);

	text_t = &avp->osd.textStatic;

	PARA_ASSERT_VALUE(text_t,-1);

	
	if(text_t->pic.ps)
		NEXUSAPP_Suface_Osd_Free(&text_t->pic);

	text_t->picSize = 0;
	text_t->picRecSize = 0;
	text_t->picDisplayMode = 0;

	return 0;
}

int NEXUSAPP_Osd_StaticText_Ready(struct av_playpump_t *avp)
{
	struct av_osd_text_t *text_t = &avp->osd.textStatic;
	struct  osdFlash *flashPic;
	struct bmzimage_t axbmp;
	char *buf;
	int framebuffId=0;
	struct framebuff_t *pSfb ;
	NEXUS_Rect position;
	
	PARA_ASSERT_VALUE(avp,-1);
	
	text_t = &avp->osd.textStatic;

	if(text_t->picSize)	
		PARA_ASSERT_VALUE(text_t->pic.ps,-1);
	
	DBG_OSD(("NEXUSAPP_Osd_StaticText_Ready come in!\n"));
	
	if(text_t->fbId == FRAMEBUFF_HD)
		framebuffId = SURFACE_HD_DISPLAY;
	else if(text_t->fbId == FRAMEBUFF_SD)
		framebuffId = SURFACE_SD_DISPLAY;
	else if(text_t->fbId == FRAMEBUFF_MW)
		framebuffId = SURFACE_MW_BUFFER;

	DBG_OSD(("picDisplayMode : %d \n",text_t->picDisplayMode));
	
	memcpy(&position,&text_t->position,sizeof(NEXUS_Rect));

/* pic  display tmp surface */
	if(framebuffId == SURFACE_MW_BUFFER)
		pSfb = &avp->framebuff[SURFACE_MW_BUFFER];
	else if(text_t->picDisplayMode)
		pSfb = &avp->framebuff[SURFACE_SD_BUFFER];
	else
		pSfb = &avp->framebuff[SURFACE_HD_BUFFER];
	
/* set flash_info infomation */
	flashPic = (struct  osdFlash *)malloc(sizeof(struct osdFlash));
	if(!flashPic)
		goto out;
	
	memset(flashPic,0x00,sizeof(struct  osdFlash));
	flashPic->avp = avp;
	flashPic->mode = text_t->picDisplayMode;
	flashPic->pSfb = pSfb;
	flashPic->pDfb = &avp->framebuff[framebuffId];
	memcpy(&flashPic->startPoint,&position,sizeof(NEXUS_Rect));

	if(!text_t->picSize)
	{
		NEXUSAPP_Osd_PicFlash_Action(flashPic);
		goto out;
	}

/* pic to tmp framebuff */
	DBG_OSD(("pic to tmp framebuff \n"));

	if(text_t->compressType == Ctrl_Osd_Compress_Type_eZlibCompress)
	{
		memset(&axbmp,0x00,sizeof(struct bmzimage_t));
		buf = (char *)text_t->pic.ps;
		
		if(NEXUSAPP_Surface_Load_Bmz(&axbmp,BMZ_COLOR_ARGB32,buf,text_t->picSize))
			return -1;
		
		NEXUSAPP_Surface_Display_Bmz(&axbmp,&position,pSfb);
		NEXUSAPP_Surface_Bmzimage_Clean(&axbmp);
	}
	else
	{
		NEXUSAPP_Surface_Display_Pic(avp,&text_t->pic,&position,text_t->textWidth,1);
	}

/* mw framebuff buffer check */
	if(framebuffId == SURFACE_MW_BUFFER)
		goto out;
	
/* tmp framebuff blit to real framebuff*/
	DBG_OSD(("tmp framebuff blit to real framebuff \n"));

	/* clean framebuff rect before display pic , expect mode == 3 , flash clean*/
	if(text_t->picDisplayMode !=3)
		NEXUSAPP_Osd_Framebuff_Fill(avp,&position,0,framebuffId);
	
	if(text_t->picDisplayMode)
	{
		NEXUSAPP_Osd_PicFlash_Action(flashPic);
	}
	else
	{
		/* A and B framebuff sync*/
		text_t->position.width -= 2;
		NEXUSAPP_Osd_Framebuff_Blit(avp,&text_t->position,&text_t->position,1,framebuffId);
		NEXUSAPP_Osd_State_Sync(avp,0);
		NEXUSAPP_Osd_State_Sync(avp,2);
	}
	DBG_OSD(("clean framebuff! \n"));

out:
	NEXUSAPP_Osd_StaticText_Free(avp);
	return 0;	
}

void  NEXUSAPP_Osd_RollText_Colse(struct av_playpump_t *avp)
{

	if(avp->framebuff[2].mem.buffer)
	{
		NEXUSAPP_Ctrl_OsdState_Set(avp,CTRL_STATE_ITEM_eTEXTROLL,CTRL_OSD_STATE_STOP);
		NEXUS_Surface_Destroy(avp->framebuff[2].surface);
		avp->osd.textRoll.textWidth = 0;
		avp->framebuff[2].mem.buffer = NULL;
		NEXUSAPP_Osd_Framebuff_Fill(avp,&avp->osd.textRoll.destRect,0,0);
	}
}

int  NEXUSAPP_Osd_RollText_Malloc(struct av_playpump_t *avp,struct osd_addContext_info_t *info)
{
	struct av_osd_text_roll_t *picTemp;
	
	PARA_ASSERT_VALUE(avp,-1);
	PARA_ASSERT_VALUE(info,-1);
	
	picTemp = &avp->osd.textRoll;
	
	PARA_ASSERT_VALUE(picTemp,-1);
	
	picTemp->picWidth = info->picWidth;
	picTemp->picHeight = info->picHeight;

	picTemp->compressType = info->contextType;

	if(picTemp->picSize<=0)
		return -1;
	
	NEXUSAPP_Suface_Osd_Alloc(&picTemp->pic,picTemp->picWidth,picTemp->picHeight,picTemp->picSize);
	
	PARA_ASSERT_VALUE(picTemp->pic.ps,-1);

	NEXUSAPP_Suface_Osd_Fill(&picTemp->pic,0);

	return 0;
	
}

int  NEXUSAPP_Osd_RollText_Free(struct av_playpump_t *avp)
{
	struct av_osd_text_roll_t *picTemp;
	
	PARA_ASSERT_VALUE(avp,-1);
	
	picTemp = &avp->osd.textRoll;

	PARA_ASSERT_VALUE(picTemp,-1);

	NEXUSAPP_Suface_Osd_Free(&picTemp->pic);

	picTemp->picSize = 0;
	picTemp->picRecSize= 0;

	return 0;
}

int  NEXUSAPP_Osd_RollText_Ready(struct av_playpump_t *avp)
{
	struct av_osd_text_roll_t *picTemp;
	struct bmzimage_t axbmp;
	char *buf = NULL;
	int index;
		
	PARA_ASSERT_VALUE(avp,-1);
	
	picTemp = &avp->osd.textRoll;

	PARA_ASSERT_VALUE(picTemp,-1);

	DBG_OSD(("NEXUSAPP_Osd_RollText_Ready come in!\n"));
	
	NEXUSAPP_Osd_RollText_Colse(avp);

	picTemp->textWidth = picTemp->picWidth;

	picTemp->sourceRect.x = 0;
	picTemp->sourceRect.y = 0;
	picTemp->sourceRect.width = picTemp->picWidth;
	picTemp->sourceRect.height = picTemp->picHeight;

	NEXUS_Surface_GetDefaultCreateSettings(&avp->framebuff[2].createSettings);
	avp->framebuff[2].createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
	avp->framebuff[2].createSettings.width = picTemp->textWidth+60;
	avp->framebuff[2].createSettings.height = picTemp->pic.hy;

	index = NEXUSAPP_Surface_MemIndex_Get(avp,avp->framebuff[2].createSettings.width,avp->framebuff[2].createSettings.height);
	avp->framebuff[2].createSettings.heap = avp->platformConfig.heap[index];
	
	avp->framebuff[2].surface = NEXUS_Surface_Create(&avp->framebuff[2].createSettings);
	if(avp->framebuff[2].surface ==NULL)
		return -1;
	
	NEXUS_Surface_GetMemory(avp->framebuff[2].surface, &avp->framebuff[2].mem);
	BKNI_Memset(avp->framebuff[2].mem.buffer, 0, avp->framebuff[2].createSettings.height * avp->framebuff[2].mem.pitch);

	if(picTemp->compressType == Ctrl_Osd_Compress_Type_eZlibCompress)
	{
		memset(&axbmp,0x00,sizeof(struct bmzimage_t));
		buf = (char *)picTemp->pic.ps;
		if(NEXUSAPP_Surface_Load_Bmz(&axbmp,BMZ_COLOR_ARGB32,buf,picTemp->picSize))
			return -1;

		NEXUSAPP_Surface_Display_Img(&axbmp,&picTemp->sourceRect,&avp->framebuff[2]);
		memcpy(&picTemp->destRect,&picTemp->position,sizeof(NEXUS_Rect));
		NEXUSAPP_Surface_Bmzimage_Clean(&axbmp);
	}
	else
	{
		
		NEXUSAPP_Surface_Display_Pic(avp,&picTemp->pic,&picTemp->sourceRect,picTemp->textWidth,2);
		memcpy(&picTemp->destRect,&picTemp->position,sizeof(NEXUS_Rect));
	}
	
	NEXUSAPP_Osd_RollText_Free(avp);	

	picTemp->displayCount = picTemp->NewCount;
	
	DBG_OSD(("NEXUSAPP_Osd_RollText_Ready succ out !\n"));

	
	return 0;
	
	
}

void NEXUSAPP_Osd_RollText_Info_Set(av_playpump *avp)
{
	struct av_osd_text_roll_t *picTemp = &avp->osd.textRoll;
	/* roll text step pix set!*/
	picTemp->pixStep = 1;

	picTemp->displayRect.x = picTemp->destRect.x + picTemp->destRect.width;
	picTemp->displayRect.y = picTemp->destRect.y;
	picTemp->displayRect.width = 0;
	picTemp->displayRect.height = picTemp->destRect.height;

	picTemp->bufferRect.x = picTemp->sourceRect.x;
	picTemp->bufferRect.y = picTemp->sourceRect.y;
	picTemp->bufferRect.width = 0;
	picTemp->bufferRect.height = picTemp->sourceRect.height;
	
}

int NEXUSAPP_Osd_RollText_Display(av_playpump *avp)
{
	int state;
	NEXUS_Rect clean_rect;
	struct av_osd_text_roll_t *picTemp = &avp->osd.textRoll;

	NEXUSAPP_Ctrl_OsdState_Get(avp,CTRL_STATE_ITEM_eTEXTROLL,&state);
	if( !avp->framebuff[2].mem.buffer || state != CTRL_OSD_STATE_PLAY )
		return 0;
	
	if((picTemp->textWidth<<1 )> picTemp->destRect.width)
	{

		if(picTemp->displayRect.width < picTemp->destRect.width
			&&picTemp->displayRect.x > picTemp->destRect.x)
		{
			picTemp->displayRect.x -= picTemp->pixStep<<1;
			picTemp->bufferRect.width +=picTemp->pixStep;
		}
		
		if(picTemp->displayRect.x <= picTemp->destRect.x)
		{
			picTemp->bufferRect.x +=picTemp->pixStep;
		}
	
		if(picTemp->textWidth - picTemp->bufferRect.x  + picTemp->sourceRect.x <= (picTemp->destRect.width>>1))
		{
			picTemp->bufferRect.width -=picTemp->pixStep;
		}
		
	}
	else
	{
		if(picTemp->bufferRect.width < picTemp->textWidth
		&&picTemp->displayRect.x > picTemp->destRect.x)
		{
			
			picTemp->bufferRect.width +=picTemp->pixStep;
			picTemp->displayRect.width =picTemp->bufferRect.width;
		} 

		if(picTemp->displayRect.x <= picTemp->destRect.x)
		{
			
			picTemp->bufferRect.x +=picTemp->pixStep;
			picTemp->bufferRect.width -=picTemp->pixStep;
			picTemp->displayRect.width =picTemp->bufferRect.width;

		} 
		else
		{
			picTemp->displayRect.x -= picTemp->pixStep << 1;
		}
	}


	if(picTemp->bufferRect.width == 0)
	{
		picTemp->displayCount -- ;
		if(picTemp->displayCount <=0)
			return 1;
		else
		{
			NEXUSAPP_Osd_RollText_Info_Set(avp);
			return 0;
		}
	}
	
	picTemp->displayRect.width = picTemp->bufferRect.width <<1;
	picTemp->displayRect.height = picTemp->bufferRect.height <<1;

	if(picTemp->displayRect.height > picTemp->destRect.height)
	{
		picTemp->displayRect.height = picTemp->destRect.height;
		picTemp->bufferRect.height = picTemp->displayRect.height >>1;
	}

	NEXUSAPP_Osd_Framebuff_Blit(avp,&picTemp->bufferRect,&picTemp->displayRect,2,0);

/* add by ch , clean display rect left .*/
	clean_rect.x = picTemp->displayRect.x + picTemp->displayRect.width;

	if(clean_rect.x + 20 > 1920)
		return 0;
	
	clean_rect.y = picTemp->displayRect.y;
	clean_rect.width = 20;
	clean_rect.height = picTemp->displayRect.height;
	NEXUSAPP_Osd_Framebuff_Fill(avp,&clean_rect,0,0);

	return 0;
	
/* add by ch , when roll text step mode 3 , clean display rect left .*/
	if(picTemp->displayRect.x <= picTemp->destRect.x)
	{
		clean_rect.x = picTemp->displayRect.x + picTemp->displayRect.width;
		clean_rect.y = picTemp->displayRect.y;
		clean_rect.width = 20;
		clean_rect.height = picTemp->displayRect.height;
		NEXUSAPP_Osd_Framebuff_Fill(avp,&clean_rect,0,0);
	}
	
	return 0;
}

int NEXUSAPP_Osd_Piz_Respond(av_playpump *avp)
{
	Ctrl_OsdState_item item;	
	int statehd=0 ;
	int statesd=0 ;

	NEXUSAPP_Ctrl_OsdState_Get(avp,CTRL_STATE_ITEM_ePIZHD,&statehd);
	NEXUSAPP_Ctrl_OsdState_Get(avp,CTRL_STATE_ITEM_ePIZSD,&statesd);
	pthread_mutex_lock(&avp->osd.piz.pizLock);
	if(statehd== CTRL_OSD_STATE_CHANGE)
	{
		item = CTRL_STATE_ITEM_ePIZHD;
		NEXUSAPP_Ctrl_OsdState_Set(avp,item,CTRL_OSD_STATE_BUSY);
		NEXUSAPP_Osd_PizDisplay_Clean(avp,&avp->osd.piz.pizHd,0);
		if(!NEXUSAPP_Osd_Piz_Ready(avp , &avp->osd.piz.pizHd,CTRL_PIZ_HD_PATH))
			NEXUSAPP_Ctrl_OsdState_Set(avp,item,CTRL_OSD_STATE_PLAY);
		else
		{
			NEXUSAPP_Ctrl_OsdState_Set(avp,item,CTRL_OSD_STATE_ERROR);
			NEXUSAPP_Osd_PizDisplay_Clean(avp,&avp->osd.piz.pizHd,0);
		}
	}
	
	if(statesd== CTRL_OSD_STATE_CHANGE)
	{
		item = CTRL_STATE_ITEM_ePIZSD;
		NEXUSAPP_Ctrl_OsdState_Set(avp,item,CTRL_OSD_STATE_BUSY);
		NEXUSAPP_Osd_PizDisplay_Clean(avp,&avp->osd.piz.pizHd,3);
		if(!NEXUSAPP_Osd_Piz_Ready(avp , &avp->osd.piz.pizSd,CTRL_PIZ_SD_PATH))
			NEXUSAPP_Ctrl_OsdState_Set(avp,item,CTRL_OSD_STATE_PLAY);
		else
		{
			NEXUSAPP_Ctrl_OsdState_Set(avp,item,CTRL_OSD_STATE_ERROR);
			NEXUSAPP_Osd_PizDisplay_Clean(avp,&avp->osd.piz.pizHd,3);
		}
	}
	pthread_mutex_unlock(&avp->osd.piz.pizLock);
	return 0;
}

int NEXUSAPP_Osd_TextRoll_Respond(struct av_playpump_t *avp)
{
	int state =0 ;
	
	NEXUSAPP_Ctrl_OsdState_Get(avp,CTRL_STATE_ITEM_eTEXTROLL,&state);
	pthread_mutex_lock(&avp->osd.textRoll.RollLock);
	if(state == CTRL_OSD_STATE_CHANGE)
	{		
		if(!NEXUSAPP_Osd_RollText_Ready(avp))
		{
			NEXUSAPP_Osd_RollText_Info_Set(avp);
			NEXUSAPP_Ctrl_OsdState_Set(avp,CTRL_STATE_ITEM_eTEXTROLL,CTRL_OSD_STATE_PLAY);
		}
		else
			NEXUSAPP_Ctrl_OsdState_Set(avp,CTRL_STATE_ITEM_eTEXTROLL,CTRL_OSD_STATE_ERROR);
	}
	pthread_mutex_unlock(&avp->osd.textRoll.RollLock);

	return 0;
}
int NEXUSAPP_Osd_TextStatic_Respond(struct av_playpump_t *avp)
{
	int state =0 ;
	NEXUSAPP_Ctrl_OsdState_Get(avp,CTRL_STATE_ITEM_eTEXTSTATIC,&state);
	pthread_mutex_lock(&avp->osd.textStatic.staticLock);
	if(state == CTRL_OSD_STATE_CHANGE)
	{
		if(!NEXUSAPP_Osd_StaticText_Ready(avp))
			NEXUSAPP_Ctrl_OsdState_Set(avp,CTRL_STATE_ITEM_eTEXTSTATIC,CTRL_OSD_STATE_PLAY);
		else
			NEXUSAPP_Ctrl_OsdState_Set(avp,CTRL_STATE_ITEM_eTEXTSTATIC,CTRL_OSD_STATE_ERROR);
	}
	pthread_mutex_unlock(&avp->osd.textStatic.staticLock);
	return 0;

}

int NEXUSAPP_Osd_State_Respond(struct av_playpump_t *avp)
{
	
	NEXUSAPP_Osd_Piz_Respond(avp);
	NEXUSAPP_Osd_TextRoll_Respond(avp);
	NEXUSAPP_Osd_TextStatic_Respond(avp);
	return 0;
}

int NEXUSAPP_Osd_State_Ready(struct av_playpump_t *avp)
{
	PARA_ASSERT_VALUE(avp, -1);

	avp->osd.piz.pizHd.state 	= CTRL_OSD_STATE_STOP;
	avp->osd.piz.pizSd.state 	= CTRL_OSD_STATE_STOP;
	avp->osd.textStatic.state	= CTRL_OSD_STATE_STOP;
	avp->osd.textRoll.state 	= CTRL_OSD_STATE_STOP;

	syncSRect.x = 0 ;
	syncSRect.y = 50 ;
	syncSRect.width = 1920 ;
	syncSRect.height = 150 ;
	
	syncDRect.x = 10 ;
	syncDRect.y = 20 ;
	syncDRect.width = 710 ;
	syncDRect.height =  68;

	syncSstRect.x = 600 ;
	syncSstRect.y = 200 ;
	syncSstRect.width = 1300 ;
	syncSstRect.height = 100 ;
	
	syncDstRect.x = 0 ;
	syncDstRect.y = 90;
	syncDstRect.width = 700;
	syncDstRect.height =  60;
	
	
	return 0;
}

int NEXUSAPP_Osd_State_Sync(struct av_playpump_t *avp , int mode)
{
		
	PARA_ASSERT_VALUE(avp, -1);

	if(avp->display.mode != CTRL_DISPLAY_MODE_eA_A_SYNC)
		return 0;
	
	switch(mode)
	{
		case 0:
			NEXUSAPP_Osd_Framebuff_Fill(avp,&syncDstRect,0,3);
			break;
		case 1:
			NEXUSAPP_Osd_Framebuff_Blit(avp,&syncSRect,&syncDRect,0,3);
			break;
		case 2:
			NEXUSAPP_Osd_Framebuff_Blit(avp,&syncSstRect,&syncDstRect,0,3);
			break;
		case 3:
			
		default:
			break;
	}

	return 0;
}

int NEXUSAPP_Osd_Mode_Get(av_playpump *avp)
{
	return avp->hdPlayer.uiDisplay3DMode;
}

void NEXUSAPP_Osd_Mode_Set(av_playpump *avp ,int mode)
{
	int iFramebufferWidth =0;
	int iFramebufferHeigh = 0;
	int zoffset = 8;
	int scaled_zoffset;

	iFramebufferWidth = avp->framebuff[SURFACE_MW_BUFFER].createSettings.width;
	iFramebufferHeigh = avp->framebuff[SURFACE_MW_BUFFER].createSettings.height;

	scaled_zoffset = zoffset * iFramebufferWidth/(iFramebufferWidth/2);
	
	switch(mode)
	{
		case SURFACE_OSD_MODE_2D:
			NEXUS_Display_SetGraphicsFramebuffer(avp->display.display_hd,avp->framebuff[SURFACE_HD_DISPLAY].surface);
			break;
		case SURFACE_OSD_MODE_3DRL:

			DBG_APP(("mode: L/R half with %d zoffset\n", zoffset));

			syncSRect.x 		= scaled_zoffset<0?-scaled_zoffset:0;
			syncSRect.y 		= 0;
			syncSRect.width 	= iFramebufferWidth - abs(scaled_zoffset);
			syncSRect.height 	= 125;

			syncDRect.x 		= zoffset>0?zoffset:0;
			syncDRect.y 		= 0;
			syncDRect.width 	= iFramebufferWidth/2 - abs(zoffset);
			syncDRect.height 	= 125;

			syncSstRect.x 		= scaled_zoffset>0?scaled_zoffset:0;
			syncSstRect.y 		= 0;
			syncSstRect.width 	= iFramebufferWidth - abs(scaled_zoffset);
			syncSstRect.height 	= 125;

			syncDstRect.x		= iFramebufferWidth/2 - (zoffset<0?zoffset:0);
			syncDstRect.y		= 0 ;
			syncDstRect.width	= iFramebufferWidth/2 - abs(zoffset);
			syncDstRect.height	= 125;
			
			NEXUS_Display_SetGraphicsFramebuffer(avp->display.display_hd,avp->framebuff[SURFACE_MW_BUFFER].surface);
			break;
		default:
			break;
	}	
}

void NEXUSAPP_Osd_Mode_3Ddo(av_playpump *avp)
{
	int iOsdMode ;
	NEXUS_Rect tmpSRect;
	NEXUS_Rect tmpDRect;
	NEXUS_Rect tmpcDstRect;

	
	tmpSRect.x = 0;
	tmpSRect.y = 125;
	tmpSRect.width = avp->framebuff[SURFACE_HD_DISPLAY].createSettings.width;
	tmpSRect.height = avp->framebuff[SURFACE_HD_DISPLAY].createSettings.height - 125;

	memcpy(&tmpDRect , &tmpSRect , sizeof(NEXUS_Rect));
	memcpy(&tmpcDstRect , &tmpSRect , sizeof(NEXUS_Rect));

	tmpDRect.width = avp->framebuff[SURFACE_MW_BUFFER].createSettings.width /2 ;

	tmpcDstRect.x += avp->framebuff[SURFACE_MW_BUFFER].createSettings.width /2;
	tmpcDstRect.width = avp->framebuff[SURFACE_MW_BUFFER].createSettings.width /2 ;

	iOsdMode = NEXUSAPP_Osd_Mode_Get(avp);

	switch(iOsdMode)
	{
		case SURFACE_OSD_MODE_2D:
			break;
		case SURFACE_OSD_MODE_3DRL:
			NEXUSAPP_Osd_Framebuff_Blit(avp,&syncSRect,&syncDRect,SURFACE_HD_DISPLAY,SURFACE_MW_BUFFER);
			NEXUSAPP_Osd_Framebuff_Blit(avp,&syncSstRect,&syncDstRect,SURFACE_HD_DISPLAY,SURFACE_MW_BUFFER);

			NEXUSAPP_Osd_Framebuff_Blit(avp,&tmpSRect,&tmpDRect,SURFACE_HD_DISPLAY,SURFACE_MW_BUFFER);
			NEXUSAPP_Osd_Framebuff_Blit(avp,&tmpSRect,&tmpcDstRect,SURFACE_HD_DISPLAY,SURFACE_MW_BUFFER);
			break;
		case SURFACE_OSD_MODE_3DTB:
			break;
		default:
			break;
	}
		
}

int NEXUSAPP_Osd_State_Sync_all(struct av_playpump_t *avp)
{
	NEXUS_Rect SRect;
	NEXUS_Rect DRect;
	int iOsdMode ;
	iOsdMode = NEXUSAPP_Osd_Mode_Get(avp);

	PARA_ASSERT_VALUE(avp, -1);

	if(avp->framebuffIndex == 0)
		return 0;

	memset(&SRect , 0x00 ,sizeof(NEXUS_Rect));
	memset(&DRect , 0x00 ,sizeof(NEXUS_Rect));

	SRect.width = avp->framebuff[SURFACE_HD_DISPLAY].createSettings.width;
	SRect.height = avp->framebuff[SURFACE_HD_DISPLAY].createSettings.height;

	DRect.width = avp->framebuff[SURFACE_SD_DISPLAY].createSettings.width;
	DRect.height = avp->framebuff[SURFACE_SD_DISPLAY].createSettings.height;

	switch(iOsdMode)
	{
		case SURFACE_OSD_MODE_2D:
			NEXUSAPP_Osd_Framebuff_Blit(avp,&SRect,&DRect,SURFACE_HD_DISPLAY,SURFACE_SD_DISPLAY);
			break;
		case SURFACE_OSD_MODE_3DRL:
			NEXUSAPP_Osd_Framebuff_Blit(avp,&SRect,&DRect,SURFACE_MW_BUFFER,SURFACE_SD_DISPLAY);
			break;
		case SURFACE_OSD_MODE_3DTB:
			break;
		default:
			break;
	}
		

	
	return 0;
}

void *NEXUSAPP_Osd_Start_Pthread(void *context)
{
	struct av_playpump_t *avp = NULL;

	PARA_ASSERT_VALUE(context,NULL);

	avp = (struct av_playpump_t*)context;

	signal(SIGNAL_EXIT, NEXUSAPP_Signal_Exit);
	
	NEXUSAPP_Osd_State_Ready(avp);
	
	while(1)
	{
		usleep(33000);

		NEXUSAPP_Osd_State_Respond(avp);

		NEXUSAPP_Osd_Piz_Display(avp,FRAMEBUFF_HD);
		
		NEXUSAPP_Osd_Piz_Display(avp,FRAMEBUFF_SD);
		
		if(NEXUSAPP_Osd_RollText_Display(avp))
		{
			NEXUSAPP_Ctrl_Respond_Send(avp->socketCtrlFd,
				CTRL_OSD_MESSAGE_INITIATIVE,CTRL_OSD_INITIATIVE_ROLLTEXT_COMPLETE);
			
			NEXUSAPP_Osd_RollText_Colse(avp);
		}

		/*
		NEXUSAPP_Osd_State_Sync(avp,1);
		*/
		
		/* 3DTV osd do */
		NEXUSAPP_Osd_Mode_3Ddo(avp);
		/* end */
		
		/* av output framebuff sync*/
		NEXUSAPP_Osd_State_Sync_all(avp);
	
	}

	return NULL;
}

#endif

