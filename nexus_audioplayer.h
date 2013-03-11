#ifndef NEXUS_AUDIOPLAYER_H
#define NEXUS_AUDIOPLAYER_H
/***********************************************************************************************/
#include "nexus_player.h"
/***********************************************************************************************/
void NEXUSAPP_AudioPlayer_Stop(av_playpump  *avp);
int 	NEXUSAPP_AudioPlayer_Start(av_playpump  *avp);
int 	NEXUSAPP_AudioPlayer_Play(av_playpump  *avp);
int 	NEXUSAPP_AudioPlayer_Pause(av_playpump  *avp);
int 	NEXUSAPP_AudioPlayer_Ready(av_playpump *avp);
int 	NEXUSAPP_AudioPlayer_PlayFile(av_playpump *avp ,  char *fname);
/***********************************************************************************************/
#endif

