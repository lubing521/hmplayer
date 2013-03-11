/**************************************************************************
2011_01_19 mody by hh
1.mody struct osd_ctrl_info_t for mw
2011_12_19 mody by hh
1.add pip info set  : PLAYER_CTRL_SEC_PIPINFO_SET
***************************************************************************/
#ifndef NEXUS_CTRL_SEC_H
#define NEXUS_CTRL_SEC_H
/***************************************************************************/
#include "nexus_player.h"
#include "nexus_ctrl.h"
/***************************************************************************/
#define CTRL_PIZ_PATH "/mnt/ramdisk/piz.tmp"

#define PLAYER_CTRL_SEC_PIZPLAYER_PLAY		1
#define PLAYER_CTRL_SEC_PIZPLAYER_STOP		2
#define PLAYER_CTRL_SEC_PIZPLAYER_APPEND	3
#define PLAYER_CTRL_SEC_PIZPLAYER_CLEAN		4
#define PLAYER_CTRL_SEC_PIZPLAYER_MODY		5
/***************************************************************************/
#define PLAYER_CTRL_SEC_PIPINFO_SET			10
/***************************************************************************/
#define PLAYER_CTRL_SEC_CONTEXT_PIZ			1
/***************************************************************************/
/*range between -32768 and 32767. The default is 0*/
/*NEXUS_PictureCtrlCommonSettings*/
#define CONTRAST 			21	/* 对比度*/
#define SATURATION 			22	/* 饱和度*/
#define HUE					23	/* 色彩、色相*/
#define BRIGHTNESS			24	/* 亮度*/
#define COLORTEMPENABLE	25	/* 色温开关*/
#define COLORTEMP			26	/* 色温*/
#define SHARNESSENABLE		27	/* 锐利度、清晰度开关*/
#define SARPNESS			28	/* 锐利度*/

/*NEXUS_PictureCtrlDitherSettings*/
#define REDUCESMOOTH		29	/* 数字抖动-减少平滑打开*/
#define SMOOTHENABLE		30	/* 数字抖动-平滑打开*/
#define SMOOTHLIMIT			31	/* 数字抖动-平滑限制*/

/*NEXUS_PictureCtrlCmsSettings*/
#define SATURATIONGAIN_R	32	/*饱和增益-红*/
#define SATURATIONGAIN_G	33	/*饱和增益-绿*/
#define SATURATIONGAIN_B	34	/*饱和增益-兰*/
#define SATURATIONGAIN_C	35	/*饱和增益-青色*/
#define SATURATIONGAIN_M	36	/*饱和增益-品红*/
#define SATURATIONGAIN_Y	37	/*饱和增益-黄*/

#define HUEGAIN_R			38	/*色调增益-红*/
#define HUEGAIN_G			39	/*色调增益-绿*/
#define HUEGAIN_B			40	/*色调增益-兰*/
#define HUEGAIN_C			41	/*色调增益-青色*/
#define HUEGAIN_M			42	/*色调增益-品红*/
#define HUEGAIN_Y			43	/*色调增益-黄*/

/*NEXUS_VideoWindowDnrSettings*/
/* mode range :  0 : disable , 1 : bypass , 2 :enable*/
/* level range : -100 ... 2^31 ,default = 0 */
#define WIN_DNR_MNR_MODE	44	/* 蚊式降噪模式, 0 : disable , 1 : bypass , 2 :enable */
#define WIN_DNR_MNR_LEVEL	45	/* 蚊式降噪等级, -100 ... 2^31 , default = 0 */ 

#define WIN_DNR_BNR_MODE	46	/* 模糊降低模式, 降低畫面上如馬賽克般的模糊現象*/
#define WIN_DNR_BNR_LEVEL	47

#define WIN_DNR_DCR_MODE	48	/* 数字轮廓模糊模式*/
#define WIN_DNR_DCR_LEVEL	49

#define WIN_NORLINEAR_SCALING	50	/* 非线性缩放*/
/***************************************************************************/
typedef struct ctrl_sec_info_t
{
	int iPlayerId;				/*  	HD/SD Stream Id  见下ID宏定义  */
	int iCmdId ; 				/*  	控制命令，见下宏定义 	*/
	int iValue;					/*	控制命令参数，见下定义,如果有附加消息*/
	int iFbId;					/*	id = 1 HD framebuff , id = 2 SD framebuff	*/
	char szValue[48];			/*  控制命令字符型参数*/
	osd_rect position;			/*  控制命令区域类参数*/
}ctrl_sec_info;

typedef struct ctrl_sec_t
{
	int iFlagId;					/*  	信息标识，暂定 0x1 */
	ctrl_sec_info info;
}ctrl_sec;

typedef struct ctrl_sec_addContext_info_t
{
	int iFbId;								/*	ID= 1 HD 屏显示 , id = 2 SD 屏显示*/
	int iContextId;							/*  附加信息类型*/
	int	iValue;								/*	附加信息参数*/
	char szValue[48];						/*	附加信息参数*/
	unsigned int uiTotalCount;				/*	附加信息总条数			*/
	unsigned int uiIndex;					/*	附加信息流水号			*/
	unsigned int uiContextLen;				/*	该帧附加信息内容长度*/	
	osd_rect position;						/*  控制命令区域类参数*/
	char szContext[ADDCONTEXT_LEN_MAX];		/*	附加消息内容				*/
}ctrl_sec_addContext_info;

typedef struct ctrl_sec_addContext_t
{
	int iFlagId;						/*  	信息标识，暂定 0x2 	*/
	ctrl_sec_addContext_info info;
}ctrl_sec_addContext;

/***************************************************************************/
void NEXUSAPP_Ctrl_Sec_Init(socket_t *socket_info);
void NEXUSAPP_Ctrl_Sec_Start(ctrl_start_info *s_info);
void NEXUSAPP_Ctrl_Sec_Stop(struct av_playpump_t *avp);
int NEXUSAPP_Ctrl_Window_Set(av_playpump *avp , int cmd ,int *value , int mode);

#endif

