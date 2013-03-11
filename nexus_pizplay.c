/**************************************************************************
2011_05_07 create by hh
***************************************************************************/
#ifndef NEXUS_PIZPLAYER_C
#define NEXUS_PIZPLAYER_C
/***************************************************************************/
#include "nexus_link.h"
#include "nexus_pizplay.h"
#include "nexus_osd.h"
/***************************************************************************/
nexusLinkList Head_pizplayer = NULL;
/***************************************************************************/
/*************************************************
  Function:    	NEXUSAPP_PizPlayer_PizFlagSet
  Description:	set piz flag , piz flag is piz frame logic frame , 0: same of last frame , 1: new frame 
  Input:			1. char *flag 	piz flag 
  				2. int iSize		flag size
  				3. int index		logic frame index 
  				4. bool value	is same of last frame 
  								true : new frame
  								false :  same of last frame
  Output:		
  Return:
  other : 		
*************************************************/
void NEXUSAPP_PizPlayer_PizFlagSet(unsigned char *flag , int iSize , int index , unsigned char value)
{
/*
	int byte_no = 0;
	int bit_no =0 ;
	char byte = 0x01 ;
*/
	PARA_ASSERT(flag);

	if(index > iSize -1)
		return ;

	flag[index] = value;
	
/*
	byte_no = index / 8;
	bit_no = index % 8;
	if( index % 8 )
		byte_no ++;

	if(byte_no > iSize)
		return ;

	if(value)
	{
		byte = byte << bit_no ;
		byte = byte | flag[byte_no];
		flag[byte_no] = byte ;
	}
*/	
}

/*************************************************
  Function:    	NEXUSAPP_PizPlayer_PizFlagGet
  Description:	get piz flag , piz flag is piz frame logic frame , 0: same of last frame , 1: new frame 
  Input:			1. char *flag 	piz flag 
  				2. int index		logic frame index 
  Output:		1. int value	is same of last frame 
  								1 :  new frame
  								0 :  same of last frame
  								-1 : error
  Return:
  other : 		
*************************************************/
int NEXUSAPP_PizPlayer_PizFlagGet(unsigned char *flag , int iLen , int index)
{
/*
	int byte_no = 0;
	int bit_no =0 ;
	char byte = 0  ;
*/
	int no = 0;

	PARA_ASSERT_VALUE(flag,-1);

	if(index > iLen -1)
		return -1;

	no = flag[index];

	return no;
	
/*
	
	byte_no = index / 8;
	bit_no = index % 8;
	if( index % 8 )
		byte_no ++;

	if(byte_no > 7)
		return 0;

	byte = flag[byte_no];

	byte = (byte >> bit_no ) & 0x01;

	if(byte)
		return 1;

	return 0;
*/
}

/*************************************************
  Function:    	NEXUSAPP_PizPlayer_PizToFb
  Description:	load piz file to fb
  Input:			1. char *bname  		piz file path
  				2. data_t *my_data		piz link node info
  Output:		0    :	succful
  				-1  :	error
  Return:
  other : 		
*************************************************/

