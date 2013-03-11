/**************************************************************************
2011_01_19 mody by hh
1.mody function NEXUSAPP_Ctrl_Action()  for mw
***************************************************************************/
#ifndef NEXUS_CTRL_C
#define NEXUS_CTRL_C

#include "nexus_ctrl.h"
#include "nexus_osd.h"
#include "nexus_mw.h"

/*************************************************
  Function:    	TRACE_CONTENTS
  Description:	Debug tool , print hex data
  Input:		1 . int iLen 	:	message buffer size
  			2 . char *szStr	:	message buffer
  Output:		
  Return:
  other : 		
*************************************************/
void TRACE_CONTENTS(int iLen,char * szStr)
{
	int iLine;
	int uiInx=0;
	
	char szTemp[400];
/*
	if(!DEBUG)
		return;
*/
	
#define HEX_HIGH(c) (  ((unsigned char)((c&0xf0)>>4)>=10)?(unsigned char)((c&0xf0)>>4)-10+'A':(unsigned char)((c&0xf0)>>4)+'0' )
#define HEX_LOW(c)	(  ((unsigned char)((c&0x0f)   )>=10)?(unsigned char)((c&0x0f)   )-10+'A':(unsigned char)((c&0x0f)   )+'0' )
#define ASCII(c)    ((((unsigned char)c)>=0x7f || ((unsigned char)c)<=0x20)? '.':c)
	

	while(1)
	{
		sprintf(szTemp,"\x0d\012Total Len:%d\x0d\012Page %d:inx %d\x0d\x0a",iLen,uiInx/(5*7)+1,uiInx);
		printf(szTemp);
		for (iLine=1;iLine<=16;iLine++)
		{
			sprintf(szTemp,"%c%c %c%c %c%c %c%c %c%c %c%c %c%c %c%c %c%c %c%c %c%c %c%c %c%c %c%c %c%c %c%c |%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\x0d\x0a",
				HEX_HIGH(szStr[uiInx   ]),HEX_LOW(szStr[uiInx   ]),
				HEX_HIGH(szStr[uiInx+ 1]),HEX_LOW(szStr[uiInx+ 1]),
				HEX_HIGH(szStr[uiInx+ 2]),HEX_LOW(szStr[uiInx+ 2]),
				HEX_HIGH(szStr[uiInx+ 3]),HEX_LOW(szStr[uiInx+ 3]),
				HEX_HIGH(szStr[uiInx+ 4]),HEX_LOW(szStr[uiInx+ 4]),
				HEX_HIGH(szStr[uiInx+ 5]),HEX_LOW(szStr[uiInx+ 5]),
				HEX_HIGH(szStr[uiInx+ 6]),HEX_LOW(szStr[uiInx+ 6]),
				HEX_HIGH(szStr[uiInx+ 7]),HEX_LOW(szStr[uiInx+ 7]),
				HEX_HIGH(szStr[uiInx+ 8]),HEX_LOW(szStr[uiInx+ 8]),
				HEX_HIGH(szStr[uiInx+ 9]),HEX_LOW(szStr[uiInx+ 9]),
				HEX_HIGH(szStr[uiInx+10]),HEX_LOW(szStr[uiInx+10]),
				HEX_HIGH(szStr[uiInx+11]),HEX_LOW(szStr[uiInx+11]),
				HEX_HIGH(szStr[uiInx+12]),HEX_LOW(szStr[uiInx+12]),
				HEX_HIGH(szStr[uiInx+13]),HEX_LOW(szStr[uiInx+13]),
				HEX_HIGH(szStr[uiInx+14]),HEX_LOW(szStr[uiInx+14]),
				HEX_HIGH(szStr[uiInx+15]),HEX_LOW(szStr[uiInx+15]),
				ASCII(szStr[uiInx   ]),
				ASCII(szStr[uiInx+ 1]),
				ASCII(szStr[uiInx+ 2]),
				ASCII(szStr[uiInx+ 3]),
				ASCII(szStr[uiInx+ 4]),
				ASCII(szStr[uiInx+ 5]),
				ASCII(szStr[uiInx+ 6]),
				ASCII(szStr[uiInx+ 7]),
				ASCII(szStr[uiInx+ 8]),
				ASCII(szStr[uiInx+ 9]),
				ASCII(szStr[uiInx+10]),
				ASCII(szStr[uiInx+11]),
				ASCII(szStr[uiInx+12]),
				ASCII(szStr[uiInx+13]),
				ASCII(szStr[uiInx+14]),
				ASCII(szStr[uiInx+15])
				);
			printf(szTemp);
			memset(szTemp,0x00,sizeof(szTemp));
			uiInx+=16;
			if (uiInx>=iLen)
			{
				printf("The end\r\n");
				return;
			}
		}
	}
}

/*************************************************
  Function:    	NEXUSAPP_Windows_Disconnect
  Description:	window disconnect to video decoder
  Input:		1 . NEXUS_VideoWindowHandle window			:  window handle
  			2 . NEXUS_VideoDecoderHandle videoDecoder	:  video decoder handle
  Output:		
  Return:
  other : 		
*************************************************/
void NEXUSAPP_Windows_Disconnect(NEXUS_VideoWindowHandle window ,NEXUS_VideoDecoderHandle videoDecoder)
{
	PARA_ASSERT(window);
	
	NEXUS_VideoWindow_RemoveInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));
}

/*************************************************
  Function:    	NEXUSAPP_Windows_Connect
  Description:	window connect to video decoder
  Input:		1 . NEXUS_VideoWindowHandle window			:  window handle
  			2 . NEXUS_VideoDecoderHandle videoDecoder	:  video decoder handle
  Output:		
  Return:
  other : 		
*************************************************/
void NEXUSAPP_Windows_Connect(NEXUS_VideoWindowHandle window ,NEXUS_VideoDecoderHandle videoDecoder)
{
	PARA_ASSERT(window);
		
	NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));
}

/*************************************************
  Function:    	NEXUSAPP_Ctrl_Windows_StateSet
  Description:	window state set , visible or invisible
  Input:		1 . struct av_playpump_t *avp			:  playpump info struct
  			2 . NEXUS_VideoWindowHandle window		:  video decoder handle
  			3 . bool visible							:  true : visible , false : invisible
  Output:		
  Return:		0 : succeed
  other : 		
*************************************************/
int NEXUSAPP_Ctrl_Windows_StateSet(struct av_playpump_t *avp,NEXUS_VideoWindowHandle window, bool visible)
{
	if(window == NULL )
		return -1;

	if(avp->display.winHd_1 == window)
	{	
		if(!avp->display.pipVisible&&visible)
			return 0;
	}
	
	return NEXUSAPP_Ctrl_Windows_Visible(window,visible);
	
}

/*************************************************
  Function:    	NEXUSAPP_Ctrl_Windows_Visible
  Description:	window visible or invisible
  Input:		1 . NEXUS_VideoWindowHandle window		:  video decoder handle
  			2 . bool visible							:  true : visible , false : invisible
  Output:		
  Return:		0 : succeed
  other : 		
*************************************************/
int NEXUSAPP_Ctrl_Windows_Visible(NEXUS_VideoWindowHandle window, bool visible)
{

	NEXUS_VideoWindowSettings windowSettings;
	
	if(!window)
		return 0;

	NEXUS_VideoWindow_GetSettings(window,&windowSettings);

	if(windowSettings.visible != visible)
	{
		windowSettings.visible = visible;
		NEXUS_VideoWindow_SetSettings(window,&windowSettings);
	}
	return 0;
}

/*************************************************
  Function:    	NEXUSAPP_Ctrl_Windows_Visible
  Description:	window visible or invisible
  Input:		1 . NEXUS_VideoWindowHandle window		:  video decoder handle
  			2 . bool visible							:  true : visible , false : invisible
  Output:		
  Return:		0 : succeed
  other : 		
*************************************************/
void NEXUSAPP_Windows_Alpha(NEXUS_VideoWindowHandle window , unsigned char alpha)
{
	NEXUS_VideoWindowSettings  windowsettings;	
	
	NEXUS_VideoWindow_GetSettings(window,&windowsettings);
	windowsettings.alpha = alpha;
	NEXUS_VideoWindow_SetSettings(window,&windowsettings);
}


/*************************************************
  Function:    	NEXUSAPP_Ctrl_PipWindow_PosSet
  Description:	window info set
  Input:			1 . struct av_playpump_t *avp		:  playpump handle
  Output:		
  Return:		0 : succeed
  other : 		
*************************************************/
int NEXUSAPP_Ctrl_PipWindow_PosSet(struct av_playpump_t *avp)
{

	NEXUS_VideoWindowSettings windowSettings;
	
	PARA_ASSERT_VALUE(avp->display.winHd_1,CTRL_OSD_ERROR_TRANSPORT);

	NEXUS_VideoWindow_GetSettings(avp->display.winHd_1,&windowSettings);
	memset(&windowSettings.position,0x00,sizeof(NEXUS_Rect));
	memcpy(&windowSettings.position,&avp->display.pipInfo,sizeof(NEXUS_Rect));
	windowSettings.visible = avp->display.pipVisible;
	windowSettings.allocateFullScreen = false;
	windowSettings.contentMode =NEXUS_VideoWindowContentMode_eBox;
	NEXUS_VideoWindow_SetSettings(avp->display.winHd_1,&windowSettings);
	
	return 0;
}
/*************************************************
  Function:    	NEXUSAPP_Playpump_Windows_Disconnect
  Description:	playpump HD/SD player 's windows disconnect video decoder 
  Input:		1 . struct av_player_t *player		:  HD/SD player info struct
  Output:		
  Return:		
  other : 		
*************************************************/
void NEXUSAPP_Playpump_Windows_Disconnect(struct av_player_t *player)
{

	NEXUSAPP_Windows_Disconnect(player->win_0,player->videoDecoder);
	NEXUSAPP_Windows_Disconnect(player->win_1,player->videoDecoder);

	return ;
}

/*************************************************
  Function:    	NEXUSAPP_Playpump_Windows_Connect
  Description:	playpump HD/SD player 's windows connect video decoder 
  Input:		1 . struct av_player_t *player		:  HD/SD player info struct
  Output:		
  Return:		
  other : 		
*************************************************/
void NEXUSAPP_Playpump_Windows_Connect(struct av_player_t *player )
{
	if(player->win_0)
		NEXUSAPP_Windows_Connect(player->win_0,player->videoDecoder);

	if(player->win_1)
		NEXUSAPP_Windows_Connect(player->win_1,player->videoDecoder);
	
	return ;
}

/*************************************************
  Function:    	NEXUSAPP_Ctrl_Pip_Visible
  Description:	HD display pip control , visible or invisible 
  Input:		1 . struct av_playpump_t *avp		:  playpump player info struct
  			2 . bool visible						:  true : visible , false : invisible
  Output:		
  Return:		1 . 0 : succeed  
  other : 		
*************************************************/
static int NEXUSAPP_Ctrl_Pip_Visible( struct av_playpump_t *avp, bool visible)
{

	PARA_ASSERT_VALUE(avp->display.display_hd,CTRL_OSD_ERROR_TRANSPORT);
	PARA_ASSERT_VALUE(avp->sdPlayer.videoDecoder,CTRL_OSD_ERROR_TRANSPORT);

	return NEXUSAPP_Ctrl_Windows_Visible(avp->display.winHd_1,visible);
}
/*************************************************
  Function:    	NEXUSAPP_Ctrl_PipInfo_Set
  Description:	HD display pip window info set
  Input:		1 . struct av_playpump_t *avp		:  playpump player info struct
  			2 . osd_rect *position				:  winhd_1 window position
  Output:		
  Return:		1 . 0 : succeed
  			    	2 . -1: error
  other : 		
*************************************************/
int NEXUSAPP_Ctrl_PipInfo_Set( struct av_playpump_t *avp, osd_rect *position)
{

	PARA_ASSERT_VALUE(avp->display.winHd_1,CTRL_OSD_ERROR_TRANSPORT);
	PARA_ASSERT_VALUE(position,CTRL_OSD_ERROR_TRANSPORT);

	memset(&avp->display.pipInfo,0x00,sizeof(NEXUS_Rect));
	memcpy(&avp->display.pipInfo,position,sizeof(NEXUS_Rect));
	
	return NEXUSAPP_Ctrl_PipWindow_PosSet(avp);
}
/*************************************************
  Function:    	NEXUSAPP_Ctrl_DisplayMode_Change
  Description:	HD/SD display mode change action
  Input:		1 . struct av_playpump_t *avp		:  playpump player info struct
  Output:		
  Return:		1 . 0 : succeed  
  other : 		
*************************************************/
int NEXUSAPP_Ctrl_DisplayMode_Change( struct av_playpump_t *avp)
{

	NEXUS_VideoWindowSettings  settings_tmp;
	NEXUS_VideoWindowSettings  videowsettings;
	NEXUS_VideoWindowHandle window;
	
	NEXUS_VideoWindow_GetSettings(avp->display.winHd_1,&settings_tmp);
	NEXUS_VideoWindow_GetSettings(avp->display.winHd_0,&videowsettings);	
	
	NEXUS_DisplayModule_SetUpdateMode(NEXUS_DisplayUpdateMode_eManual);
	if(avp->uiWinNorm)
	{
		NEXUS_VideoWindow_SetSettings(avp->display.winHd_1, &videowsettings);
		NEXUS_VideoWindow_SetSettings(avp->display.winHd_0, &settings_tmp);
	}
	else
	{
		NEXUS_VideoWindow_SetSettings(avp->display.winHd_0, &videowsettings);
		NEXUS_VideoWindow_SetSettings(avp->display.winHd_1, &settings_tmp);
	}

	window = avp->display.winHd_1;
	avp->display.winHd_1 = avp->display.winHd_0;
	avp->display.winHd_0 = window;
	
	NEXUS_DisplayModule_SetUpdateMode(NEXUS_DisplayUpdateMode_eAuto);
	return 0;

}

