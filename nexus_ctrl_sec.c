/**************************************************************************
2011_01_19 mody by hh
1.mody function NEXUSAPP_Ctrl_Action()  for mw
2011_12_19 mody by hh
1.add pip info set  : add PLAYER_CTRL_SEC_PIPINFO_SET mode in NEXUSAPP_Ctrl_Sec_Action func
***************************************************************************/
#ifndef NEXUS_CTRL_SEC_C
#define NEXUS_CTRL_SEC_C
/***************************************************************************/
#include "nexus_picture_ctrl.h"
#include "nexus_video_adj.h"
#include "nexus_video_window.h"
#include "nexus_display_types.h"
#include "nexus_ctrl_sec.h"
#include "nexus_pizplay.h"
#include "nexus_ctrl.h"
#include "nexus_osd.h"
#include "nexus_mw.h"
/***************************************************************************/
/*************************************************
  Function:    	NEXUSAPP_Ctrl_Sec_PizContext
  Description:	piz picture 's context receive , if receive complete , change piz state , osd thread will be action .
  Input:		1 . struct av_playpump_t *avp	:  playpump player info struct	
  			2 . osd_addContext_info_t *info	:  osd_addContext_info_t info struct
  Output:						
  Return:		1 . 0   : succeed
  				2 . -1 : tcp transport error
  				3 . -2 : save file error 
  other : 		
*************************************************/
int NEXUSAPP_Ctrl_Sec_PizContext(av_playpump *avp, ctrl_sec_addContext_info *info)
{

	av_osd_piz_player *player = NULL;
	PizPlayerSettings Settings;
		
	PARA_ASSERT_VALUE(avp,CTRL_OSD_ERROR_TRANSPORT);
	PARA_ASSERT_VALUE(info,CTRL_OSD_ERROR_TRANSPORT); 

	player = &avp->osd.pizPlayer;

	PARA_ASSERT_VALUE(player,CTRL_OSD_ERROR_TRANSPORT);
	PARA_ASSERT_VALUE(player->fp,CTRL_OSD_ERROR_TRANSPORT);

	DBG_APP(("piz context: index = %d , len = %d , recsize = %d , size = %d\n",
		info->uiIndex , info->uiContextLen , player->uiRecSize ,info->uiTotalCount ));

	/* protect pizbuf*/
	if(
		info->uiContextLen > ADDCONTEXT_LEN_MAX
		||info->uiIndex > info->uiTotalCount
	)
	{
		player->uiSize =0;
		return CTRL_OSD_ERROR_TRANSPORT;
	}

	player->uiIndex = info->uiIndex;
	
	fwrite(info->szContext,1,info->uiContextLen,player->fp);
	
	player->uiRecSize += info->uiContextLen;

	if(info->uiIndex == info->uiTotalCount)
	{
		
		Settings.iLoop = info->iValue;
		strcpy(Settings.szAppName,info->szValue);
		strcpy(Settings.szPath,CTRL_PIZ_PATH);
		memcpy(&Settings.dest_rect,&info->position,sizeof(NEXUS_Rect));
		NEXUSAPP_PizPlayer_Init(avp,&Settings);
		fclose(player->fp);
	}
	
	return CTRL_OSD_ERROR_NOTHING;
}


int NEXUSAPP_Ctrl_Respond_WinAct(int iSockFd ,  ctrl_sec_info *ctrlData)
{
	if(iSockFd <= 0)
		goto error_out;
	
	if(socket_tcp_server_send(iSockFd, (char *)ctrlData,sizeof(ctrl_sec_info))<0)
	{
		DBG_APP(("NEXUSAPP_Ctrl_Respond_Send: send error !\n"));
		goto error_out;
	}

	return 0;

error_out:
	return CTRL_OSD_ERROR_TRANSPORT;
}

