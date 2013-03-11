/**************************************************************************
2011_01_19 mody by hh
1.mody struct osd_ctrl_info_t for mw
***************************************************************************/
#ifndef NEXUS_PIZPLAYER_H
#define NEXUS_PIZPLAYER_H
/***************************************************************************/
#include "nexus_player.h"
/***************************************************************************/
#define PIZPLAYER_MAX	10
/***************************************************************************/
typedef struct PizPlayerSettings_t
{
	char		szPath[40];
	char 	szAppName[40];
	int		iLoop;
	NEXUS_Rect dest_rect;
}PizPlayerSettings;
/***************************************************************************/
void 	NEXUSAPP_PizPlayer_Ready(struct av_playpump_t *avp);
int 	NEXUSAPP_PizPlayer_Check(struct av_playpump_t *avp , char *pAppName);
int 	NEXUSAPP_PizPlayer_Init(struct av_playpump_t *avp , PizPlayerSettings *pSetting);
int 	NEXUSAPP_PizPlayer_Setting(struct av_playpump_t *avp ,PizPlayerSettings *pSetting);
int 	NEXUSAPP_PizPlayer_Start(struct av_playpump_t *avp , char *pAppName);
int 	NEXUSAPP_PizPlayer_Stop(struct av_playpump_t *avp , char *pAppName);
int 	NEXUSAPP_PizPlayer_Destroy(struct av_playpump_t *avp , char *pAppName);
/***************************************************************************/

#endif

