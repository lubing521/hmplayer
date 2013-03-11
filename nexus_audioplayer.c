#ifndef HM_AUDIOPLAYER_C
#define HM_AUDIOPLAYER_C
/************************************************************************************/
#include "nexus_audioplayer.h"
/************************************************************************************/
static void play_endOfStreamCallback(void *context, int param)
{
	av_playpump  *avp = (av_playpump  *)context;

	BSTD_UNUSED(param);

	PARA_ASSERT(avp);
	
	NEXUSAPP_AudioPlayer_Stop(avp);

	return;
}
/************************************************************************************/
/*************************************************
Function:    	NEXUSAPP_AudioPlayer_Stop
Description:	playpump stop 
Input:		1 . struct av_playpump_t *avp	:  playpump info struct 
Output:		
Return:                        
*************************************************/
void NEXUSAPP_AudioPlayer_Stop(av_playpump  *avp)
{
	if(avp == NULL || avp->audioplayer.start == false)
		return ;

	NEXUS_Playback_Stop(avp->audioplayer.playback);
	NEXUS_AudioDecoder_Stop(avp->audioplayer.audioDecoder);
	NEXUS_AudioDecoder_Flush(avp->audioplayer.audioDecoder);
	if(avp->audioplayer.file)
		NEXUS_FilePlay_Close(avp->audioplayer.file);

	avp->audioplayer.file = NULL;
	avp->audioplayer.start = false;

	return;
}

/*************************************************
Function:    	NEXUSAPP_AudioPlayer_Start
Description:	playpump start 
Input:		1 . av_playpump  *avp
Output:		
Return:           
*************************************************/
int NEXUSAPP_AudioPlayer_Start(av_playpump  *avp)
{
	NEXUS_PlaybackStartSettings playbackStartSettings;
	

	if(avp == NULL || avp->audioplayer.playback == NULL || avp->audioplayer.file == NULL)
		return -1;


	NEXUS_Playback_GetDefaultStartSettings(&playbackStartSettings);

	playbackStartSettings.mode = NEXUS_PlaybackMode_eAutoBitrate;
	/* Start decoders */
	NEXUS_AudioDecoder_Start(avp->audioplayer.audioDecoder, &avp->audioplayer.audioProgram);

	/* Start playback */
	NEXUS_Playback_Start(avp->audioplayer.playback, avp->audioplayer.file, NULL);
	avp->audioplayer.start = true;

	return 0;
}

/*************************************************
Function:    	NEXUSAPP_AudioPlayer_Play
Description:	audioplayer play 
Input:		1 . av_playpump  *avp
Output:		
Return:           	nexus error
*************************************************/
int NEXUSAPP_AudioPlayer_Play(av_playpump  *avp)
{		
	NEXUS_Error rc;
	if(avp == NULL || avp->audioplayer.playback == NULL || avp->audioplayer.start == false)
		return -1;
	
	rc = NEXUS_Playback_Play(avp->audioplayer.playback);
	
	BDBG_ASSERT(!rc);

	return 0;
}
/*************************************************
Function:    	NEXUSAPP_AudioPlayer_Pause
Description:	audioplayer pause 
Input:		1 . av_playpump  *avp
Output:		
Return:           	nexus error
*************************************************/
int NEXUSAPP_AudioPlayer_Pause(av_playpump  *avp)
{		
	NEXUS_Error rc;
	
	rc = NEXUS_Playback_Pause(avp->audioplayer.playback);
	
	BDBG_ASSERT(!rc);

	return 0;
}

/*************************************************
Function:    	NEXUSAPP_AudioPlayer_Init
Description:	playpump stop 
Input:		1 . struct av_playpump_t *avp	:  playpump info struct
			2 .  char *fname				:  stream file all path
Output:		
Return:                        
************************************************/
int NEXUSAPP_AudioPlayer_Init(av_playpump *avp,  char *fname)
{
	if(avp->audioplayer.start || avp->audioplayer.file)
		NEXUSAPP_AudioPlayer_Stop(avp);


	/* open playback media stream file */
	avp->audioplayer.file = NEXUS_FilePlay_OpenPosix(fname, NULL);

	if (!avp->audioplayer.file) {
		DBG_AUDIO_PLAYER(("can't open file:%s\n", fname));
		return -1;
	}
	
	return 0;
}