/*************************************************
  Function:    	NEXUSAPP_Ctrl_Sec_CmdGet
  Description:	get osd ctrl message info struct
  Input:			1 .  int iSockFd					:  control center socket fd			
  Output:		1 .  ctrl_sec_info *ctrlData 		:  ctrl message info struct					
  Return:		1 .  0   : succeed
  				2 . -1 : tcp transport error
  other : 		
*************************************************/
int NEXUSAPP_Ctrl_Sec_CmdGet(int iSockFd , ctrl_sec_info *ctrlData)
{

	unsigned long ulRercLen = sizeof(ctrl_sec_info);

	PARA_ASSERT_VALUE(ctrlData,-1);
	
	if(iSockFd < 0)
		return -1;
	
	DBG_APP(("NEXUSAPP_Ctrl_Sec_CmdGet: start recv!\n"));
	
	if(socket_tcp_server_read(iSockFd, (char *)ctrlData,&ulRercLen)<0)
	{
		DBG_APP(("NEXUSAPP_Ctrl_CmdInfo_Get: recv error!\n"));
		return -1;
	}

	DBG_APP(("NEXUSAPP_Ctrl_Sec_CmdGet:cmd = %d!\n",ctrlData->iCmdId));
	
	return 0;
}


/*************************************************
  Function:    	NEXUSAPP_Ctrl_Pict_CommonSet
  Description:	windows settings 
  Input:
  Output:						
  Return:
  other : 		
*************************************************/
void NEXUSAPP_Ctrl_Pict_CommonGet(av_playpump *avp , int id , int *value)
{
	int ivalue = 0 ;
	NEXUS_PictureCtrlCommonSettings pComSettings;

	NEXUS_PictureCtrl_GetCommonSettings(avp->display.winHd_0,&pComSettings);

	switch(id)
	{
		case CONTRAST:
			ivalue = pComSettings.contrast;
			break;
		case SATURATION:
			ivalue = pComSettings.saturation;
			break;
		case HUE:
			ivalue = pComSettings.hue;
			break;
		case BRIGHTNESS:
			ivalue = pComSettings.brightness;
			break;
		case COLORTEMPENABLE:
			ivalue = pComSettings.colorTempEnabled;
			break;
		case COLORTEMP:
			ivalue = pComSettings.colorTemp;
			break;
		case SHARNESSENABLE:
			ivalue = pComSettings.sharpnessEnable;
			break;			
		case SARPNESS:
			ivalue = pComSettings.sharpness;
			break;						
		default:
			break;
	}

	*value = ivalue;
	
	return;
}
/*************************************************
  Function:    	NEXUSAPP_Ctrl_Pict_CommonSet
  Description:	windows settings 
  Input:
  Output:						
  Return:
  other : 		
*************************************************/
void NEXUSAPP_Ctrl_Pict_CommonSet(av_playpump *avp , int id , int ivalue)
{
	NEXUS_PictureCtrlCommonSettings pComSettings;

	NEXUS_PictureCtrl_GetCommonSettings(avp->display.winHd_0,&pComSettings);

	switch(id)
	{
		case CONTRAST:
			pComSettings.contrast = ivalue;
			break;
		case SATURATION:
			pComSettings.saturation = ivalue;
			break;
		case HUE:
			pComSettings.hue = ivalue;
			break;
		case BRIGHTNESS:
			pComSettings.brightness = ivalue;
			break;
		case COLORTEMPENABLE:
			pComSettings.colorTempEnabled = ivalue;
			break;
		case COLORTEMP:
			pComSettings.colorTemp = ivalue;
			break;
		case SHARNESSENABLE:
			pComSettings.sharpnessEnable = ivalue;
			break;			
		case SARPNESS:
			pComSettings.sharpness = ivalue;
			break;						
		default:
			return;
	}

	NEXUS_PictureCtrl_SetCommonSettings(avp->display.winHd_0,&pComSettings);
}

/*************************************************
  Function:    	NEXUSAPP_Ctrl_Pict_CommonSet
  Description:	windows settings 
  Input:
  Output:						
  Return:
  other : 		
*************************************************/
void NEXUSAPP_Ctrl_Pict_DitherGet(av_playpump *avp , int id , int *value)
{
	int ivalue = 0;
	NEXUS_PictureCtrlDitherSettings	pDitherSettings;	

	NEXUS_PictureCtrl_GetDitherSettings(avp->display.winHd_0,&pDitherSettings);

	switch(id)
	{
		case REDUCESMOOTH:
			ivalue = pDitherSettings.reduceSmooth;
			break;
		case SMOOTHENABLE:
			ivalue = pDitherSettings.smoothEnable;
			break;
		case SMOOTHLIMIT:
			ivalue = pDitherSettings.smoothLimit;
			break;
		default:
			break;;	
	}

	*value = ivalue;
	return;
}

