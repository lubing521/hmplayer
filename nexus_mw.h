#ifndef NEXUS_MW_H
#define NEXUS_MW_H
/***********************************************************************************************/
#include "nexus_player.h"
/***********************************************************************************************/
#define OSD_MW_STATE_IDLE		0
#define OSD_MW_STATE_DISPLAYING	1
#define OSD_MW_STATE_CLEANINT	2
#define OSD_MW_STATE_BLITINT		3
#define OSD_MW_STATE_EVENT		4
/***********************************************************************************************/
int NEXUSAPP_Mwall_Default_Rect(struct av_playpump_t *avp , NEXUS_Rect *dfRect);
int NEXUSAPP_Mwall_Blit_Return(struct av_playpump_t *avp);
int NEXUSAPP_Mwall_Blit_Rect(struct av_playpump_t *avp ,NEXUS_Rect *sRect , NEXUS_Rect *dRect);
int NEXUSAPP_Mwall_Display(struct av_playpump_t *avp);
int NEXUSAPP_Mwall_Clean(struct av_playpump_t *avp);
void  NEXUSAPP_Mwall_State_Get(struct av_playpump_t *avp , int *iState);
void  NEXUSAPP_Mwall_State_Set(struct av_playpump_t *avp , int iState);
/***********************************************************************************************/
#endif

