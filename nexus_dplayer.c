#ifndef NEXUS_DPLAYER_C
#define NEXUS_DPLAYER_C
/************************************************************************************/
#include "nexus_player.h"
#include "nexus_dplayer.h"
#include "nexus_link.h"
#include "nexus_ctrl.h"
#include "nexusio_file_io.h"
#include "nexusio_mftp_io.h"
/************************************************************************************/
typedef struct dplay_list_data_t{
	NEXUSIOProtocol *myIo;
	nexusIoContext context;
	char name[128];
}dplay_list_data;
/************************************************************************************/
typedef struct dplay_myNode_t
{
	dplay_list_data data;   
	struct dplay_myNode_t *next;
} dplayNode, *dplayLinkList;
/************************************************************************************/
typedef struct dplay_state_t
{
	int state;
	av_playpump  *avp;
	char playsong[128];
	char currentsong[128];
	DplayerSongType playType;
	DplayerSongType currentType;
	BKNI_EventHandle Event;
	pthread_mutex_t DplayLock;
} DplayState;
/************************************************************************************/
dplayLinkList Head_DplayList = NULL;
static DplayState Dplayer;
/************************************************************************************/
void NEXUSAPP_DPlayer_Link_Init(dplayLinkList *Head_pointer)
{
	*Head_pointer = NULL; 
}
/************************************************************************************/
void NEXUSAPP_DPlayer_Link_Clean(dplayLinkList *Head_pointer)
{
	dplayLinkList p, q;   

	p = *Head_pointer;   

	while (p != NULL)   
	{   
		q = p;   
		p = p->next;   
		free(q);
	}
}
/************************************************************************************/
int NEXUSAPP_DPlayer_LINK_Insert(dplayLinkList *Head_pointer, dplay_list_data *x)
{
	dplayNode *p; 
	
	p = (dplayNode *) malloc(sizeof(dplayNode));   
	if (p == NULL)   
		goto error_out;
	
	memcpy(&p->data,x,sizeof(dplay_list_data));
	   
	p->next = *Head_pointer;   
	*Head_pointer = p;   
	   
	return OK; 

error_out:
	 return OverFlow;
}
/************************************************************************************/
int NEXUSAPP_DPlayer_LINK_Delete(dplayLinkList *Head_pointer, char *name)
{   
	dplayNode *p, *q =NULL;   

	p = *Head_pointer;   
	if (!strcmp(p->data.name,name))
	{   
		*Head_pointer = (*Head_pointer)->next;   
		free(p);   
		goto succ_out;
	}   
	else  
	{   
		q = p; p = p->next;  
		while(p != NULL)   
		{   
			if (!strcmp(p->data.name,name) ) 
			{   
				q->next = p->next;   
				free(p);   
				goto succ_out;   
			}   
			q = p; p = p->next;   
		}   
	}   

	return Error; 

succ_out:
	return OK;  
  
}
/************************************************************************************/
int  NEXUSAPP_DPlayer_Link_Length(dplayLinkList Head)
{
	dplayLinkList p = Head; 
	int sum = 0; 
	
	while(p != NULL) 
	{ 
		sum++; 
		p = p->next;	
	} 

	return sum; 
}
/************************************************************************************/
dplayLinkList NEXUSAPP_DPlayer_LINK_Find(dplayLinkList Head,char *name)
{
	dplayLinkList p;   
	p = Head;   
	while(p != NULL)   
	{   		
		if(!strcmp(p->data.name,name))
			break;
		p = p->next;   
	}   
	return p;   
}
/*************************************************
Function:    	NEXUSAPP_DPlayer_CSong_Set
Description:	NEXUSAPP_DPlayer_CSong_Set
Input:		
Output:		
Return:                        
*************************************************/
void NEXUSAPP_DPlayer_CSong_Set(void)
{

	pthread_mutex_lock(&Dplayer.DplayLock);

	Dplayer.currentType = Dplayer.playType;
	Dplayer.playType = DPLAYER_Songtype_eMax;
	
	memset(Dplayer.currentsong, 0x00 ,sizeof(Dplayer.currentsong));
	memcpy(Dplayer.currentsong,Dplayer.playsong, sizeof(Dplayer.playsong));
	memset(Dplayer.playsong , 0x00 ,sizeof(Dplayer.playsong));

	pthread_mutex_unlock(&Dplayer.DplayLock);
}