/*************************************************
  Function:    	NEXUSAPP_Ctrl_Pict_CommonSet
  Description:	windows settings 
  Input:
  Output:						
  Return:
  other : 		
*************************************************/
void NEXUSAPP_Ctrl_Pict_DitherSet(av_playpump *avp , int id , int ivalue)
{
	NEXUS_PictureCtrlDitherSettings	pDitherSettings;	

	NEXUS_PictureCtrl_GetDitherSettings(avp->display.winHd_0,&pDitherSettings);

	switch(id)
	{
		case REDUCESMOOTH:
			pDitherSettings.reduceSmooth = ivalue;
			break;
		case SMOOTHENABLE:
			pDitherSettings.smoothEnable = ivalue;
			break;
		case SMOOTHLIMIT:
			pDitherSettings.smoothLimit = ivalue;
			break;
		default:
			return;	
	}

	NEXUS_PictureCtrl_SetDitherSettings(avp->display.winHd_0,&pDitherSettings);
}

/*************************************************
  Function:    	NEXUSAPP_Ctrl_Pict_CmsGet
  Description:	windows settings 
  Input:
  Output:						
  Return:
  other : 		
*************************************************/
void NEXUSAPP_Ctrl_Pict_CmsGet(av_playpump *avp , int id , int *value)
{
	int ivalue = 0;
	NEXUS_PictureCtrlCmsSettings pCmsSettings;

	NEXUS_PictureCtrl_GetCmsSettings(avp->display.winHd_0,&pCmsSettings);

	switch(id)
	{
		case SATURATIONGAIN_R:
			ivalue = pCmsSettings.saturationGain.red;
			break;
		case SATURATIONGAIN_G:
			ivalue = pCmsSettings.saturationGain.green;
			break;
		case SATURATIONGAIN_B:
			ivalue = pCmsSettings.saturationGain.blue;
			break;
		case SATURATIONGAIN_C:
			ivalue = pCmsSettings.saturationGain.cyan;
			break;
		case SATURATIONGAIN_M:
			ivalue = pCmsSettings.saturationGain.magenta;
			break;
		case SATURATIONGAIN_Y:
			ivalue = pCmsSettings.saturationGain.yellow;	
			break;
		/**********************************************/	
		case HUEGAIN_R:
			ivalue = pCmsSettings.hueGain.red;
			break;
		case HUEGAIN_G:
			ivalue = pCmsSettings.hueGain.green;
			break;
		case HUEGAIN_B:
			ivalue = pCmsSettings.hueGain.blue;
			break;
		case HUEGAIN_C:
			ivalue = pCmsSettings.hueGain.cyan;
			break;
		case HUEGAIN_M:
			ivalue = pCmsSettings.hueGain.magenta;
			break;
		case HUEGAIN_Y:
			ivalue = pCmsSettings.hueGain.yellow;	
			break;
		default:
			break;	
	}

	*value = ivalue;
	
	return;
}

/*************************************************
  Function:    	NEXUSAPP_Ctrl_Pict_CommonSet
  Description:	windows settings 
  Input:
  Output:						
  Return:
  other : 		
*************************************************/
void NEXUSAPP_Ctrl_Pict_CmsSet(av_playpump *avp , int id , int ivalue)
{

	NEXUS_PictureCtrlCmsSettings pCmsSettings;

	NEXUS_PictureCtrl_GetCmsSettings(avp->display.winHd_0,&pCmsSettings);

	switch(id)
	{
		case SATURATIONGAIN_R:
			pCmsSettings.saturationGain.red = ivalue;
			break;
		case SATURATIONGAIN_G:
			pCmsSettings.saturationGain.green = ivalue;
			break;
		case SATURATIONGAIN_B:
			pCmsSettings.saturationGain.blue = ivalue;
			break;
		case SATURATIONGAIN_C:
			pCmsSettings.saturationGain.cyan = ivalue;
			break;
		case SATURATIONGAIN_M:
			pCmsSettings.saturationGain.magenta = ivalue;
			break;
		case SATURATIONGAIN_Y:
			pCmsSettings.saturationGain.yellow = ivalue;	
			break;
		/**********************************************/	
		case HUEGAIN_R:
			pCmsSettings.hueGain.red = ivalue;
			break;
		case HUEGAIN_G:
			pCmsSettings.hueGain.green = ivalue;
			break;
		case HUEGAIN_B:
			pCmsSettings.hueGain.blue = ivalue;
			break;
		case HUEGAIN_C:
			pCmsSettings.hueGain.cyan = ivalue;
			break;
		case HUEGAIN_M:
			pCmsSettings.hueGain.magenta = ivalue;
			break;
		case HUEGAIN_Y:
			pCmsSettings.hueGain.yellow = ivalue;	
			break;
		default:
			return;	
	}

	NEXUS_PictureCtrl_SetCmsSettings(avp->display.winHd_0,&pCmsSettings);
	return;
}

