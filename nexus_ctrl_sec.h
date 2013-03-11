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
#define CONTRAST 			21	/* �Աȶ�*/
#define SATURATION 			22	/* ���Ͷ�*/
#define HUE					23	/* ɫ�ʡ�ɫ��*/
#define BRIGHTNESS			24	/* ����*/
#define COLORTEMPENABLE	25	/* ɫ�¿���*/
#define COLORTEMP			26	/* ɫ��*/
#define SHARNESSENABLE		27	/* �����ȡ������ȿ���*/
#define SARPNESS			28	/* ������*/

/*NEXUS_PictureCtrlDitherSettings*/
#define REDUCESMOOTH		29	/* ���ֶ���-����ƽ����*/
#define SMOOTHENABLE		30	/* ���ֶ���-ƽ����*/
#define SMOOTHLIMIT			31	/* ���ֶ���-ƽ������*/

/*NEXUS_PictureCtrlCmsSettings*/
#define SATURATIONGAIN_R	32	/*��������-��*/
#define SATURATIONGAIN_G	33	/*��������-��*/
#define SATURATIONGAIN_B	34	/*��������-��*/
#define SATURATIONGAIN_C	35	/*��������-��ɫ*/
#define SATURATIONGAIN_M	36	/*��������-Ʒ��*/
#define SATURATIONGAIN_Y	37	/*��������-��*/

#define HUEGAIN_R			38	/*ɫ������-��*/
#define HUEGAIN_G			39	/*ɫ������-��*/
#define HUEGAIN_B			40	/*ɫ������-��*/
#define HUEGAIN_C			41	/*ɫ������-��ɫ*/
#define HUEGAIN_M			42	/*ɫ������-Ʒ��*/
#define HUEGAIN_Y			43	/*ɫ������-��*/

/*NEXUS_VideoWindowDnrSettings*/
/* mode range :  0 : disable , 1 : bypass , 2 :enable*/
/* level range : -100 ... 2^31 ,default = 0 */
#define WIN_DNR_MNR_MODE	44	/* ��ʽ����ģʽ, 0 : disable , 1 : bypass , 2 :enable */
#define WIN_DNR_MNR_LEVEL	45	/* ��ʽ����ȼ�, -100 ... 2^31 , default = 0 */ 

#define WIN_DNR_BNR_MODE	46	/* ģ������ģʽ, ���ͮ��������Rِ�˰��ģ���F��*/
#define WIN_DNR_BNR_LEVEL	47

#define WIN_DNR_DCR_MODE	48	/* ��������ģ��ģʽ*/
#define WIN_DNR_DCR_LEVEL	49

#define WIN_NORLINEAR_SCALING	50	/* ����������*/
/***************************************************************************/
typedef struct ctrl_sec_info_t
{
	int iPlayerId;				/*  	HD/SD Stream Id  ����ID�궨��  */
	int iCmdId ; 				/*  	����������º궨�� 	*/
	int iValue;					/*	����������������¶���,����и�����Ϣ*/
	int iFbId;					/*	id = 1 HD framebuff , id = 2 SD framebuff	*/
	char szValue[48];			/*  ���������ַ��Ͳ���*/
	osd_rect position;			/*  �����������������*/
}ctrl_sec_info;

typedef struct ctrl_sec_t
{
	int iFlagId;					/*  	��Ϣ��ʶ���ݶ� 0x1 */
	ctrl_sec_info info;
}ctrl_sec;

typedef struct ctrl_sec_addContext_info_t
{
	int iFbId;								/*	ID= 1 HD ����ʾ , id = 2 SD ����ʾ*/
	int iContextId;							/*  ������Ϣ����*/
	int	iValue;								/*	������Ϣ����*/
	char szValue[48];						/*	������Ϣ����*/
	unsigned int uiTotalCount;				/*	������Ϣ������			*/
	unsigned int uiIndex;					/*	������Ϣ��ˮ��			*/
	unsigned int uiContextLen;				/*	��֡������Ϣ���ݳ���*/	
	osd_rect position;						/*  �����������������*/
	char szContext[ADDCONTEXT_LEN_MAX];		/*	������Ϣ����				*/
}ctrl_sec_addContext_info;

typedef struct ctrl_sec_addContext_t
{
	int iFlagId;						/*  	��Ϣ��ʶ���ݶ� 0x2 	*/
	ctrl_sec_addContext_info info;
}ctrl_sec_addContext;

/***************************************************************************/
void NEXUSAPP_Ctrl_Sec_Init(socket_t *socket_info);
void NEXUSAPP_Ctrl_Sec_Start(ctrl_start_info *s_info);
void NEXUSAPP_Ctrl_Sec_Stop(struct av_playpump_t *avp);
int NEXUSAPP_Ctrl_Window_Set(av_playpump *avp , int cmd ,int *value , int mode);

#endif