/*************************************************
Function:    	NEXUSAPP_DPlayer_PSong_Set
Description:	DPlayer State Set
Input:		1 . char *fname	:  play song name
			2 .  int iFLen		:  play song name path length
			3 . DplayerSongType songType : play song type
Output:		
Return:                        
*************************************************/
void NEXUSAPP_DPlayer_PSong_Set(char *fname , int iFLen , DplayerSongType songType)
{

	int iCopyLen = 0;

	iCopyLen = sizeof(Dplayer.playsong);

	if(iFLen < iCopyLen)
		iCopyLen = iFLen ;

	pthread_mutex_lock(&Dplayer.DplayLock);

	memset(Dplayer.playsong , 0x00 ,sizeof(Dplayer.playsong));
	
	if(fname)
		memcpy(Dplayer.playsong , fname , iCopyLen);

	Dplayer.playType = songType;

	pthread_mutex_unlock(&Dplayer.DplayLock);
}

/*************************************************
  Function:    	NEXUSAPP_DPlayer_Callback
  Description:	Set DPlayer event callback , it will be use by playpump start 
  Input:		1 . void *context	: callback context.
  			2 . int param		: parameter
  Output:
  Return:
*************************************************/
static void NEXUSAPP_DPlayer_Callback(void *context, int param)
{
	BSTD_UNUSED(param);
	BKNI_SetEvent((BKNI_EventHandle)context);
}
/*************************************************
Function:    	NEXUSAPP_DPlayer_Stop
Description:	playpump stop 
Input:		1 . struct av_playpump_t *avp	: playpump info struct
			2 . DplayerSongType songtype	: song type 
Output:		
Return:                        
*************************************************/
void NEXUSAPP_DPlayer_Stop(av_playpump  *avp )
{
	av_player *player = &avp->sdPlayer;
	
	if(player->ioContext.priv_data)
		player->myIo->url_close(&player->ioContext);

	NEXUS_Playpump_Flush(player->playpump);

	return ;
}