/*************************************************
  Function:    	NEXUSAPP_Ctrl_Pict_DnrGet
  Description:	windows settings 
  Input:
  Output:						
  Return:
  other : 		
*************************************************/
void NEXUSAPP_Ctrl_Pict_DnrGet(av_playpump *avp , int id , int *value)
{
	int ivalue = 0;
	
	NEXUS_VideoWindowDnrSettings	pDnrSettings;	

	NEXUS_VideoWindow_GetDnrSettings(avp->display.winHd_0,&pDnrSettings);

	switch(id)
	{
		case WIN_DNR_MNR_MODE:
			ivalue = pDnrSettings.mnr.mode ;
			break;
		case WIN_DNR_MNR_LEVEL:
			ivalue = pDnrSettings.mnr.level;
			break;
		case WIN_DNR_BNR_MODE:
			ivalue = pDnrSettings.bnr.mode;
			break;
		case WIN_DNR_BNR_LEVEL:
			ivalue = pDnrSettings.bnr.level;
			break;	
		case WIN_DNR_DCR_MODE:
			ivalue = pDnrSettings.dcr.mode;
			break;
		case WIN_DNR_DCR_LEVEL:
			ivalue = pDnrSettings.dcr.level;
			break;			
		default:
			break;;	
	}

	*value = ivalue;
	
	return;
}

/*************************************************
  Function:    	NEXUSAPP_Ctrl_Pict_DnrSet
  Description:	windows settings 
  Input:
  Output:						
  Return:
  other : 		
*************************************************/
void NEXUSAPP_Ctrl_Pict_DnrSet(av_playpump *avp , int id , int ivalue)
{
	NEXUS_VideoWindowDnrSettings	pDnrSettings;	

	NEXUS_VideoWindow_GetDnrSettings(avp->display.winHd_0,&pDnrSettings);

	switch(id)
	{
		case WIN_DNR_MNR_MODE:
			pDnrSettings.mnr.mode = ivalue;
			break;
		case WIN_DNR_MNR_LEVEL:
			pDnrSettings.mnr.level = ivalue;
			break;
		case WIN_DNR_BNR_MODE:
			pDnrSettings.bnr.mode = ivalue;
			break;
		case WIN_DNR_BNR_LEVEL:
			pDnrSettings.bnr.level = ivalue;
			break;	
		case WIN_DNR_DCR_MODE:
			pDnrSettings.dcr.mode = ivalue;
			break;
		case WIN_DNR_DCR_LEVEL:
			pDnrSettings.dcr.level = ivalue;
			break;			
		default:
			break;;	
	}

	NEXUS_VideoWindow_SetDnrSettings(avp->display.winHd_0,&pDnrSettings);
}

/*************************************************
  Function:    	NEXUSAPP_Ctrl_Pict_LinearGet
  Description:	windows settings 
  Input:
  Output:						
  Return:
  other : 		
*************************************************/
void NEXUSAPP_Ctrl_Pict_LinearGet(av_playpump *avp , int id , int *value)
{
	NEXUS_VideoWindowSettings  windowsettings;

	NEXUS_VideoWindow_GetSettings(avp->display.winHd_0,&windowsettings);

	(void *)id ;
	
	if(windowsettings.contentMode == NEXUS_VideoWindowContentMode_eFullNonLinear)
		*value = 1;
	else
		*value = 0;
}