/*************************************************
  Function:    	NEXUSAPP_Ctrl_DisplayMode_Preset
  Description:	HD/SD display mode change preset . it will set change display mode  without close video decoder , 
  			and will be call by begin of NEXUSAPP_Ctrl_DisplayMode_Set function.
  Input:		1 . struct av_playpump_t *avp		:  playpump player info struct
  			2 . int sourceMode 					:  
  Output:		
  Return:		1 . 0 : succeed  
  other : 		
*************************************************/
int NEXUSAPP_Ctrl_DisplayMode_Preset(struct av_playpump_t *avp,int sourceMode , int destMode)
{
	int sMode , dMode; 

	sMode = sourceMode;
	dMode = destMode;

	MSG_APP(("smode = %d , dmode = %d \n",sMode,dMode));

	if((sMode== 1 && (dMode == 5 || dMode == 7))
	||(sMode == 2 && (dMode == 6 ||dMode == 8))
	||(sMode == 3 && (dMode == 7 || dMode == 5))
	||(sMode == 4 && (dMode == 8 || dMode == 6))
	)
	{
		NEXUSAPP_Ctrl_Pip_Visible(avp,true);
		sMode +=4;
	}
	
	if((sMode == 5 && (dMode == 1||dMode ==3))
	||(sMode == 6 && (dMode == 2||dMode ==4))
	||(sMode == 7 && (dMode == 3||dMode ==1))
	||(sMode == 8 && (dMode == 4||dMode ==2))
	)
	{
		NEXUSAPP_Ctrl_Pip_Visible(avp,false);
		sMode -=4;
	}

	MSG_APP(("smode = %d , dmode = %d \n",sMode,dMode));
	
	if((sMode== 1 && dMode == 3)
	||(sMode== 2 && dMode == 4)
	||(sMode== 3 && dMode == 1)
	||(sMode== 4 && dMode == 2)
	||(sMode== 5 && dMode == 7)
	||(sMode ==6 && dMode == 8)
	||(sMode== 7 && dMode == 5)
	||(sMode ==8 && dMode == 6)
	)
	{
		NEXUSAPP_Ctrl_DisplayMode_Change(avp);	
		sMode = dMode ;
	}

	MSG_APP(("smode = %d , dmode = %d \n",sMode,dMode));

	if(sMode == destMode)
		return 0;
	else
		return -1;

}

/*************************************************
  Function:    	NEXUSAPP_Ctrl_DisplayMode_Set
  Description:	HD/SD display mode change  . 
  Input:		1 . struct av_playpump_t *avp		:  playpump player info struct
  			2 . int mode						:  display mode , reference to "Display_Mode" define 					:  
  Output:		
  Return:		1 . 0 : succeed  
  other : 		
*************************************************/
int NEXUSAPP_Ctrl_DisplayMode_Set(struct av_playpump_t *avp,int mode)
{
	NEXUS_PlaypumpOpenPidChannelSettings settings;
	av_player *player ;

	player = &avp->sdPlayer;

	if(mode == avp->display.mode || mode >= CTRL_DISPLAY_MODE_eMax)
		return 0;

	pthread_mutex_lock(&avp->playerLock);
/* must be mody later important , 2011-02-23 !!!!!!!!!!!!!!*/
	DBG_APP(("NEXUSAPP_Ctrl_DisplayMode_Set : display = %d , mode = %d\n",avp->display.mode,mode));
	if(avp->display.mode == CTRL_DISPLAY_MODE_eA_A && mode == CTRL_DISPLAY_MODE_eA_B)
	{
		
		NEXUSAPP_Ctrl_Windows_Visible(avp->display.winSd_0,false);

		if(avp->sdPlayer.decoderStart)
			NEXUS_VideoDecoder_Stop(avp->sdPlayer.videoDecoder);

		/*NEXUSAPP_Windows_Disconnect(avp->display.winSd_0,avp->hdPlayer.videoDecoder);*/
		NEXUS_VideoWindow_RemoveAllInputs(avp->display.winSd_0);

		NEXUSAPP_Windows_Connect(avp->display.winSd_0,avp->sdPlayer.videoDecoder);

		NEXUS_Playpump_GetDefaultOpenPidChannelSettings(&settings);
		settings.pidType = NEXUS_PidType_eVideo;
		player->videoPidChannel.pidChannel = NEXUS_Playpump_OpenPidChannel(player->playpump,player->videoPidChannel.pid, &settings);
	
		player->videoProgram.pidChannel = player->videoPidChannel.pidChannel;
		player->videoProgram.stcChannel = player->stcChannel;

		if(avp->sdPlayer.decoderStart)
			NEXUS_VideoDecoder_Start(avp->sdPlayer.videoDecoder , &avp->sdPlayer.videoProgram);

		avp->hdPlayer.win_0 = avp->display.winHd_0;
		avp->hdPlayer.win_1 = NULL;
		avp->sdPlayer.win_0 = avp->display.winSd_0;
		avp->sdPlayer.win_1 = NULL;
		
		NEXUSAPP_Ctrl_Windows_Visible(avp->display.winSd_0,true);
	}
	else if(avp->display.mode == CTRL_DISPLAY_MODE_eA_B && mode == CTRL_DISPLAY_MODE_eA_A)
	{
	
		NEXUSAPP_Ctrl_Windows_Visible(avp->display.winSd_0,false);

		/*NEXUS_VideoDecoder_Stop(avp->sdPlayer.videoDecoder);*/
		/*NEXUSAPP_Windows_Disconnect(avp->display.winSd_0,avp->sdPlayer.videoDecoder);*/
		NEXUS_VideoWindow_RemoveAllInputs(avp->display.winSd_0);
		NEXUSAPP_Windows_Connect(avp->display.winSd_0,avp->hdPlayer.videoDecoder);
		/*
		if(avp->sdPlayer.decoderStart)
			NEXUS_VideoDecoder_Start(avp->sdPlayer.videoDecoder , &avp->sdPlayer.videoProgram);
		*/

		avp->hdPlayer.win_0 = avp->display.winHd_0;
		avp->hdPlayer.win_1 = avp->display.winSd_0;;
		avp->sdPlayer.win_0 = NULL;
		avp->sdPlayer.win_1 = NULL;
		
		NEXUSAPP_Ctrl_Windows_Visible(avp->display.winSd_0,true);
	}
	else
	{
		;
	}

	avp->display.mode = mode ;
	pthread_mutex_unlock(&avp->playerLock);

	return 0;
/* end mody **************/
	


/* deal with mode change without close decoder */
	if(!NEXUSAPP_Ctrl_DisplayMode_Preset(avp,avp->display.mode,mode))
	{
		avp->display.mode = mode;
		return 0;
	}	
	MSG_APP(("NEXUSAPP_Ctrl_DisplayMode_Set :  lock player!\n"));

	pthread_mutex_lock(&avp->playerLock);

	MSG_APP(("NEXUSAPP_Ctrl_DisplayMode_Set :  stop decoder!\n"));

/*	must be test , because  nexus sdk say must be close decorder
	
	if(avp->hdPlayer.decoderStart)
		NEXUS_VideoDecoder_Stop(avp->hdPlayer.videoDecoder);

	if(avp->sdPlayer.decoderStart)
		NEXUS_VideoDecoder_Stop(avp->sdPlayer.videoDecoder);
	
	if(avp->audioDecoderStart)
		NEXUS_AudioDecoder_Stop(avp->audioDecoder);
*/

	NEXUSAPP_Ctrl_Windows_Visible(avp->display.winHd_0,false);
	NEXUSAPP_Ctrl_Windows_Visible(avp->display.winSd_0,false);
	NEXUSAPP_Ctrl_Windows_Visible(avp->display.winHd_1,false);

	MSG_APP(("NEXUSAPP_Ctrl_DisplayMode_Set :  init windows!\n"));
	NEXUSAPP_Playpump_Windows_Disconnect(&avp->hdPlayer);
	NEXUSAPP_Playpump_Windows_Disconnect(&avp->sdPlayer);	
	
	avp->hdPlayer.win_0 = NULL;
	avp->hdPlayer.win_1 = NULL;
	avp->sdPlayer.win_0 = NULL;
	avp->sdPlayer.win_1 = NULL;

	MSG_APP(("NEXUSAPP_Ctrl_DisplayMode_Set : set windows!\n"));

	switch(mode)
	{
		case CTRL_DISPLAY_MODE_eA_B:
			avp->hdPlayer.win_0 = avp->display.winHd_0;
			avp->sdPlayer.win_0 = avp->display.winSd_0;
			avp->sdPlayer.win_1 = avp->display.winHd_1;
			avp->display.pipVisible = false;
			break;
		case CTRL_DISPLAY_MODE_eB_A:
			avp->hdPlayer.win_0 = avp->display.winSd_0;
			avp->sdPlayer.win_0 = avp->display.winHd_0;
			avp->hdPlayer.win_1 = avp->display.winHd_1;
			avp->display.pipVisible = false;
			break;
		case CTRL_DISPLAY_MODE_eA_A:
		case CTRL_DISPLAY_MODE_eA_A_SYNC:	
			avp->hdPlayer.win_0 = avp->display.winHd_0;
			avp->hdPlayer.win_1 = avp->display.winSd_0;
			avp->sdPlayer.win_0 = avp->display.winHd_1;
			avp->display.pipVisible = false;
			break;
		case CTRL_DISPLAY_MODE_eB_B:		
			avp->sdPlayer.win_0 = avp->display.winHd_0;
			avp->sdPlayer.win_1 = avp->display.winSd_0;
			avp->hdPlayer.win_0 = avp->display.winHd_1;
			avp->display.pipVisible = false;
			break;
		case CTRL_DISPLAY_MODE_eAB_A:
			avp->hdPlayer.win_0 = avp->display.winHd_0;
			avp->hdPlayer.win_1 = avp->display.winSd_0;
			avp->sdPlayer.win_0 = avp->display.winHd_1;
			avp->display.pipVisible = true;
			break;
		case CTRL_DISPLAY_MODE_eAB_B:
			avp->hdPlayer.win_0 = avp->display.winHd_0;
			avp->sdPlayer.win_0 = avp->display.winSd_0;
			avp->sdPlayer.win_1 = avp->display.winHd_1;
			avp->display.pipVisible = true;
			break;
		case CTRL_DISPLAY_MODE_eBA_A:
			avp->sdPlayer.win_0 = avp->display.winHd_0;
			avp->hdPlayer.win_1 = avp->display.winSd_0;
			avp->hdPlayer.win_0 = avp->display.winHd_1;
			avp->display.pipVisible = true;
			break;
		case CTRL_DISPLAY_MODE_eBA_B:	
			avp->sdPlayer.win_0 = avp->display.winHd_0;
			avp->sdPlayer.win_1 = avp->display.winSd_0;
			avp->hdPlayer.win_0 = avp->display.winHd_1;
			avp->display.pipVisible = true;			
			break;
		default:
			break;
	}	
	
	NEXUSAPP_Playpump_Windows_Connect(&avp->hdPlayer);
	NEXUSAPP_Playpump_Windows_Connect(&avp->sdPlayer);

	NEXUSAPP_Ctrl_Windows_Visible(avp->display.winHd_0,true);
	NEXUSAPP_Ctrl_Windows_Visible(avp->display.winSd_0,true);
	NEXUSAPP_Ctrl_Pip_Visible(avp,avp->display.pipVisible);
/*			
	if(avp->hdPlayer.decoderStart)
	{
		NEXUS_VideoDecoder_Start(avp->hdPlayer.videoDecoder,&avp->hdPlayer.videoProgram);
	}
	if(avp->sdPlayer.decoderStart)
	{
		NEXUS_VideoDecoder_Start(avp->sdPlayer.videoDecoder,&avp->sdPlayer.videoProgram);
	}
	
	if(avp->hdPlayer.audioSelected&&avp->audioDecoderStart)
		NEXUS_AudioDecoder_Start(avp->audioDecoder,&avp->hdPlayer.audioProgram);

	if(avp->sdPlayer.audioSelected&&avp->audioDecoderStart)
		NEXUS_AudioDecoder_Start(avp->audioDecoder,&avp->sdPlayer.audioProgram);
*/
	avp->display.mode = mode ;
	pthread_mutex_unlock(&avp->playerLock);

	return 0;	
	
}