/*************************************************
Function:    	NEXUSAPP_DPlayer_Start
Description:	playpump start 
Input:		1 . av_playpump  *avp
Output:		
Return:           
*************************************************/
int NEXUSAPP_DPlayer_Start(av_playpump  *avp)
{
	NEXUS_VideoDecoderStatus vstatus;
	NEXUS_PlaypumpStatus pStatus;
	NEXUS_PlaypumpOpenPidChannelSettings settings;
	NEXUS_PlaypumpSettings playpumpSettings;
	NEXUS_PlaypumpOpenSettings openSettings;

	NEXUS_Playpump_GetDefaultOpenSettings(&openSettings);
	openSettings.fifoSize = 1 * 1024 *1024 ;//256000;
	openSettings.alignment = 12;
	openSettings.numDescriptors = 100;

	av_player *player = &avp->sdPlayer;

	PARA_ASSERT_VALUE(player,-1);	

	DBG_APP(("NEXUSAPP_DPlayer_Start\n"));

	
	NEXUS_VideoDecoder_GetStatus(player->videoDecoder, &vstatus);

	if(player->decoderStart == false)
	{
		DBG_APP(("NEXUSAPP_DPlayer_Start : init playpump\n"));

		if(player->playpump == NULL)
			player->playpump = NEXUS_Playpump_Open(1, &openSettings);

		
		NEXUS_Playpump_GetSettings(player->playpump, &playpumpSettings);
		playpumpSettings.transportType = player->transportType;
		playpumpSettings.dataCallback.callback = NEXUSAPP_DPlayer_Callback;
		playpumpSettings.dataCallback.context = player->Event;
		NEXUS_Playpump_SetSettings(player->playpump, &playpumpSettings);
		NEXUS_Playpump_Start(player->playpump);

		DBG_APP(("NEXUSAPP_DPlayer_Start :NEXUS_Playpump_Start\n"));
		
		NEXUS_Playpump_GetDefaultOpenPidChannelSettings(&settings);
		settings.pidType = NEXUS_PidType_eVideo;
		player->videoPidChannel.pidChannel = NEXUS_Playpump_OpenPidChannel(player->playpump,player->videoPidChannel.pid, &settings);

		player->videoProgram.pidChannel = player->videoPidChannel.pidChannel;
		player->videoProgram.stcChannel = player->stcChannel;

		NEXUS_VideoWindow_RemoveAllInputs(avp->display.winSd_0);
		NEXUSAPP_Windows_Connect(avp->display.winSd_0, player->videoDecoder);
		
		NEXUS_VideoDecoder_Start(player->videoDecoder, &player->videoProgram);
		
		DBG_APP(("NEXUSAPP_DPlayer_Start :NEXUS_VideoDecoder_Start\n"));

		//NEXUSAPP_Ctrl_Windows_StateSet(avp,player->win_0,true);

		DBG_APP(("NEXUSAPP_DPlayer_Start :NEXUSAPP_Ctrl_Windows_StateSet\n"));

		player->decoderStart = true;
	}
	else
	{
		if (vstatus.started)
		{
			DBG_APP(("NEXUSAPP_DPlayer_Start : Flush \n"));
			NEXUS_VideoDecoder_Flush(player->videoDecoder);
			NEXUS_Playpump_Flush(player->playpump);
			
			NEXUS_VideoDecoder_GetStatus(player->videoDecoder, &vstatus);
			NEXUS_Playpump_GetStatus(player->playpump,&pStatus);
			DBG_APP(("NEXUSAPP_DPlayer_Play : playpump : %d , fifo = %d  \n",pStatus.started,pStatus.fifoDepth));
			DBG_APP(("NEXUSAPP_DPlayer_Play : videodecoder : %d , fifo = %d  \n",vstatus.started,vstatus.fifoDepth));
			
		}
		else
		{			
			DBG_APP(("NEXUSAPP_DPlayer_Start : Decoder Start \n"));

			NEXUS_Playpump_GetDefaultOpenPidChannelSettings(&settings);
			settings.pidType = NEXUS_PidType_eVideo;
			player->videoPidChannel.pidChannel = NEXUS_Playpump_OpenPidChannel(player->playpump,player->videoPidChannel.pid, &settings);

			player->videoProgram.pidChannel = player->videoPidChannel.pidChannel;
			player->videoProgram.stcChannel = player->stcChannel;
			
			NEXUS_VideoWindow_RemoveAllInputs(avp->display.winSd_0);
			NEXUSAPP_Windows_Connect(avp->display.winSd_0, player->videoDecoder);

			NEXUS_VideoDecoder_Start(player->videoDecoder, &player->videoProgram);
		}
	}

	DBG_APP(("NEXUSAPP_DPlayer_Start :set state play\n"));
	
	NEXUSAPP_Playpump_Static_Set(Dplayer.avp,PLAYER_PLAYPUMP_SD,PLAYER_CTRL_PLAY);

	return 0;
}