/*************************************************
  Function:    	NEXUSAPP_Ctrl_Pict_LinearSet
  Description:	windows settings 
  Input:
  Output:						
  Return:
  other : 		
*************************************************/
void NEXUSAPP_Ctrl_Pict_LinearSet(av_playpump *avp , int id , int ivalue)
{
	NEXUS_VideoWindowSettings  windowsettings;
	NEXUS_VideoWindowScalerSettings scalerSettings;

	NEXUS_VideoWindow_GetSettings(avp->display.winHd_0,&windowsettings);
	NEXUS_VideoWindow_GetScalerSettings(avp->display.winHd_0, &scalerSettings);

	(void *)id ;
	
	if(ivalue)
	{
		windowsettings.contentMode = NEXUS_VideoWindowContentMode_eFullNonLinear;

		scalerSettings.nonLinearScaling = true;

		scalerSettings.verticalDejagging = true;
		scalerSettings.horizontalLumaDeringing = true;
		scalerSettings.verticalLumaDeringing = true;
		scalerSettings.horizontalChromaDeringing = true;
		scalerSettings.verticalChromaDeringing = true;

		scalerSettings.bandwidthEquationParams.bias = NEXUS_ScalerCaptureBias_eScalerBeforeCapture;
		scalerSettings.bandwidthEquationParams.delta = 1*1000*1000;
	}
	else
	{
		windowsettings.contentMode = NEXUS_VideoWindowContentMode_eFull;

		scalerSettings.nonLinearScaling = false;

		scalerSettings.verticalDejagging = false;
		scalerSettings.horizontalLumaDeringing = false;
		scalerSettings.verticalLumaDeringing = false;
		scalerSettings.horizontalChromaDeringing = false;
		scalerSettings.verticalChromaDeringing = false;
	}


	NEXUS_VideoWindow_SetSettings(avp->display.winHd_0,&windowsettings);
	NEXUS_VideoWindow_SetScalerSettings(avp->display.winHd_0, &scalerSettings);
	
}

/*************************************************
  Function:    	NEXUSAPP_Ctrl_Window_Set
  Description:	windows settings 
  Input:		1 . av_playpump *avp		:  playpump player info struct	
  			
  Output:						
  Return:	1 . 0   : succeed
  		2 . -1 : tcp transport error
  other : 		
*************************************************/
int NEXUSAPP_Ctrl_Window_Set(av_playpump *avp , int cmd ,int *value , int mode)
{
	int iRet = 0;

	switch(cmd)
	{
		case CONTRAST:
		case SATURATION:
		case HUE:
		case BRIGHTNESS:
		case COLORTEMPENABLE:
		case COLORTEMP:
		case SHARNESSENABLE:
		case SARPNESS:
			if(mode)
				NEXUSAPP_Ctrl_Pict_CommonSet(avp,cmd,*value);
			else
				NEXUSAPP_Ctrl_Pict_CommonGet(avp,cmd,value);

			iRet = 1; 
			
			break;
		case REDUCESMOOTH:
		case SMOOTHENABLE:
		case SMOOTHLIMIT:
			if(mode)
				NEXUSAPP_Ctrl_Pict_DitherSet(avp,cmd,*value);
			else
				NEXUSAPP_Ctrl_Pict_DitherGet(avp,cmd,value);

			iRet = 1;
			break;
		case SATURATIONGAIN_R:
		case SATURATIONGAIN_G:
		case SATURATIONGAIN_B:
		case SATURATIONGAIN_C:
		case SATURATIONGAIN_M:
		case SATURATIONGAIN_Y:
		case HUEGAIN_R:
		case HUEGAIN_G:
		case HUEGAIN_B:
		case HUEGAIN_C:
		case HUEGAIN_M:
		case HUEGAIN_Y:
			if(mode)
				NEXUSAPP_Ctrl_Pict_CmsSet(avp,cmd,*value);
			else
				NEXUSAPP_Ctrl_Pict_CmsGet(avp,cmd,value);

			iRet = 1; 
			
			break;
		case WIN_DNR_MNR_MODE:
		case WIN_DNR_MNR_LEVEL:
		case WIN_DNR_BNR_MODE:
		case WIN_DNR_BNR_LEVEL:
		case WIN_DNR_DCR_MODE:
		case WIN_DNR_DCR_LEVEL:
			if(mode)
				NEXUSAPP_Ctrl_Pict_DnrSet(avp,cmd,*value);
			else
				NEXUSAPP_Ctrl_Pict_DnrGet(avp,cmd,value);

			iRet = 1;
			break;

		case  WIN_NORLINEAR_SCALING:
			if(mode)
				NEXUSAPP_Ctrl_Pict_LinearSet(avp,cmd,*value);
			else
				NEXUSAPP_Ctrl_Pict_LinearGet(avp,cmd,value);

			iRet = 1;
			break;
		default:
			break;	
	}

	return iRet ;
}

