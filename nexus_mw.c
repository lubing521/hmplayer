#ifndef NEXUS_MW_C
#define NEXUS_MW_C

#include "nexus_mw.h"
#include "nexus_osd.h"

int  NEXUSAPP_Mwall_State_Ctrl(struct av_playpump_t *avp , int hopeState)
{
	int iState , i = 0;
	NEXUS_Rect rect;

	PARA_ASSERT_VALUE(avp,-1);

	NEXUSAPP_Surface_SDSurface_DfRectGet(&rect);

	NEXUSAPP_Mwall_State_Get(avp , &iState);

	switch(iState)
	{
		case OSD_MW_STATE_IDLE:
			break;
		case OSD_MW_STATE_CLEANINT:
		case OSD_MW_STATE_DISPLAYING:
			if(iState == hopeState)
				return -1;
			
			NEXUSAPP_Mwall_State_Set(avp,OSD_MW_STATE_EVENT);
			while(iState!=OSD_MW_STATE_IDLE && i < 100)
			{
				NEXUSAPP_Mwall_State_Get(avp , &iState);
				i++;
				usleep(100000);
			}
			
			/* blit default rect to sd display window 
			if(avp->osd.mw.default_rect.width)
				NEXUSAPP_Osd_Framebuff_Blit(avp,&avp->osd.mw.default_rect,&rect,SURFACE_MW_BUFFER,SURFACE_SD_DISPLAY);
*/
			break;
		case OSD_MW_STATE_EVENT:
			while(iState != OSD_MW_STATE_IDLE && i < 100)
			{
				i++;
				usleep(1000);
			}
			break;
		default:
			break;
	}

	NEXUSAPP_Mwall_State_Set(avp,hopeState);

	return 0;
}

void  NEXUSAPP_Mwall_State_Get(struct av_playpump_t *avp , int *iState)
{
	pthread_mutex_lock(&avp->osd.mw.mwStateLock);

	*iState = avp->osd.mw.mwState;
	
	pthread_mutex_unlock(&avp->osd.mw.mwStateLock);	
}

void  NEXUSAPP_Mwall_State_Set(struct av_playpump_t *avp , int iState)
{
	pthread_mutex_lock(&avp->osd.mw.mwStateLock);

	avp->osd.mw.mwState = iState;
	
	pthread_mutex_unlock(&avp->osd.mw.mwStateLock);		
}

int NEXUSAPP_Mwall_Default_Rect(struct av_playpump_t *avp , NEXUS_Rect *dfRect)
{
	PARA_ASSERT_VALUE(avp,-1);
	PARA_ASSERT_VALUE(dfRect,-1);

	memcpy(&avp->osd.mw.default_rect,dfRect,sizeof(NEXUS_Rect));
	return 0;
}

int NEXUSAPP_Mwall_Blit_Rect(struct av_playpump_t *avp ,NEXUS_Rect *sRect , NEXUS_Rect *dRect)
{
	NEXUS_Rect rect;

	PARA_ASSERT_VALUE(avp,-1);
	PARA_ASSERT_VALUE(sRect,-1);
	PARA_ASSERT_VALUE(dRect,-1);

	NEXUSAPP_Surface_SDSurface_DfRectGet(&rect);

	NEXUSAPP_Mwall_State_Ctrl(avp , OSD_MW_STATE_BLITINT);

	pthread_mutex_lock(&avp->osd.mw.mwActionLock);
	
	/* blit sd display surface to mw backup surface */
	NEXUSAPP_Osd_Framebuff_Blit(avp,&rect,&rect,SURFACE_SD_DISPLAY,SURFACE_MW_BACKUP);
	/* blit mw buffer surface rect to sd display surface rect */
	NEXUSAPP_Osd_Framebuff_Blit(avp,sRect,dRect,SURFACE_MW_BUFFER,SURFACE_SD_DISPLAY);

	pthread_mutex_unlock(&avp->osd.mw.mwActionLock);
	
	NEXUSAPP_Mwall_State_Set(avp , OSD_MW_STATE_IDLE);
	
	return 0;
}