/*************************************************
Function:    	NEXUSAPP_DPlayer_Play
Description:	audioplayer play 
Input:		1 . av_playpump  *avp
Output:		
Return:           	nexus error
*************************************************/
int NEXUSAPP_DPlayer_Play( av_playpump  *avp)
{
	char *buffer;
	int iRet =0;
	int state = -1;
	unsigned int buffer_size;
	NEXUS_Error rc = 0;
	NEXUS_VideoDecoderStatus vstatus;
	NEXUS_PlaypumpStatus pStatus;
	int alpha = 0;
	
	av_player *player = &avp->sdPlayer;
	
	if(!player->decoderStart)
		return 0;
	
	NEXUS_VideoDecoder_GetStatus(player->videoDecoder, &vstatus);
	NEXUS_Playpump_GetStatus(player->playpump,&pStatus);
	DBG_APP(("NEXUSAPP_DPlayer_Play : playpump : %d , decoder : %d \n",pStatus.started,vstatus.started));

	NEXUS_Playpump_SetPause(player->playpump, false);
	
	while(1)
	{
		NEXUS_VideoDecoder_GetStatus(player->videoDecoder, &vstatus);
/*
		DBG_APP(("\ndecode video: %dx%d, pts %#x,  [fifo=%d%%],[qd:%d]\n",	
		vstatus.source.width, vstatus.source.height, vstatus.pts,vstatus.fifoSize?(vstatus.fifoDepth*100)/vstatus.fifoSize:0,vstatus.queueDepth));
*/
		if(vstatus.queueDepth && alpha == 0)
		{
			NEXUSAPP_Windows_Alpha(Dplayer.avp->display.winSd_0 , 255);
			NEXUSAPP_Ctrl_Windows_Visible(Dplayer.avp->display.winSd_0,true);
			alpha = 255;
		}
		
		NEXUSAPP_Playpump_Static_Get(player,&state);
		
		switch(state)
		{
			case PLAYER_CTRL_PAUSE:
				DBG_APP(("NEXUSAPP_DPlayer_Play :PLAYER_CTRL_PAUSE\n"));
				continue;
			case PLAYER_CTRL_STOP:
				NEXUS_Playpump_SetPause(player->playpump, true);
				NEXUS_VideoDecoder_Flush(player->videoDecoder);
				NEXUS_Playpump_Flush(player->playpump);
				NEXUS_VideoDecoder_Stop(player->videoDecoder);
				DBG_APP(("recv stop cmd , return !\n"));
				return 0;
				break;
			default :
				break;
		}
		
		if (NEXUS_Playpump_GetBuffer(player->playpump, (void **)&buffer, &buffer_size))
		{
			DBG_APP(("nexus playpump get buff error ,go to start_playpump\n"));
			return -1;
		}
		
		if (buffer_size == 0 ) {
			BKNI_WaitForEvent(player->Event, BKNI_INFINITE);
			continue;
		}

		if (buffer_size > player->uiloopCount )
			buffer_size = player->uiloopCount;

		iRet = player->myIo->url_read(&player->ioContext,(unsigned char *)buffer,buffer_size);
		
		if(iRet == 0)
		{
			player->myIo->url_seek(&player->ioContext,0,0);
			iRet = player->myIo->url_read(&player->ioContext,(unsigned char *)buffer,buffer_size);
		}
				
		if(iRet < 0)
			break;
		
		rc = NEXUS_Playpump_ReadComplete(player->playpump, 0, iRet);
		
		if(rc)
			MSG_DISPLAY(("NEXUS_Playpump_ReadComplete : rc = %d\n",rc));

	}

	return 0;
}
/*************************************************
Function:    	NEXUSAPP_DPlayer_Pause
Description:	audioplayer pause 
Input:		1 . av_playpump  *avp
Output:		
Return:           	nexus error
*************************************************
int NEXUSAPP_DPlayer_Pause(av_playpump  *avp)
{
	
	return 0;
}
*/
/*************************************************
Function:    	NEXUSAPP_DPlayer_Init
Description:	playpump stop 
Input:		1 . struct av_playpump_t *avp	:  playpump info struct
			2 .  char *fname				:  stream file all path
Output:		
Return:                        
************************************************/
int NEXUSAPP_DPlayer_Init(av_playpump *avp,  char *fname , DplayerSongType songtype)
{
	char *ptr = NULL;
	char szFileName[1024];
	dplayLinkList pNode;
	av_stream_info streamInfo;
	av_player *player = &avp->sdPlayer;
	struct util_opts_t opts;

	DBG_APP(("NEXUSAPP_DPlayer_Init name : %s , type = %d , video  =%d , pid = %d , song = %d \n",fname,player->transportType,player->videoProgram.codec ,player->videoPidChannel.pid,songtype));

	if( fname == NULL || strlen(fname) <= 0 )
		goto dplayer_init_error_out; 

	memset(szFileName,0x00,sizeof(szFileName));
	
	if( songtype == DPLAYER_Songtype_eBVSong)
	{		
		pNode = NEXUSAPP_DPlayer_LINK_Find(Head_DplayList , fname);	

		if(pNode==NULL || pNode->data.myIo == NULL || pNode->data.context.priv_data == NULL)
			goto dplayer_init_error_out;

		player->myIo = pNode->data.myIo;
		memcpy(&player->ioContext , &pNode->data.context , sizeof(nexusIoContext));
		if (player->myIo->url_open(&player->ioContext, pNode->data.name, 0))
		{
			if(player->decoderStart)
			{
				NEXUS_VideoDecoder_Flush(player->videoDecoder);
				NEXUS_Playpump_Flush(player->playpump);
			}
		}	
	}
	else if(songtype == DPLAYER_Songtype_eRSSong)
	{
		/* mftp song */
		ptr = strstr( fname , "#" );
		if(ptr == NULL)
			goto dplayer_init_error_out;

		ptr++;
		
		sprintf(szFileName,"mftp://%d//#%s" , player->playerId + 2 , ptr);
		
		player->myIo = nexusio_mftp_prtocol_get();
		
		if(player->myIo->url_open(&player->ioContext,szFileName,0))
		{
			if(player->decoderStart)
			{
				NEXUS_VideoDecoder_Flush(player->videoDecoder);
				NEXUS_Playpump_Flush(player->playpump);
				NEXUS_VideoDecoder_Stop(player->videoDecoder);
				NEXUS_Playpump_Stop(player->playpump);
				NEXUS_Playpump_CloseAllPidChannels(player->playpump);
				player->decoderStart = false;
			}
			
			goto dplayer_init_error_out;
		}
	}
	else
		goto dplayer_init_error_out;
	
	DBG_APP(("recv song name = %s \n",szFileName));
	
	NEXUSAPP_Playpump_Codec_Get(player->ioContext.filename,&streamInfo);
	
	if (player->ioContext.fullname)
	{
		if(strstr(player->ioContext.fullname, "AS") != NULL)
		{
			NEXUSAPP_Util_Opts_Init(&opts);
			opts.filename = player->ioContext.fullname;
			if (cmdline_probe(&opts.common, opts.filename, &opts.indexname))
			{
				DBG_APP(("DPlayer Init file:%s probe failed\n", opts.filename));
				goto dplayer_init_error_out;
			}

			streamInfo.transportType = opts.common.transportType;
			streamInfo.videoCodec = opts.common.videoCodec;
			streamInfo.pid_video = opts.common.videoPid;

			DBG_APP(("%s:transportType:%d, videoCodec:%d, videPid:%d\n", szFileName, streamInfo.transportType, streamInfo.videoCodec,  streamInfo.pid_video ));
		}
	}
	
	if(streamInfo.transportType != (int )player->transportType 
	||streamInfo.videoCodec != (int )player->videoProgram.codec
	||streamInfo.pid_video != player->videoPidChannel.pid)
	{
		if(player->decoderStart)
		{
			NEXUS_VideoDecoder_Flush(player->videoDecoder);
			NEXUS_Playpump_Flush(player->playpump);
			NEXUS_VideoDecoder_Stop(player->videoDecoder);
			NEXUS_Playpump_Stop(player->playpump);
			NEXUS_Playpump_CloseAllPidChannels(player->playpump);
			player->decoderStart = false;
		}
		
		streamInfo.ifHd = 1;
		
		NEXUSAPP_Playpump_Codec_Set(player , &streamInfo);
		NEXUSAPP_Playpump_Depth_Ctrl_Get(player , &streamInfo);
	}

	DBG_APP(("recv song codec  type = %d , video  =%d , pid = %d \n",streamInfo.transportType,streamInfo.videoCodec ,streamInfo.pid_video));
	DBG_APP(("now song codec  type = %d , video  =%d , pid = %d \n",player->transportType,player->videoProgram.codec ,player->videoPidChannel.pid));

	NEXUSAPP_DPlayer_CSong_Set();
	
	return 0;

dplayer_init_error_out:
	return -1;
}

