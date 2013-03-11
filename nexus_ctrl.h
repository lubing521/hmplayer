/**************************************************************************
2011_01_19 mody by hh
1.mody struct osd_ctrl_info_t for mw
***************************************************************************/
#ifndef NEXUS_CTRL_H
#define NEXUS_CTRL_H
/***************************************************************************/
#include "nexus_player.h"
#include "nexus_gpio.h"
#include "nexus_transport.h"
#include "nexus_ctrl.h"
/***************************************************************************/
#define CTRL_PIZ_HD_PATH "/mnt/ramdisk/hd.logo"
#define CTRL_PIZ_SD_PATH "/mnt/ramdisk/sd.logo"
#define ADDCONTEXT_LEN_MAX				4096
/***************************************************************************/
#define AUDIO_MUTE	1
#define AUDIO_UNMUTE	0
/***************************************************************************/
/* ctrl frambuffer display id */
#define FRAMEBUFF_HD 				1
#define FRAMEBUFF_SD 					2
#define FRAMEBUFF_MW 				3
/***************************************************************************/
/* ctrl frambuffer display id */
#define H264_FIFO_DEPTH_HD	16384
#define H264_FIFO_DEPTH_SD		10240
/***************************************************************************/
/* ctrl transport frame flag */
#define CTRL_FLAG_ID 					0x1
#define CTRL_ADDCONTEXT_ID 				0x2
#define CTRL_RESPOND_ID 				0x10
/* ctrl osd state */
#define CTRL_OSD_STATE_CHANGE 			1
#define CTRL_OSD_STATE_PLAY			 	2
#define CTRL_OSD_STATE_STOP		 		3
#define CTRL_OSD_STATE_COMPLETE		 	4
#define CTRL_OSD_STATE_BUSY				5
#define CTRL_OSD_STATE_ERROR			6
/***************************************************************************/
/*
*	ctrl display mode
*	A : HD video stream 
*	B : SD video stream
*	example:  
*		AB_A :  HD display : hd video + sd video(pip) 
*			     SD display :  hd video			
*/
typedef enum Display_Mode
{
	CTRL_DISPLAY_MODE_eA_A=1,		/*	AB_A 	*/	
	CTRL_DISPLAY_MODE_eA_B,    		/*	AB_B	*/	
	CTRL_DISPLAY_MODE_eB_A,			/*	BA_A	*/
	CTRL_DISPLAY_MODE_eB_B,			/*	BA_B	*/		
	CTRL_DISPLAY_MODE_eAB_A,		
	CTRL_DISPLAY_MODE_eAB_B,			
	CTRL_DISPLAY_MODE_eBA_A, 
	CTRL_DISPLAY_MODE_eBA_B,
	CTRL_DISPLAY_MODE_eA_A_SYNC,	/* A = B  SYNC , include osd */
	CTRL_DISPLAY_MODE_eMax
}Display_Mode;
/***************************************************************************/
typedef enum Ctrl_OsdState_item
{
	CTRL_STATE_ITEM_ePIZHD = 0,
	CTRL_STATE_ITEM_ePIZSD,		
	CTRL_STATE_ITEM_eTEXTSTATIC,
	CTRL_STATE_ITEM_eTEXTROLL,
	CTRL_STATE_ITEM_eMax
} Ctrl_OsdState_item;
/***************************************************************************/
typedef enum Ctrl_Osd_Compress_Type
{
	Ctrl_Osd_Compress_Type_eUnCompress = 0,
	Ctrl_Osd_Compress_Type_eZlibCompress,
	Ctrl_Osd_Compress_Type_eMax
}Ctrl_Osd_Compress_Type;
/***************************************************************************/
typedef struct osd_rect_t
{
	short x;
	short y;
	unsigned short width;
	unsigned short height;
}osd_rect;
/***************************************************************************/
typedef struct osd_ctrl_info_t
{
	int playerId;				/*  	HD/SD Stream Id  ����ID�궨��  */
	int cmdId ; 				/*  	����������º궨�� 	*/
	int value;					/*	����������������¶���,����и�����Ϣ*/
	int fbId;					/*	id = 1 HD framebuff , id = 2 SD framebuff	*/

	/* mody function 2011_01_19*/
	osd_rect position;	/*	PIP ��ʾλ��,MW Ĭ������,MW BLIT Դ����	*/
	/* add parameter for mw 2011_01_19*/
	osd_rect position_t;	/*	MW BLIT Ŀ������*/
}osd_ctrl_info;
/***************************************************************************/
struct osd_ctrl_t
{
	int flagId;					/*  	��Ϣ��ʶ���ݶ� 0x1 */
	osd_ctrl_info info;
};
/***************************************************************************/
struct osd_addContext_info_t
{
	int fbId;								/*	ID= 1 HD ����ʾ , id = 2 SD ����ʾ*/
	Ctrl_OsdState_item contextId;			/*	������Ϣ��������			*/
	Ctrl_Osd_Compress_Type contextType;	/*	������Ϣ����ѹ����ʽ	*/
	unsigned int iTotalCount;				/*	������Ϣ������			*/
	unsigned int iIndex;					/*	������Ϣ��ˮ��			*/
	unsigned int picWidth;					/*	ͼƬ���						*/
	unsigned int picHeight;					/*	ͼƬ����						*/
	unsigned int contextLen;				/*	��֡������Ϣ���ݳ���	*/		
	int displayCount;						/*	����ͼƬ��������,��̬ͼƬ������ʾģʽ	*/
	char context[ADDCONTEXT_LEN_MAX];		/*	������Ϣ����				*/
};
/***************************************************************************/
struct osd_addContext_t
{
	int flagId;						/*  	��Ϣ��ʶ���ݶ� 0x2 	*/
	struct osd_addContext_info_t info;
};
/***************************************************************************/
typedef struct osd_ctrl_respond_t
{
	int flagId;					/*  	0x10 : ������Ϣ��Ӧ��0x20:������Ϣ	*/
	int message;				/*	��Ϣ����:								*/
}osd_ctrl_respond;							/*	������Ϣ��Ӧʱ:	��������			*/
							/*	������Ϣ:			������Ϣ����	*/