/*************************************************
Function:    	NEXUSAPP_AudioPlayer_Ready
Description:	playpump stop 
Input:		1 . struct av_playpump_t *avp	:  playpump info struct
			2 .  char *fname				:  stream file all path
Output:		
Return:                        
*************************************************/
int NEXUSAPP_AudioPlayer_Ready(av_playpump *avp)
{	
	NEXUS_Error rc;
	NEXUS_PlaybackSettings playbackSettings;
	NEXUS_PlaybackPidChannelSettings playbackPidSettings;

	if(avp->mixer == NULL )
		return -1;

	/* open audio player playpump */
	avp->audioplayer.playpump = NEXUS_Playpump_Open(2, NULL);
	BDBG_ASSERT(avp->audioplayer.playpump);

	/* open audio player playback */
	avp->audioplayer.playback= NEXUS_Playback_Create();
	BDBG_ASSERT(avp->audioplayer.playback);

	/* set audio player playback */
	NEXUS_Playback_GetSettings(avp->audioplayer.playback, &playbackSettings);
	playbackSettings.playpump = avp->audioplayer.playpump;
	playbackSettings.playpumpSettings.transportType = NEXUS_TransportType_eEs;
	playbackSettings.startPaused = false;
	playbackSettings.stcChannel = avp->audioplayer.stcChannel;
	playbackSettings.endOfStreamAction = NEXUS_PlaybackLoopMode_ePlay;
	playbackSettings.endOfStreamCallback.context = avp;
	playbackSettings.endOfStreamCallback.callback = play_endOfStreamCallback;
	rc = NEXUS_Playback_SetSettings(avp->audioplayer.playback, &playbackSettings);
	BDBG_ASSERT(!rc);

	/* open playback audio pid , mp3 es stream use 1, why 1? */
	NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
	playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
	playbackPidSettings.pidTypeSettings.audio.primary = avp->audioplayer.audioDecoder;
	playbackPidSettings.pidSettings.pidTypeSettings.audio.codec = NEXUS_AudioCodec_eMp3;
	
	avp->audioplayer.audioPidChannel = 
		NEXUS_Playback_OpenPidChannel(avp->audioplayer.playback, 1, &playbackPidSettings);

	NEXUS_AudioDecoder_GetDefaultStartSettings(&avp->audioplayer.audioProgram);
	
	avp->audioplayer.audioProgram.codec = NEXUS_AudioCodec_eMp3;
	avp->audioplayer.audioProgram.pidChannel = avp->audioplayer.audioPidChannel;
	avp->audioplayer.audioProgram.stcChannel = avp->audioplayer.stcChannel;

	return 0;
}

/*************************************************
Function:    	NEXUSAPP_AudioPlayer_Stop
Description:	playpump stop 
Input:		1 . struct av_playpump_t *avp	:  playpump info struct 
Output:		
Return:                        
*************************************************/
void NEXUSAPP_AudioPlayer_Clean(av_playpump  *avp)
{
	if(avp == NULL)
		return ;
	
	NEXUS_AudioDecoder_Stop(avp->audioplayer.audioDecoder);
	NEXUS_Playback_Stop(avp->audioplayer.playback);
	NEXUS_Playback_CloseAllPidChannels(avp->audioplayer.playback);
	NEXUS_Playback_Destroy(avp->audioplayer.playback);
	NEXUS_Playpump_Close(avp->audioplayer.playpump);
	
	if(avp->audioplayer.file)
		NEXUS_FilePlay_Close(avp->audioplayer.file);
	
	avp->audioplayer.playback = NULL;
	avp->audioplayer.file = NULL;
	avp->audioplayer.start = false;
}


/*************************************************
Function:    	NEXUSAPP_AudioPlayer_PlayFile
Description:	playpump stop 
Input:		1 . struct av_playpump_t *avp	:  playpump info struct
			2 .  char *fname				:  stream file all path
Output:		
Return:                        
*************************************************/
int NEXUSAPP_AudioPlayer_PlayFile(av_playpump *avp ,  char *fname)
{
	if(avp == NULL || fname == NULL)
		return -1;
	
	if(NEXUSAPP_AudioPlayer_Init(avp,fname))
		return -1;
	
	if(NEXUSAPP_AudioPlayer_Start(avp))
		return -1;
	
	/*NEXUSAPP_AudioPlayer_Play(avp);*/

	return 0;
}

/*************************************************
Function:    	NEXUSAPP_Start_Sd_Pthread
Description:	SD thread 
Input:		1 . void *context		:  thread  input parameters  struct 
Output:		
Return:
other : 		
*************************************************
static void *NEXUSAPP_AudioPlayer_Pthread(void *context)
{
	av_playpump *avp = NULL;

	PARA_ASSERT_VALUE(context,NULL);
	
	avp = (av_playpump *)context;

	PARA_ASSERT_VALUE(avp,NULL);
}

*************************************************
Function:    	NEXUSAPP_AudioPlayer_Init
Description:	playpump stop 
Input:		1 . struct av_playpump_t *avp	:  playpump info struct
			2 .  char *fname				:  stream file all path
Output:		
Return:                        
************************************************
int NEXUSAPP_AudioPlayer_Init(av_playpump *avp)
{
	pthread_t threadCtrl;
	pthread_attr_t attr;

	PARA_ASSERT(avp);

	pthread_attr_init (&attr);
	pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&threadCtrl,&attr, NEXUSAPP_AudioPlayer_Pthread,avp);
	pthread_attr_destroy (&attr);

	return ;
}
***********************************************************************************************/
#endif