int NEXUSAPP_PizPlayer_PizToFb(char *bName , data_t *my_data)
{
	int iRet = 0 , i , iFlag = 0 ;
	nexus_xxpiz axpiz;
	NEXUS_Rect startPoint;

	PARA_ASSERT_VALUE(bName,-1);
	PARA_ASSERT_VALUE(my_data,-1);
	
	memset(&axpiz , 0x00 , sizeof(nexus_xxpiz));
	
	if(!NEXUSAPP_Xxpiz_Load_From_File(&axpiz,bName))
	{
		DBG_APP(("NEXUSAPP_Xxpiz_Load_From_File: fail!\n"));
		NEXUSAPP_Xxpiz_Clean_Frame(&axpiz);
		return -1;
	}
	
	DBG_APP(("NEXUSAPP_PizPlayer_PizToFb:creat surface : %d : %d , frame = %d!\n",
		axpiz.frame_axbmp.wx,axpiz.frame_axbmp.hy,axpiz.frame_count));
	
	/* create buffer surface */
	NEXUS_Surface_GetDefaultCreateSettings(&my_data->fb.createSettings);
	my_data->fb.createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
	my_data->fb.createSettings.width = axpiz.frame_axbmp.wx ;
	my_data->fb.createSettings.height = axpiz.frame_axbmp.hy * axpiz.frame_count;

	my_data->wx = axpiz.frame_axbmp.wx;
	my_data->hy = axpiz.frame_axbmp.hy;
	my_data->iCount = axpiz.logic_count;
	
	if(NEXUSAPP_Surface_MyCreate(&my_data->fb))
		return -1;

	startPoint.y = startPoint.x = 0;
	startPoint.width = axpiz.frame_axbmp.wx;
	startPoint.height = axpiz.frame_axbmp.hy;

	NEXUSAPP_Xxpiz_Reset_Frame(&axpiz);

	DBG_APP(("NEXUSAPP_PizPlayer_PizToFb: fb : %d x %d!\n",
			my_data->fb.createSettings.width,
			my_data->fb.createSettings.height));
	
	DBG_APP(("NEXUSAPP_PizPlayer_PizToFb: frame : %d !\n",axpiz.logic_count));
		
	for(i = 0; i < axpiz.logic_count; i++)
	{
		if(iFlag)
		{
			NEXUSAPP_PizPlayer_PizFlagSet(my_data->flag,DATA_FLAG_MAX,i,1);	
			continue;
		}
		
		iRet = NEXUSAPP_Xxpiz_Stepp_Frame(&axpiz);
		switch(iRet)
		{
			case 1 : /* new frame */
				NEXUSAPP_Surface_Display_Img(&axpiz.frame_axbmp,&startPoint,&my_data->fb);
				startPoint.y += axpiz.frame_axbmp.hy;
				break;
			case 2:  /* same of last frame */
				NEXUSAPP_PizPlayer_PizFlagSet(my_data->flag,DATA_FLAG_MAX,i,1);	
				break;
			default: /*lastest frame , but late have same frame ????? why*/
				NEXUSAPP_PizPlayer_PizFlagSet(my_data->flag,DATA_FLAG_MAX,i,1);	
				iFlag = 1;
				break;
				
		}
	}

	NEXUSAPP_Xxpiz_Clean_Frame(&axpiz);

	return 0;
	
}

/*************************************************
  Function:    	NEXUSAPP_PizPlayer_Destroy
  Description:	delete piz 
  Input:			1. char *szAppName		piz app name
  Output:		1. 0	  	succful
  				2. -1  		no found app 
  				3. -2 		no found app 
  Return:
  other : 
*************************************************/
int NEXUSAPP_PizPlayer_Destroy(av_playpump *avp , char *pAppName)
{

	nexusLinkList node;
	int iRet = 0 ;

	PARA_ASSERT_VALUE(avp,-1);
	PARA_ASSERT_VALUE(pAppName,-1);

	DBG_APP(("NEXUSAPP_PizPlayer_Destroy : in!\n"));
	
	node = NEXUSAPP_LINK_Location_FindName(Head_pizplayer,pAppName);

	if(!node)
		return -1;
	
	pthread_mutex_lock(&avp->playerPizLock);
	
	node->data.start = false;

	if(node->data.fb.surface)
		NEXUS_Surface_Destroy(node->data.fb.surface);

	iRet = NEXUSAPP_LINK_Delete_myLinkList(&Head_pizplayer,pAppName);

	pthread_mutex_unlock(&avp->playerPizLock);
	
	return iRet;
	
}

