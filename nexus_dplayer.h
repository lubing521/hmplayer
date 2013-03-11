#ifndef NEXUS_DPLAYER_H
#define NEXUS_DPLAYER_H
/***********************************************************************************************/
#define DPLAYER_LIST_MAX	20
/***********************************************************************************************/
typedef enum NEXUS_DplayerSongType
{
	DPLAYER_Songtype_eBVSong = 0,		/* backgroud video media */
	DPLAYER_Songtype_eRSSong,			/* Remote server video media */
	DPLAYER_Songtype_eMax,	
}DplayerSongType;
/***********************************************************************************************/
int NEXUSAPP_DPlayer_Ready(av_playpump *avp);
int NEXUSAPP_DPlayer_Aped_Song(char *fname , int iLen);
int NEXUSAPP_DPlayer_Play_Song(char *fname ,int iFLen ,DplayerSongType songType);
#endif