/*************************************************
  Function:    	NEXUSAPP_Ctrl_Window_Action
  Description:	windows settings 
  Input:		1 . av_playpump *avp		:  playpump player info struct	
  			2 . ctrl_sec_info *ctrlData 	:  commond message info 
  Output:						
  Return:	1 . 0   : succeed
  		2 . -1 : tcp transport error
  other : 		
*************************************************/
int NEXUSAPP_Ctrl_Window_Action(av_playpump *avp , ctrl_sec_info *ctrlData)
{
	PARA_ASSERT_VALUE(avp->display.winHd_0 , 0);
	
	DBG_APP(("NEXUSAPP_Ctrl_Window_Action in: id = %d , value = %d , mode = %d<<-------!\n",ctrlData->iCmdId,ctrlData->iValue,ctrlData->iPlayerId));

	return NEXUSAPP_Ctrl_Window_Set(avp,ctrlData->iCmdId,&ctrlData->iValue,ctrlData->iPlayerId);
}

/*************************************************
  Function:    	NEXUSAPP_Ctrl_Sec_Action
  Description:	commond message  action 
  Input:		1 . av_playpump *avp		:  playpump player info struct	
  			2 . ctrl_sec_info *ctrlData 	:  commond message info 
  Output:						
  Return:	1 . 0   : succeed
  			2 . -1 : tcp transport error
  other : 		
*************************************************/
int NEXUSAPP_Ctrl_Sec_Action(av_playpump *avp , ctrl_sec_info *ctrlData)
{
	int iRet = CTRL_OSD_ERROR_CMDERROR;
	av_osd_piz_player *player = NULL;
	PizPlayerSettings pSetting;

	PARA_ASSERT_VALUE(avp,-1);
	PARA_ASSERT_VALUE(ctrlData,-1);

	player = &avp->osd.pizPlayer;

	PARA_ASSERT_VALUE(player,-1);
	
	DBG_APP(("NEXUSAPP_Ctrl_Sec_Action in: id = %d <<-------!\n",ctrlData->iCmdId));

	if(NEXUSAPP_Ctrl_Window_Action(avp,ctrlData))
	{
		iRet = NEXUSAPP_Ctrl_Respond_WinAct(avp->socketCtrlSecFd,ctrlData);
		return iRet;
	}
	
	switch(ctrlData->iCmdId)
	{
		case PLAYER_CTRL_SEC_PIPINFO_SET:
			NEXUSAPP_Ctrl_PipInfo_Set(avp,&ctrlData->position);
			break;
		case PLAYER_CTRL_SEC_PIZPLAYER_PLAY:
			iRet = NEXUSAPP_PizPlayer_Start(avp,ctrlData->szValue);
			break;
		case PLAYER_CTRL_SEC_PIZPLAYER_STOP:
			iRet = NEXUSAPP_PizPlayer_Stop(avp,ctrlData->szValue);
			break;
		case PLAYER_CTRL_SEC_PIZPLAYER_APPEND:
			if(NEXUSAPP_PizPlayer_Check(avp,ctrlData->szValue))
			{
				iRet = -2;
				break;
			}

			if(player->fp)
				fclose(player->fp);
			
			unlink(CTRL_PIZ_PATH);
								
			if((player->fp = fopen(CTRL_PIZ_PATH,"ab+")) == NULL)
			{
				iRet = -1;
				break;
			}
			
			player->iState = 0;
			player->uiIndex = 0;
			player->uiRecSize = 0;
			player->uiSize = ctrlData->iValue;

			DBG_APP(("piz size = %d \n" , ctrlData->iValue));
			
			strcpy(player->szName,ctrlData->szValue);
			memcpy(&player->position,&ctrlData->position,sizeof(NEXUS_Rect));
			break;
		case PLAYER_CTRL_SEC_PIZPLAYER_CLEAN:
			iRet = NEXUSAPP_PizPlayer_Destroy(avp,ctrlData->szValue);
			break;
		case PLAYER_CTRL_SEC_PIZPLAYER_MODY:
			memset(&pSetting,0x00,sizeof(PizPlayerSettings));
			pSetting.iLoop = ctrlData->iValue;
			strcpy(pSetting.szAppName , ctrlData->szValue);
			memcpy(&pSetting.dest_rect , &ctrlData->position , sizeof(NEXUS_Rect));
			iRet = NEXUSAPP_PizPlayer_Setting(avp,&pSetting);
			break;	
		default:
			break;
	}

	return NEXUSAPP_Ctrl_Respond_Send(avp->socketCtrlSecFd,CTRL_OSD_MESSAGE_ERROR,iRet);
	
}