/*************************************************
  Function:    	NEXUSAPP_PizPlayer_Create
  Description:	create piz player node 
  Input:		
  Output:		1. 0 ~ 9  	piz player count max 9
  				2. -1  		create error , out of player max
  				3. -2 		create error , mem voer flow
  Return:
  other : 
*************************************************/
int NEXUSAPP_PizPlayer_Create(av_playpump *avp , char *fName , char *pAppName , unsigned int uiLen)
{
	int iCount = 0 , iRet = 0;
	data_t my_data;

	PARA_ASSERT_VALUE(avp,-1);
	PARA_ASSERT_VALUE(fName,-1);
	PARA_ASSERT_VALUE(pAppName,-1);
	
	iCount = NEXUSAPP_LINK_Length_myLinkList(Head_pizplayer);

	DBG_APP(("NEXUSAPP_PizPlayer_Create: icount = %d !\n",iCount));
	
	if(iCount > PIZPLAYER_MAX)
		return -2;

	memset(&my_data,0x00,sizeof(data_t));

	if(NEXUSAPP_PizPlayer_PizToFb(fName, &my_data))
		return -1;
	
	memcpy(my_data.name , pAppName, uiLen > sizeof(my_data.name)?sizeof(my_data.name):uiLen);

	pthread_mutex_lock(&avp->playerPizLock);
	iRet = NEXUSAPP_LINK_Insert_First(&Head_pizplayer,&my_data);
	pthread_mutex_unlock(&avp->playerPizLock);
	
	if(iRet)
	{
		/* must by mody , clean fb  and node*/
		NEXUSAPP_PizPlayer_Destroy(avp,pAppName);
		return -1;
	}
	
	return iCount ;
}

/*************************************************
  Function:    	NEXUSAPP_PizPlayer_Create
  Description:	create piz player node 
  Input:		
  Output:		1. 0   	succfull
  				2. -1  	no found node , please create node first , reference NEXUSAPP_PizPlayer_Create
  Return:
  other : 
*************************************************/
int NEXUSAPP_PizPlayer_Open(PizPlayerSettings *pSetting)
{
	nexusLinkList pNode;

	PARA_ASSERT_VALUE(pSetting,-1);
	
	pNode = NEXUSAPP_LINK_Location_FindName(Head_pizplayer,pSetting->szAppName);	

	if(!pNode)
		goto error;
	
	memcpy(&pNode->data.dest_rect,&pSetting->dest_rect , sizeof(NEXUS_Rect));

	pNode->data.loop = pSetting->iLoop;
	
	return 0;

error:
	return -1;
	
}

/*************************************************
  Function:    	NEXUSAPP_PizPlayer_Check
  Description:	init one  piz player node 
  Input:		
  Output:		1. 0   	succfull
  				2. -1  	already has same name piz 
  Return:
  other : 
*************************************************/
int NEXUSAPP_PizPlayer_Check(av_playpump *avp , char *pAppName)
{
	nexusLinkList pNode;

	PARA_ASSERT_VALUE(avp,-1);
	PARA_ASSERT_VALUE(pAppName,-1);
	
	pNode = NEXUSAPP_LINK_Location_FindName(Head_pizplayer,pAppName);	

	if(pNode)
		return -1;
	else
		return 0;
}

/*************************************************
  Function:    	NEXUSAPP_PizPlayer_Init
  Description:	init one  piz player node 
  Input:		
  Output:		1. 0   	succfull
  				2. -1  	piz player node not create
  Return:
  other : 
*************************************************/
int NEXUSAPP_PizPlayer_Init(av_playpump *avp , PizPlayerSettings *pSetting)
{
	PARA_ASSERT_VALUE(avp,-1);
	PARA_ASSERT_VALUE(pSetting,-1);
	
	if(NEXUSAPP_PizPlayer_Create(avp , pSetting->szPath, pSetting->szAppName , strlen(pSetting->szAppName) ) < 0)
		return -1;	

	
	return NEXUSAPP_PizPlayer_Open(pSetting);	
}