/***************************************************************************/
/* ��Ӧ��Ϣ����*/
#define CTRL_OSD_MESSAGE_ERROR		0x10
#define CTRL_OSD_MESSAGE_INITIATIVE	0x20
/***************************************************************************/
/* ��Ӧ������Ϣ����*/
#define CTRL_OSD_ERROR_NOTHING		0
#define CTRL_OSD_ERROR_TRANSPORT		-1
#define CTRL_OSD_ERROR_MEMORY		-2
#define CTRL_OSD_ERROR_BUSY			-3
#define CTRL_OSD_ERROR_CMDERROR		-4
/***************************************************************************/
/* ��Ӧ������Ϣ����*/
#define CTRL_OSD_INITIATIVE_ROLLTEXT_COMPLETE		1
#define CTRL_OSD_INITIATIVE_PICFLASH_COMPLETE		2
/***************************************************************************/
void NEXUSAPP_Windows_Disconnect(NEXUS_VideoWindowHandle window ,NEXUS_VideoDecoderHandle videoDecoder);
void NEXUSAPP_Windows_Connect(NEXUS_VideoWindowHandle window ,NEXUS_VideoDecoderHandle videoDecoder);
int 	NEXUSAPP_Ctrl_PipInfo_Set( struct av_playpump_t *avp, osd_rect *position);
int 	NEXUSAPP_Ctrl_Windows_StateSet(struct av_playpump_t *avp,NEXUS_VideoWindowHandle window, bool visible);
int 	NEXUSAPP_Ctrl_Respond_Send(int iSockFd ,int messageType, int message);
int 	NEXUSAPP_Ctrl_Windows_Visible(NEXUS_VideoWindowHandle window, bool visible);
void NEXUSAPP_Windows_Alpha(NEXUS_VideoWindowHandle window , unsigned char alpha);
void NEXUSAPP_Ctrl_OsdState_Set(struct av_playpump_t *avp,Ctrl_OsdState_item osdItem,int value);
void NEXUSAPP_Ctrl_OsdState_Get(struct av_playpump_t *avp , Ctrl_OsdState_item osdItem, int *value);
int  	NEXUSAPP_Ctrl_VoiceVolume_Set(av_playpump *avp,int volume);
int 	NEXUSAPP_Ctrl_DisplayMode_Set(struct av_playpump_t *avp,int mode);
int  	NEXUSAPP_Ctrl_VoiceVolume_Silent( struct av_playpump_t *avp,int ifsilent);
int 	NEXUSAPP_Ctrl_Audio_Mode_Set(av_playpump *avp,av_player *player ,int value);
int 	NEXUSAPP_Ctrl_AudioDecoder_Set(av_playpump *avp,av_player *player ,int compressedAudio);
void NEXUSAPP_Ctrl_AudioDac_State_Set(av_playpump *avp , int mode );
void NEXUSAPP_Playpump_Windows_Disconnect(struct av_player_t *player);
void NEXUSAPP_Playpump_Windows_Connect(struct av_player_t *player );
int 	NEXUSAPP_Playpump_Static_Set(struct av_playpump_t *avp, int playpumpId,int state);
int 	NEXUSAPP_Playpump_Static_Get(struct av_player_t *player,int *state);
int 	NEXUSAPP_Playpump_State_Ctrl(av_playpump *avp,av_player *player);
void NEXUSAPP_Playpump_Depth_Ctrl_Get(struct av_player_t *player  , av_stream_info *streamInfo);
int 	NEXUSAPP_Playpump_StreamInfo_Get( struct av_player_t *player , av_stream_info *streamInfo );
void NEXUSAPP_Playpump_Ctrl( struct av_playpump_t *avp);

void NEXUSAPP_Ctrl_Init(socket_t *socket_info);
void NEXUSAPP_Ctrl_Start(ctrl_start_info *info);
void NEXUSAPP_Ctrl_Stop(struct av_playpump_t *avp);
int  	NEXUSAPP_Ctrl_Check(int iSockfd);

void TRACE_CONTENTS(int iLen,char * szStr);
/* gaoshengtu api , in libnexus.so */
/* enable spdif output 1 , disable spdif output 0 */
void OwnControlSpdif(int tInputOpenOrNot);


#endif