/*************************************************
  Function:    	NEXUSAPP_Start_Dplayer_Pthread
  Description:	HD thread 
  			1 . void *context		:  thread  input parameters  struct 
  Output:		
  Return:           
*************************************************/
static void *NEXUSAPP_Dplayer_Pthread(void *context)
{
	av_player *player ;
	av_stream_info stream_info;
	av_playpump *avp = NULL;
	DplayState *Dplayer =  (DplayState *)context;
		
	signal(SIGNAL_EXIT, NEXUSAPP_Signal_Exit);
		
	memset(&stream_info,0x00,sizeof(stream_info));

	avp = Dplayer->avp;
	
	player = &avp->sdPlayer;

	player->playerId = PLAYER_PLAYPUMP_SD;

	DBG_APP(("\n\nDplayer:  start dplayer playpump pthread! \n"));

	NEXUSAPP_Playpump_Static_Set(avp,PLAYER_PLAYPUMP_SD , PLAYER_CTRL_STOP);

	BKNI_WaitForEvent(Dplayer->Event, 0xffffffff);

	while(!avp->uiIfHdRun)
		usleep(30000);
	
	while(1)
	{
		if(NEXUSAPP_DPlayer_Init( avp , Dplayer->playsong , Dplayer->playType))
		{
			BKNI_WaitForEvent(Dplayer->Event, 0xffffffff);
			continue;
		}
		
		NEXUSAPP_DPlayer_Start(avp);
		
		NEXUSAPP_DPlayer_Play(avp);		

		NEXUSAPP_DPlayer_Stop(avp);
	}
	
	return NULL;
		
}

