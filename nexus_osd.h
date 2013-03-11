#ifndef NEXUS_OSD_H
#define NEXUS_OSD_H
/***********************************************************************************************/
#include "nexus_player.h"
#include "nexus_ctrl.h"
/***********************************************************************************************/
#define DISPLAY_FORMAT_HD_WX 1280
#define DISPLAY_FORMAT_HD_HY 720
#define DISPLAY_FORMAT_SD_WX 720
#define DISPLAY_FORMAT_SD_HY 480
/***********************************************************************************************/
#define BMZ_COLOR_ARGB32 6
#define BMZ_COLOR_BGRA32 7
/***********************************************************************************************/
#define FLASH_FADEIN				1
#define FLASH_FADEOUT			2
#define FLSHA_HIGHLIGHT_BUTTION	3
/***********************************************************************************************/
#define SURFACE_OSD_MODE_2D 	1
#define SURFACE_OSD_MODE_3DRL 	2
#define SURFACE_OSD_MODE_3DTB 	3
/***********************************************************************************************/
#define SURFACE_HD_DISPLAY 			0
#define SURFACE_HD_BUFFER			1
#define SURFACE_RText_BUFFER			2
#define SURFACE_SD_DISPLAY			3
#define SURFACE_SD_BUFFER			4
#define SURFACE_MW_BUFFER			5
#define SURFACE_MW_BACKUP			6
/***********************************************************************************************/
struct  osdFlash
{
	int mode;
	int ifmw;
	struct framebuff_t *pSfb;
	struct framebuff_t *pDfb;
	struct av_playpump_t *avp;
	NEXUS_Rect startPoint;
};
/***********************************************************************************************/
typedef struct
{
	char b;
	char g;
	char r;
	char a;	
}pixel_t;
/***********************************************************************************************/
int  NEXUSAPP_Surface_MyCreate(struct framebuff_t *fb);
int  NEXUSAPP_Surface_Display_Img(struct bmzimage_t *axbmp , NEXUS_Rect *startPoint , struct framebuff_t *fb);
void NEXUSAPP_Osd_Graphics2D_Blit(struct av_playpump_t *avp , NEXUS_Graphics2DBlitSettings *pblitSettings);
void NEXUSAPP_Osd_Surface_Fill(struct av_playpump_t *avp,NEXUS_Rect *rect ,NEXUS_Pixel color,NEXUS_SurfaceHandle surface);
int  NEXUSAPP_Surface_DefaultCreate( struct av_playpump_t *avp);
void *NEXUSAPP_Osd_Start_Pthread(void *context);
void NEXUSAPP_Osd_Piz_Close(struct  xxpiz_info_t *info);
void NEXUSAPP_Osd_RollText_Colse(struct av_playpump_t *avp);
void NEXUSAPP_Osd_Framebuff_Fill(struct av_playpump_t *avp,NEXUS_Rect *rect ,NEXUS_Pixel color,int framebuffId);
void NEXUSAPP_Osd_Framebuff_Blit(struct av_playpump_t *avp,NEXUS_Rect *SourceRect,NEXUS_Rect *DestRect,int SourceSurfaceIndex,int destSuerfaceIndex);

int 	NEXUSAPP_Osd_StaticText_Malloc(struct av_playpump_t *avp, struct osd_addContext_info_t *info);
int 	NEXUSAPP_Osd_StaticText_Free(struct av_playpump_t *avp);
int 	NEXUSAPP_Osd_PizBuffer_Malloc(struct  xxpiz_info_t *xxpizInfo);
int 	NEXUSAPP_Osd_PizBuffer_Free(struct  xxpiz_info_t *xxpizInfo);
int 	NEXUSAPP_Osd_RollText_Malloc(struct av_playpump_t *avp,struct osd_addContext_info_t *info);
int 	NEXUSAPP_Osd_RollText_Free(struct av_playpump_t *avp);
int 	NEXUSAPP_Osd_State_Sync(struct av_playpump_t *avp , int mode);
/***********************************************************************************************/
/*mw*/
int 	NEXUSAPP_Osd_PicFlash_Action(struct  osdFlash *flash);
void NEXUSAPP_Surface_SDSurface_DfRectGet( NEXUS_Rect *rect);
void	NEXUSAPP_Surface_HDSurface_DfRectGet( NEXUS_Rect *rect);
/***********************************************************************************************/
/*add by hh ,use 3dTV osd mode */
void NEXUSAPP_Osd_Mode_Set(av_playpump *avp ,int mode);
/***********************************************************************************************/
void NEXUSAPP_Surface_From_Video(av_playpump *avp , int playerid);
void NEXUSAPP_Surface_Video_Close(av_playpump *avp , int playerid);
/***********************************************************************************************/
#endif