/*************************************************
  Function:    	NEXUSAPP_Ctrl_VoiceVolume_Init
  Description:	change voice volume format , nexus rang : -9000 ~ 0  , -9000 : mute , 0 : max
  			we must change rang  0~100  to  -9000 ~ -60 ,  
  			change  rule : 
  				1 . -9000		:  mute 0 
				2 . -5010 		:  1
				3 . -60 		:  max  100
				4 . step is 50
				
  Input:		1 . int32_t volume		:  playpump player info struct 
  Output:		
  Return:		changed value
  other : 		
*************************************************/
int32_t NEXUSAPP_Ctrl_VoiceVolume_Init(int32_t volume)
{
	int32_t minVolume = -5000;
	int32_t stepValue = 50;

	if(volume == 0)
		return -9000;
	
	return (minVolume+(stepValue*volume));
}

/*************************************************
  Function:    	NEXUSAPP_Ctrl_VoiceVolume_Set
  Description:	Set  dac and spdif 's voice volume 
  Input:		1 . struct av_playpump_t *avp		:  playpump player info struct			
  			2 . int volume						:  voice volume , rang : 0 ~ 100
  Output:		
  Return:		1 . 0 : succeed
  other : 		
*************************************************/
int  NEXUSAPP_Ctrl_VoiceVolume_LowSet(av_playpump *avp,int volume)
{
	int32_t vol;
	NEXUS_AudioOutputSettings pSet;
	NEXUS_AudioOutput output;

	output = NEXUS_AudioDac_GetConnector(avp->platformConfig.outputs.audioDacs[0]);
	NEXUS_AudioOutput_GetSettings(output,&pSet);
	pSet.volumeType = NEXUS_AudioVolumeType_eDecibel;

	if(volume>100)
		volume = 100;
	else if(volume<0)
		volume = 0;
	
	vol = NEXUSAPP_Ctrl_VoiceVolume_Init(volume);

	pSet.leftVolume = vol;
	pSet.rightVolume = vol;
		
	NEXUS_AudioOutput_SetSettings(output,&pSet);

	if(!avp->hdPlayer.ifcompressedAudio)
	{
		output = NEXUS_SpdifOutput_GetConnector(avp->platformConfig.outputs.spdif[0]);
		NEXUS_AudioOutput_GetSettings(output,&pSet);
		pSet.leftVolume = vol;
		pSet.rightVolume = vol;
		NEXUS_AudioOutput_SetSettings(output,&pSet);
	}
	
	output =NEXUS_HdmiOutput_GetAudioConnector(avp->platformConfig.outputs.hdmi[0]);
	NEXUS_AudioOutput_GetSettings(output,&pSet);
	pSet.volumeType = NEXUS_AudioVolumeType_eDecibel;
	pSet.leftVolume = vol;
	pSet.rightVolume = vol;
	NEXUS_AudioOutput_SetSettings(output,&pSet);

	return 0;
	
}

/*************************************************
  Function:    	NEXUSAPP_Ctrl_VoiceVolume_Set
  Description:	Set  dac and spdif 's voice volume 
  Input:		1 . struct av_playpump_t *avp		:  playpump player info struct			
  			2 . int volume						:  voice volume , rang : 0 ~ 100
  Output:		
  Return:		1 . 0 : succeed
  other : 		
*************************************************/
int  NEXUSAPP_Ctrl_VoiceVolume_Set(av_playpump *avp,int volume)
{
	int iRet = 0;

	pthread_mutex_lock(&avp->playerLock);

	iRet = NEXUSAPP_Ctrl_VoiceVolume_LowSet(avp , volume);
	
	pthread_mutex_unlock(&avp->playerLock);

	return iRet;
}

/*************************************************
  Function:    	NEXUSAPP_Ctrl_VoiceVolume_FadeIn
  Description:  audio volume fade in , start pthread when audio unmute;
  Input:		1 . struct av_playpump_t *avp		:  playpump player info struct			
  Output:		
  Return:		
  other : 		
*************************************************/
void  *NEXUSAPP_Ctrl_VoiceVolume_FadeIn(void *context)
{
	int i = 1;
	int iFlag = 1;
	int setpmsec = 0;
	av_playpump *avp = (av_playpump *)context;

	if(avp == NULL)
		return NULL;

	if(avp->audioVolume == 0 )
		return NULL;
	
	avp->audioChangeState = 1;
	
	/* 1 SEC complete */
	setpmsec = (1000 / avp->audioVolume); 

	DBG_APP(("VoiceVolume_FadeIn : volume = %d , msec = %d\n",avp->audioVolume , setpmsec));

	NEXUSAPP_Ctrl_AudioDac_State_Set(avp, AUDIO_UNMUTE);	
	iFlag = 0;

	while(i < avp->audioVolume)
	{	
		usleep(setpmsec * 1000);
		NEXUSAPP_Ctrl_VoiceVolume_Set(avp,i);
		i++;

		if(avp->audioChangeState == 0 )
			goto Volume_FadeIn_thread_out;

	}

Volume_FadeIn_thread_out:
	
	NEXUSAPP_Ctrl_VoiceVolume_Set(avp,avp->audioVolume);
	
	avp->audioChangeState = 0;

	return NULL;
}

/*************************************************
  Function:    	NEXUSAPP_Ctrl_VoiceVolume_Silent
  Description:	Set  dac and spdif 's silent 
  Input:		1 . struct av_playpump_t *avp		:  playpump player info struct			
  			2 . int volume						:  voice volume , rang : 0 ~ 100
  Output:		
  Return:		1 . 0 : succeed
  other : 		
*************************************************/
int  NEXUSAPP_Ctrl_VoiceVolume_Silent(av_playpump *avp,int ifsilent)
{
	NEXUS_AudioOutputSettings pSet;
	NEXUS_AudioOutputSettings pSetTmp;	
	NEXUS_AudioOutput output;
	pthread_t threadAudioFadein;
	pthread_attr_t attr;

	output = NEXUS_AudioDac_GetConnector(avp->platformConfig.outputs.audioDacs[0]);
	NEXUS_AudioOutput_GetSettings(output,&pSet);
		
	if(ifsilent)
		pSet.muted = true;
	else
		pSet.muted = false;
	
	NEXUS_AudioOutput_SetSettings(output,&pSet);
	
	output =NEXUS_HdmiOutput_GetAudioConnector(avp->platformConfig.outputs.hdmi[0]);
	NEXUS_AudioOutput_GetSettings(output,&pSetTmp);
	pSetTmp.muted = pSet.muted;
	NEXUS_AudioOutput_SetSettings(output,&pSetTmp);

	if(ifsilent == AUDIO_UNMUTE)
	{
		#if 0
		/* remove by hh , 2010_10_08
		add by hh , 2012_07_09*/
		if(avp->audioSilentCtrl)
		{
			OwnControlSpdif(1);
		}
		else
		{
			output =NEXUS_SpdifOutput_GetConnector(avp->platformConfig.outputs.spdif[0]);
			NEXUS_AudioOutput_GetSettings(output,&pSetTmp);
			pSetTmp.muted = pSet.muted;
			NEXUS_AudioOutput_SetSettings(output,&pSetTmp);
		}
		#else
			output =NEXUS_SpdifOutput_GetConnector(avp->platformConfig.outputs.spdif[0]);
			NEXUS_AudioOutput_GetSettings(output,&pSetTmp);
			pSetTmp.muted = pSet.muted;
			NEXUS_AudioOutput_SetSettings(output,&pSetTmp);

		#endif

		NEXUSAPP_Ctrl_VoiceVolume_LowSet(avp , 0);
		
		DBG_APP(("VoiceVolume_Silent : set dac unmute , state = %d \n" , avp->audioChangeState));
		
		NEXUSAPP_Ctrl_AudioDac_State_Set(avp, AUDIO_UNMUTE);

		if(avp->audioChangeState == 0)
			avp->audioChangeState = 1;
		else
			return 0;
		
		pthread_attr_init (&attr);
		pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);
		pthread_create(&threadAudioFadein,&attr, NEXUSAPP_Ctrl_VoiceVolume_FadeIn,avp);
		pthread_attr_destroy (&attr);
	}
	else
	{
		#if 0
		/* remove by hh , 2010_10_08
		add by hh , 2012_07_07*/
		if(avp->audioSilentCtrl)
		{
			OwnControlSpdif(0);
		}
		else
		{
			output =NEXUS_SpdifOutput_GetConnector(avp->platformConfig.outputs.spdif[0]);
			NEXUS_AudioOutput_GetSettings(output,&pSetTmp);
			pSetTmp.muted = pSet.muted;			
			NEXUS_AudioOutput_SetSettings(output,&pSetTmp);
		}
		#else
			output =NEXUS_SpdifOutput_GetConnector(avp->platformConfig.outputs.spdif[0]);
			NEXUS_AudioOutput_GetSettings(output,&pSetTmp);
			pSetTmp.muted = pSet.muted;			
			NEXUS_AudioOutput_SetSettings(output,&pSetTmp);

		#endif
		
		avp->audioChangeState = 0;
		NEXUSAPP_Ctrl_VoiceVolume_LowSet(avp , 0);

		DBG_APP(("VoiceVolume_Silent : set dac mute , state = %d \n" , avp->audioChangeState));
		
		NEXUSAPP_Ctrl_AudioDac_State_Set(avp, AUDIO_MUTE);
	}


	return 0;
	
}
/*************************************************
  Function:    	NEXUSAPP_Ctrl_AudioChannel_Set
  Description:	audio channel set , left . right  or Stereo 
  Input:		1 . struct av_playpump_t *avp		:  playpump player info struct			
  			2 . int value						:  reference to  " typedef enum PLAYER_AudioChannelMode" define
  Output:		
  Return:		1 . 0 : succeed
  other : 		
*************************************************/
int NEXUSAPP_Ctrl_AudioChannel_Set(av_playpump *avp,av_player *player ,int value)
{
	NEXUS_AudioOutputSettings pSet;
	NEXUS_AudioOutput output;

	if(avp->audioDecoderStart == false || player->audioSelected == false)
		return -1;
	
	pthread_mutex_lock(&avp->playerLock);
	
	output = NEXUS_AudioDac_GetConnector(avp->platformConfig.outputs.audioDacs[0]);
	NEXUS_AudioOutput_GetSettings(output,&pSet);
	pSet.channelMode = value;
	MSG_APP((">> audio channel set  ...[%d]\n",value));
	NEXUS_AudioOutput_SetSettings(output,&pSet);
	
	output =NEXUS_HdmiOutput_GetAudioConnector(avp->platformConfig.outputs.hdmi[0]);
	NEXUS_AudioOutput_GetSettings(output,&pSet);
	pSet.channelMode = value;
	NEXUS_AudioOutput_SetSettings(output,&pSet);

	DBG_APP(("----->>>>>>>> [%s] -- [%s]channel mode = %d!!\n" ,
		player->playerId == PLAYER_PLAYPUMP_HD?"HD":"SD",
		player->ifcompressedAudio?"pass":"decoder",pSet.channelMode));

/*
	if(!player->ifcompressedAudio)
	{
*/

	output = NEXUS_SpdifOutput_GetConnector(avp->platformConfig.outputs.spdif[0]);
	NEXUS_AudioOutput_GetSettings(output,&pSet);
	pSet.channelMode = value;
	NEXUS_AudioOutput_SetSettings(output,&pSet);

/*
	}
*/
 	pthread_mutex_unlock(&avp->playerLock);

	return 0;
}
	