/*************************************************
Function:    	NEXUSAPP_DPlayer_Ready
Description:	playpump stop 
Input:		1 . struct av_playpump_t *avp	:  playpump info struct
Output:		
Return:                        
*************************************************/
int NEXUSAPP_DPlayer_Ready(av_playpump *avp)
{
	pthread_t threadDplayer;
	pthread_attr_t attr;

	memset(&Dplayer,0x00,sizeof(DplayState));
	
	NEXUSAPP_DPlayer_Link_Clean(&Head_DplayList);
	NEXUSAPP_DPlayer_Link_Init(&Head_DplayList);
	
	pthread_mutex_init(&Dplayer.DplayLock,NULL);
	BKNI_CreateEvent(&Dplayer.Event);

	PARA_ASSERT_VALUE(avp,-1);

	Dplayer.avp = avp ;
/*
	NEXUSAPP_DPlayer_Aped_Song("/mnt/mb/01.H4X" , strlen("/mnt/mb/01.H4X"));
	NEXUSAPP_DPlayer_Aped_Song("/mnt/mb/02.H4X" , strlen("/mnt/mb/02.H4X"));
	NEXUSAPP_DPlayer_Aped_Song("/mnt/mb/03.H4X" , strlen("/mnt/mb/03.H4X"));
	NEXUSAPP_DPlayer_Aped_Song("/mnt/mb/04.H4X" , strlen("/mnt/mb/04.H4X"));
	NEXUSAPP_DPlayer_Aped_Song("/mnt/mb/05.H4X" , strlen("/mnt/mb/05.H4X"));
	NEXUSAPP_DPlayer_Aped_Song("/mnt/mb/06.H4X" , strlen("/mnt/mb/06.H4X"));
	NEXUSAPP_DPlayer_Aped_Song("/mnt/mb/07.H4X" , strlen("/mnt/mb/07.H4X"));
*/	
	pthread_attr_init (&attr);
	pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&threadDplayer,&attr, NEXUSAPP_Dplayer_Pthread,&Dplayer);
	pthread_attr_destroy (&attr);
	
	return 0;
}

/*************************************************
Function:    	NEXUSAPP_DPlayer_Clean
Description:	playpump stop 
Input:		
Output:		
Return:                        
*************************************************/
void NEXUSAPP_DPlayer_Clean(void)
{
	NEXUSAPP_DPlayer_Link_Clean(&Head_DplayList);
}