int NEXUSAPP_Mwall_Blit_Return(struct av_playpump_t *avp)
{
	NEXUS_Rect rect;

	PARA_ASSERT_VALUE(avp,-1);

	NEXUSAPP_Surface_SDSurface_DfRectGet(&rect);

	NEXUSAPP_Mwall_State_Ctrl(avp , OSD_MW_STATE_BLITINT);

	pthread_mutex_lock(&avp->osd.mw.mwActionLock);

	/* blit mw backup surface to sd display surface*/
	NEXUSAPP_Osd_Framebuff_Blit(avp,&rect,&rect,SURFACE_MW_BACKUP,SURFACE_SD_DISPLAY);

	pthread_mutex_unlock(&avp->osd.mw.mwActionLock);

	NEXUSAPP_Mwall_State_Set(avp , OSD_MW_STATE_IDLE);
	
	return 0;
}

int NEXUSAPP_Mwall_Display(struct av_playpump_t *avp)
{
	NEXUS_Rect rect;
	struct  osdFlash *flash;
		
	PARA_ASSERT_VALUE(avp,-1);

	DBG_OSD(("NEXUSAPP_Mwall_Display come in\n"));
	
	NEXUSAPP_Surface_SDSurface_DfRectGet(&rect);
	
	/* blit mw buffer surface default rect to sd display*/

	flash = (struct  osdFlash *)malloc(sizeof(struct osdFlash));
	if(!flash)
		return -1;
	
	memset(flash,0x00,sizeof(struct  osdFlash));
	flash->avp = avp;
	flash->mode = FLASH_FADEIN;
	flash->ifmw = 1;
	flash->pSfb = &avp->framebuff[SURFACE_MW_BUFFER];
	flash->pDfb = &avp->framebuff[SURFACE_SD_DISPLAY];
	memcpy(&flash->startPoint,&avp->osd.mw.default_rect,sizeof(NEXUS_Rect));

	DBG_OSD(("NEXUSAPP_Mwall_Display come in\n"));
	
	if(NEXUSAPP_Mwall_State_Ctrl(avp,OSD_MW_STATE_DISPLAYING))
		return 0;

	DBG_OSD(("NEXUSAPP_Mwall_Display init ok!\n"));
	
	pthread_mutex_lock(&avp->osd.mw.mwActionLock);
	
	NEXUSAPP_Osd_PicFlash_Action(flash);

	DBG_OSD(("NEXUSAPP_Osd_PicFlash_Action ok!\n"));
		
	return 0;
}

int NEXUSAPP_Mwall_Clean(struct av_playpump_t *avp)
{
	NEXUS_Rect dfRect;
	struct  osdFlash *flash;
		
	PARA_ASSERT_VALUE(avp,-1);

	NEXUSAPP_Surface_SDSurface_DfRectGet(&dfRect);
		
	/* blit mw buffer surface default rect to sd display*/
	flash = (struct  osdFlash *)malloc(sizeof(struct osdFlash));
	if(!flash)
		return -1;

	memset(flash,0x00,sizeof(struct  osdFlash));
	flash->avp = avp;
	flash->mode = FLASH_FADEOUT;
	flash->ifmw = 1;
	flash->pSfb = &avp->framebuff[SURFACE_SD_DISPLAY];
	flash->pDfb = &avp->framebuff[SURFACE_SD_DISPLAY];
	memcpy(&flash->startPoint,&avp->osd.mw.default_rect,sizeof(NEXUS_Rect));

	if(NEXUSAPP_Mwall_State_Ctrl(avp,OSD_MW_STATE_CLEANINT))
		return 0;
	
	pthread_mutex_lock(&avp->osd.mw.mwActionLock);
	
	NEXUSAPP_Osd_PicFlash_Action(flash);
		
	return 0;
}
#endif