static void NEXUSAPP_Ctrl_AudioDecoder_Silent(NEXUS_AudioDecoderHandle tInputAudioDecoder , int silent)
{
	NEXUS_AudioDecoderSettings tTempAudioDecoderSettings;
	int tTempVolume,i,j;

	NEXUS_AudioDecoder_GetSettings( tInputAudioDecoder,&tTempAudioDecoderSettings);    

	if(silent == AUDIO_MUTE )
		tTempVolume = 1 ;
	else
		tTempVolume = NEXUS_AUDIO_VOLUME_LINEAR_NORMAL;

	DBG_APP(("NEXUSAPP_Ctrl_AudioDecoder_Silent: source = %d  --> %d \n",tTempAudioDecoderSettings.volumeMatrix[0][0],tTempVolume));
	
	for(i=0;i<NEXUS_AudioChannel_eMax;i++)
	{
		for(j=0;j<NEXUS_AudioChannel_eMax;j++)
		{
			if( tTempAudioDecoderSettings.volumeMatrix[i][j]!=0 )
				tTempAudioDecoderSettings.volumeMatrix[i][j]=tTempVolume;
		}
	}
	
       NEXUS_AudioDecoder_SetSettings(tInputAudioDecoder,&tTempAudioDecoderSettings);
	
}