/*************************************************
  Function:    	NEXUSAPP_Ctrl_Sec_AddInfo_Get
  Description:	Add context info struct get from tcp socket
  Input:			1 . int iSockFd							:  control center socket fd
  				2 . ctrl_sec_addContext_info *ctrlData	:  add context info struct
  Output:						
  Return:		1 . 0   : succeed
  				2 . -1 : tcp transport error or input parameter is nulll
  other : 		
*************************************************/
int NEXUSAPP_Ctrl_Sec_AdInfoGet(int iSockFd , ctrl_sec_addContext_info *ctrlData)
{

	unsigned long ulRecvlen = sizeof(ctrl_sec_addContext_info);

	PARA_ASSERT_VALUE(ctrlData,-1);
	
	if(iSockFd <= 0)
		return -1;

	if(socket_tcp_server_read(iSockFd, (char *)ctrlData,&ulRecvlen)<0)
	{
		DBG_APP(("NEXUSAPP_Ctrl_AddContextInfo_Get: recv error !\n"));
		return -1;
	}

	DBG_APP(("AddContextInfo : contextid:%d\n",ctrlData->iContextId));
	
	return 0;
}


/*************************************************
  Function:    	NEXUSAPP_Ctrl_Sec_AdInfoAction
  Description:	Get add context data  , and respond .
  Input:		1 .av_playpump *avp				:  playpump player info struct	
  			2 . ctrl_sec_addContext_info *info	:  osd_addContext_info_t info struct
  Output:						
  Return:	1 . 0   : succeed
  			2 . -1 : tcp transport error
  			3 . -2 : mem error
  other : 		
*************************************************/
int NEXUSAPP_Ctrl_Sec_AdInfoAction(av_playpump *avp,  ctrl_sec_addContext_info *info)
{
	int iRet =0 ;

	PARA_ASSERT_VALUE(avp,-1);
	PARA_ASSERT_VALUE(info,-1);
	
	switch(info->iContextId)
	{
		case PLAYER_CTRL_SEC_CONTEXT_PIZ:
			iRet = NEXUSAPP_Ctrl_Sec_PizContext(avp,info);
			break;
		default:
			break;
	}

	return NEXUSAPP_Ctrl_Respond_Send(avp->socketCtrlSecFd,CTRL_OSD_MESSAGE_ERROR,iRet);	
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
int NEXUSAPP_Ctrl_Sec_Flag_Get(int iSockFd , unsigned int *iFlag)
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
  Function:    	NEXUSAPP_Ctrl_Sec_Probe
  Description:	NEXUSAPP_Ctrl_Sec_Pthread : probe func
  Input:			1 . struct av_playpump_t *avp	:  hmplayer info
  Output:						
  Return:		
  other : 		
*************************************************/
void NEXUSAPP_Ctrl_Sec_Probe( struct av_playpump_t *avp)
{

	ctrl_sec_info ctrl;
	ctrl_sec_addContext_info ctrl_addContex;

	int iSockFd;
	uint32_t iCmdFlag =0 ;

	PARA_ASSERT(avp);

	iSockFd = avp->socketCtrlSecFd;
	
	DBG_APP(("NEXUSAPP_Playpump_Ctrl ,socket : %d!\n",iSockFd));
	
	avp->ctrlSecPthreadStart = true;

	while(1)
	{
		usleep(1000);

		if(NEXUSAPP_Ctrl_Sec_Flag_Get(iSockFd,&iCmdFlag))
			break;

		if(iCmdFlag == CTRL_FLAG_ID)
		{
			if(NEXUSAPP_Ctrl_Sec_CmdGet(iSockFd,&ctrl))
				break;

			if(NEXUSAPP_Ctrl_Sec_Action(avp,&ctrl))
				break;
		}
		else
		{
			if(NEXUSAPP_Ctrl_Sec_AdInfoGet(iSockFd,&ctrl_addContex))
				break;
			
			if(NEXUSAPP_Ctrl_Sec_AdInfoAction(avp,&ctrl_addContex))
				break;
		}

	}

	DBG_APP(("ctrl sec tcp stop!\n"));
	NEXUSAPP_Ctrl_Sec_Stop(avp);
	avp->ctrlSecPthreadStart  = false;

	return ;
	

}


/*************************************************
  Function:    	NEXUSAPP_Ctrl_Sec_Pthread
  Description:	sec ctrl thread func
  Input:			1 . void *context	: struct av_playpump_t avp 
  Output:						
  Return:		
  other : 		
*************************************************/
void *NEXUSAPP_Ctrl_Sec_Pthread(void *context)
{
	av_playpump *avp;
	socket_t *socket_info;
	ctrl_start_info *info = (ctrl_start_info *)context;

	PARA_ASSERT_VALUE(info,NULL);
	
	avp = info->avp;
	socket_info = info->socket_info;
	
	PARA_ASSERT_VALUE(avp,NULL);
	PARA_ASSERT_VALUE(socket_info,NULL);

	if(avp->socketCtrlSecFd!=-1)
			socket_tcp_server_close(&avp->socketCtrlSecFd);

	while(1)
	{
		if((avp->socketCtrlSecFd = socket_tcp_server_open(socket_info))<=0)
			goto close_sock;
		
		if(NEXUSAPP_Ctrl_Check(avp->socketCtrlSecFd))
			goto close_sock;
		
		NEXUSAPP_Ctrl_Sec_Probe(avp);

		continue;
		
close_sock:
		
		socket_tcp_server_close(&avp->socketCtrlSecFd);	
		usleep(100000);
		
	}

	return NULL;
}


/*************************************************
  Function:    	NEXUSAPP_Ctrl_Sec_Check
  Description:	ctrl thread stop , free all malloc buffer and close socket
  Input:		1 . int iSockfd	:  socket line check	
  Output:						
  Return:		0  : check succ
  			-1: check error
  other : 		
*************************************************/
int  NEXUSAPP_Ctrl_Sec_Check(int iSockfd)
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
  Function:    	NEXUSAPP_Ctrl_Sec_Init
  Description:	control center socket init and start
  Input:			1 . socket_t *socket_info	:  socket info
  Output:		
  Return:		
  other : 		
*************************************************/
void NEXUSAPP_Ctrl_Sec_Init(socket_t *socket_info)
{
	PARA_ASSERT(socket_info);
	
	memset(socket_info,0,sizeof(socket_t));
	
	while(1)
	{
		if(!socket_tcp_server_init(socket_info,SEC_PLAYER_CTCL_PORT))
			break;

		sleep(1);
	}
	
	return ;
}


/*************************************************
  Function:    	NEXUSAPP_Ctrl_Sec_Stop
  Description:	ctrl thread stop , free all malloc buffer and close socket
  Input:			1 . struct av_playpump_t *avp	:  playpump player info struct		
  Output:						
  Return:		
  other : 		
*************************************************/
void NEXUSAPP_Ctrl_Sec_Stop(av_playpump *avp)
{
	socket_tcp_server_close(&avp->socketCtrlSecFd);
}

/*************************************************
  Function:    	NEXUSAPP_Ctrl_Sec_Start
  Description:	ctrl sec thread start .
  Input:		1 . struct av_playpump_t *avp	:  playpump player info struct	
  			2 . socket_t *socket_info 		:  ctrl center socket info 
  Output:						
  Return:		
  other : 		
*************************************************/
void NEXUSAPP_Ctrl_Sec_Start(ctrl_start_info *s_info)
{
	pthread_t threadCtrl;
	pthread_attr_t attr;

	PARA_ASSERT(s_info);
	PARA_ASSERT(s_info->avp);
	PARA_ASSERT(s_info->socket_info);

	pthread_attr_init (&attr);
	pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&threadCtrl,&attr, NEXUSAPP_Ctrl_Sec_Pthread,s_info);
	pthread_attr_destroy (&attr);

	return ;
}

#endif