/*************************************************
Function:    	NEXUSAPP_DPlayer_Append_Song
Description:	append song to dplayer list 
Input:		1 .  char *fname	:  song file all path
			2 .  int iLen		:  song file all path length
Output:		
Return:                        
*************************************************/
int NEXUSAPP_DPlayer_Aped_Song(char *fname , int iLen)
{
	int iCount = 0;
	int iCopyLen = 0;
	dplay_list_data my_data;

	iCount = NEXUSAPP_DPlayer_Link_Length(Head_DplayList);

	DBG_APP(("NEXUSAPP_DPlayer_Append_Song: icount = %d !\n",iCount));
	
	if(iCount > DPLAYER_LIST_MAX)
		return -2;

	if(NEXUSAPP_DPlayer_LINK_Find(Head_DplayList , fname) != NULL)
		return 0;

	memset(&my_data , 0x00 , sizeof(dplay_list_data));

	my_data.myIo = nexusio_file_prtocol_get();
	
	if(my_data.myIo->url_open(&my_data.context,fname,0))
		return -1;

	iCopyLen  = sizeof(my_data.name);
	if( iLen < iCopyLen)
		iCopyLen = iLen;
		
	memcpy(my_data.name , fname , iCopyLen);
	
	return NEXUSAPP_DPlayer_LINK_Insert(&Head_DplayList , &my_data);
}

/*************************************************
Function:    	NEXUSAPP_DPlayer_PlayFile
Description:	playpump stop 
Input:		1 . struct av_playpump_t *avp	:  playpump info struct
			2 .  char *fname				:  stream file all path
Output:		
Return:                        
*************************************************/
int NEXUSAPP_DPlayer_Play_Song(char *fname ,int iFLen ,DplayerSongType songType)
{
	int iState= -1;

	PARA_ASSERT_VALUE(fname,-1);
	PARA_ASSERT_VALUE(Dplayer.avp,-1);
	
	DBG_APP(("NEXUSAPP_DPlayer_Play_Song: name = %s !\n",fname));

	if(!strncmp(Dplayer.currentsong , fname , iFLen))
	{
		NEXUSAPP_Playpump_Static_Get(&Dplayer.avp->sdPlayer , &iState);

		if(iState == PLAYER_CTRL_PLAY)
			return 0;
	}
	
	if(songType == DPLAYER_Songtype_eBVSong)
	{
		if(NEXUSAPP_DPlayer_LINK_Find(Head_DplayList , fname) == NULL)
			return -1;
	}
	
	NEXUSAPP_DPlayer_PSong_Set(fname,iFLen,songType);

	NEXUSAPP_Windows_Alpha(Dplayer.avp->display.winSd_0 , 50);
	NEXUSAPP_Ctrl_Windows_Visible(Dplayer.avp->display.winSd_0,false);
	
	NEXUSAPP_Playpump_Static_Set(Dplayer.avp,PLAYER_PLAYPUMP_SD,PLAYER_CTRL_STOP);

	BKNI_SetEvent(Dplayer.Event);
	BKNI_SetEvent(Dplayer.avp->sdPlayer.Event);

	return 0;
	
}

/*************************************************
Function:    	NEXUSAPP_DPlayer_Pthread
Description:	SD thread 
Input:		1 . void *context		:  thread  input parameters  struct 
Output:		
Return:
other : 		
*************************************************
static void *NEXUSAPP_DPlayer_Pthread(void *context)
{
	av_playpump *avp = NULL;

	PARA_ASSERT_VALUE(context,NULL);
	
	avp = (av_playpump *)context;

	PARA_ASSERT_VALUE(avp,NULL);
}

*************************************************
Function:    	NEXUSAPP_DPlayer_Init
Description:	playpump stop 
Input:		1 . struct av_playpump_t *avp	:  playpump info struct
			2 .  char *fname				:  stream file all path
Output:		
Return:                        
************************************************
int NEXUSAPP_DPlayer_Init(av_playpump *avp)
{
	pthread_t threadCtrl;
	pthread_attr_t attr;

	PARA_ASSERT(avp);

	pthread_attr_init (&attr);
	pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&threadCtrl,&attr, NEXUSAPP_DPlayer_Pthread,avp);
	pthread_attr_destroy (&attr);

	return ;
}
***********************************************************************************************/
#endif