int NEXUSAPP_Ctrl_AudioDecoder_Set(av_playpump *avp,av_player *player ,int compressedAudio)
{
	NEXUS_AudioOutput output;
	NEXUS_AudioOutputSettings pSet;

	if(player->playerId == PLAYER_PLAYPUMP_SD)
		return 0;
	
	output = NEXUS_SpdifOutput_GetConnector(avp->platformConfig.outputs.spdif[0]);

	/* Remove All connecter to spdif output */
	NEXUS_AudioOutput_RemoveAllInputs(output);
	
	/* compressedAudio audioDecoder direct connect to spdif */
	if(compressedAudio)
	{
		NEXUS_AudioOutput_GetSettings(output,&pSet);
		pSet.channelMode = NEXUS_AudioChannelMode_eStereo;
		NEXUS_AudioOutput_SetSettings(output,&pSet);

		NEXUS_AudioMixer_RemoveInput(avp->mixer,NEXUS_AudioDecoder_GetConnector(avp->audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
			
		NEXUS_AudioOutput_AddInput(output,NEXUS_AudioDecoder_GetConnector(avp->audioDecoder, NEXUS_AudioConnectorType_eCompressed));		
	}
	else/* uncompressedAudio audioDecoder connect to mixer */
	{	
		/* add by hh , add second audiodecoder 2012_09_24*/
		NEXUSAPP_Ctrl_AudioDecoder_Silent(avp->audioDecoder , AUDIO_UNMUTE);
		NEXUSAPP_Ctrl_AudioDecoder_Silent(avp->audio2Decoder, AUDIO_MUTE);
		
		NEXUS_AudioMixer_AddInput(avp->mixer,NEXUS_AudioDecoder_GetConnector(avp->audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
		NEXUS_AudioMixer_AddInput(avp->mixer,NEXUS_AudioDecoder_GetConnector(avp->audio2Decoder, NEXUS_AudioDecoderConnectorType_eStereo));

		/* Add  to the mixer */
		NEXUS_AudioOutput_AddInput(output,NEXUS_AudioMixer_GetConnector(avp->mixer));
	}
	
	return 0;
}

/*************************************************
  Function:    	NEXUSAPP_Ctrl_AudioChannel_Set
  Description:	audio channel set , left . right  or Stereo 
  Input:		1 . struct av_playpump_t *avp		:  playpump player info struct			
  			2 . int value						:  reference to  " typedef enum PLAYER_AudioChannelMode" define
  Output:		
  Return:		1 . 0 : succeed
  other : 		
*************************************************/
int NEXUSAPP_Ctrl_AudioTrack_Set(av_playpump *avp,av_player *player ,int value)
{

	int pidIndex = 0;

	pidIndex = value - 3;

	if(pidIndex < 0 || pidIndex > 1)
		return -1;

	if(player->audioPidChannel[pidIndex].pid <= 0)
		return -1;

	if(avp->audioDecoderStart == false || player->audioSelected == false)
		return -1;

	/* spdif test open retrun; */
	
	DBG_APP(("NEXUSAPP_Ctrl_AudioTrack_Set : %d \n" ,value));
	
	/* mody compressed audio decoder 2012_05_20
	*/
	
	if(player->ifcompressedAudio)
	{
		pthread_mutex_lock(&avp->playerLock);
		DBG_APP(("NEXUSAPP_Ctrl_AudioTrack_Set : compressed audio %d \n" , pidIndex));

		if (pidIndex) /*high track*/
		{
			NEXUS_AudioDecoder_Stop(avp->audioDecoder);
			NEXUS_AudioDecoder_Start(avp->audioDecoder, &player->audio2Program);
		}
		else
		{
			NEXUS_AudioDecoder_Stop(avp->audioDecoder);
			NEXUS_AudioDecoder_Start(avp->audioDecoder, &player->audioProgram);
		}	
	}
	else
	{
		pthread_mutex_lock(&avp->playerLock);
		
		if (pidIndex)
		{
			NEXUSAPP_Ctrl_AudioDecoder_Silent(avp->audioDecoder , AUDIO_MUTE);
			NEXUSAPP_Ctrl_AudioDecoder_Silent(avp->audio2Decoder , AUDIO_UNMUTE);
		}
		else
		{
			NEXUSAPP_Ctrl_AudioDecoder_Silent(avp->audio2Decoder , AUDIO_MUTE);
			NEXUSAPP_Ctrl_AudioDecoder_Silent(avp->audioDecoder , AUDIO_UNMUTE);
		}
	}

	pthread_mutex_unlock(&avp->playerLock);
	
	/* must be set eStereo *to change track , 7231 SDK change stereo enum*/
	NEXUSAPP_Ctrl_AudioChannel_Set(avp,player,NEXUS_AudioChannelMode_eStereo);

	DBG_OSD(("NEXUSAPP_Ctrl_AudioTrack_Set : out \n"));
	return 0;
}

int NEXUSAPP_Ctrl_Audio_Mode_Set(av_playpump *avp,av_player *player ,int value)
{
	if(avp == NULL||player==NULL)
		return -1;

	player->audiomode = value;

	if(!player->decoderStart)
		return -1;
	DBG_APP(("NEXUSAPP_Ctrl_Audio_Mode_Set: %d\n",player->audiomode));
	
	if(value == 1 || value == 2)
		return NEXUSAPP_Ctrl_AudioChannel_Set(avp,player,value);
	else
		return NEXUSAPP_Ctrl_AudioTrack_Set(avp,player,value);
}

void NEXUSAPP_Ctrl_AudioDac_GPIO_Set(av_playpump *avp , int gpio , int mode )
{
	NEXUS_GpioHandle tTempGpio;
	NEXUS_GpioSettings tTempGpioSettings;
	av_playpump *player = avp ;

	/* unused player */
	(void *)player;

	NEXUS_Gpio_GetDefaultSettings(NEXUS_GpioType_eStandard, &tTempGpioSettings);   

	tTempGpioSettings.mode = NEXUS_GpioMode_eOutputPushPull;

	tTempGpio = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, gpio, &tTempGpioSettings);
		
	if( tTempGpio != 0 )
	{
		NEXUS_Gpio_GetSettings(tTempGpio,&tTempGpioSettings);
		
		tTempGpioSettings.value = mode;
	
		NEXUS_Gpio_SetSettings(tTempGpio,&tTempGpioSettings);

		NEXUS_Gpio_Close( tTempGpio );                        			 // 关闭IO 口
	}
}
void NEXUSAPP_Ctrl_AudioDac_State_Set(av_playpump *avp , int mode )
{
	NEXUS_GpioHandle tTempGpio;
	NEXUS_GpioSettings tTempGpioSettings;
	
	NEXUS_AudioOutputSettings pSet;
	NEXUS_AudioOutput output;	

	output = NEXUS_AudioDac_GetConnector(avp->platformConfig.outputs.audioDacs[0]);
	NEXUS_AudioOutput_GetSettings(output,&pSet);
	if(mode == AUDIO_MUTE)
		pSet.muted = true;
	else
		pSet.muted = false;
	NEXUS_AudioOutput_SetSettings(output,&pSet);

	output = NEXUS_AudioDac_GetConnector(avp->platformConfig.outputs.audioDacs[0]);
	NEXUS_AudioOutput_GetSettings(output,&pSet);
	if(mode == AUDIO_UNMUTE)
	{
		pSet.leftVolume = -9000;
		pSet.rightVolume = -9000;
	}
	NEXUS_AudioOutput_SetSettings(output,&pSet);

	usleep(10000);

	

	if(mode == AUDIO_MUTE)
		NEXUSAPP_Ctrl_AudioDac_GPIO_Set(avp,55,NEXUS_GpioValue_eLow); /* dac codec output mute */
	else
	{
		NEXUSAPP_Ctrl_AudioDac_GPIO_Set(avp,61,NEXUS_GpioValue_eHigh);
		usleep(100000);
		NEXUSAPP_Ctrl_AudioDac_GPIO_Set(avp,55,NEXUS_GpioValue_eHigh); /* dac codec output mute */
		usleep(100000);
		NEXUSAPP_Ctrl_AudioDac_GPIO_Set(avp,61,NEXUS_GpioValue_eLow);
	}
	
	return ;

	NEXUS_Gpio_GetDefaultSettings(NEXUS_GpioType_eStandard, &tTempGpioSettings);   

	tTempGpioSettings.mode = NEXUS_GpioMode_eOutputPushPull;

	tTempGpio = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 55, &tTempGpioSettings);
		
	if( tTempGpio != 0 )
	{
		NEXUS_Gpio_GetSettings(tTempGpio,&tTempGpioSettings);
		
		if(mode == AUDIO_MUTE)
			tTempGpioSettings.value = NEXUS_GpioValue_eLow; /* dac codec output mute */
		else
			tTempGpioSettings.value = NEXUS_GpioValue_eHigh;/* dac codec output unmute*/				

		NEXUS_Gpio_SetSettings(tTempGpio,&tTempGpioSettings);

		NEXUS_Gpio_Close( tTempGpio );                        			 // 关闭IO 口
	}


	usleep(100000);
}

/*************************************************
  Function:    	NEXUSAPP_Ctrl_AudioSource_Set
  Description:	audio source  set ,  HD audio or SD audio input  
  Input:		1 . struct av_playpump_t *avp		:  playpump player info struct			
  			2 . int value						:  reference to  " typedef enum PLAYER_AudioSource" define
  Output:		
  Return:		1 . 0 : succeed
  other : 		
*************************************************/
int  NEXUSAPP_Ctrl_AudioSource_Set( struct av_playpump_t *avp,int value)
{	
	pthread_mutex_lock(&avp->playerLock);

	if(value == PLAYER_AudioSource_eHd 
		&& !avp->hdPlayer.audioSelected
		&& avp->hdPlayer.decoderStart
	)
	{		
		NEXUS_AudioDecoder_Stop(avp->audioDecoder);
		NEXUS_AudioDecoder_Start(avp->audioDecoder,&avp->hdPlayer.audioProgram);
		avp->hdPlayer.audioSelected = true;
		avp->sdPlayer.audioSelected = false;
	}
	else if(value == PLAYER_AudioSource_eSd 
		&& !avp->sdPlayer.audioSelected
		&& avp->sdPlayer.decoderStart
	)
	{
		NEXUS_AudioDecoder_Stop(avp->audioDecoder);
		NEXUS_AudioDecoder_Start(avp->audioDecoder,&avp->sdPlayer.audioProgram);
		avp->sdPlayer.audioSelected = true;
		avp->hdPlayer.audioSelected = false;
	}

	pthread_mutex_unlock(&avp->playerLock);

	return 0;
}

/*************************************************
  Function:    	NEXUSAPP_Ctrl_OsdState_Set
  Description:	Osd state set 
  Input:		1 . struct av_playpump_t *avp		:  playpump player info struct			
  			2 . Ctrl_OsdState_item osdItem		:  reference to  " Ctrl_OsdState_item osdItem" define
  			3 . int value						:  osd item state 
  						#define CTRL_OSD_STATE_CHANGE 			1
						#define CTRL_OSD_STATE_PLAY			 	2
						#define CTRL_OSD_STATE_STOP		 		3
						#define CTRL_OSD_STATE_COMPLETE		 	4
						#define CTRL_OSD_STATE_BUSY				5
						#define CTRL_OSD_STATE_ERROR			 	6
  Output:		
  Return:		1 . 0 : succeed
  other : 		
*************************************************/
void NEXUSAPP_Ctrl_OsdState_Set(struct av_playpump_t *avp,Ctrl_OsdState_item osdItem,int value)
{
	
	pthread_mutex_lock(&avp->osd.osdLock);
	switch(osdItem)
	{
		case CTRL_STATE_ITEM_ePIZHD:
			avp->osd.piz.pizHd.state= value;
			break;
		case CTRL_STATE_ITEM_ePIZSD:
			avp->osd.piz.pizSd.state= value;
			break;	
		case CTRL_STATE_ITEM_eTEXTSTATIC:
			avp->osd.textStatic.state = value;
			break;
		case CTRL_STATE_ITEM_eTEXTROLL:
			avp->osd.textRoll.state =value;
			break;
		default:
			break;
	}	
	pthread_mutex_unlock(&avp->osd.osdLock);
}

/*************************************************
  Function:    	NEXUSAPP_Ctrl_OsdState_Set
  Description:	Osd state get 
  Input:		1 . struct av_playpump_t *avp		:  playpump player info struct			
  			2 . Ctrl_OsdState_item osdItem		:  reference to  " Ctrl_OsdState_item osdItem" define
  Output:		1 . int *value 						:  osd state
  						#define CTRL_OSD_STATE_CHANGE 			1
						#define CTRL_OSD_STATE_PLAY			 	2
						#define CTRL_OSD_STATE_STOP		 		3
						#define CTRL_OSD_STATE_COMPLETE		 	4
						#define CTRL_OSD_STATE_BUSY				5
						#define CTRL_OSD_STATE_ERROR			 	6
  Return:		1 . 0 : succeed
  other : 		
*************************************************/
void NEXUSAPP_Ctrl_OsdState_Get(struct av_playpump_t *avp , Ctrl_OsdState_item osdItem, int *value)
{
	pthread_mutex_lock(&avp->osd.osdLock);
	switch(osdItem)
	{
		case CTRL_STATE_ITEM_ePIZHD:
			*value = avp->osd.piz.pizHd.state;
			break;
		case CTRL_STATE_ITEM_ePIZSD:
			*value = avp->osd.piz.pizSd.state;
			break;			
		case CTRL_STATE_ITEM_eTEXTSTATIC:
			*value = avp->osd.textStatic.state;
			break;
		case CTRL_STATE_ITEM_eTEXTROLL:
			*value = avp->osd.textRoll.state;
			break;
		default:
			break;
	}	
	pthread_mutex_unlock(&avp->osd.osdLock);

}

/*************************************************
  Function:    	NEXUSAPP_Ctrl_Flag_Get
  Description:	osd message flag get
  Input:		1 . int iSockFd		:  control center socket fd			
  Output:		1 . unsigned int *iFlag 						
  					CTRL_FLAG_ID 			:	ctrl message flag 
  					CTRL_ADDCONTEXT_ID		:	add context message flag
  Return:		1 . 0 	  : succeed
  			2 . -1 : tcp transport error
  other : 		
*************************************************/
int NEXUSAPP_Ctrl_CmdFlag_Get(int iSockFd , unsigned int *iFlag)
{
	int icmdFlag =0 ;
	char szRecv[8];
	unsigned long ulRecvlen =4;
	
	while(1)
	{
		memset(&szRecv,0x00,sizeof(szRecv));
		
		if(socket_tcp_server_read(iSockFd, szRecv,&ulRecvlen)<0)
		{
			MSG_APP(("NEXUSAPP_Ctrl_CmdFlag_Get: recv error !\n"));
			return -1;
		}

		memcpy(&icmdFlag,szRecv,4);
		
		if(icmdFlag == CTRL_FLAG_ID||icmdFlag == CTRL_ADDCONTEXT_ID	)
			break;
	}	

	*iFlag =icmdFlag ;
		
	return 0;
}

/*************************************************
  Function:    	NEXUSAPP_Ctrl_CmdInfo_Get
  Description:	get osd ctrl message info struct
  Input:		1 . int iSockFd						:  control center socket fd			
  Output:		1 .  struct osd_ctrl_info_t *ctrlData 	:  ctrl message info struct					
  Return:		1 . 0   : succeed
  			2 . -1 : tcp transport error
  other : 		
*************************************************/
int NEXUSAPP_Ctrl_CmdInfo_Get(int iSockFd , struct osd_ctrl_info_t *ctrlData)
{

	struct osd_ctrl_info_t Data_t;
	unsigned long ulRercLen =sizeof(Data_t);
	
	if(iSockFd==-1||ctrlData == NULL)
		return -1;

	memset(&Data_t,0x00,sizeof(Data_t));
	
	if(socket_tcp_server_read(iSockFd, (char *)&Data_t,&ulRercLen)<0)
	{
		DBG_APP(("NEXUSAPP_Ctrl_CmdInfo_Get: recv error!\n"));
		return -1;
	}
	memcpy(ctrlData,&Data_t,sizeof(Data_t));
	
	return 0;
}

/*************************************************
  Function:    	NEXUSAPP_Ctrl_Respond_Send
  Description:	control message respond , received every control message , we must be respond
  			initiative message used by roll text complete.
  Input:		1 . int iSockFd						:  control center socket fd
  			2 . int messageType				:  
  								#define CTRL_OSD_MESSAGE_ERROR		0x10
								#define CTRL_OSD_MESSAGE_INITIATIVE	0x20
			3 . int message 		
								#define CTRL_OSD_ERROR_NOTHING		0
								#define CTRL_OSD_ERROR_TRANSPORT		-1
								#define CTRL_OSD_ERROR_MEMORY		-2
								#define CTRL_OSD_ERROR_BUSY			-3
								
  Output:						
  Return:		1 . 0   : succeed
  			2 . -1 : tcp transport error
  other : 		
*************************************************/
int NEXUSAPP_Ctrl_Respond_Send(int iSockFd ,int messageType, int message)
{
	osd_ctrl_respond respond;

	memset(&respond,0x00,sizeof(respond));

	respond.flagId = messageType;
	respond.message= message;

	if(iSockFd == -1)
		goto error_out;
	
	if(socket_tcp_server_send(iSockFd, (char *)&respond,sizeof(osd_ctrl_respond))<0)
	{
		DBG_APP(("NEXUSAPP_Ctrl_Respond_Send: send error !\n"));
		goto error_out;
	}

	MSG_APP(("respond send ok: flag : %d , mess: %d\n",respond.flagId,respond.message));

	return 0;

error_out:
	return CTRL_OSD_ERROR_TRANSPORT;
	
}

/*************************************************
  Function:    	NEXUSAPP_Ctrl_AddContextInfo_Get
  Description:	Add context info struct get from tcp socket
  Input:		1 . int iSockFd							:  control center socket fd
  			2 . struct osd_addContext_info_t *ctrlData	:  add context info struct
  Output:						
  Return:		1 . 0   : succeed
  			2 . -1 : tcp transport error or input parameter is nulll
  other : 		
*************************************************/
int NEXUSAPP_Ctrl_AddContextInfo_Get(int iSockFd , struct osd_addContext_info_t *ctrlData)
{

	struct osd_addContext_info_t Data_t;
	unsigned long ulRecvlen =sizeof(Data_t);

	
	if(iSockFd==-1||ctrlData == NULL)
		return -1;

	memset(&Data_t,0x00,sizeof(Data_t));
	
	if(socket_tcp_server_read(iSockFd, (char *)&Data_t,&ulRecvlen)<0)
	{
		DBG_APP(("NEXUSAPP_Ctrl_AddContextInfo_Get: recv error !\n"));
		return -1;
	}
	memcpy(ctrlData,&Data_t,sizeof(Data_t));

	MSG_APP(("AddContextInfo : fbid : %d , contextid:%d, context type : %d\n",ctrlData->fbId,ctrlData->contextId,ctrlData->contextType));
	
	return 0;
}

/*************************************************
  Function:    	NEXUSAPP_Ctrl_ActionWaitComplete
  Description:	wait for ctrl commond complete , time out 4S .
  Input:		1 . struct av_playpump_t *avp	:  playpump player info struct	
  			2 . Ctrl_OsdState_item osdItem	:  Ctrl_OsdState_item
  Output:						
  Return:		1 . 0   : succeed
  			2 . -1 : wait time out or error out
  other : 		
*************************************************/
int NEXUSAPP_Ctrl_ActionWaitComplete(struct av_playpump_t *avp , Ctrl_OsdState_item osdItem)
{
	int value = -1;
	int iWaitCount = 4000;
	while(iWaitCount>0)
	{
		iWaitCount -- ;
		usleep(1000);
		NEXUSAPP_Ctrl_OsdState_Get(avp,osdItem,&value);

		if(value == CTRL_OSD_STATE_PLAY || value == CTRL_OSD_STATE_ERROR)
			break;
	}

	if(value ==CTRL_OSD_STATE_PLAY)
		return 0;
	else
		return -1;
}

/*************************************************
  Function:    	NEXUSAPP_Ctrl_PizContextInfo_Action
  Description:	piz picture 's context receive , if receive complete , change piz state , osd thread will be action .
  Input:		1 . struct av_playpump_t *avp	:  playpump player info struct	
  			2 . osd_addContext_info_t *info	:  osd_addContext_info_t info struct
  Output:						
  Return:		1 . 0   : succeed
  			2 . -1 : tcp transport error
  other : 		
*************************************************/
int NEXUSAPP_Ctrl_PizContextInfo_Action( struct av_playpump_t *avp,  struct osd_addContext_info_t *info)
{
	xxpiz_info *xxpizInfo;
	Ctrl_OsdState_item item;
	int iRet = 0;
	
	PARA_ASSERT_VALUE(avp,-1);
	PARA_ASSERT_VALUE(info,-1); 

	if(info->fbId == FRAMEBUFF_HD)
	{
		xxpizInfo = &avp->osd.piz.pizHd;
		item = CTRL_STATE_ITEM_ePIZHD;
	}
	else
	{
		xxpizInfo = &avp->osd.piz.pizSd;
		item = CTRL_STATE_ITEM_ePIZSD;
	}
	
	if(info->iIndex == 1)
	{
		
		if(xxpizInfo->pizBuf)
			NEXUSAPP_Osd_PizBuffer_Free(xxpizInfo);

		DBG_OSD(("file size = %d \n",xxpizInfo->pizSize));
	
		if(NEXUSAPP_Osd_PizBuffer_Malloc(xxpizInfo))
		{
			DBG_OSD(("malloc error!\n "));
			return CTRL_OSD_ERROR_MEMORY;
		}	
	}
	
	/* protect pizbuf*/
	if(xxpizInfo->pizBuf ==NULL
	||xxpizInfo->pizRecSize +info->contextLen > xxpizInfo->pizSize
	||info->contextLen > ADDCONTEXT_LEN_MAX
	||info->iIndex > info->iTotalCount
	)
	{
		NEXUSAPP_Osd_PizBuffer_Free(xxpizInfo);
		xxpizInfo->pizSize =0;
		xxpizInfo->pizRecSize =0;
		return CTRL_OSD_ERROR_TRANSPORT;
	}
	
	memcpy(xxpizInfo->pizBuf+xxpizInfo->pizRecSize,info->context,info->contextLen);

	xxpizInfo->pizRecSize +=info->contextLen;

	if(info->iIndex == info->iTotalCount)
	{
		if(xxpizInfo->pizSize == xxpizInfo->pizRecSize)
		{
				
			NEXUSAPP_Ctrl_OsdState_Set(avp,item,CTRL_OSD_STATE_CHANGE);
			iRet = NEXUSAPP_Ctrl_ActionWaitComplete(avp,item);
			return iRet ;
		}
		else
		{
			NEXUSAPP_Osd_PizBuffer_Free(xxpizInfo);
			xxpizInfo->pizSize =0;
			xxpizInfo->pizRecSize =0;
			return CTRL_OSD_ERROR_TRANSPORT;
		}
	}
	
	return CTRL_OSD_ERROR_NOTHING;
}

/*************************************************
  Function:    	NEXUSAPP_Ctrl_StaticContextInfo_Action
  Description:	static  picture 's context receive , if receive complete , change static picture's state , osd thread will be action .
  Input:		1 . struct av_playpump_t *avp	:  playpump player info struct	
  			2 . osd_addContext_info_t *info	:  osd_addContext_info_t info struct
  Output:						
  Return:		1 . 0   : succeed
  			2 . -1 : tcp transport error
  other : 		
*************************************************/
int NEXUSAPP_Ctrl_StaticContextInfo_Action( struct av_playpump_t *avp,  struct osd_addContext_info_t *info)
{
	struct av_osd_text_t *text_t;
	char *pBuf =NULL;
	int iRet = 0;

	PARA_ASSERT_VALUE(avp,-1);
	PARA_ASSERT_VALUE(info,-1);

	text_t = &avp->osd.textStatic;

	PARA_ASSERT_VALUE(text_t,-1);

	/* pic clean flash */
	DBG_OSD(("text action : pic size : %d , playmode : %d\n",text_t->picSize,info->displayCount));
	
	if(text_t->picSize == 0)
	{
		text_t->picDisplayMode = info->displayCount;
		NEXUSAPP_Ctrl_OsdState_Set(avp,CTRL_STATE_ITEM_eTEXTSTATIC,CTRL_OSD_STATE_CHANGE);
		iRet = NEXUSAPP_Ctrl_ActionWaitComplete(avp,CTRL_STATE_ITEM_eTEXTSTATIC);
		DBG_OSD(("text action complete,repond : %d\n",iRet));
		return iRet ;
	}
	

	if(info->iIndex == 1)
	{	
		if(text_t->pic.ps)
			NEXUSAPP_Osd_StaticText_Free(avp);						
		
		if(NEXUSAPP_Osd_StaticText_Malloc(avp,info))
			return CTRL_OSD_ERROR_MEMORY;

		text_t->picDisplayMode = info->displayCount;
	}
		
	/* protect pizbuf*/
	if(text_t->pic.ps==NULL
	||text_t->picRecSize + info->contextLen> text_t->picSize
	||info->contextLen > ADDCONTEXT_LEN_MAX
	||info->iIndex > info->iTotalCount
	||text_t->fbId != info->fbId
	)
	{
		
		NEXUSAPP_Osd_StaticText_Free(avp);
		text_t->picSize =0;
		text_t->picRecSize =0;
		return CTRL_OSD_ERROR_TRANSPORT;
	}

	pBuf = (char *)text_t->pic.ps;
	
	memcpy(pBuf+text_t->picRecSize,info->context,info->contextLen);

	text_t->picRecSize +=info->contextLen;

	if(info->iIndex == info->iTotalCount)
	{
		if(text_t->picRecSize == text_t->picSize)
		{
			NEXUSAPP_Ctrl_OsdState_Set(avp,CTRL_STATE_ITEM_eTEXTSTATIC,CTRL_OSD_STATE_CHANGE);
			DBG_OSD(("wait for static text action complete!\n"));
			iRet = NEXUSAPP_Ctrl_ActionWaitComplete(avp,CTRL_STATE_ITEM_eTEXTSTATIC);
			DBG_OSD(("text action complete,repond : %d\n",iRet));
			return iRet ;
		}
		else
		{
			NEXUSAPP_Osd_StaticText_Free(avp);
			text_t->picSize =0;
			text_t->picRecSize =0;
			return CTRL_OSD_ERROR_TRANSPORT;
		}
	}

	return 0;
	
}

/*************************************************
  Function:    	NEXUSAPP_Ctrl_RollContextInfo_Action
  Description:	roll text  picture 's context receive , if receive complete , change roll text picture's state , osd thread will be action .
  Input:		1 . struct av_playpump_t *avp	:  playpump player info struct	
  			2 . osd_addContext_info_t *info	:  osd_addContext_info_t info struct
  Output:						
  Return:		1 . 0   : succeed
  			2 . -1 : tcp transport error
  other : 		
*************************************************/
int NEXUSAPP_Ctrl_RollContextInfo_Action( struct av_playpump_t *avp,  struct osd_addContext_info_t *info)
{
	struct av_osd_text_roll_t *text_t;
	char *pBuf =NULL;
	int iRet = 0;


	PARA_ASSERT_VALUE(avp,-1);
	PARA_ASSERT_VALUE(info,-1);

	text_t = &avp->osd.textRoll;

	PARA_ASSERT_VALUE(text_t,-1);

	
	if(info->iIndex == 1)
	{	
		if(text_t->pic.ps)
			NEXUSAPP_Osd_RollText_Free(avp);
		
		if(NEXUSAPP_Osd_RollText_Malloc(avp,info))
			return CTRL_OSD_ERROR_MEMORY;

		text_t->NewCount = info->displayCount;
		DBG_OSD(("roll text info : index = %d , preset size = %d\n",info->iIndex,text_t->picSize));
	}

	/* protect buffer*/
	if(text_t->pic.ps==NULL
	||text_t->picRecSize +info->contextLen > text_t->picSize
	||info->contextLen > ADDCONTEXT_LEN_MAX
	)
	{
		NEXUSAPP_Osd_RollText_Free(avp);
		text_t->picSize =0;
		text_t->picRecSize =0;
		return CTRL_OSD_ERROR_TRANSPORT;
	}

	pBuf = (char *)text_t->pic.ps;

	memcpy(pBuf+text_t->picRecSize,info->context,info->contextLen);

	text_t->picRecSize +=info->contextLen;

	if(info->iIndex == info->iTotalCount )
	{
		if(text_t->picRecSize == text_t->picSize)
		{
			NEXUSAPP_Ctrl_OsdState_Set(avp,CTRL_STATE_ITEM_eTEXTROLL,CTRL_OSD_STATE_CHANGE);
			DBG_OSD(("wait for roll text action complete!\n"));
			iRet = NEXUSAPP_Ctrl_ActionWaitComplete(avp,CTRL_STATE_ITEM_eTEXTROLL);
			DBG_OSD(("text action complete,repond : %d\n",iRet));
			return iRet ;
		}
		else
		{
			NEXUSAPP_Osd_RollText_Free(avp);
			text_t->picSize =0;
			text_t->picRecSize =0;
			return CTRL_OSD_ERROR_TRANSPORT;
		}
	}
	
	return 0;
}

/*************************************************
  Function:    	NEXUSAPP_Ctrl_AddContextInfo_Action
  Description:	Get add context data  , and respond .
  Input:		1 . struct av_playpump_t *avp	:  playpump player info struct	
  			2 . osd_addContext_info_t *info	:  osd_addContext_info_t info struct
  Output:						
  Return:		1 . 0   : succeed
  			2 . -1 : tcp transport error
  other : 		
*************************************************/
int NEXUSAPP_Ctrl_AddContextInfo_Action( struct av_playpump_t *avp,  struct osd_addContext_info_t *info)
{
	int iRet =0 ;
	
	switch(info->contextId)
	{
		case CTRL_STATE_ITEM_eTEXTSTATIC:
			iRet = NEXUSAPP_Ctrl_StaticContextInfo_Action(avp,info);
			break;
		case CTRL_STATE_ITEM_eTEXTROLL:
			iRet =  NEXUSAPP_Ctrl_RollContextInfo_Action(avp,info);
			break;
		case CTRL_STATE_ITEM_ePIZHD:
		case CTRL_STATE_ITEM_ePIZSD:	
			iRet =  NEXUSAPP_Ctrl_PizContextInfo_Action(avp,info);
			break;
		default:
			break;
	}

	return NEXUSAPP_Ctrl_Respond_Send(avp->socketCtrlFd,CTRL_OSD_MESSAGE_ERROR,iRet);	
	
}

/*************************************************
  Function:    	NEXUSAPP_Ctrl_Action
  Description:	commond message  action 
  Input:		1 . struct av_playpump_t *avp		:  playpump player info struct	
  			2 . struct osd_ctrl_info_t *ctrlData 	:  commond message info 
  Output:						
  Return:		1 . 0   : succeed
  			2 . -1 : tcp transport error
  other : 		
*************************************************/
int NEXUSAPP_Ctrl_Action( struct av_playpump_t *avp,  struct osd_ctrl_info_t *ctrlData)
{
	av_player *player =NULL;
	NEXUS_PlaypumpHandle playpump;
	xxpiz_info *xxpizInfo;
	NEXUS_Rect position;
	NEXUS_Rect position_t;
	int iRet = 0 , iState = 0 , i;
	
	if(avp == NULL ||avp->socketCtrlFd == -1||ctrlData->playerId > 2 )
	{
		iRet = -1;
		goto action_out;
	}

		
	if(ctrlData->playerId == PLAYER_PLAYPUMP_HD)
	{
		if(!avp->hdPlayer.playpump)
			goto action_out;
		
		player = &avp->hdPlayer;
		playpump = avp->hdPlayer.playpump;
	}
	else
	{
		if(!avp->sdPlayer.playpump)
			goto action_out;
		
		player = &avp->sdPlayer;
		playpump = avp->sdPlayer.playpump;
	}		

	DBG_APP(("NEXUSAPP_Ctrl_Action : cmd id = %d\n",ctrlData->cmdId));
	
	switch(ctrlData->cmdId)
	{
		case PLAYER_CTRL_PLAY:
			/* add by hh , 2012_09_07*/
			BKNI_SetEvent(player->Event);
			
			pthread_mutex_lock(&avp->playerLock);
			if(player->decoderStart&&player->playpump)
			{			
				NEXUS_Playpump_SetPause(playpump, false);
				
				iRet = NEXUSAPP_Playpump_Static_Set(avp,ctrlData->playerId,PLAYER_CTRL_PLAY);
				
				/* add by hh , 2012_07_07*/
				if(avp->audioSilent == AUDIO_UNMUTE && player->audioSelected)
					NEXUSAPP_Ctrl_VoiceVolume_Silent(avp,AUDIO_UNMUTE);
			}
			pthread_mutex_unlock(&avp->playerLock);
			break;
		case PLAYER_CTRL_PAUSE:
			/* add by hh , 2012_09_07*/
			BKNI_SetEvent(player->Event);
			/*iRet = NEXUSAPP_Playpump_Static_Set(avp,ctrlData->playerId,PLAYER_CTRL_PAUSE);*/

			/*pthread_mutex_lock(&avp->playerLock);
			if(player->decoderStart&&player->playpump)*/
			{			
				NEXUS_Playpump_SetPause(playpump, true);
				iRet = NEXUSAPP_Playpump_Static_Set(avp,ctrlData->playerId,PLAYER_CTRL_PAUSE);
				/* add by hh , 2012_07_07*/
				NEXUSAPP_Ctrl_VoiceVolume_Silent(avp,AUDIO_MUTE);
			}
			/*pthread_mutex_unlock(&avp->playerLock);*/

			break;
		case PLAYER_CTRL_STOP:
			/* add by hh , 2012_09_07*/
			BKNI_SetEvent(player->Event);
			
			/* add by hh , 2012_07_09*/
			pthread_mutex_lock(&avp->playerLock);
			if(avp->audioSilent == AUDIO_UNMUTE && player->audioSelected)
				NEXUSAPP_Ctrl_VoiceVolume_Silent(avp,AUDIO_MUTE);
			pthread_mutex_unlock(&avp->playerLock);
			
			NEXUSAPP_Playpump_Static_Get(player,&iState);
			if(iState == PLAYER_CTRL_PLAY_READY)
				break;
			
			NEXUSAPP_Playpump_Static_Set(avp,ctrlData->playerId,PLAYER_CTRL_STOP);
			
/* ADD BY HH FOR STOP TO READY STATE ON 20120412*/
			for(i=0;i<500;i++ )
			{
				NEXUSAPP_Playpump_Static_Get(player,&iState);
				if(iState == PLAYER_CTRL_PLAY_READY)
					break;
				
				usleep(10000);
			}
			
			if(iState != PLAYER_CTRL_PLAY_READY)
				iRet = -1;
/* END ADD */			
			break;
		case PLAYER_CTRL_PLAYER_SILENT:
			/* add audio silent state save for spdif , add by hh , 2012_07_07*/
			avp->audioSilent = ctrlData->value;
			
			/* add by hh , 2012_07_07*/
			NEXUSAPP_Playpump_Static_Get(player,&iState);
			
			if(iState == PLAYER_CTRL_PAUSE && ctrlData->value == AUDIO_UNMUTE)
				break;
			/*end */
			
			pthread_mutex_lock(&avp->playerLock);
			iRet = NEXUSAPP_Ctrl_VoiceVolume_Silent(avp,ctrlData->value);
			pthread_mutex_unlock(&avp->playerLock);
			break;			
		case PLAYER_CTRL_VOICE_VOLUME_SET:
			/* add audio volume state save for fade in audio , add by hh , 2012_07_07*/
			

			if(ctrlData->value>100)
				ctrlData->value = 100;
			else if(ctrlData->value<0)
				ctrlData->value = 0;

			avp->audioVolume = ctrlData->value;

			/* audio volume fade in ing !!!*/
			if(avp->audioChangeState)
				break;

			iRet = NEXUSAPP_Ctrl_VoiceVolume_Set(avp,avp->audioVolume);
			break;
		case PLAYER_CTRL_AUDIO_CHANNEL_SET:
			iRet = NEXUSAPP_Ctrl_Audio_Mode_Set(avp,player,ctrlData->value);
			break;
		case PLAYER_CTRL_AUDIO_MUL_CHANNEL_SET:
			/*dvb used this change channel*/
			;
			break;
		case PLAYER_CTRL_AUDIO_SOURCE_SET:
			iRet = NEXUSAPP_Ctrl_AudioSource_Set(avp,ctrlData->value);
			break;
		case PLAYER_CTRL_DISPLAY_MODE:
			iRet = NEXUSAPP_Ctrl_DisplayMode_Set(avp,ctrlData->value);
			break;
		case PLAYER_CTRL_PIP_WINDOW_VISIBLE:
			iRet = NEXUSAPP_Ctrl_Pip_Visible(avp,(bool *)ctrlData->value);
			break;
		case PLAYER_CTRL_PIP_WINDOW_INFO:
			iRet = NEXUSAPP_Ctrl_Pip_Visible(avp,(bool *)ctrlData->value);
			break;					
		case PLAYER_CTRL_OSD_PIZ:
			pthread_mutex_lock(&avp->osd.piz.pizLock);
			
			if(ctrlData->fbId == FRAMEBUFF_HD)
				xxpizInfo = &avp->osd.piz.pizHd;
			else
				xxpizInfo = &avp->osd.piz.pizSd;
						
			if(xxpizInfo->pizBuf)
				iRet = NEXUSAPP_Osd_PizBuffer_Free(xxpizInfo);

			xxpizInfo->pizSize = ctrlData->value;

			DBG_OSD(("piz size = %d , piz fb id = %d \n",xxpizInfo->pizSize,ctrlData->fbId ));
			memcpy(&xxpizInfo->newPosition,&ctrlData->position,sizeof(NEXUS_Rect));
			pthread_mutex_unlock(&avp->osd.piz.pizLock);
			
			break;
		case PLAYER_CTRL_OSD_TEXT_ROLL:
			pthread_mutex_lock(&avp->osd.textRoll.RollLock);
			 if(avp->osd.textRoll.pic.ps)
				iRet = NEXUSAPP_Osd_RollText_Free(avp);		

			 avp->osd.textRoll.picSize = ctrlData->value;
			 memcpy(&avp->osd.textRoll.position,&ctrlData->position,sizeof(NEXUS_Rect));
			 pthread_mutex_unlock(&avp->osd.textRoll.RollLock);
			break;
		case PLAYER_CTRL_OSD_TEXT_STATIC:		
			pthread_mutex_lock(&avp->osd.textStatic.staticLock);
			if(ctrlData->value == 0)
			{
				memcpy(&position,&ctrlData->position,sizeof(NEXUS_Rect));
				DBG_OSD(("static clean rect ,fbid : %d , %d:%d ,%d * %d , \n",ctrlData->fbId,position.x,position.y,position.width,position.height));
				
				if(ctrlData->fbId == FRAMEBUFF_HD)
				{
					NEXUSAPP_Osd_Framebuff_Fill(avp,&position,0,0);
					NEXUSAPP_Osd_State_Sync(avp,0);
				}
				else
				{
					NEXUSAPP_Osd_Framebuff_Fill(avp,&position,0,3);
				}
				
				iRet = 0;
				pthread_mutex_unlock(&avp->osd.textStatic.staticLock);
				
				break;
			}
			
			 if(avp->osd.textStatic.pic.ps)
				iRet =NEXUSAPP_Osd_StaticText_Free(avp);

			avp->osd.textStatic.picSize = ctrlData->value;
			avp->osd.textStatic.fbId = ctrlData->fbId;
			memcpy(&avp->osd.textStatic.position,&ctrlData->position,sizeof(NEXUS_Rect));
			pthread_mutex_unlock(&avp->osd.textStatic.staticLock);
			break;
		case PLAYER_CTRL_OSD_PIC_FLASH:
			pthread_mutex_lock(&avp->osd.textStatic.staticLock);
			 if(avp->osd.textStatic.pic.ps)
				NEXUSAPP_Osd_StaticText_Free(avp);

			avp->osd.textStatic.picSize = ctrlData->value;
			avp->osd.textStatic.fbId = ctrlData->fbId;
			memcpy(&avp->osd.textStatic.position,&ctrlData->position,sizeof(NEXUS_Rect));
			pthread_mutex_unlock(&avp->osd.textStatic.staticLock);
			break;
		/*add mw ctrl cmd by hh on 2011_01_19*/		
		case PLAYER_CTRL_MWALL_PICSET:
			pthread_mutex_lock(&avp->osd.textStatic.staticLock);

			if(avp->osd.textStatic.pic.ps)
				iRet =NEXUSAPP_Osd_StaticText_Free(avp);

			avp->osd.textStatic.picSize = ctrlData->value;
			avp->osd.textStatic.fbId = ctrlData->fbId;
			memcpy(&avp->osd.textStatic.position,&ctrlData->position,sizeof(NEXUS_Rect));
			
			pthread_mutex_unlock(&avp->osd.textStatic.staticLock);
			break;
		case PLAYER_CTRL_MWALL_DFSET:
			position.x = ctrlData->position.x;
			position.y = ctrlData->position.y;
			position.width = ctrlData->position.width;
			position.height = ctrlData->position.height;
			NEXUSAPP_Mwall_Default_Rect(avp,&position);
			break;
		case PLAYER_CTRL_MWALL_DFDISPLAY:
			DBG_OSD(("PLAYER_CTRL_MWALL_DFDISPLAY cmd recv!\n"));
			NEXUSAPP_Mwall_Display(avp);
			break;
		case PLAYER_CTRL_MWALL_PBLIT:
			position.x = ctrlData->position.x;
			position.y = ctrlData->position.y;
			position.width = ctrlData->position.width;
			position.height = ctrlData->position.height;

			position_t.x = ctrlData->position_t.x;
			position_t.y = ctrlData->position_t.y;
			position_t.width = ctrlData->position_t.width;
			position_t.height = ctrlData->position_t.height;
			NEXUSAPP_Mwall_Blit_Rect(avp,&position,&position_t);
			break;
		case PLAYER_CTRL_MWALL_RETURN:
			NEXUSAPP_Mwall_Blit_Return(avp);
			break;
		case	PLAYER_CTRL_MWALL_ClEAN:
			NEXUSAPP_Mwall_Clean(avp);
			break;
		default:
			break;
	}

	DBG_APP(("NEXUSAPP_Ctrl_Action : end id = %d , result = %d\n",ctrlData->cmdId,iRet));

action_out:

	return NEXUSAPP_Ctrl_Respond_Send(avp->socketCtrlFd,CTRL_OSD_MESSAGE_ERROR,iRet);
	
}

/*************************************************
  Function:    	NEXUSAPP_Playpump_Static_Set
  Description:	HD/SD playpump state set
  Input:		1 . struct av_playpump_t *avp	:  playpump player info struct	
  			2 . int playpumpId				:  playpump id  (PLAYER_PLAYPUMP_HD or PLAYER_PLAYPUMP_SD)
			3 . int state 					:  
						#define PLAYER_CTRL_PLAY 						0
						#define PLAYER_CTRL_PAUSE 					1
						#define PLAYER_CTRL_STOP 						2	
						#define PLAYER_CTRL_VOICE_UP	 				3
						#define PLAYER_CTRL_VOICE_DOWN 				4
						#define PLAYER_CTRL_WINDOW_TRANGE_STREAM	5

  Output:						
  Return:		1 . 0   : succeed
  other : 		
*************************************************/
int NEXUSAPP_Playpump_Static_Set(  struct av_playpump_t *avp, int playpumpId,int state)
{
	
	if(playpumpId==PLAYER_PLAYPUMP_HD)
	{
		pthread_mutex_lock(&avp->hdPlayer.lock);
		avp->hdPlayer.state = state;
		pthread_mutex_unlock(&avp->hdPlayer.lock);
	}
	else
	{
		pthread_mutex_lock(&avp->sdPlayer.lock);
		avp->sdPlayer.state = state;
		pthread_mutex_unlock(&avp->sdPlayer.lock);
	}

	return 0;
}	

/*************************************************
  Function:    	NEXUSAPP_Playpump_Static_Get
  Description:	HD/SD playpump state get
  Input:		1 . struct av_player_t *player	:  playpump player info struct	
  Output:		1 . int *state 					:			
						#define PLAYER_CTRL_PLAY 						0
						#define PLAYER_CTRL_PAUSE 					1
						#define PLAYER_CTRL_STOP 						2	
						#define PLAYER_CTRL_VOICE_UP	 				3
						#define PLAYER_CTRL_VOICE_DOWN 				4
						#define PLAYER_CTRL_WINDOW_TRANGE_STREAM	5


  Return:		1 . 0   : succeed
  other : 		
*************************************************/
int NEXUSAPP_Playpump_Static_Get(struct av_player_t *player,int *state)
{	
	PARA_ASSERT_VALUE(player,2);

	pthread_mutex_lock(&player->lock);
	*state = player->state;
	pthread_mutex_unlock(&player->lock);

	return 0;
}

/*************************************************
  Function:    	NEXUSAPP_Playpump_State_Ctrl
  Description:	HD/SD playpump state control 
  Input:		1 . struct av_player_t *player	:  playpump player info struct	
  Output:		
  Return:		1 . 0   : normal
  			2 . 1   : pause or playpump buffer 's depth  too large
  			3 . 2   : stop 
  			
  other : 		
*************************************************/
int NEXUSAPP_Playpump_State_Ctrl(av_playpump *avp,av_player *player)
{

	int state = 0;

	NEXUS_VideoDecoderStatus vStatus;
	NEXUS_AudioDecoderStatus aStatus;

	NEXUSAPP_Playpump_Static_Get(player,&state);
	NEXUS_VideoDecoder_GetStatus(player->videoDecoder, &vStatus);
	
	NEXUS_AudioDecoder_GetStatus(avp->audioDecoder, &aStatus);
	
	switch(state)
	{
		case PLAYER_CTRL_PAUSE:
			return 1;
		case PLAYER_CTRL_PLAY_END:
			DBG_APP(("NEXUSAPP_Playpump_State_Ctrl: play end![%s]\n", player->playerId?"SD":"HD"));
			while( vStatus.queueDepth)
			{
				/* recv stop now cmd add by hh , 2012_05_16*/
				NEXUSAPP_Playpump_Static_Get(player,&state);
				if(state == PLAYER_CTRL_STOP)
					break;

				NEXUS_VideoDecoder_GetStatus(player->videoDecoder, &vStatus);
				usleep(100000);
			}
		case PLAYER_CTRL_STOP:
			DBG_APP(("NEXUSAPP_Playpump_State_Ctrl: play stop![%s]\n", player->playerId?"SD":"HD"));
			if(player->playerId == PLAYER_PLAYPUMP_SD)
			{
				//NEXUSAPP_Surface_From_Video(avp,player->playerId);
				DBG_APP(("NEXUSAPP_Playpump_State_Ctrl: print scream to surface!\n"));
			}
			return 2;	
		default:
			break;
	}


	if(vStatus.queueDepth > player->depthCtrl)
	{
		/* add by hh , 2012_09_07 , add pts check !*/
		if(player->playerId == PLAYER_PLAYPUMP_HD && aStatus.queuedFrames < 10 && aStatus.pts > 0)
			return 0;
		else
			return 1;

		return 1;
	}
	else
		return 0;

	return 0;
	
}

/*************************************************
  Function:    	NEXUSAPP_Playpump_Depth_Ctrl_Get
  Description:	HD/SD playpump buffer depth control 
  Input:		1 . struct av_player_t *player	:  playpump player info struct	
  			2 . av_stream_info_t *streamInfo  :  HD/SD stream format info
  Output:		
  Return:		
  other : 		
*************************************************/
void NEXUSAPP_Playpump_Depth_Ctrl_Get(struct av_player_t *player  , av_stream_info *streamInfo)
{
	/*default depth = fifosize[99%]  */
		
	DBG_APP(("[%s-%d]input stream bitrate : %d \n",__FUNCTION__, __LINE__, streamInfo->bitRate));

	if(streamInfo->videoCodec == NEXUS_VideoCodec_eH264)
	{
		if(streamInfo->ifHd == 1)
		{
			player->depthCtrl = 5;
			player->uiloopCount = H264_FIFO_DEPTH_HD;
		}
		else
		{
			player->depthCtrl = 2;
			player->uiloopCount = H264_FIFO_DEPTH_SD ;
		}
	}	
	else
	{
		if(streamInfo->ifHd == 1)
		{
			player->depthCtrl = 5;
			player->uiloopCount = 655360;
		}
		else
		{
			player->depthCtrl  = 1;
			player->uiloopCount = 25600;
		}
	}


	MSG_APP(("[%s-%d]input stream bitrate : %d , change value : %d , set count : %d,depth:%d\n",__FUNCTION__, __LINE__, streamInfo->bitRate,player->uiloopCount,player->uiloopCount,player->depthCtrl));

	return;
}

/*************************************************
  Function:    	NEXUSAPP_Playpump_StreamInfo_Get
  Description:	HD/SD stream 's format get ,  it will be loop receive data until get the correct streaminfo struct
  Input:		1 . struct av_player_t *player	:  playpump player info struct	
  			2 . av_stream_info_t *streamInfo  :  HD/SD stream format info
  Output:		
  Return:		
  other : 		
*************************************************/
int NEXUSAPP_Playpump_StreamInfo_Get( struct av_player_t *player , av_stream_info *streamInfo)
{
	unsigned long ulRecvlen =sizeof(av_stream_info);
	
	while(1)
	{
		usleep(100000);
		memset(streamInfo,0x00,sizeof(av_stream_info));

		if(socket_tcp_server_read_timeout(player->socketFd,(char *)streamInfo,&ulRecvlen,3)<0)
		{
			DBG_APP(("tcp error ,get stream info error!\n"));
			return -1;
		}

		if(streamInfo->iFlag!=1)
			continue;	
		
		break;
	}

	return 0 ;

}


/*************************************************
  Function:    	NEXUSAPP_Playpump_Ctrl
  Description:	control center thread main function , loop  receive control commond to action 
  Input:		1 . struct av_player_t *player	:  playpump player info struct	
  Output:		1 . socket_t *socket_info		:  socket info 
  Return:		
  other : 		
*************************************************/
void NEXUSAPP_Playpump_Ctrl( struct av_playpump_t *avp)
{

	struct osd_ctrl_info_t ctrl;
	struct osd_addContext_info_t ctrl_addContex;
	NEXUS_VideoDecoderStatus status;
	NEXUS_AudioDecoderStatus aStatus;
	
	uint32_t stc;
	uint32_t iCmdFlag =0 ;

	DBG_APP(("NEXUSAPP_Playpump_Ctrl ,socket : %d!\n",avp->socketCtrlFd));
	
	avp->ctrlPthreadStart = true;

	while(1)
	{
		usleep(1000);

		if(NEXUSAPP_Ctrl_CmdFlag_Get(avp->socketCtrlFd,&iCmdFlag))
			break;

		if(iCmdFlag == CTRL_FLAG_ID)
		{
			if(NEXUSAPP_Ctrl_CmdInfo_Get(avp->socketCtrlFd,&ctrl))
				break;

			pthread_mutex_lock(&avp->ctrlLock);
			if(NEXUSAPP_Ctrl_Action(avp,&ctrl))
			{
				pthread_mutex_unlock(&avp->ctrlLock);
				break;
			}
			pthread_mutex_unlock(&avp->ctrlLock);

		}
		else
		{
			if(NEXUSAPP_Ctrl_AddContextInfo_Get(avp->socketCtrlFd,&ctrl_addContex))
				break;
			
			pthread_mutex_lock(&avp->ctrlLock);
			if(NEXUSAPP_Ctrl_AddContextInfo_Action(avp,&ctrl_addContex))
			{
				pthread_mutex_unlock(&avp->ctrlLock);
				break;
			}
			pthread_mutex_unlock(&avp->ctrlLock);
		}

		if(!avp->hdPlayer.decoderStart)
			continue;

		NEXUS_AudioDecoder_GetStatus(avp->audioDecoder, &aStatus);
		NEXUS_VideoDecoder_GetStatus(avp->hdPlayer.videoDecoder, &status);
		/*NEXUS_StcChannel_GetStc(avp->hdPlayer.videoProgram.stcChannel, &stc);*/
		/*MSG_APP(("\ndecode video[hd]: %dx%d, pts %#x, stc %#x (diff %d), [fifo=%d%%],[qd:%d]\n",	
		status.source.width, status.source.height, status.pts, stc, status.pts - stc,status.fifoSize?(status.fifoDepth*100)/status.fifoSize:0,status.queueDepth));*/

		MSG_APP(("\ndecode audio[hd]: pts %#x , [fifo=%d],[qd:%d]\n",
		aStatus.pts,aStatus.fifoDepth,aStatus.queuedFrames));

		
		if(!avp->sdPlayer.decoderStart)
			continue;

		NEXUS_VideoDecoder_GetStatus(avp->sdPlayer.videoDecoder, &status);
		/*NEXUS_StcChannel_GetStc(avp->sdPlayer.videoProgram.stcChannel, &stc);*/
		/*MSG_APP(("decode[sd]: %dx%d, pts %#x, stc %#x (diff %d),[fifo=%d%%],[qd:%d]\n\n",
		status.source.width, status.source.height, status.pts, stc, status.pts - stc,status.fifoSize?(status.fifoDepth*100)/status.fifoSize:0,status.queueDepth));*/

	}

	DBG_APP(("ctrl tcp stop!\n"));
	NEXUSAPP_Ctrl_Stop(avp);
	avp->ctrlPthreadStart  = false;

	return ;
	

}

void *NEXUSAPP_Ctrl_Pthread(void *context)
{
	struct av_playpump_t *avp = (struct av_playpump_t *)context;

	DBG_APP(("NEXUSAPP_Ctrl_Pthread : socket = %d\n",avp->socketCtrlFd));
	
	if(avp)
		NEXUSAPP_Playpump_Ctrl(avp);

	DBG_APP(("NEXUSAPP_Ctrl_Pthread : finish!\n"));
		
	return NULL;
}

void NEXUSAPP_Ctrl_Pthread_Up(struct av_playpump_t *avp)
{
	pthread_t threadCtrl;
	pthread_attr_t attr;

	DBG_APP(("NEXUSAPP_Ctrl_Pthread_Up : socket = %d\n",avp->socketCtrlFd));

	pthread_attr_init (&attr);
	pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&threadCtrl,&attr, NEXUSAPP_Ctrl_Pthread,avp);
	pthread_attr_destroy (&attr);

	return ;
}


/*************************************************
  Function:    	NEXUSAPP_Ctrl_Check
  Description:	ctrl thread stop , free all malloc buffer and close socket
  Input:		1 . int iSockfd	:  socket line check	
  Output:						
  Return:		0  : check succ
  			-1: check error
  other : 		
*************************************************/
int  NEXUSAPP_Ctrl_Check(int iSockfd)
{
	char szBuf[128];
	unsigned long ulRecv = 0;
	
	if(socket_tcp_server_send(iSockfd,LINE_CHECK,strlen(LINE_CHECK)))
		return -1;

	memset(szBuf,0x00,sizeof(szBuf));
	ulRecv = strlen(LINE_CHECK_OK);
	if(socket_tcp_server_read_timeout(iSockfd,szBuf,&ulRecv,3))
		return -1;

	if(strstr(szBuf,LINE_CHECK_OK)==NULL)
		return -1;

	return 0;
}

/*************************************************
  Function:    	NEXUSAPP_Ctrl_Init
  Description:	control center socket init and start
  Input:		1 . socket_t *socket_info		:  socket info 	
  Output:		
  Return:		
  other : 		
*************************************************/
void NEXUSAPP_Ctrl_Init(socket_t *socket_info)
{
	memset(socket_info,0,sizeof(socket_t));

	while(1)
	{
		if(!socket_tcp_server_init(socket_info,HD_SD_PLAYER_CTCL_PORT))
			break;

		sleep(1);
	}
	
	return ;
}


/*************************************************
  Function:    	NEXUSAPP_Ctrl_Stop
  Description:	ctrl thread stop , free all malloc buffer and close socket
  Input:		1 . struct av_playpump_t *avp	:  playpump player info struct		
  Output:						
  Return:		
  other : 		
*************************************************/
void NEXUSAPP_Ctrl_Stop(struct av_playpump_t *avp)
{
	NEXUSAPP_Osd_StaticText_Free(avp);
	NEXUSAPP_Osd_RollText_Free(avp);
	NEXUSAPP_Osd_PizBuffer_Free(&avp->osd.piz.pizHd);
	NEXUSAPP_Osd_PizBuffer_Free(&avp->osd.piz.pizSd);
	socket_tcp_server_close(&avp->socketCtrlFd);
}

/*************************************************
  Function:    	NEXUSAPP_Ctrl_Start_Pthread
  Description:	accept new client for ctrl center .
  Input:		1 . struct av_playpump_t *avp	:  playpump player info struct	
  			2 . socket_t *socket_info 		:  ctrl center socket info 
  Output:						
  Return:		
  other : 		
*************************************************/
void *NEXUSAPP_Ctrl_Start_Pthread(void *context)
{
	av_playpump *avp;
	socket_t *socket_info;
	ctrl_start_info *info = (ctrl_start_info *)context;

	PARA_ASSERT_VALUE(info,NULL);
	
	avp = info->avp;
	socket_info = info->socket_info;
	
	PARA_ASSERT_VALUE(avp,NULL);
	PARA_ASSERT_VALUE(socket_info,NULL);

	if(avp->socketCtrlFd!=-1)
		socket_tcp_server_close(&avp->socketCtrlFd);
	
	info->start = true ;
	
	while(1)
	{		
		if((avp->socketCtrlFd = socket_tcp_server_open(socket_info)) <= 0)
			goto close_sock;
		
		if(NEXUSAPP_Ctrl_Check(avp->socketCtrlFd))
			goto close_sock;

		if(avp->ctrlPthreadStart)
			goto close_sock;
		
		NEXUSAPP_Ctrl_Pthread_Up(avp);

		continue;
		
close_sock:
		
		socket_tcp_server_close(&avp->socketCtrlFd);
		usleep(100000);
		
	}

	return NULL;
}

/*************************************************
  Function:    	NEXUSAPP_Ctrl_Start
  Description:	accept new client for ctrl center .
  Input:		1 . struct av_playpump_t *avp	:  playpump player info struct	
  			2 . socket_t *socket_info 		:  ctrl center socket info 
  Output:						
  Return:		
  other : 		
*************************************************/
void NEXUSAPP_Ctrl_Start(ctrl_start_info *info)
{
	pthread_t threadCtrl;
	pthread_attr_t attr;
	
	PARA_ASSERT(info);
	
	pthread_attr_init (&attr);
	pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&threadCtrl,&attr, NEXUSAPP_Ctrl_Start_Pthread, info);
	pthread_attr_destroy (&attr);
	
	return;
}

#endif