/*************************************************
  Function:    	NEXUSAPP_PizPlayer_Setting
  Description:	start piz play parameter
  Input:			1. char *appName				:	piz app name
  				2. PizPlayerSettings *pSetting 	:	piz play settings
  Output:		1. 0   	succfull
  				2. -1  	no found node , please create node first , reference NEXUSAPP_PizPlayer_Create
  Return:
  other : 
*************************************************/
int NEXUSAPP_PizPlayer_Setting(av_playpump *avp ,PizPlayerSettings *pSetting)
{
	nexusLinkList pNode;
	int iFlag = 0;

	PARA_ASSERT_VALUE(avp,-1);
	PARA_ASSERT_VALUE(pSetting,-1);
	
	pNode = NEXUSAPP_LINK_Location_FindName(Head_pizplayer,pSetting->szAppName);	
	
	if(!pNode)
		goto error;
	
	pthread_mutex_lock(&avp->playerPizLock);
	if(pNode->data.start)
	{
		pNode->data.start = false;
		iFlag = 1;
	}
	pthread_mutex_unlock(&avp->playerPizLock);

	pNode->data.loop = pSetting->iLoop ;
	pNode->data.nowloop = pSetting->iLoop ;
	memcpy(&pNode->data.dest_rect , &pSetting->dest_rect , sizeof(NEXUS_Rect));

	if(iFlag)
	{
		pthread_mutex_lock(&avp->playerPizLock);
		pNode->data.start = true;
		pthread_mutex_unlock(&avp->playerPizLock);
	}
	
	return 0;

error:
	return -1;
	
}

/*************************************************
  Function:    	NEXUSAPP_PizPlayer_Start
  Description:	start piz play
  Input:			1. char *appName	piz app name
  Output:		1. 0   	succfull
  				2. -1  	no found node , please create node first , reference NEXUSAPP_PizPlayer_Create
  Return:
  other : 
*************************************************/
int NEXUSAPP_PizPlayer_Start(av_playpump *avp , char *pAppName)
{
	nexusLinkList pNode;

	PARA_ASSERT_VALUE(avp,-1);
	PARA_ASSERT_VALUE(pAppName,-1);
	
	pNode = NEXUSAPP_LINK_Location_FindName(Head_pizplayer,pAppName);	
	
	if(!pNode)
		goto error;

	pthread_mutex_lock(&avp->playerPizLock);
	pNode->data.index = 0;
	pNode->data.pyindex = 0;	
	pNode->data.nowloop = pNode->data.loop;
	pNode->data.start = true;
	pthread_mutex_unlock(&avp->playerPizLock);
	
	return 0;

error:
	return -1;
	
}

/*************************************************
  Function:    	NEXUSAPP_PizPlayer_Stop
  Description:	stop piz play
  Input:			1. char *pAppName		:piz app name
  Output:		1. 0   	succfull
  				2. -1  	no found node , please create node first , reference NEXUSAPP_PizPlayer_Create
  Return:
  other : 
*************************************************/
int NEXUSAPP_PizPlayer_Stop(av_playpump *avp , char *pAppName)
{
	nexusLinkList pNode;

	PARA_ASSERT_VALUE(avp,-1);
	PARA_ASSERT_VALUE(pAppName,-1);
	
	pNode = NEXUSAPP_LINK_Location_FindName(Head_pizplayer,pAppName);	

	if(!pNode)
		goto error;

	pthread_mutex_lock(&avp->playerPizLock);
	pNode->data.start = false ;
	pNode->data.index = 0;
	pNode->data.pyindex = 0;
	NEXUSAPP_Osd_Framebuff_Fill(avp,&pNode->data.dest_rect,0,0);
	pthread_mutex_unlock(&avp->playerPizLock);

	DBG_APP(("NEXUSAPP_PizPlayer_Stop\n"));
	
	return 0;

error:
	return -1;
	
}

int NEXUSAPP_PizPlayer_PizAppAdapte(struct av_playpump_t *avp , data_t *data)
{

	PARA_ASSERT_VALUE(avp,-1);
	PARA_ASSERT_VALUE(data,-1);

	if(data->index >= data->iCount)
	{
		data->index = 0;
		data->pyindex = 0;

		if(data->nowloop > 0 )
			data->nowloop--;
		
		if(data->nowloop == 0 )
		{
			data->start = false ;
			NEXUSAPP_Osd_Framebuff_Fill(avp,&data->dest_rect,0,0);	
			return 1;
		}
		
	}

	return 0;
}

/*************************************************
  Function:    	NEXUSAPP_PizPlayer_Pthread
  Description:	piz player link init 
  Input:		
  Output:		
  Return:
  other : 		
*************************************************/
int NEXUSAPP_PizPlayer_PizAppPlay(struct av_playpump_t *avp , data_t *data)
{
	NEXUS_Graphics2DBlitSettings blitSettings;

	PARA_ASSERT_VALUE(avp,-1);
	PARA_ASSERT_VALUE(data,-1);
	PARA_ASSERT_VALUE(data->fb.mem.buffer, -1);

	if(NEXUSAPP_PizPlayer_PizAppAdapte(avp,data))
		goto finish;

	if(NEXUSAPP_PizPlayer_PizFlagGet(data->flag, DATA_FLAG_MAX,data->index))
	{
		goto succ;
	}
	
	/* blit piz buffer  rect to output surface  */
	NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
	blitSettings.source.surface = data->fb.surface;
	blitSettings.source.rect.x = 0 ;
	blitSettings.source.rect.y = data->hy * data->pyindex; 
	blitSettings.source.rect.height= data->hy;
	blitSettings.source.rect.width = data->wx;

	blitSettings.output.surface = avp->framebuff[0].surface;
	blitSettings.output.rect.x = data->dest_rect.x;
	blitSettings.output.rect.y = data->dest_rect.y;
	blitSettings.output.rect.height = data->dest_rect.height;
	blitSettings.output.rect.width  = data->dest_rect.width;

	NEXUSAPP_Osd_Graphics2D_Blit(avp,&blitSettings);
	
	data->pyindex++;

succ:
	data->index ++ ;
	return 0;
finish:
	return 1;
	
}

/*************************************************
  Function:    	NEXUSAPP_PizPlayer_Pthread
  Description:	piz player link init 
  Input:		
  Output:		
  Return:
  other : 		
*************************************************/
static void *NEXUSAPP_PizPlayer_Pthread(void *context)
{
	av_playpump *avp = NULL;
	nexusLinkList	pizApp;
	
	PARA_ASSERT_VALUE(context,NULL);
	
	avp = (struct av_playpump_t*)context;

	DBG_APP(("NEXUSAPP_PizPlayer_Pthread: pthread start !\n"));
	while(1)
	{
		usleep(30000);

		pizApp = Head_pizplayer ;
		
		while(pizApp)
		{
			usleep(10000);
			pthread_mutex_lock(&avp->playerPizLock);

			if(!pizApp)
			{
				pthread_mutex_unlock(&avp->playerPizLock);
				break;
			}
			if(pizApp->data.start)
			{
				NEXUSAPP_PizPlayer_PizAppPlay(avp,&pizApp->data);
			}
			
			pthread_mutex_unlock(&avp->playerPizLock);

			pizApp = pizApp->next;
			
		}
	}
	
}

/*************************************************
  Function:    	NEXUSAPP_PizPlayer_Ready
  Description:	piz player link init 
  Input:		
  Output:		
  Return:
  other : 		
*************************************************/
void NEXUSAPP_PizPlayer_Ready(struct av_playpump_t *avp)
{
	pthread_attr_t attr;
	pthread_t thread_piz;

	PARA_ASSERT(avp);

	NEXUSAPP_LINK_SetNull_myLinkList(&Head_pizplayer);
	NEXUSAPP_LINK_Init_myLinkList(&Head_pizplayer);	

	pthread_attr_init (&attr);
	pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&thread_piz,&attr, NEXUSAPP_PizPlayer_Pthread,avp);
	pthread_attr_destroy (&attr);
	
}

#endif

