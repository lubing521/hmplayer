#ifndef HM_PLAYER_C
#define HM_PLAYER_C
/************************************************************************************/
#include <sys/ioctl.h>
#include <linux/hidraw.h>
#include "nexus_player.h"
#include "nexus_dplayer.h"
#include "nexus_link.h"
#include "nexus_ctrl.h"
#include "nexus_mw.h"
#include "nexus_ctrl_sec.h"
#include "nexus_osd.h"
#include "nexus_pizplay.h"
#include "nexus_component_output.h"
#include "nexusio_mftp_wd.h"
#include "nexusio_mftp_io.h"
#include "nexusio_file_io.h"
#include "nexus_audioplayer.h"
#include "nexus_platform.h"
#include "nexus_core_utils.h"
#include "nexus_video_decoder.h"
#include "nexus_video_decoder_trick.h"
#include "nexus_video_adj.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_display_vbi.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_decoder_trick.h"
#include "nexus_audio_output.h"
#include "nexus_audio_input.h"
#include "nexus_audio_playback.h"
#include "nexus_spdif_output.h"
#include "nexus_component_output.h"
#include "nexus_surface.h"
#include "nexus_playback.h"
#include "nexus_file.h"
#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif
#include "bstd.h"
#include "bkni.h"
#include "nexus_gpio.h"
#include "nexus_types.h"
#include "bchp_sun_top_ctrl.h"
#include "bchp_aon_pin_ctrl.h"
#include "cimov.h"
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include "hm_security_pub.h"
#include <sys/stat.h>
#include <semaphore.h>
#if 0
#include "sqlite3.h"
#endif
/************************************************************************************/

#define MAINPID	"/tmp/hplayMain.pid"
#define HPLAY_PID_SIGNED	57
/************************************************************************************/
#define TRANSPORT_UNIXSOCK	0
#define TRANSPORT_TCPSOCK	1
#define HDSD_TRANSPORT_TCP 1
#define USOCK_HD_NAME	"/tmp/hplayer_fifo"
#define USOCK_SD_NAME	"/tmp/splayer_fifo"
#define HMPLAYER_VERSION "HMPLAYER Build Time(" __DATE__ "," __TIME__ ")"
/************************************************************************************/
unsigned int uiVgaMode  = 0;
int nexus_debug_opt = 0;
/************************************************************************************/
socket_t lib_ctrl_SocketInfo;
socket_t lib_ctrl_sec_SocketInfo;
ctrl_start_info lib_ctrl_sec_info;
ctrl_start_info lib_ctrl_info;
av_pthread_context lib_hdThreadContext = {PLAYER_PLAYPUMP_HD, NULL};
av_pthread_context lib_sdThreadContext = {PLAYER_PLAYPUMP_SD, NULL};
sem_t *adddisk_semid, *rmdisk_semid, *song_searched_semid, *song_finish_semid;
int g_secure_pass_flag = -1;
int g_iVersion = -1;
extern char g_current_date[10];
static int g_song_in_search_flag = 0;
static int g_in_record_flag = 0;

#if 0
static sqlite3 *db;
static char **selectResult = 0;//存储查询到的信息
static int nResult = 0;		//行数
static int mResult = 0;		//列数
#endif

/************************************************************************************/
struct nexus_stream_tpye
{
	char name[16];
	unsigned int compressed;
	unsigned int transportType;
	unsigned int videoCodec;
	unsigned int audioCodec;
	unsigned int pid_video;
	unsigned int pid_audio_1;
	unsigned int pid_audio_2;
	unsigned int pid_audio_3;
	unsigned int pid_audio_4;	
};
/************************************************************************************/
#define STREAM_TYPE_MAX 34
struct nexus_stream_tpye stream_type[STREAM_TYPE_MAX] =
{
	{"T42F",0,NEXUS_TransportType_eTs ,NEXUS_VideoCodec_eH264,NEXUS_AudioCodec_eMpeg,0x1011,0x1100,0x1101,0x1102,0x1103},
	{"T42H",0,NEXUS_TransportType_eTs ,NEXUS_VideoCodec_eH264,NEXUS_AudioCodec_eMpeg,0x1011,0x1100,0x1101,0x1102,0x1103},

	{"T4AF",0,NEXUS_TransportType_eTs ,NEXUS_VideoCodec_eH264,NEXUS_AudioCodec_eAac,0x1011,0x1100,0x1101,0x1102,0x1103},
	{"T4AH",0,NEXUS_TransportType_eTs ,NEXUS_VideoCodec_eH264,NEXUS_AudioCodec_eAac,0x1011,0x1100,0x1101,0x1102,0x1103},

	{"T46F",1,NEXUS_TransportType_eTs ,NEXUS_VideoCodec_eH264,NEXUS_AudioCodec_eAc3,0x1011,0x1100,0x1101,0x1102,0x1103},
	{"T46H",1,NEXUS_TransportType_eTs ,NEXUS_VideoCodec_eH264,NEXUS_AudioCodec_eAc3,0x1011,0x1100,0x1101,0x1102,0x1103},

	{"T26F",1,NEXUS_TransportType_eTs ,NEXUS_VideoCodec_eMpeg2,NEXUS_AudioCodec_eAc3,0x1011,0x1100,0x1101,0x1102,0x1103},
	{"T26H",1,NEXUS_TransportType_eTs ,NEXUS_VideoCodec_eMpeg2,NEXUS_AudioCodec_eAc3,0x1011,0x1100,0x1101,0x1102,0x1103},

	{"M1X",0,NEXUS_TransportType_eMpeg1Ps ,NEXUS_VideoCodec_eMpeg1,NEXUS_AudioCodec_eMpeg,0xe0,0xc0,0xc1,0xc2,0xc3},
	{"M1S",0,NEXUS_TransportType_eMpeg1Ps ,NEXUS_VideoCodec_eMpeg1,NEXUS_AudioCodec_eMpeg,0xe0,0xc0,0xc1,0xc2,0xc3},

	{"M2X",0,NEXUS_TransportType_eMpeg2Pes ,NEXUS_VideoCodec_eMpeg2,NEXUS_AudioCodec_eMpeg,0xe0,0xc0,0xc1,0xc2,0xc3},
	{"M2P",0,NEXUS_TransportType_eMpeg2Pes ,NEXUS_VideoCodec_eMpeg2,NEXUS_AudioCodec_eMpeg,0xe0,0xc0,0xc1,0xc2,0xc3},
	
	{"VOX",0,NEXUS_TransportType_eVob ,NEXUS_VideoCodec_eMpeg2,NEXUS_AudioCodec_eAc3,0xe0,0x80BD,0x81BD,0x82BD,0x83BD},
	{"VOB",0,NEXUS_TransportType_eVob ,NEXUS_VideoCodec_eMpeg2,NEXUS_AudioCodec_eAc3,0xe0,0x80BD,0x81BD,0x82BD,0x83BD},

	{"H4X",0,NEXUS_TransportType_eMpeg2Pes ,NEXUS_VideoCodec_eH264,NEXUS_AudioCodec_eMpeg,0xe0,0xc0,0xc1,0xc2,0xc3},
	{"H4P",0,NEXUS_TransportType_eMpeg2Pes ,NEXUS_VideoCodec_eH264,NEXUS_AudioCodec_eMpeg,0xe0,0xc0,0xc1,0xc2,0xc3},

	{"P26F",1,NEXUS_TransportType_eVob ,NEXUS_VideoCodec_eH264,NEXUS_AudioCodec_eAc3,0xe0,0x80BD,0x81BD,0x82BD,0x83BD},
	{"P26H",1,NEXUS_TransportType_eVob ,NEXUS_VideoCodec_eH264,NEXUS_AudioCodec_eAc3,0xe0,0x80BD,0x81BD,0x82BD,0x83BD},

	{"P4DH",	1,NEXUS_TransportType_eMpeg2Pes ,NEXUS_VideoCodec_eH264,NEXUS_AudioCodec_eDts,0xe0,0xc0,0xc1,0xc2,0xc3},
	{"P4DF",	1,NEXUS_TransportType_eMpeg2Pes ,NEXUS_VideoCodec_eH264,NEXUS_AudioCodec_eDts,0xe0,0xc0,0xc1,0xc2,0xc3},

	{"P42H",	0,NEXUS_TransportType_eMpeg2Pes ,NEXUS_VideoCodec_eH264,NEXUS_AudioCodec_eMpeg,0xe0,0xc0,0xc1,0xc2,0xc3},
	{"P42F",	0,NEXUS_TransportType_eMpeg2Pes ,NEXUS_VideoCodec_eH264,NEXUS_AudioCodec_eMpeg,0xe0,0xc0,0xc1,0xc2,0xc3},	

	{"P46H",	1,NEXUS_TransportType_eVob ,NEXUS_VideoCodec_eH264,NEXUS_AudioCodec_eAc3,0xe0,0x80BD,0x81BD,0x82BD,0x83BD},
	{"P46F",	1,NEXUS_TransportType_eVob ,NEXUS_VideoCodec_eH264,NEXUS_AudioCodec_eAc3,0xe0,0x80BD,0x81BD,0x82BD,0x83BD},	

	{"P4AH",	0,NEXUS_TransportType_eMpeg2Pes,NEXUS_VideoCodec_eH264,NEXUS_AudioCodec_eAacPlusAdts,0xe0,0xc0,0xc1,0xc2,0xc3},
	{"P4AF",	0,NEXUS_TransportType_eMpeg2Pes,NEXUS_VideoCodec_eH264,NEXUS_AudioCodec_eAacPlusAdts,0xe0,0xc0,0xc1,0xc2,0xc3},

	{"K42H",	0,NEXUS_TransportType_eMkv ,NEXUS_VideoCodec_eH264,NEXUS_AudioCodec_eAac,0x01,0x02,0x03,0x04,0x05},
	{"K42F",	0,NEXUS_TransportType_eMkv ,NEXUS_VideoCodec_eH264,NEXUS_AudioCodec_eAac,0x01,0x02,0x03,0x04,0x05},	
	
	{"K46H",	1,NEXUS_TransportType_eMkv ,NEXUS_VideoCodec_eH264,NEXUS_AudioCodec_eAc3,0x01,0x02,0x03,0x04,0x05},
	{"K46F",	1,NEXUS_TransportType_eMkv ,NEXUS_VideoCodec_eH264,NEXUS_AudioCodec_eAc3,0x01,0x02,0x03,0x04,0x05},

	{"mpg",	0,NEXUS_TransportType_eMpeg1Ps ,NEXUS_VideoCodec_eMpeg1,NEXUS_AudioCodec_eMpeg,0xe0,0xc0,0xc1,0xc2,0xc3},
	{"MPG",	0,NEXUS_TransportType_eMpeg1Ps ,NEXUS_VideoCodec_eMpeg1,NEXUS_AudioCodec_eMpeg,0xe0,0xc0,0xc1,0xc2,0xc3},
	{"MP4F",	0,NEXUS_TransportType_eMp4 ,NEXUS_VideoCodec_eH264,NEXUS_AudioCodec_eAac,0x01,0x02,0x03,0x04,0x05},
	{"MP4H",	0,NEXUS_TransportType_eMp4 ,NEXUS_VideoCodec_eH264,NEXUS_AudioCodec_eAac,0x01,0x02,0x03,0x04,0x05},
};

extern void NEXUS_Platform_P_SetWakeupDevices(const NEXUS_PlatformStandbySettings *pSettings);
/************************************************************************************/
char * NEXUSAPP_GetVersion(void)
{
	return HMPLAYER_VERSION;
}

int NEXUSAPP_PreRecord(void)
{
	g_in_record_flag = 1;

	if(system("umnt=$(cat /proc/mounts|awk '{print $2}'|grep /mnt/udisk);[ ! -z $umnt ] && umount /mnt/udisk -l;"))
	{
		printf("[%s]umount fail\n", __FUNCTION__);
	}

	sleep(1);

	if(system("mod=$(lsmod|awk '{print $1}'|grep ehci_brcm);[ ! -z $mod ] && rmmod ehci_brcm"))
	{
		printf("[%s]rmmod ehci_brcm fail\n", __FUNCTION__);
	}

	return 0;
}

int NEXUSAPP_PostRecord(void)
{
	if(system("umnt=$(cat /proc/mounts|awk '{print $2}'|grep /mnt/udisk);[ ! -z $umnt ] && umount /mnt/udisk -l;"))
	{
		printf("[%s]umount fail\n", __FUNCTION__);
	}

	sleep(1);
	
	if(system("mod=$(lsmod|awk '{print $1}'|grep ehci_brcm);[ -z $mod ] && insmod /lib/modules/ehci-brcm.ko"))
	{
		printf("[%s]insmod ehci_brcm fail\n", __FUNCTION__);
	}

	g_in_record_flag = 0;
	
	return 0;
}
int NEXUSAPP_Standby(void)
{
	NEXUS_PlatformStandbySettings tTempNexusStandbySettings;

	NEXUS_Platform_GetStandbySettings(&tTempNexusStandbySettings);
	tTempNexusStandbySettings.wakeupSettings.gpio = true;
	NEXUS_Platform_P_SetWakeupDevices(&tTempNexusStandbySettings);
				
	DBG_APP(("StandBy...\n"));
	system("echo 1 > /sys/devices/platform/brcmstb/halt_mode");
	system("halt");

	return 0;
}

/* 
FUNC:int NEXUSAPP_DisplayMode_Set
Input: mode, 1 A_A, 2 A_B
Return Value: 0 succ
*/
int NEXUSAPP_DisplayMode_Set(int mode)
{
	struct av_playpump_t *avp;
	int ret = 0;

	avp = lib_ctrl_info.avp;

	ret = NEXUSAPP_Ctrl_DisplayMode_Set(avp, mode);

	return ret;
}


int NEXUSAPP_Find_Stream_Info(char *filename)
{
	struct common_opts_t common;

	memset(&common, 0, sizeof(common));
	common.transportType = NEXUS_TransportType_eTs;
	common.extVideoCodec = NEXUS_VideoCodec_eNone;
	common.videoCodec = NEXUS_VideoCodec_eMpeg2;
	common.audioCodec = NEXUS_AudioCodec_eMpeg;
	common.contentMode = NEXUS_VideoWindowContentMode_eFull;
	common.compressedAudio = false;
	common.multichannelAudio = false;
	common.detectAvcExtension = false;
	common.decodedAudio = true;
	common.playpumpTimestampReordering = true;
	common.videoDecoder = 0;
	common.tsTimestampType = NEXUS_TransportTimestampType_eNone;
	common.videoFrameRate = NEXUS_VideoFrameRate_eUnknown;
	common.displayOrientation = NEXUS_VideoOrientation_e2D;
	common.sourceOrientation = NEXUS_VideoDecoderSourceOrientation_e2D;
	common.pcm = false;
	common.useCompositeOutput = false;
	common.useComponentOutput = true;
	common.useHdmiOutput = true;
	common.displayFormat = NEXUS_VideoFormat_eNtsc;
	common.displayType = NEXUS_DisplayType_eAuto;
	common.probe = true;

	memset(&common.pcm_config, 0, sizeof(common.pcm_config));
	common.pcm_config.sample_size = 16;
	common.pcm_config.sample_rate = 44100;
	common.pcm_config.channel_count = 2;

	if (cmdline_probe(&common, filename, NULL))
	{
		DBG_APP(("!!!!!!!!!!!file:%s probe failed\n", filename));
		return -1;
	}
	
	if (common.transportType == NEXUS_TransportType_eMkv || common.transportType == NEXUS_TransportType_eFlv)
	{
		DBG_APP(("MKV and FLV format currently not support\n"));
		return -1;
	}
	
	if (common.audio2Pid)
		return 2;
	if (common.audioPid)
		return 1;

	return 0;
	
}


int NEXUSAPP_Find_Outside_SongDisk(void)
{	
	int song_semval, adddisk_semval;

	if(!(sem_getvalue(song_searched_semid, &song_semval)) && !(sem_getvalue(adddisk_semid, &adddisk_semval)))
	{
		if(1 == song_semval && 0 == adddisk_semval) /*add disk && song not searched*/
		{
			return 1;
		}

		if(0 == song_semval && 1 == adddisk_semval) /*added disk have been searched*/
		{
			return 2;
		}
	}	

	return 0;
}

int NEXUSAPP_Search_Songs(void)
{	
	g_song_in_search_flag = 1;
	
	if (system("killall -s KILL qtabd"))
		printf("!!!!!kill qtabd failed\n");
	
	if (system("killall -s KILL mftpd"))
		printf("!!!!!kill mftpd failed\n");

	if (system("mftpd -w"))
		printf("!!!!!restart mftpd failed\n");

	sleep(2);

	if (system("qtabd -s -d &"))
		printf("!!!!!restart qtabd failed\n");

	sem_wait(song_finish_semid);

	g_song_in_search_flag = 0;

	sleep(1);
	
	return 0;
}

int NEXUSAPP_Check_Song_Disk_Detach(void)
{
	int song_semval, rmdisk_semval, adddisk_semval;

	if(!(sem_getvalue(adddisk_semid, &adddisk_semval)) &&!(sem_getvalue(song_searched_semid, &song_semval)) && !(sem_getvalue(rmdisk_semid, &rmdisk_semval)))
	{
		/*printf("[%s-%d]adddisk_semval:%d, song_semval:%d, rmdisk_semval:%d\n", __FUNCTION__, __LINE__, adddisk_semval, song_semval, rmdisk_semval);*/
		
		if(0 == song_semval && 0 == rmdisk_semval && !g_in_record_flag) /*song searched && remove disk*/
		{
			if (!g_song_in_search_flag)
			{
				return 1;
			}
		}
	}

	return 0;
}


/*************************************************
  Function:    	my_trace
  Description:	trace function , send debug message to file.
  Input:		1 . char *buf 		: debug message buffer
  			2 . int iLen 		: message size
  			3 . char *filename 	: log file full path
  Output:		
  Return:
  other : 		
*************************************************/
void my_trace(char *buf, int iLen ,char *filename)
{
	FILE *fp=NULL;

	if(iLen<=0)
		return;

	fp = fopen(filename,"ab+");
	if(fp==NULL)
	{
		DBG_APP(("open file error!\n"));
		return ;
	}

	fwrite(buf,1,iLen,fp);
	fclose(fp);
	
}
/*************************************************
  Function:    	NEXUSAPP_Signal_Exit
  Description:	pthread exit with signal
  Input:		
  Output:		
  Return:            	
*************************************************/
void NEXUSAPP_Signal_Exit(int sig)
{
	int retval = 100;
	sig = 0;
	DBG_APP(("get signal exit ! thread exit now !\n"));
	pthread_exit(&retval);
}

void NEXUSAPP_Effect_Set(int mode)
{
	av_playpump *avp = lib_hdThreadContext.avp ;

	if(avp == NULL)
		return;

	pthread_mutex_lock(&avp->playerLock);		
	avp->audioSilentCtrl = mode ;
	pthread_mutex_unlock(&avp->playerLock);
}

/*************************************************
  Function:    	NEXUSAPP_Effect_Check
  Description:	stc channel init 
  Input:		1 . struct av_playpump_t *avp	:  playpump info struct 
  			2 . int playpumpId				:  playpump id  (PLAYER_PLAYPUMP_HD or PLAYER_PLAYPUMP_SD)
  Output:		
  Return:            
*************************************************/
void NEXUSAPP_Effect_Check(av_playpump *avp)
{
	char dev_name[64];
	int dev_number;
	int dev_handle;
	struct hidraw_devinfo info;

	for(dev_number = 0; dev_number < 8; dev_number ++)
	{
		memset(dev_name,0x00,sizeof(dev_name));
		
		sprintf(dev_name, "/dev/hidraw%d", dev_number);

		if((dev_handle = open(dev_name, O_RDWR)) >= 0)
		{
			if(ioctl(dev_handle, HIDIOCGRAWINFO, &info) == 0)
			{
				if(info.vendor == 0x2010 && info.product == 0x0117)
				{
					avp->audioSilentCtrl = 1;

					return ;
				}
			}
		}
		
		close(dev_handle);
	}
	
	avp->audioSilentCtrl = 0;
	
}
/*************************************************
  Function:    	NEXUSAPP_StcChannel_Init
  Description:	stc channel init 
  Input:		1 . struct av_playpump_t *avp	:  playpump info struct 
  			2 . int playpumpId				:  playpump id  (PLAYER_PLAYPUMP_HD or PLAYER_PLAYPUMP_SD)
  Output:		
  Return:            
*************************************************/
void NEXUSAPP_StcChannel_Init(av_playpump *avp,int playpumpId)
{

	NEXUS_StcChannelSettings stcSettings,stcSettings1;

	if(playpumpId == PLAYER_PLAYPUMP_HD)
	{
		MSG_APP(("hd stc create  !\n"));
		NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
		stcSettings.timebase = NEXUS_Timebase_e0;		
		stcSettings.mode = NEXUS_StcChannelMode_eAuto;
		avp->hdPlayer.stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);
	}
	else
	{		
		MSG_APP(("sd stc create  !\n"));
		NEXUS_StcChannel_GetDefaultSettings(1, &stcSettings1);
		stcSettings1.timebase = NEXUS_Timebase_e1;
		stcSettings1.mode = NEXUS_StcChannelMode_eAuto;
		avp->sdPlayer.stcChannel = NEXUS_StcChannel_Open(1, &stcSettings1);
	}

	/* add audio player stc by hh 2012_04_17*/
	NEXUS_StcChannel_GetDefaultSettings(2, &stcSettings);
	stcSettings.timebase = NEXUS_Timebase_e2;
	stcSettings.mode = NEXUS_StcChannelMode_eAuto;
	stcSettings.modeSettings.Auto.transportType =NEXUS_TransportType_eEs;
	stcSettings.modeSettings.Auto.behavior = NEXUS_StcChannelAutoModeBehavior_eAudioMaster;
	avp->audioplayer.stcChannel = NEXUS_StcChannel_Open(2, &stcSettings);
	
}
/*************************************************
  Function:    	NEXUSAPP_Decoder_Pid_Init
  Description:	decoder pid init , pid is media stream id , defaut 0xe0 is video stream,0xc0 is audio stream
  Input:		1 . struct av_playpump_t *avp	:  playpump info struct 
  			2 . int playpumpId				:  playpump id  (PLAYER_PLAYPUMP_HD or PLAYER_PLAYPUMP_SD)
  Output:		
  Return:            1.  0 : succeed 
*************************************************/
int NEXUSAPP_Decoder_Pid_Init(av_playpump *avp , int playpumpId)
{	
	int i;
	av_player *player;
	NEXUS_PlaypumpOpenPidChannelSettings settings;
	NEXUS_Playpump_GetDefaultOpenPidChannelSettings(&settings);

	if(avp == NULL )
		return -1;
	
	if(playpumpId == PLAYER_PLAYPUMP_HD)
		player = &avp->hdPlayer;
	else
		player = &avp->sdPlayer;
		

	settings.pidType = NEXUS_PidType_eVideo;
	player->videoPidChannel.pidChannel = NEXUS_Playpump_OpenPidChannel(player->playpump,player->videoPidChannel.pid, &settings);
	settings.pidType = NEXUS_PidType_eAudio;
	settings.pidTypeSettings.audio.codec = player->audioProgram.codec;

	for(i = 0 ;i < 5 ; i++)
	{
		if(player->audioPidChannel[i].pid)
			player->audioPidChannel[i].pidChannel = 
				NEXUS_Playpump_OpenPidChannel(player->playpump, player->audioPidChannel[i].pid, &settings);
	}
	
	return 0;
}
/*************************************************
  Function:    	NEXUSAPP_Decoder_Pid_Close
  Description:	decoder pid init , pid is media stream id , defaut 0xe0 is video stream,0xc0 is audio stream
  Input:		1 . av_player *player	:  playpump player 
  			2 . mode				:  pid mode  (AUDIO  or VIDEO)  0:video 1:audio
  Output:		
  Return:            1.  0 : succeed 
*************************************************/
int NEXUSAPP_Decoder_Pid_Close(av_player *player , int mode )
{
	int i;

	if(player == NULL)
		return -1;
	
	if(mode == 0)
	{
		NEXUS_PidChannel_Close(player->videoPidChannel.pidChannel);
		player->videoPidChannel.pid = 0;
		player->videoPidChannel.pidChannel = 0;
	}
	else
	{
		for(i=0;i<5;i++)
		{
			DBG_APP(("NEXUSAPP_Decoder_Pid_Close : %d = %d !\n",i,player->audioPidChannel[i].pid));
			
			if(player->audioPidChannel[i].pid)
			{
				NEXUS_PidChannel_Close(player->audioPidChannel[i].pidChannel);
				player->audioPidChannel[i].pid = 0;
				player->audioPidChannel[i].pidChannel = 0;
			}
		}
	}

	return 0;

}
/*************************************************
  Function:    	NEXUSAPP_VideoDecoder_Programe_Init
  Description:	video decoder programe init , it will be used by video decoder start
  Input:		1 . struct av_playpump_t *avp	:  playpump info struct 
  			2 . int playpumpId				:  playpump id  (PLAYER_PLAYPUMP_HD or PLAYER_PLAYPUMP_SD)
  Output:		
  Return:            1.  0 : succeed           
*************************************************/
int NEXUSAPP_VideoDecoder_Programe_Init(av_playpump *avp , int playpumpId)
{
	av_player *player = NULL;
	NEXUS_VideoDecoderSettings videoDecoderSettings;

	if(avp == NULL)
		return -1;

	if(playpumpId == PLAYER_PLAYPUMP_HD)
		player = &avp->hdPlayer;
	else
		player = &avp->sdPlayer;

	if(player == NULL)
		return -1;
	
	player->videoProgram.pidChannel = player->videoPidChannel.pidChannel;
	player->videoProgram.stcChannel = player->stcChannel;

	NEXUS_VideoDecoder_GetSettings(player->videoDecoder , &videoDecoderSettings);
	videoDecoderSettings.customSourceOrientation = true;
	switch(player->uiDisplay3DMode)
	{
		case SURFACE_OSD_MODE_2D:
			videoDecoderSettings.sourceOrientation = NEXUS_VideoDecoderSourceOrientation_e2D;
			break;
		case SURFACE_OSD_MODE_3DRL:
			videoDecoderSettings.sourceOrientation = NEXUS_VideoDecoderSourceOrientation_e3D_LeftRightEnhancedResolution;
			break;
		default:
			videoDecoderSettings.sourceOrientation = NEXUS_VideoDecoderSourceOrientation_e2D;
			break;
	}

	 NEXUS_VideoDecoder_SetSettings(player->videoDecoder, &videoDecoderSettings);

	return 0;
}

/*************************************************
  Function:    	NEXUSAPP_VideoDecoder_Open
  Description:	NEXUS_VideoDecoder_Open
  Input:		1 . struct av_playpump_t *avp	:  playpump info struct 
  			2 . int playpumpId				:  playpump id  (PLAYER_PLAYPUMP_HD or PLAYER_PLAYPUMP_SD)
  Output:		
  Return:            1.  0 : succeed              
  other:		hd video decoder open with index : 0 , sd video decoder open with index 1 
*************************************************/
int NEXUSAPP_VideoDecoder_Open(av_playpump *avp,int playpumpId)
{
	NEXUS_VideoDecoderOpenSettings pOpenSettings;
	NEXUS_VideoDecoder_GetDefaultOpenSettings(&pOpenSettings);

	if(avp == NULL)
		return -1;
	
	if(playpumpId == PLAYER_PLAYPUMP_HD)
		avp->hdPlayer.videoDecoder = NEXUS_VideoDecoder_Open(0, &pOpenSettings); 
	else
		avp->sdPlayer.videoDecoder = NEXUS_VideoDecoder_Open(1, &pOpenSettings); 

	return 0;
}

/*************************************************
  Function:    	NEXUSAPP_AudioDecoder_Programe_Init
  Description:	Audio Decoder programe init ,it will be used by audio decoder start
  Input:		1 . struct av_playpump_t *avp	:  playpump info struct 
  			2 . int playpumpId				:  playpump id  (PLAYER_PLAYPUMP_HD or PLAYER_PLAYPUMP_SD)
  Output:		
  Return:            1.  0 : succeed             
*************************************************/
int NEXUSAPP_AudioDecoder_Programe_Init(av_playpump *avp , int playpumpId)
{

	av_player *player;

	if(avp == NULL)
		return -1;

	if(playpumpId == PLAYER_PLAYPUMP_HD)
		player = &avp->hdPlayer;	
	else
		player = &avp->sdPlayer;

	/* must be set*/
	player->audioProgram.pidChannel = player->audioPidChannel[0].pidChannel;
	player->audioProgram.stcChannel = player->stcChannel;

	if (player->audioPidChannel[1].pidChannel)
	{
		player->audio2Program.pidChannel = player->audioPidChannel[1].pidChannel;
		player->audio2Program.stcChannel = player->stcChannel;
	}

	return 0;
}
/*************************************************
  Function:    	NEXUSAPP_AudioDecoder_Init
  Description:	audio decoder init , audio decoder open and connect to dac ,spdif .
  Input:		1 . struct av_playpump_t *avp	:  playpump info struct 
  Output:		
  Return:            1.  0 : succeed    
*************************************************/
int NEXUSAPP_AudioDecoder_Init( av_playpump *avp)
{
	NEXUS_AudioOutputSettings audioOutputSettings;
	NEXUS_AudioDecoderOpenSettings audioDecoderOpenSettings;
	NEXUS_AudioMixerSettings mixerSettings;

	MSG_APP(("audio   init and start !\n"));
	NEXUS_AudioDecoder_GetDefaultOpenSettings(&audioDecoderOpenSettings);
	audioDecoderOpenSettings.multichannelFormat = NEXUS_AudioMultichannelFormat_e5_1;

	/* add audio player audiodecoder by hh 2012_04_17*/
	avp->audioDecoder= NEXUS_AudioDecoder_Open(0, NULL);
	
	/*lidj:added 20120915*/
	avp->audio2Decoder = NEXUS_AudioDecoder_Open(1,NULL);
	
	avp->audioplayer.audioDecoder = NEXUS_AudioDecoder_Open(2, NULL);

	/* add mixer out put by hh 2012_04_17*/
	NEXUS_AudioMixer_GetDefaultSettings(&mixerSettings);
	avp->mixer = NEXUS_AudioMixer_Open(&mixerSettings);

	/* Add both decoders to the mixer */
	NEXUS_AudioMixer_AddInput(avp->mixer, 
		NEXUS_AudioDecoder_GetConnector(avp->audioDecoder, 
		NEXUS_AudioDecoderConnectorType_eStereo));

	NEXUS_AudioMixer_AddInput(avp->mixer,
		NEXUS_AudioDecoder_GetConnector(avp->audio2Decoder,
		NEXUS_AudioDecoderConnectorType_eStereo));
	
	NEXUS_AudioMixer_AddInput(avp->mixer, 
		NEXUS_AudioDecoder_GetConnector(avp->audioplayer.audioDecoder, 
		NEXUS_AudioDecoderConnectorType_eStereo));

	/* Add DAC to the mixer output */
	NEXUS_AudioOutput_AddInput(
	    NEXUS_AudioDac_GetConnector(avp->platformConfig.outputs.audioDacs[0]),
	    NEXUS_AudioMixer_GetConnector(avp->mixer));
	

	/* Attach SPDIF to the mixed output */
	NEXUS_AudioOutput_AddInput(
	    NEXUS_SpdifOutput_GetConnector(avp->platformConfig.outputs.spdif[0]),
	    NEXUS_AudioMixer_GetConnector(avp->mixer));

	/* Attach HDMI to the mixed output */
	NEXUS_AudioOutput_AddInput(
		NEXUS_HdmiOutput_GetAudioConnector(avp->platformConfig.outputs.hdmi[0]),
		NEXUS_AudioMixer_GetConnector(avp->mixer));

	NEXUS_AudioMixer_Start(avp->mixer);

	NEXUS_AudioOutput_GetSettings(
		NEXUS_AudioDac_GetConnector(avp->platformConfig.outputs.audioDacs[0]),
		&audioOutputSettings);
	
	audioOutputSettings.volumeType = NEXUS_AudioVolumeType_eDecibel;
	audioOutputSettings.leftVolume = NEXUS_AUDIO_VOLUME_DB_MAX;
	audioOutputSettings.rightVolume = NEXUS_AUDIO_VOLUME_DB_MAX;
	audioOutputSettings.defaultSampleRate = 48000;
	
	NEXUS_AudioOutput_SetSettings(
		NEXUS_AudioDac_GetConnector(avp->platformConfig.outputs.audioDacs[0]),
		&audioOutputSettings);
	
/*	sstest */
	NEXUS_AudioOutput_SetSettings(
		NEXUS_SpdifOutput_GetConnector(avp->platformConfig.outputs.spdif[0]),
		&audioOutputSettings);

	return 0;
}

void NEXUSAPP_Hdmi_mode_set(av_playpump *avp , int mode )
{
	NEXUS_HdmiOutputVendorSpecificInfoFrame vsi;
	NEXUS_HdmiOutput_GetVendorSpecificInfoFrame(avp->platformConfig.outputs.hdmi[0], &vsi);

	NEXUS_Display_RemoveOutput(avp->display.display_hd, NEXUS_HdmiOutput_GetVideoConnector(avp->platformConfig.outputs.hdmi[0]));
	NEXUS_Display_AddOutput(avp->display.display_hd, NEXUS_HdmiOutput_GetVideoConnector(avp->platformConfig.outputs.hdmi[0]));
	switch(mode)
	{
		case 1:
			vsi.hdmiVideoFormat = NEXUS_HdmiVendorSpecificInfoFrame_HDMIVideoFormat_eNone;
			break;
		case 2:
			vsi.hdmiVideoFormat = NEXUS_HdmiVendorSpecificInfoFrame_HDMIVideoFormat_e3DFormat;
			vsi.hdmi3DStructure = NEXUS_HdmiVendorSpecificInfoFrame_3DStructure_eSidexSideHalf;
			break;
		case 3:
			vsi.hdmiVideoFormat = NEXUS_HdmiVendorSpecificInfoFrame_HDMIVideoFormat_e3DFormat;
			vsi.hdmi3DStructure = NEXUS_HdmiVendorSpecificInfoFrame_3DStructure_eTopAndBottom;
			break;
		default:
			vsi.hdmiVideoFormat = NEXUS_HdmiVendorSpecificInfoFrame_HDMIVideoFormat_eNone;
			break;
	}
	
	NEXUS_HdmiOutput_SetVendorSpecificInfoFrame(avp->platformConfig.outputs.hdmi[0], &vsi);
}

static void NEXUSAPP_Hdmihotplug_Reset(av_playpump *avp);
static void NEXUSAPP_Hdmihotplug_callback(void *pParam, int iParam)
{
	NEXUS_HdmiOutputHandle hdmi;
	NEXUS_DisplayHandle display;
	NEXUS_DisplaySettings displaySettings;
	NEXUS_HdmiOutputStatus status;
	NEXUS_Error rc;
	av_playpump *avp = (av_playpump *)pParam;

	PARA_ASSERT(avp);
	PARA_ASSERT(avp->display.display_hd);
	PARA_ASSERT(avp->platformConfig.outputs.hdmi[0]);

	hdmi = avp->platformConfig.outputs.hdmi[0];
	display = (NEXUS_DisplayHandle)iParam;

	rc = NEXUS_HdmiOutput_GetStatus(hdmi, &status);

	/* the app can choose to switch to the preferred format, but it's not required. */
	if (!rc &&  status.connected )
	{
		NEXUS_Display_GetSettings(display, &displaySettings);
		if ( !status.videoFormatSupported[displaySettings.format] )
		{
			#if 0
			status.preferredVideoFormat =NEXUS_VideoFormat_e1080i;
			
			displaySettings.format = status.preferredVideoFormat;
			
			NEXUS_Display_SetSettings(display, &displaySettings);

			#endif
		}
	}
    
}

static void NEXUSAPP_Hdmihotplug_Reset(av_playpump *avp)
{
	NEXUS_HdmiOutputSettings hdmiSettings;
	NEXUS_HdmiOutputHandle hdmi;
	
	PARA_ASSERT(avp);

	hdmi = avp->platformConfig.outputs.hdmi[0];
	
	if (avp->display.display_hd)
	{	
		NEXUS_Display_RemoveOutput(avp->display.display_hd,NEXUS_HdmiOutput_GetVideoConnector(hdmi));
		NEXUS_Display_AddOutput(avp->display.display_hd, NEXUS_HdmiOutput_GetVideoConnector(hdmi));

		NEXUS_HdmiOutput_GetSettings(avp->platformConfig.outputs.hdmi[0], &hdmiSettings);
		hdmiSettings.hotplugCallback.callback = NEXUSAPP_Hdmihotplug_callback;
		hdmiSettings.hotplugCallback.context = avp;
		hdmiSettings.hotplugCallback.param = (int)avp->display.display_hd;
		NEXUS_HdmiOutput_SetSettings(avp->platformConfig.outputs.hdmi[0], &hdmiSettings);		
	}
}

/*************************************************
  Function:    	NEXUSAPP_Display_Init
  Description:	HD/SD display init , set HD/SD display format , and connect to composite and hdmi output. 
  Input:			1 . struct av_playpump_t *avp	:  playpump info struct
    				2 .  int playpumpId 				:  playpump id  (PLAYER_PLAYPUMP_HD or PLAYER_PLAYPUMP_SD)
  Output:		
  Return:            1.  0 : succeed    
*************************************************/
int NEXUSAPP_Display_Init(av_playpump *avp ,  int playpumpId)
{

	if( playpumpId  == PLAYER_PLAYPUMP_HD )
	{
	 	/* Bring up hd display */
		avp->display.display_hd = (NEXUS_DisplayHandle)MyRetDisplay(0);	
		if(avp->display.display_hd == NULL)
			return -1;
		/*remove it here by hh; Date:2011-11-08*/
		NEXUSAPP_Hdmihotplug_Reset(avp);
	}
	else
	{
		/* Bring up sd display */
		avp->display.display_sd = (NEXUS_DisplayHandle)MyRetDisplay(1);
		if(avp->display.display_sd == NULL)
			return -1;
	}
	
	return 0;
}	

/*************************************************
  Function:    	NEXUSAPP_Display_Reset
  Description:	HD/SD display reset , reset HD/SD display format , and connect to composite and hdmi output. 
  Input:		1 . struct av_playpump_t *avp	:  playpump info struct
  			2 .  int playpumpId 				:  playpump id  (PLAYER_PLAYPUMP_HD or PLAYER_PLAYPUMP_SD)
  Output:		
  Return:                   
*************************************************/
void NEXUSAPP_Display_Reset(av_playpump *avp , int playpumpId )
{
	if(playpumpId == PLAYER_PLAYPUMP_HD)
	{
		NEXUS_Display_Close(avp->display.display_hd);
		avp->display.display_hd = NULL;
		NEXUSAPP_Display_Init(avp,PLAYER_PLAYPUMP_HD);
		NEXUS_Display_SetGraphicsFramebuffer(avp->display.display_hd,avp->framebuff[0].surface);
	}
	else
	{
	/*
		NEXUS_Display_Close(avp->display.display_sd);
		avp->display.display_sd = NULL;
		NEXUSAPP_Display_Init(avp,PLAYER_PLAYPUMP_SD);
		NEXUS_Display_SetGraphicsFramebuffer(avp->display.display_sd,avp->framebuff[3].surface);
	*/
	}
}


/*************************************************
  Function:    	NEXUSAPP_PlayerWindows_Init
  Description:	HD/SD windows init , set HD/SD windows format ,  connect to HD/SD display, 
  			and default the display mode.  Display mode reference to "typedef enum Display_Mode" define.
  Input:		1 . struct av_playpump_t *avp	:  playpump info struct 
  Output:		
  Return:                   
*************************************************/
void NEXUSAPP_PlayerWindows_Init(av_playpump *avp)
{

	switch(avp->display.mode)
	{
		case CTRL_DISPLAY_MODE_eA_B:
			avp->hdPlayer.win_0 = avp->display.winHd_0;
			avp->sdPlayer.win_0 = avp->display.winSd_0;
			avp->sdPlayer.win_1 = avp->display.winSd_0;
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
			avp->sdPlayer.win_1 = avp->display.winHd_1;
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
}

/*************************************************
  Function:    	NEXUSAPP_Windows_Init
  Description:	HD/SD windows init , set HD/SD windows format ,  connect to HD/SD display, 
  			and default the display mode.  Display mode reference to "typedef enum Display_Mode" define.
  Input:		1 . struct av_playpump_t *avp	:  playpump info struct 
  Output:		
  Return:            1.  0 : succeed          
*************************************************/
int NEXUSAPP_Windows_Init(av_playpump *avp)
{

	NEXUS_VideoWindowSettings  windowsettings;
	NEXUS_VideoWindowScalerSettings scalerSettings;
	NEXUS_VideoWindowMadSettings madSettings;
	int iValue = 0;
	
/*	HD DISPLAY */	
	avp->display.winHd_0= NEXUS_VideoWindow_Open(avp->display.display_hd, 0);

	if(avp->display.winHd_0)
	{

		NEXUS_DisplayModule_SetAutomaticPictureQuality();

		NEXUS_VideoWindow_GetSettings(avp->display.winHd_0,&windowsettings);
		windowsettings.autoMaster = false;		
		windowsettings.allocateFullScreen = true;
		/* the following settings allow smooth scaling of video.
		it may cause bandwidth problems on some systems. 
		some of these settings must be applied before NEXUS_VideoWindow_AddInput. */
		windowsettings.scaleFactorRounding.enabled = true;
		windowsettings.scaleFactorRounding.horizontalTolerance = 0;
		windowsettings.scaleFactorRounding.verticalTolerance   = 0;
		/* end*/
		windowsettings.contentMode = NEXUS_VideoWindowContentMode_eFull;/*NEXUS_VideoWindowContentMode_eFullNonLinear;*/
		NEXUS_VideoWindow_SetSettings(avp->display.winHd_0,&windowsettings);

		NEXUS_VideoWindow_GetScalerSettings(avp->display.winHd_0, &scalerSettings);
		scalerSettings.nonLinearScaling = true;

		scalerSettings.verticalDejagging = true;
		scalerSettings.horizontalLumaDeringing = true;
		scalerSettings.verticalLumaDeringing = true;
		scalerSettings.horizontalChromaDeringing = true;
		scalerSettings.verticalChromaDeringing = true;

		NEXUS_VideoWindow_SetScalerSettings(avp->display.winHd_0, &scalerSettings);
		NEXUS_VideoWindow_GetMadSettings(avp->display.winHd_0,&madSettings);
		madSettings.deinterlace = true;
		NEXUS_VideoWindow_SetMadSettings(avp->display.winHd_0,&madSettings);

		/* CONTRAST = 800 */
		iValue = 800 ;
		/*NEXUSAPP_Ctrl_Window_Set(avp,CONTRAST,&iValue,1);*/

		/* SATURATION = 2000 */
		iValue = 2000 ;
		/*NEXUSAPP_Ctrl_Window_Set(avp,SATURATION,&iValue,1);*/

		/* SHARNESSENABLE  enable*/
		iValue = 1 ;
		NEXUSAPP_Ctrl_Window_Set(avp,SHARNESSENABLE,&iValue,1);

		/* SARPNESS  = -20000 */
		iValue = -20000 ;
		NEXUSAPP_Ctrl_Window_Set(avp,SARPNESS,&iValue,1);

		/* WIN_DNR_MNR_MODE  enable*/
		iValue = 2 ;
		NEXUSAPP_Ctrl_Window_Set(avp,WIN_DNR_MNR_MODE,&iValue,1);

		/* WIN_DNR_MNR_LEVEL  = 0 */
		iValue = 0 ;
		NEXUSAPP_Ctrl_Window_Set(avp,WIN_DNR_MNR_LEVEL,&iValue,1);

		/* WIN_DNR_BNR_MODE  enable*/
		iValue = 2 ;
		NEXUSAPP_Ctrl_Window_Set(avp,WIN_DNR_BNR_MODE,&iValue,1);

		/* WIN_DNR_BNR_LEVEL  = 0 */
		iValue = 0 ;
		NEXUSAPP_Ctrl_Window_Set(avp,WIN_DNR_BNR_LEVEL,&iValue,1);	

		/* WIN_DNR_DCR_MODE  enable*/
		iValue = 2 ;
		NEXUSAPP_Ctrl_Window_Set(avp,WIN_DNR_DCR_MODE,&iValue,1);

		/* WIN_DNR_BNR_MODE  = 0 */
		iValue = 0 ;
		NEXUSAPP_Ctrl_Window_Set(avp,WIN_DNR_DCR_LEVEL,&iValue,1);			
		
	}


/*	SD DISPLAY */	
	avp->display.winSd_0= NEXUS_VideoWindow_Open(avp->display.display_sd, 0);

	if(avp->display.winSd_0)
	{
		NEXUS_VideoWindow_GetSettings(avp->display.winSd_0,&windowsettings);
		windowsettings.allocateFullScreen = true;
		NEXUS_VideoWindow_SetSettings(avp->display.winSd_0,&windowsettings);
	}

	avp->hdPlayer.win_0 =  avp->display.winHd_0;
	avp->hdPlayer.win_1 =  avp->display.winSd_0;
	avp->sdPlayer.win_0 = 0;
	avp->sdPlayer.win_1 = 0;
/*	
	NEXUSAPP_PlayerWindows_Init(avp);
*/	
	
	return 0;

}

/*************************************************
  Function:    	NEXUSAPP_Event_Creat
  Description:	Create system event , HD/SD player event and graphics 2d event.
  Input:		1 . struct av_playpump_t *avp	:  playpump info struct 
  Output:		
  Return:                   
*************************************************/
void NEXUSAPP_Event_Creat(av_playpump *avp)
{
	BKNI_CreateEvent(&avp->hdPlayer.Event);
	BKNI_CreateEvent(&avp->sdPlayer.Event);
	BKNI_CreateEvent(&avp->g2dEvent);
}

/*************************************************
  Function:    	NEXUSAPP_Playpump_Codec_Get
  Description:	Get HD/SD stream format codec.
  Input:		1 . char *fname						:  playpump info struct 
  			2 . struct av_stream_info_t *streamInfo	:  HD/SD stream info struct	
  Output:		
  Return:                   
*************************************************/
void NEXUSAPP_Playpump_Codec_Get(char *fname , av_stream_info *streamInfo )
{
	int i;
	
	memset(streamInfo , 0x00, sizeof(av_stream_info));
	
	for(i = 0; i < STREAM_TYPE_MAX ; i++)
	{
		if(!strncasecmp(stream_type[i].name ,fname,strlen(fname)))
		{
			streamInfo->transportType = stream_type[i].transportType;
			streamInfo->videoCodec = stream_type[i].videoCodec;
			streamInfo->audioCodec = stream_type[i].audioCodec;
			streamInfo->pid_video = stream_type[i].pid_video;
			streamInfo->pid_audio_1 = stream_type[i].pid_audio_1;
			streamInfo->pid_audio_2 = stream_type[i].pid_audio_2;
			DBG_APP(("NEXUSAPP_Playpump_Codec_Get = %s , codec = %s \n",fname,stream_type[i].name));
			break;
		}
	}
}

/*************************************************
  Function:    	NEXUSAPP_Playpump_Codec_Set
  Description:	Set HD/SD stream format codec. It must be set before NEXUSAPP_Playpump_Play
  Input:		1 . struct av_playpump_t *avp			:  playpump info struct 
  			2 . struct av_stream_info_t *streamInfo	:  HD/SD stream info struct	
  Output:		
  Return:                   
*************************************************/
int NEXUSAPP_Playpump_Codec_Set(av_player *player , av_stream_info *streamInfo )
{
	int i;
	char *ptr;

	if((ptr = strstr(streamInfo->szSongName,"/mnt/udisk/"))!=NULL)
		return 0;
	
	NEXUS_VideoDecoder_GetDefaultStartSettings(&player->videoProgram);
	NEXUS_AudioDecoder_GetDefaultStartSettings(&player->audioProgram);

	/* set uncompreeAudio mode to default*/
	player->ifcompressedAudio = 0;

	if(streamInfo->transportType)
		goto info_copy;
		
	DBG_APP(("set stream codec name = %s \n",player->ioContext.filename));

	memset(streamInfo,0x00,sizeof(av_stream_info));

	memset(&player->audioPidChannel,0x00,(sizeof(av_pidChannel)*5));
	
	for(i = 0; i < STREAM_TYPE_MAX ; i++)
	{
		
		if(!strncasecmp(stream_type[i].name , player->ioContext.filename,strlen( player->ioContext.filename)))
		{
			streamInfo->transportType = stream_type[i].transportType;
			streamInfo->videoCodec = stream_type[i].videoCodec;
			streamInfo->audioCodec = stream_type[i].audioCodec;
			streamInfo->pid_video = stream_type[i].pid_video;
			streamInfo->pid_audio_1 = stream_type[i].pid_audio_1;
			streamInfo->pid_audio_2 = stream_type[i].pid_audio_2;
			player->ifcompressedAudio = stream_type[i].compressed;
			DBG_APP(("find stream codec name = %s , codec = %s \n",player->ioContext.filename,stream_type[i].name));
			goto info_copy;
		}
	}

	if(streamInfo->transportType == 0)
		goto codec_set_error_out;

info_copy:
	
	player->transportType = streamInfo->transportType;
	
	player->videoProgram.codec = streamInfo->videoCodec;	
	player->audioProgram.codec = streamInfo->audioCodec;
	player->audio2Program.codec = streamInfo->audioCodec;
	
	player->videoPidChannel.pid = streamInfo->pid_video;
	player->audioPidChannel[0].pid = streamInfo->pid_audio_1;

	if(streamInfo->pid_audio_2)
		player->audioPidChannel[1].pid = streamInfo->pid_audio_2;

	DBG_APP(("transport type = %d \n",player->transportType));
	DBG_APP(("videoProgram type = %d \n",player->videoProgram.codec));
	DBG_APP(("audioProgram type = %d \n",player->audioProgram.codec));
	DBG_APP(("if hd = %d \n",streamInfo->ifHd));
	DBG_APP(("video_pid = %d \n",player->videoPidChannel.pid));
	DBG_APP(("audio_pid_1 = %d \n",player->audioPidChannel[0].pid ));
	DBG_APP(("audio_pid_2 = %d \n",player->audioPidChannel[1].pid ));
	
	return 0;	

codec_set_error_out:
	
	return -1;
}

/*************************************************
  Function:    	NEXUSAPP_Playpump_Callback
  Description:	Set HD/SD playpump event callback , it will be call by playpump start
  Input:		1 . void *context	: callback context.
  				2 . int param		: parameter
  Output:
  Return:
*************************************************/
static void NEXUSAPP_Playpump_Callback(void *context, int param)
{
	BSTD_UNUSED(param);
	BKNI_SetEvent((BKNI_EventHandle)context);
}

static void NEXUSAPP_Playpump_SD_Callback(void *context, int param)
{
	BSTD_UNUSED(param);
	BKNI_SetEvent((BKNI_EventHandle)context);
}

/*************************************************
  Function:    	NEXUSAPP_Playpump_Pthread_Init
  Description:	NEXUS app playpump thread mutex init.
  Input:		1 . struct av_playpump_t *avp			:  playpump info struct 
  Output:		
  Return:                   
*************************************************/
static void NEXUSAPP_Playpump_Pthread_Init(av_playpump *avp)
{
	pthread_mutex_init(&avp->hdPlayer.lock,NULL);
	pthread_mutex_init(&avp->sdPlayer.lock,NULL);
	pthread_mutex_init(&avp->osd.osdLock,NULL);
	pthread_mutex_init(&avp->osd.piz.pizLock,NULL);
	pthread_mutex_init(&avp->osd.textRoll.RollLock,NULL);
	pthread_mutex_init(&avp->osd.textStatic.staticLock,NULL);
	pthread_mutex_init(&avp->osd.mw.mwStateLock,NULL);
	pthread_mutex_init(&avp->osd.mw.mwActionLock,NULL);	
	pthread_mutex_init(&avp->ctrlLock,NULL);
	pthread_mutex_init(&avp->playerLock,NULL);
	pthread_mutex_init(&avp->playerPizLock,NULL);
	pthread_mutex_init(&avp->g2dFillLock,NULL);

/* 	must be strat mftp watch dog thread for nexusio_mftp_io */
	nexusio_mftp_watch_dog_start_thread();
}
/*************************************************
  Function:    	NEXUSAPP_Playpump_Pthread_Init
  Description:	NEXUS playpump init and start.
  Input:		1 . struct av_player_t *player	:  playpump player info struct
  			2 . int playpumpId				:  playpump id  (PLAYER_PLAYPUMP_HD or PLAYER_PLAYPUMP_SD)
  Output:		
   Return:           1.  0 : succeed                  
*************************************************/
int NEXUSAPP_Playpump_Init(av_player *player , int playpumpId)
{
	NEXUS_PlaypumpSettings playpumpSettings;
	NEXUS_PlaypumpOpenSettings openSettings;

	NEXUS_Playpump_GetDefaultOpenSettings(&openSettings);
	openSettings.fifoSize = 1540096;
	openSettings.alignment = 12;
	openSettings.numDescriptors = 100;
	
	if(playpumpId == PLAYER_PLAYPUMP_HD)
	{
		MSG_APP(("playpump HD  init and start !\n"));
		player->playpump = NEXUS_Playpump_Open(0, &openSettings);
		/*init  hd playpump*/
		NEXUS_Playpump_GetSettings(player->playpump, &playpumpSettings);
		playpumpSettings.transportType = player->transportType;
		playpumpSettings.dataCallback.callback = NEXUSAPP_Playpump_Callback;
		playpumpSettings.dataCallback.context = player->Event;
		NEXUS_Playpump_SetSettings(player->playpump, &playpumpSettings);
		NEXUS_Playpump_Start(player->playpump);
	}
	else
	{
		MSG_APP(("playpump SD  init and start !\n"));

		player->playpump = NEXUS_Playpump_Open(1, &openSettings);

		/*init sd playpump*/
		NEXUS_Playpump_GetSettings(player->playpump, &playpumpSettings);
		playpumpSettings.transportType = player->transportType;
		playpumpSettings.dataCallback.callback = NEXUSAPP_Playpump_SD_Callback;
		playpumpSettings.dataCallback.context = player->Event;
		NEXUS_Playpump_SetSettings(player->playpump, &playpumpSettings);
		NEXUS_Playpump_Start(player->playpump);
	}

	return 0;
}

/*************************************************
  Function:    	NEXUSAPP_Playpump_Respone
  Description:	NEXUS player respone message 
  Input:		1 . int fd			:  socket handle
  			2 . int error		:  respone error message 
  Output:		
  Return:           1.  0 : succeed
  			2. -1 : error
*************************************************/
static int NEXUSAPP_Playpump_Respone(int fd , int error )
{
	osd_ctrl_respond respond;

	respond.flagId = 0x10;
	respond.message = error;
	return socket_tcp_server_send(fd,(char *)&respond,sizeof(osd_ctrl_respond));
}

/*************************************************
  Function:    	NEXUSAPP_Plaupump_Ready
  Description:	NEXUS application 's parameter default.
  Input:		1 . struct av_playpump_t *avp			:  playpump info struct 
  Output:		
  Return:           1.  0 : succeed                  
*************************************************/
int NEXUSAPP_Plaupump_Ready(av_playpump  *avp)
{
	char *pDebug = NULL;
	
	avp->display.mode = CTRL_DISPLAY_MODE_eA_B;
	avp->display.pipVisible = false;
	avp->display.pipInfo.x =1400;
	avp->display.pipInfo.y =100;
	avp->display.pipInfo.width = 360;
	avp->display.pipInfo.height =270;
	
	avp->uiIfHdRun = false;
	avp->uiWinNorm = true;
	avp->hdPlayer.socketFd = -1;
	avp->sdPlayer.socketFd = -1;
	avp->socketCtrlFd =-1;

	avp->hdPlayer.decoderStart =false;
	avp->sdPlayer.decoderStart =false;
	avp->audioDecoderStart =false;
	avp->hdPlayer.audioSelected = true;
	avp->ctrlPthreadStart = false;
	
	avp->hdPlayer.state = PLAYER_CTRL_STOP;
	avp->sdPlayer.state = PLAYER_CTRL_STOP;

	avp->display.display_hd = NULL;
	avp->display.display_sd = NULL;

	avp->display.winHd_0 = NULL;
	avp->display.winHd_1 =  NULL;
	avp->display.winSd_0 = NULL;
	avp->display.winSd_1 = NULL;

	avp->framebuff[0].surface= NULL;
	avp->framebuff[1].surface= NULL;
	avp->framebuff[2].surface= NULL;

	avp->osd.mw.mwState = OSD_MW_STATE_IDLE;

	/*add by LQQ; Date:2011-11-09*/
	avp->Hdmihotplug_callback = NEXUSAPP_Hdmihotplug_callback;

	/* must be get io handle */
	avp->hdPlayer.myIo = nexusio_mftp_prtocol_get();
	avp->sdPlayer.myIo = nexusio_mftp_prtocol_get();
	memset(&avp->hdPlayer.ioContext,0x00,sizeof(nexusIoContext));
	memset(&avp->sdPlayer.ioContext,0x00,sizeof(nexusIoContext));

	avp->hdPlayer.uiLastdepth = SURFACE_OSD_MODE_2D;
	avp->hdPlayer.uiDisplay3DMode = SURFACE_OSD_MODE_2D;

	pDebug = getenv("PLAYERDEBUG");
	if(pDebug !=NULL && strstr(pDebug,"ON") != NULL)
		nexus_debug_opt = 1;
	
	return 0;
}


/*************************************************
  Function:    	NEXUSAPP_Playpump_Stop
  Description:	playpump stop 
  Input:		1 . struct av_playpump_t *avp	:  playpump info struct 
			2 . struct av_player_t *player	:  playpump player info struct
 			3 . int playpumpId				:  playpump id(PLAYER_PLAYPUMP_HD or PLAYER_PLAYPUMP_SD)
  Output:		
  Return:           1.  0 : succeed                  
*************************************************/
static void NEXUSAPP_Playpump_Stop(av_pthread_context *av, av_player *player ,int playpumpId)
{
	DBG_APP(("NEXUSAPP_Playpump_Stop : come in![%s]\n", player->playerId?"SD":"HD"));
	
/*	close my io */
	if(player->ioContext.priv_data && av->avp->uiIfHdRun)
		player->myIo->url_close(&player->ioContext);

	if(player->socketFd > 0)
		close(player->socketFd);

	if(!player->decoderStart)
		return;
	
	pthread_mutex_lock(&av->avp->playerLock);

	DBG_APP(("NEXUSAPP_Ctrl_Windows_StateSet !\n"));	
	
	if(player->audioSelected&&av->avp->audioDecoderStart)
	{	
		/* add by hh , 2012_07_07*/
		NEXUSAPP_Ctrl_VoiceVolume_Silent(av->avp,AUDIO_MUTE);
		
		NEXUS_AudioDecoder_Stop(av->avp->audioDecoder);
		NEXUS_AudioDecoder_Stop(av->avp->audio2Decoder);

		DBG_APP(("NEXUS_AudioDecoder_Stop !\n"));	

		av->avp->audioDecoderStart = false;
		
		NEXUSAPP_Decoder_Pid_Close(player,1);

		DBG_APP(("NEXUSAPP_Decoder_Pid_Close !\n"));	
	}
	
	DBG_APP(("NEXUS_VideoDecoder_Stop !\n"));
	NEXUS_VideoDecoder_Stop(player->videoDecoder);
	
	DBG_APP(("NEXUS_Playpump_Stop !\n"));	
	NEXUS_Playpump_Stop(player->playpump);
	
	NEXUSAPP_Decoder_Pid_Close(player,0);
	
	DBG_APP(("NEXUS_Playpump_Close !\n"));
	NEXUS_Playpump_Close(player->playpump);

	player->decoderStart = false;
	
	NEXUSAPP_Playpump_Static_Set(av->avp,playpumpId,PLAYER_CTRL_STOP);

	player->playpump = NULL;
	
	pthread_mutex_unlock(&av->avp->playerLock);

	DBG_APP(("NEXUSAPP_Playpump_Stop : leave!\n"));

 }


/*************************************************
  Function:    	NEXUSAPP_Playpump_Start
  Description:	playpump start 
  Input:		1 . struct av_pthread_context *av	:  thread input parameter struct 
			2 . struct av_player_t *player	:  playpump player info struct
 			3 . struct av_stream_info_t *stream_info	:  HD/SD stream info
  Output:		
  Return:           
*************************************************/
static void NEXUSAPP_Playpump_Start(av_pthread_context *av,av_player *player ,av_stream_info *stream_info)
{	
	int ret;
	
	if(av == NULL || player == NULL || stream_info == NULL)
		return;
	/* add by hh , check player state ,if started , stop it  2012_09_28*/
	if( player->decoderStart)
		NEXUSAPP_Playpump_Stop(av,player,av->playpumpID);
		
	NEXUSAPP_Playpump_Depth_Ctrl_Get(player,stream_info);

	/* effect k9001 check , if not k9001 , set ifcompressedAudio = 0*/
	if( av->avp->audioSilentCtrl == 0 )
		player->ifcompressedAudio = 0;

	pthread_mutex_lock(&av->avp->playerLock);

	NEXUSAPP_Playpump_Init(player,av->playpumpID);

	/* must be init all pid channel */
	NEXUSAPP_Decoder_Pid_Init(av->avp,av->playpumpID);

	NEXUSAPP_VideoDecoder_Programe_Init(av->avp,av->playpumpID);

	/* must be select pidchannel left !!!!!*/
	NEXUSAPP_AudioDecoder_Programe_Init(av->avp,av->playpumpID);
	
	/* mody by hh , sd player is start , switch sdwind to sd video decoder*/
	if(player->playerId == PLAYER_PLAYPUMP_HD)
	{
		if(av->avp->display.mode == CTRL_DISPLAY_MODE_eA_A)
		{
			NEXUSAPP_Windows_Connect(av->avp->display.winSd_0 , player->videoDecoder);
		}
	}
	else
	{
		NEXUS_VideoWindow_RemoveAllInputs(av->avp->display.winSd_0);
		NEXUSAPP_Windows_Connect(av->avp->display.winSd_0 , player->videoDecoder);
	}

	NEXUS_VideoDecoder_Start(player->videoDecoder, &player->videoProgram);
	
	if(player->audioSelected&&!av->avp->audioDecoderStart)
	{
		/* set audio decoder compressed mode */
		NEXUSAPP_Ctrl_AudioDecoder_Set(av->avp,player,player->ifcompressedAudio);

		DBG_APP(("NEXUS_AudioDecoder_Start [%s]\n",av->playpumpID== PLAYER_PLAYPUMP_HD ?"HD":"SD"));


		if(player->ifcompressedAudio)
		{
			DBG_APP(("NEXUS_AudioDecoder_Start compressed decoder 2012_0512 !\n"));
			NEXUS_AudioDecoder_Start(av->avp->audioDecoder,&player->audioProgram);
		}
		else
		{
			NEXUS_AudioDecoder_Start(av->avp->audioDecoder,&player->audioProgram);
			NEXUS_AudioDecoder_Start(av->avp->audio2Decoder,&player->audio2Program);		
		}

		NEXUSAPP_Ctrl_Audio_Mode_Set(av->avp,player,player->audiomode);
		
		av->avp->audioDecoderStart = true;
		player->audioSelected = true;
	}

	DBG_APP(("audioDecoderStart\n"));
	
	player->decoderStart = true;
	
	NEXUSAPP_Playpump_Static_Set(av->avp,av->playpumpID,PLAYER_CTRL_PAUSE);

	/*	end mody */
	NEXUSAPP_Ctrl_Windows_StateSet(av->avp,player->win_0,true);
	NEXUSAPP_Ctrl_Windows_StateSet(av->avp,player->win_1,true);
	
	/*3dtv , set*/
	if(player->playerId == PLAYER_PLAYPUMP_HD && player->uiLastdepth != player->uiDisplay3DMode)
	{
		/* set hdmi mode */
		NEXUSAPP_Hdmi_mode_set(av->avp,player->uiDisplay3DMode);
		/* set framebuff mode */
		NEXUSAPP_Osd_Mode_Set(av->avp,player->uiDisplay3DMode);
		player->uiLastdepth = player->uiDisplay3DMode;
	}

	/* reset audiomode on start play ,accept audiomode set first*/
	player->audiomode = -1;
	
	/*end set */
	pthread_mutex_unlock(&av->avp->playerLock);
	
	/* send respone player ready to play */	
	ret = NEXUSAPP_Playpump_Respone(player->socketFd , CTRL_OSD_ERROR_NOTHING);


	DBG_APP(("NEXUSAPP_Playpump_Start start ok!ret:%d\n", ret));
		
}
/*************************************************
  Function:    	NEXUSAPP_Playpump_CSong
  Description:	playpump check song  
  Input:		1 . struct av_pthread_context *av	:  thread input parameter struct 
			2 . struct av_player_t *player	:  playpump player info struct
 			3 . struct av_stream_info_t *stream_info	:  HD/SD stream info
  Output:		
  Return:           1.  0	 : succeed
  			2. -1 : error
*************************************************/
static int NEXUSAPP_Playpump_CSong(av_player *player , av_stream_info *stream_info )
{
	int  iRet = -1;
	char *ptr = NULL;
	char szFileName[1024];

	memset(szFileName,0x00,sizeof(szFileName));

	player->uiDisplay3DMode = SURFACE_OSD_MODE_2D;

	if((ptr = strstr(stream_info->szSongName,"/mnt/mb/"))!=NULL)
	{				
		ptr += strlen("/mnt/mb/");

		if(ptr == NULL)
			goto check_song_error_out;

		player->myIo = nexusio_file_prtocol_get();

		if(player->myIo == NULL)
			goto check_song_error_out;

		iRet = player->myIo->url_open(&player->ioContext,stream_info->szSongName,0);
	}
	else if((ptr = strstr(stream_info->szSongName,"/tmp/1.mpg"))!=NULL)
	{
		ptr += strlen("/tmp/1.mpg");

#if 0
		if(ptr != NULL)
		{
			printf("[%s-%d]not null:%c\n", __FUNCTION__, __LINE__, *ptr);
			goto check_song_error_out;
		}
#endif

		if(access(stream_info->szSongName, F_OK))
		{
			DBG_APP(("[%s-%d]file not exist\n", __FUNCTION__, __LINE__));
			goto check_song_error_out;
		}

		player->myIo = nexusio_mftp_prtocol_get();

		if(player->myIo == NULL)
		{
			DBG_APP(("Playpump CSong io null\n"));
			goto check_song_error_out;
		}
		
		player->ioContext.fullname = malloc(strlen(stream_info->szSongName));
		memset(player->ioContext.fullname, 0, sizeof(player->ioContext.fullname));
		strcpy(player->ioContext.fullname, stream_info->szSongName);

		player->ioContext.filename= malloc(4);
		memset(player->ioContext.filename, 0, 4);		
		strcpy(player->ioContext.filename, "mpg");
		/*printf("[%s-%d]filename:%s, fullname:%s\n", __FUNCTION__, __LINE__, player->ioContext.filename, player->ioContext.fullname);*/
		iRet = 0;
	}
	else if((ptr = strstr(stream_info->szSongName,"/a/"))!=NULL)
	{
		ptr += strlen("/a/");

		if (NULL == ptr)
		{
			DBG_APP(("[%s-%d]ptr null\n", __FUNCTION__, __LINE__));
			goto check_song_error_out;
		}

		if(access(stream_info->szSongName, F_OK))
		{
			DBG_APP(("[%s-%d]file not exist\n", __FUNCTION__, __LINE__));
			goto check_song_error_out;
		}
				
		player->myIo = nexusio_mftp_prtocol_get();

		if(player->myIo == NULL)
		{
			DBG_APP(("Playpump CSong io null\n"));
			goto check_song_error_out;

		}
		
		player->ioContext.fullname = malloc(strlen(stream_info->szSongName));
		memset(player->ioContext.fullname, 0, sizeof(player->ioContext.fullname));
		strcpy(player->ioContext.fullname, stream_info->szSongName);

		player->ioContext.filename= malloc(4);
		memset(player->ioContext.filename, 0, 4);		
		strcpy(player->ioContext.filename, "mpg");
		/*printf("[%s-%d]filename:%s, fullname:%s\n", __FUNCTION__, __LINE__, player->ioContext.filename, player->ioContext.fullname);*/
		iRet = 0;
		
	}
	else if(strstr(stream_info->szSongName,"#"))
	{
		/* mftp song */
		ptr = strstr(stream_info->szSongName,"#");
		if(ptr == NULL)
			goto check_song_error_out;

		ptr++;

		memset(szFileName,0x00,sizeof(szFileName));
		
		sprintf(szFileName,"mftp://%d//#%s" , player->playerId + 2 , ptr );
		
		player->myIo = nexusio_mftp_prtocol_get();

		if(player->myIo == NULL)
			goto check_song_error_out;
		
		iRet = player->myIo->url_open(&player->ioContext,szFileName,0);

		if(*ptr == 'D')
			player->uiDisplay3DMode = SURFACE_OSD_MODE_3DRL;

		if(*ptr == 'H' || *ptr == 'D')
			stream_info->ifHd = 1;
		
		DBG_APP(("recv song name = %s \n",szFileName));
	}
	else if(strstr(stream_info->szSongName,"@"))
	{		
		player->myIo = nexusio_mftp_prtocol_get();

		strcpy(szFileName,stream_info->szSongName);

		printf("recv song name = %s \n",szFileName);

		stream_info->pid_video = 0xe0;
		stream_info->pid_audio_1 = 0xc0;
		stream_info->transportType = NEXUS_TransportType_eMpeg2Pes;
		stream_info->videoCodec = NEXUS_VideoCodec_eMpeg2;
		stream_info->audioCodec = NEXUS_AudioCodec_eMpeg;

		iRet = player->myIo->url_open(&player->ioContext,szFileName,0);
	}
	else
	{
		player->myIo = nexusio_mftp_prtocol_get();
		iRet = player->myIo->url_open(&player->ioContext,stream_info->szSongName,0);
		DBG_APP(("open song name = %s , result = %d \n",player->ioContext.filename,iRet));
	}
	
	return iRet;

check_song_error_out:
	return -1;
}
/*************************************************
  Function:    	NEXUSAPP_Playpump_Prepare
  Description:	playpump prepare ,  get HD/SD stream info from socket,used before NEXUSAPP_Playpump_Start .
  			this function will be loop to receive stream info utill get correct stream info struct. 
  Input:		1 . struct av_pthread_context *av	:  thread input parameter struct 
			2 . struct av_player_t *player	:  playpump player info struct
 			3 . struct av_stream_info_t *stream_info	:  HD/SD stream info
  Output:		
  Return:           	1.  0	 : succeed
  			2. -1 : tcp socket transport error	
*************************************************/
static int NEXUSAPP_Playpump_Prepare(av_pthread_context *av,av_player *player ,void *socket_info,av_stream_info *stream_info )
{
	int error ;
	
start:
	
	if(player->ioContext.priv_data)
		player->myIo->url_close(&player->ioContext);

	if(player->socketFd > 0)
		socket_tcp_server_close(&player->socketFd);

	NEXUSAPP_Playpump_Static_Set(av->avp , player->playerId , PLAYER_CTRL_PLAY_READY);
	
#ifdef HDSD_TRANSPORT_TCP
	player->socketFd = socket_tcp_server_open((socket_t *)socket_info);
#else
	player->socketFd = socket_unix_server_open((unixsock *)socket_info);
#endif

	if(player->socketFd <= 0)
	{
		usleep(100000);
		DBG_APP(("playpump[%d]: tcp server listen error, try again!\n",av->playpumpID));
		goto start;
	}
	
	memset(stream_info,0x00,sizeof(av_stream_info));
	
	if(NEXUSAPP_Playpump_StreamInfo_Get(player,stream_info))
	{
		socket_tcp_server_close(&player->socketFd);
		DBG_APP(("NEXUSAPP_Playpump_StreamInfo_Get error ,close socket!\n"));
		goto start;
	}
	
	error = CTRL_OSD_ERROR_NOTHING;

	DBG_APP(("szSongName = %s \n",stream_info->szSongName));

	
	if(NEXUSAPP_Playpump_CSong(player,stream_info))
	{
		DBG_APP(("Csong error\n"));
		error = CTRL_OSD_ERROR_TRANSPORT;
		goto respond;
	}


	if(NEXUSAPP_Playpump_Codec_Set(player,stream_info))
	{
		DBG_APP(("Codec Set Error\n"));
		error = CTRL_OSD_ERROR_TRANSPORT;
		goto respond;
	}
		
	DBG_APP(("NEXUSAPP_Playpump_Prepare : name = %s , 3Dmode = %d\n",player->ioContext.filename,player->uiDisplay3DMode));

respond:

	if(error != CTRL_OSD_ERROR_NOTHING)
	{
		NEXUSAPP_Playpump_Respone(player->socketFd , error);

		DBG_APP(("tcp error ,get stream info error , or on found song !\n"));

		NEXUSAPP_Playpump_Stop(av,player,av->playpumpID);
		
		goto start;
	}

	return 0;

}

/*************************************************
  Function:    	NEXUSAPP_Playpump_Display_UerData
  Description:	send user data to playpump . 
  Input:		1. struct av_player_t *player	:  playpump player info struct
  Output:		
  Return:           1.  0	 : succeed
  			2. -1 : tcp socket transport error	
*************************************************/
static int NEXUSAPP_Playpump_Display_UerData(av_player *player)
{
	char *buffer;
	unsigned int buffer_size;
	NEXUS_Error rc = 0;
	int iRet =0;

	if (NEXUS_Playpump_GetBuffer(player->playpump, (void **)&buffer, &buffer_size))
	{
		DBG_APP(("nexus playpump get buff error ,go to start_playpump\n"));
		return -1;
	}
	
	if (buffer_size == 0 ) {
		BKNI_WaitForEvent(player->Event, BKNI_INFINITE);
		return 1;
	}
		
	if (buffer_size > player->uiloopCount )
		buffer_size = player->uiloopCount;

	iRet = player->myIo->url_read(&player->ioContext,(unsigned char *)buffer,buffer_size);

	rc = NEXUS_Playpump_ReadComplete(player->playpump, 0, buffer_size);
	if(rc)
		MSG_DISPLAY(("NEXUS_Playpump_ReadComplete : rc = %d\n",rc));

	return iRet;
	
}

/*************************************************
  Function:    	NEXUSAPP_Playpump_Display_FileData
  Description:	playback media file . 
  Input:		1 . struct av_playpump_t *avp		:  playpump info struct 
			2 . struct av_player_t *player		:  playpump player info struct
			3 .  char *fileName					:  media file name , it must be contain full path
  Output:		
  Return:           1.  0	 : succeed
  			2. -1 : file read error	
*************************************************/
static int NEXUSAPP_Playpump_Display_FileData(av_playpump *avp ,av_player *player,char *fileName)
{
	FILE *file=NULL;
	int n=0;
	char fname[40];
	char *buffer = NULL;
	unsigned int  buffer_size;
	int iCount = 0;

	memset(fname,0x00,sizeof(fname));
	strcpy(fname,fileName);

	file = fopen(fname, "rb");
	if (!file)
	{
		DBG_APP(("can't open file:%s\n", fname));
		return -1;
	}

	NEXUSAPP_Ctrl_VoiceVolume_Set(avp,0);

	while(1)
	{
		usleep(10000);
		
		if (NEXUS_Playpump_GetBuffer(player->playpump, (void **)&buffer, &buffer_size))
		{
			DBG_APP(("NEXUSAPP_Playpump_Display_FileData :  error !\n"));
			break;
		}

		if (buffer_size == 0) {
			BKNI_WaitForEvent(player->Event, BKNI_INFINITE);
			continue;
		}

		if(buffer_size>25*1024)
			buffer_size = 25*1024;
		
		n = fread(buffer, 1, buffer_size, file);
		if (n < 0) 
			break;
		
		if (n == 0)
		{
			while (1) {
			        NEXUS_VideoDecoderStatus status;
			        NEXUS_VideoDecoder_GetStatus(player->videoDecoder, &status);
			        if (!status.queueDepth) break;
			}
			
			NEXUSAPP_Ctrl_Windows_Visible(player->win_0,true);
			iCount ++;

			if(iCount >=2)
			{
				NEXUS_Playpump_Flush(player->playpump);
				return 0 ;
			}
			fseek(file, 0, SEEK_SET);
		}
		else
		{
			if(NEXUS_Playpump_ReadComplete(player->playpump, 0, n))
				break;
		}
	}

	return -1;
	
}

/*************************************************
  Function:    	NEXUSAPP_Playpump_Play
  Description:	playpump play , and action the playpump state change
  Input:		1 . struct av_pthread_context *av		:  thread  input parameters  struct 
			2 . struct av_player_t *player		:  playpump player info struct
  Output:		
  Return:           1.  0	 : succeed
  			2. -1 : tcp socket transport error	
*************************************************/
static int NEXUSAPP_Playpump_Play(av_pthread_context *av,av_player *player)
{			
	if(av == NULL || player == NULL)
		return -1;
	
	if(!player->decoderStart)
		return 0;

	DBG_APP(("NEXUSAPP_Playpump_Play--- [%s]: state = %d  !\n" ,player->playerId?"SD":"HD",  player->state));
	
	while(1)
	{
	
		/* protect hplay link , important !!!!!!!!!!!!!!!!!!!!*/
		if(socket_tcp_server_check_link(player->socketFd , 1000))
		{
			DBG_APP(("[%s]player->socketFd: %d, tcp link broken\n", player->playerId?"SD":"HD", player->socketFd));
			
			NEXUSAPP_Playpump_Static_Set(av->avp,player->playerId,PLAYER_CTRL_STOP);

		}
		
		switch(NEXUSAPP_Playpump_State_Ctrl(av->avp , player))
		{
			case 0:
				break;
			case 1:
				continue;
			case 2:
				DBG_APP(("recv stop cmd , return !\n"));
				return 0;
				break;
			default :
				break;
		}
		
		switch(NEXUSAPP_Playpump_Display_UerData(player))
		{				
			case -1:
				DBG_APP(("recv data error !\n"));
				NEXUSAPP_Playpump_Static_Set(av->avp,player->playerId,PLAYER_CTRL_STOP);
				break;
			case 0:
				if(player->playerId == PLAYER_PLAYPUMP_SD)
				{
					player->myIo->url_seek(&player->ioContext,0,0);
					NEXUS_Playpump_Flush(player->playpump);
				}
				else
					NEXUSAPP_Playpump_Static_Set(av->avp,player->playerId,PLAYER_CTRL_PLAY_END);
				break;		
			default:
				break;
		}
	}

	return 0;
}

/*************************************************
  Function:    	NEXUSAPP_Playpump_PlayLogo
  Description:	playpump play hm logo media
  Input		1 . struct av_pthread_context *av		:  thread  input parameters  struct 
			2 . struct av_player_t *player		:  playpump player info struct
  Output:		
  Return:           1.  0	 : succeed
*************************************************/
static int NEXUSAPP_Playpump_PlayLogo(av_pthread_context *av , av_player *player)
{
	av_stream_info stream_info;

	memset(&stream_info , 0x00 ,sizeof(av_stream_info));

	stream_info.audioCodec = 3;
	stream_info.videoCodec = 5;
	stream_info.transportType = 4;
	stream_info.ifHd = 1;
	stream_info.bitRate = 3000;
	stream_info.pid_video = 0xe0;
	stream_info.pid_audio_1 = 0xc0;

	if(NEXUSAPP_Playpump_Codec_Set(player,&stream_info))
		return -1;
	
	NEXUSAPP_Playpump_Start(av,player,&stream_info);
	NEXUSAPP_Playpump_Display_FileData(av->avp,player,"/home/logo.h4p");
	NEXUSAPP_Ctrl_VoiceVolume_Set(av->avp,100);
	return 0;
	
}

int NEXUSAPP_Util_Opts_Init(struct util_opts_t *opts)
{
	memset(opts, 0, sizeof(*opts));
	opts->stcChannelMaster = NEXUS_StcChannelAutoModeBehavior_eVideoMaster;
	opts->beginningOfStreamAction = NEXUS_PlaybackLoopMode_eLoop /*NEXUS_PlaybackLoopMode_ePlay*/;
	opts->endOfStreamAction = NEXUS_PlaybackLoopMode_eLoop /*NEXUS_PlaybackLoopMode_ePlay*/;
	opts->videoErrorHandling = NEXUS_VideoDecoderErrorHandling_eNone;
	opts->customFileIo = false;
	opts->playbackMonitor = false;
	opts->startPaused = false;
	opts->maxPlaybackDataRate = 0;
	opts->stcTrick = false;
	opts->maxDecoderRate = 32 * NEXUS_NORMAL_PLAY_SPEED;

	memset(&opts->common, 0, sizeof(opts->common));
	opts->common.transportType = NEXUS_TransportType_eTs;
	opts->common.extVideoCodec = NEXUS_VideoCodec_eNone;
	opts->common.videoCodec = NEXUS_VideoCodec_eMpeg2;
	opts->common.audioCodec = NEXUS_AudioCodec_eMpeg;
	opts->common.contentMode = NEXUS_VideoWindowContentMode_eFull;
	opts->common.compressedAudio = false;
	opts->common.multichannelAudio = false;
	opts->common.detectAvcExtension = false;
	opts->common.decodedAudio = true;
	opts->common.playpumpTimestampReordering = true;
	opts->common.videoDecoder = 0;
	opts->common.tsTimestampType = NEXUS_TransportTimestampType_eNone;
	opts->common.videoFrameRate = NEXUS_VideoFrameRate_eUnknown;
	opts->common.displayOrientation = NEXUS_VideoOrientation_e2D;
	opts->common.sourceOrientation = NEXUS_VideoDecoderSourceOrientation_e2D;
	opts->common.pcm = false;
	opts->common.useCompositeOutput = false;
	opts->common.useComponentOutput = true;
	opts->common.useHdmiOutput = true;
	opts->common.displayFormat = NEXUS_VideoFormat_eNtsc;
	opts->common.displayType = NEXUS_DisplayType_eAuto;
	opts->common.probe = true;

	memset(&opts->common.pcm_config, 0, sizeof(opts->common.pcm_config));
	opts->common.pcm_config.sample_size = 16;
	opts->common.pcm_config.sample_rate = 44100;
	opts->common.pcm_config.channel_count = 2;

	return 0;
}
static void play_endOfStreamCallback(void *context, int param)
{
	av_playpump *avp = (av_playpump*)context;

	if(param == PLAYER_PLAYPUMP_HD)
	{
		pthread_mutex_lock(&avp->hdPlayer.lock);
		avp->hdPlayer.state = PLAYER_CTRL_STOP;
		pthread_mutex_unlock(&avp->hdPlayer.lock);
	}
	else
	{
		pthread_mutex_lock(&avp->sdPlayer.lock);
		avp->sdPlayer.state = PLAYER_CTRL_STOP;
		pthread_mutex_unlock(&avp->sdPlayer.lock);
	}
		
	DBG_APP(("endOfStream\n"));
	return;
}

static int NEXUSAPP_Playback_Stop(av_pthread_context *av,av_player *player , int playpumpId)
{
	NEXUS_VideoDecoderHandle videoDecoder;
	NEXUS_AudioDecoderHandle audioDecoder, audio2Decoder;
	NEXUS_FilePlayHandle file;
	NEXUS_PlaybackHandle playback;
	NEXUS_PlaypumpHandle playpump;

	playpump = player->playpump;
	playback = player->playback;
	videoDecoder = player->videoDecoder;
	audioDecoder = av->avp->audioDecoder;
	audio2Decoder = av->avp->audio2Decoder;
	file = player->file;
	
	NEXUSAPP_Ctrl_VoiceVolume_Silent(av->avp,AUDIO_MUTE);
	
	NEXUS_Playback_Stop(playback);
	NEXUS_VideoDecoder_Stop(videoDecoder);
	player->decoderStart = false;
	NEXUS_AudioDecoder_Stop(audioDecoder);
	av->avp->audioDecoderStart = false;	
	if (audio2Decoder && av->avp->audio2DecoderStart)
	{
		NEXUS_AudioDecoder_Stop(audio2Decoder);
		av->avp->audio2DecoderStart = false;
	}	

	NEXUS_Playback_CloseAllPidChannels(playback);
	NEXUS_FilePlay_Close(file);
	NEXUS_Playback_Destroy(playback);
	NEXUS_Playpump_Close(playpump);

	player->playpump = NULL;
	player->playback = NULL;
	player->file = NULL;
	
	NEXUSAPP_Playpump_Static_Set(av->avp,playpumpId,PLAYER_CTRL_STOP);

	return 0;
}

static int NEXUSAPP_Playback_Start(av_pthread_context *av,av_player *player ,av_stream_info *stream_info)
{
	NEXUS_PlatformConfiguration platformConfig;
	NEXUS_StcChannelHandle stcChannel;
	NEXUS_StcChannelSettings stcSettings;
	NEXUS_PidChannelHandle videoPidChannel = NULL, audioPidChannel = NULL, audio2PidChannel = NULL, pcrPidChannel, videoExtPidChannel = NULL;
	NEXUS_VideoDecoderHandle videoDecoder;
	NEXUS_VideoDecoderStartSettings videoProgram;
	NEXUS_AudioDecoderHandle audioDecoder, audio2Decoder;
	NEXUS_AudioDecoderStartSettings audioProgram, audio2Program;
	NEXUS_FilePlayHandle file;
	NEXUS_PlaypumpHandle playpump;
	NEXUS_PlaybackHandle playback;
	NEXUS_PlaybackSettings playbackSettings;
	NEXUS_PlaybackPidChannelSettings playbackPidSettings;
	NEXUS_PlaybackTrickModeSettings trickSettings;
	NEXUS_Error rc;
	int playerPrevState, state, rate=1;
	struct util_opts_t opts;
	char *filename = stream_info->szSongName;

	platformConfig = av->avp->platformConfig;
	stcChannel = player->stcChannel;
	videoProgram = player->videoProgram;
	audioProgram = player->audioProgram;
	audio2Program = player->audio2Program;
	videoDecoder = player->videoDecoder;
	audioDecoder = av->avp->audioDecoder;
	audio2Decoder = av->avp->audio2Decoder;

    	BDBG_ASSERT(stcChannel);
    	BDBG_ASSERT(audioDecoder);
    	BDBG_ASSERT(videoDecoder);
		
	NEXUSAPP_Util_Opts_Init(&opts);
	opts.filename = filename;

	if (cmdline_probe(&opts.common, opts.filename, &opts.indexname))
	{
		DBG_APP(("[error]playback start file:%s probe failed\n", opts.filename));
		return 1;
	}

	if (opts.common.transportType == NEXUS_TransportType_eMkv || opts.common.transportType == NEXUS_TransportType_eMp4)
	{
		opts.indexname = opts.filename;
	}

	file = NEXUS_FilePlay_OpenPosix(opts.filename, opts.indexname);
	if (!file)
	{
		fprintf(stderr, "can't open files:%s %s\n", opts.filename, opts.indexname);
		return -1;
	}
	player->file = file;

	/* effect k9001 check , if not k9001 , set ifcompressedAudio = 0*/
	if( av->avp->audioSilentCtrl == 0 )
		player->ifcompressedAudio = 0;
	
	/* send respone player ready to play */	
	NEXUSAPP_Playpump_Respone(player->socketFd , CTRL_OSD_ERROR_NOTHING);

	player->playpump = NEXUS_Playpump_Open(0, NULL);
    	BDBG_ASSERT(player->playpump);
	playpump = player->playpump;
			
	player->playback = NEXUS_Playback_Create();
    	BDBG_ASSERT(player->playback);
	playback = player->playback;

	NEXUS_StcChannel_GetSettings(stcChannel, &stcSettings);
	stcSettings.mode = NEXUS_StcChannelMode_eAuto;
	stcSettings.modeSettings.Auto.behavior = opts.stcChannelMaster;
	stcSettings.modeSettings.Auto.transportType = opts.common.transportType;
	rc = NEXUS_StcChannel_SetSettings(stcChannel, &stcSettings);
	BDBG_ASSERT(!rc);
	
	NEXUS_Playback_GetSettings(playback, &playbackSettings);
	playbackSettings.playpump = playpump;
	playbackSettings.playpumpSettings.transportType = opts.common.transportType;
       playbackSettings.playpumpSettings.timestamp.pacing = false;
       playbackSettings.playpumpSettings.timestamp.type = opts.common.tsTimestampType;
	playbackSettings.stcChannel = stcChannel;
	playbackSettings.stcTrick = opts.stcTrick;
	playbackSettings.endOfStreamCallback.callback = play_endOfStreamCallback;
	playbackSettings.endOfStreamCallback.context = av->avp;
	playbackSettings.endOfStreamCallback.param = player->playerId;
	rc = NEXUS_Playback_SetSettings(playback, &playbackSettings);
	BDBG_ASSERT(!rc);

	if (opts.common.videoCodec != NEXUS_VideoCodec_eNone && opts.common.videoPid != 0)
	{
		NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
		playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
		playbackPidSettings.pidSettings.allowTimestampReordering = opts.common.playpumpTimestampReordering;
		playbackPidSettings.pidTypeSettings.video.decoder = videoDecoder;
		playbackPidSettings.pidTypeSettings.video.index = true;
		playbackPidSettings.pidTypeSettings.video.codec = opts.common.videoCodec;
		videoPidChannel = NEXUS_Playback_OpenPidChannel(playback, opts.common.videoPid, &playbackPidSettings);
	}

	if (opts.common.audioCodec != NEXUS_AudioCodec_eUnknown && opts.common.audioPid != 0)
	{
		NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
		playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
		playbackPidSettings.pidTypeSettings.audio.primary = audioDecoder;
		playbackPidSettings.pidSettings.pidTypeSettings.audio.codec = opts.common.audioCodec;
		audioPidChannel = NEXUS_Playback_OpenPidChannel(playback, opts.common.audioPid, &playbackPidSettings);
	}

	if (opts.common.pcrPid && opts.common.pcrPid != opts.common.videoPid && opts.common.pcrPid != opts.common.audioPid)
	{
		NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
		playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eOther;
		pcrPidChannel = NEXUS_Playback_OpenPidChannel(playback, opts.common.pcrPid, &playbackPidSettings);
	}
	
	NEXUS_VideoWindow_RemoveAllInputs(av->avp->display.winHd_0);
	NEXUSAPP_Windows_Connect(av->avp->display.winHd_0, videoDecoder);

	if(av->avp->display.mode == CTRL_DISPLAY_MODE_eA_A)
	{
		printf("[%s-%d]display mode:%d\n", __FUNCTION__, __LINE__, av->avp->display.mode);
		NEXUS_VideoWindow_RemoveAllInputs(av->avp->display.winSd_0);
		NEXUSAPP_Windows_Connect(av->avp->display.winSd_0 , player->videoDecoder);
	}

	
	NEXUSAPP_Ctrl_VoiceVolume_Silent(av->avp,AUDIO_UNMUTE);
	
	videoProgram.codec = opts.common.videoCodec;
	videoProgram.pidChannel = videoPidChannel;
	videoProgram.frameRate = opts.common.videoFrameRate;
	videoProgram.aspectRatio = opts.common.aspectRatio;
	videoProgram.sampleAspectRatio.x = opts.common.sampleAspectRatio.x;
	videoProgram.sampleAspectRatio.y = opts.common.sampleAspectRatio.y;
	videoProgram.errorHandling = opts.videoErrorHandling;
	videoProgram.timestampMode = opts.common.decoderTimestampMode;
	if (videoExtPidChannel)
	{
		videoProgram.enhancementPidChannel = videoExtPidChannel;
		videoProgram.codec = opts.common.extVideoCodec;
	}

	audioProgram.codec = opts.common.audioCodec;
	audioProgram.pidChannel = audioPidChannel;

	if (opts.common.videoPid)
	{
		rc = NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
		BDBG_ASSERT(!rc);
		player->decoderStart = true;	
	}
	
	if (opts.common.audioPid)
	{
#if B_HAS_ASF
		/* if DRC for WMA pro is available apply now */
		if (audioProgram.codec == NEXUS_AudioCodec_eWmaPro && opts.common.dynamicRangeControlValid)
		{
			NEXUS_AudioDecoderCodecSettings codecSettings;

			NEXUS_AudioDecoder_GetCodecSettings(audioDecoder, audioProgram.codec, &codecSettings);
			codecSettings.codec = audioProgram.codec;
			codecSettings.codecSettings.wmaPro.dynamicRangeControlValid = true;
			codecSettings.codecSettings.wmaPro.dynamicRangeControl.peakReference =
				opts.common.dynamicRangeControl.peakReference;
			codecSettings.codecSettings.wmaPro.dynamicRangeControl.peakTarget =
				opts.common.dynamicRangeControl.peakTarget;
			codecSettings.codecSettings.wmaPro.dynamicRangeControl.averageReference =
				opts.common.dynamicRangeControl.averageReference;
			codecSettings.codecSettings.wmaPro.dynamicRangeControl.averageTarget =
				opts.common.dynamicRangeControl.averageTarget;
			NEXUS_AudioDecoder_SetCodecSettings(audioDecoder, &codecSettings);
		}
#endif

	}

	if (opts.common.audio2Pid && opts.common.audio2Codec)
	{
		NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
		playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
		playbackPidSettings.pidTypeSettings.audio.primary = audio2Decoder;
		playbackPidSettings.pidSettings.pidTypeSettings.audio.codec = opts.common.audio2Codec;
		audio2PidChannel = NEXUS_Playback_OpenPidChannel(playback, opts.common.audio2Pid, &playbackPidSettings);

		audio2Program.codec = opts.common.audioCodec;
		audio2Program.pidChannel = audio2PidChannel;

	}
	
	/* set audio decoder compressed mode */
	NEXUSAPP_Ctrl_AudioDecoder_Set(av->avp,player,player->ifcompressedAudio);

	DBG_APP(("NEXUS_AudioDecoder_Start [%s]\n",av->playpumpID== PLAYER_PLAYPUMP_HD ?"HD":"SD"));

	if(player->ifcompressedAudio)
	{
		DBG_APP(("NEXUS_AudioDecoder_Start compressed decoder 2012_0512 !\n"));
		NEXUS_AudioDecoder_Start(audioDecoder,&audioProgram);
	}
	else
	{
		NEXUS_AudioDecoder_Start(audioDecoder,&audioProgram);
		NEXUS_AudioDecoder_Start(audio2Decoder,&audio2Program);		
	}

	NEXUSAPP_Ctrl_Audio_Mode_Set(av->avp,player,player->audiomode);
	
	av->avp->audioDecoderStart = true;
	player->audioSelected = true;

	NEXUSAPP_Ctrl_Windows_StateSet(av->avp,player->win_0,true);
	NEXUSAPP_Ctrl_Windows_StateSet(av->avp,player->win_1,true);
	
	/*3dtv , set*/
	if(player->playerId == PLAYER_PLAYPUMP_HD && player->uiLastdepth != player->uiDisplay3DMode)
	{
		/* set hdmi mode */
		NEXUSAPP_Hdmi_mode_set(av->avp,player->uiDisplay3DMode);
		/* set framebuff mode */
		NEXUSAPP_Osd_Mode_Set(av->avp,player->uiDisplay3DMode);
		player->uiLastdepth = player->uiDisplay3DMode;
	}
	
	/* reset audiomode on start play ,accept audiomode set first*/
	player->audiomode = -1;

	rc = NEXUS_Playback_Start(playback, file, NULL);
	if(rc)
	{
		printf("[%s-%d]play error\n", __FUNCTION__, __LINE__);
		return 0;
	}

	NEXUSAPP_Playpump_Static_Set(av->avp, player->playerId, PLAYER_CTRL_PLAY);

	playerPrevState = player->state;

	while(1)
	{
		BKNI_Sleep(10);
		
		NEXUSAPP_Playpump_Static_Get(player,&state);		
	
		if (PLAYER_CTRL_PAUSE == state)
		{

			if(playerPrevState != state)
			{
				NEXUS_Playback_Pause(playback);
				playerPrevState = state;
			}
		}
		else if (PLAYER_CTRL_PLAY == state)
		{

			if (playerPrevState != state)
			{
				NEXUS_Playback_Play(playback);
				playerPrevState = state;
			}	
		}
		else if (PLAYER_CTRL_PLAY_TRICKMODE == state)
		{
			rate = player->rate;
			if (rate < -16)
				rate = -16;
			if (rate > 16)
				rate = 16;
			if (0 == rate)
				rate = 1;

			NEXUS_Playback_GetDefaultTrickModeSettings(&trickSettings);
			trickSettings.rateControl = NEXUS_PlaybackRateControl_eDecoder;
			trickSettings.skipControl = NEXUS_PlaybackSkipControl_eHost;
			trickSettings.maxDecoderRate = 32 * NEXUS_NORMAL_PLAY_SPEED;
			trickSettings.rate = rate * NEXUS_NORMAL_PLAY_SPEED;
			trickSettings.mode = NEXUS_PlaybackHostTrickMode_eNone;
			NEXUS_Playback_TrickMode(player->playback, &trickSettings);

			playerPrevState = state;

		}
		else if (PLAYER_CTRL_STOP == state)
		{
			break;
		}
	
	}

	return 0;
}

/*************************************************
  Function:    	NEXUSAPP_Start_Hd_Pthread
  Description:	HD thread 
  			1 . void *context		:  thread  input parameters  struct 
  Output:		
  Return:           
*************************************************/
static void *NEXUSAPP_Start_Hd_Pthread(void *context)
{
	av_player *player ;
	av_stream_info stream_info;
	av_pthread_context *av =  (av_pthread_context *)context;
		
	signal(SIGNAL_EXIT, NEXUSAPP_Signal_Exit);
		
	memset(&stream_info,0x00,sizeof(stream_info));
		
	player = &av->avp->hdPlayer;

	player->playerId = av->playpumpID;

/*play local logo media */
	NEXUSAPP_Playpump_PlayLogo(av,player);

#ifdef HDSD_TRANSPORT_TCP	
	socket_t socket_info;
	memset(&socket_info,0,sizeof(socket_t));
	socket_tcp_server_init(&socket_info,HD_PLAYER_PORT);
#else
	unixsock socket_info;
	memset(&socket_info,0,sizeof(unixsock));
	strcpy(socket_info.szName , USOCK_HD_NAME);
	socket_unix_server_init(&socket_info);
#endif

	NEXUSAPP_Playpump_Stop(av,player,av->playpumpID);

	av->avp->uiIfHdRun = true;
	OwnControlSpdif(1);

	NEXUSAPP_Playpump_Prepare(av,player,(void *)&socket_info,&stream_info);

	while(1)
	{
		char *ptr = NULL;

		if(player->ioContext.fullname)
		{		
			ptr = strstr(player->ioContext.fullname, "AS");

			if (!ptr)
			{
				ptr = strstr(player->ioContext.fullname, "/tmp/1.mpg");
				if (!ptr)
					ptr = strstr(player->ioContext.fullname, "/a/");
			}
		}
		
		MSG_APP(("\n\nstart playpump [%d]:  start  %s playpump pthread! \n",av->playpumpID,av->playpumpID?"[SD]" : "[HD]"));


		DBG_APP(("fullname:%s, ptr:%p\n", player->ioContext.fullname, ptr));


		/*check if not user added songs*/
		if(ptr)
		{
			memset(stream_info.szSongName, 0x00 ,sizeof(stream_info.szSongName));
			strcpy(stream_info.szSongName , player->ioContext.fullname);

			DBG_APP(("playback filename:%s\n", stream_info.szSongName));

			/*	close my io */
			if(player->ioContext.priv_data && av->avp->uiIfHdRun)
				player->myIo->url_close(&player->ioContext);
			
			if (!NEXUSAPP_Playback_Start(av,player,&stream_info))
			{
				NEXUSAPP_Playback_Stop(av,player,av->playpumpID);
			}			
			
			NEXUSAPP_Playpump_Prepare(av,player,&socket_info,&stream_info);	
		}
		else
		{
			NEXUSAPP_Playpump_Start(av,player,&stream_info);

			NEXUSAPP_Playpump_Play(av,player);		
			NEXUSAPP_Playpump_Stop(av,player,av->playpumpID);
			NEXUSAPP_Playpump_Prepare(av,player,&socket_info,&stream_info);

		}

	}	
	
	DBG_APP(("HD playpump thread out!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));

	return NULL;
		
}


/*************************************************
  Function:    	NEUXSAPP_SystemFile_Creat
  Description:	Create a file to recode nexus app process pid. It will be read by daemon process. 
  			This function must be call begin of application start.
  Input:		  			
  Output:		
  Return:
  other : 		
*************************************************/
void NEUXSAPP_SystemFile_Creat(void)
{
	FILE *fp=NULL;
	char szBuf[100];
	
	fp = fopen(PID,"wb");
	if(fp == NULL)
	{
		return ;
	}
	memset(szBuf,0,sizeof(szBuf));
	sprintf(szBuf,"%d",getpid());
	fwrite(szBuf,1,strlen(szBuf),fp);
	fclose(fp);
}

/*************************************************
  Function:    	NEXUSAPP_Platform_Init
  Description:	nexus platform init.
  Input:		1 . struct av_playpump_t *avPlayer		:  playpump  info struct
  Output:		
  Return:
  other : 		
*************************************************/
void NEXUSAPP_Platform_Init(av_playpump *avPlayer)
{
	NEXUS_PlatformSettings platformSettings;
	
	NEXUS_Platform_GetDefaultSettings(&platformSettings);
	platformSettings.openFrontend = false;
	platformSettings.heap[3].memcIndex=0;
	platformSettings.heap[3].size=80*1024*1024;
	NEXUS_Platform_Init(&platformSettings);
	NEXUS_Platform_GetConfiguration(&avPlayer->platformConfig);

	/* mody by hh ,add 480i 1080i 1080p set*/
	switch(avPlayer->framebuffIndex)
	{
		case 0:
			MyInit(13, 2);	/* 1080p*/
			break;
		case 2:
			MyInit(3, 2);		/* 480i */
			break;
		default:
			MyInit(0, 2);		/* 1080i */
			break;
	}
}

/*************************************************
  Function:    	NEXUSAPP_SystemInfo_Init
  Description:	nexus application system init.
  Input:		1 . struct av_playpump_t *avPlayer		:  playpump  info struct
  Output:		
  Return:
  other : 		
*************************************************/
void NEXUSAPP_SystemInfo_Init(av_playpump *avPlayer)
{
	
	
	NEXUSAPP_Event_Creat(avPlayer);
	
	NEXUSAPP_Display_Init(avPlayer,PLAYER_PLAYPUMP_HD);
	NEXUSAPP_Display_Init(avPlayer,PLAYER_PLAYPUMP_SD);
	
	NEXUSAPP_Windows_Init(avPlayer);

	NEXUSAPP_Surface_DefaultCreate(avPlayer);

	NEXUSAPP_Hdmihotplug_Reset(avPlayer);

	NEXUSAPP_AudioDecoder_Init(avPlayer);

	return;
}
/*************************************************
  Function:    	NEXUSAPP_SystemInfo_UnInit
  Description:	NULL, no used !.
  Input:		
  Output:		
  Return:
  other : 		
*************************************************/
void NEXUSAPP_SystemInfo_UnInit(av_playpump * avPlayer)
{
	int i = 0;
	
	if(avPlayer->hdPlayer.videoDecoder)
		NEXUS_VideoDecoder_Stop(avPlayer->hdPlayer.videoDecoder);

	if(avPlayer->hdPlayer.playpump)
		NEXUS_Playpump_Stop(avPlayer->hdPlayer.playpump);

	if(avPlayer->hdPlayer.playpump)
		NEXUS_Playpump_Close(avPlayer->hdPlayer.playpump);

	DBG_APP(("%s: NEXUSAPP_Playpump_Stop  HD ok !\n",__FUNCTION__));

	if(avPlayer->sdPlayer.videoDecoder)
		NEXUS_VideoDecoder_Stop(avPlayer->sdPlayer.videoDecoder);

	if(avPlayer->sdPlayer.playpump)
		NEXUS_Playpump_Stop(avPlayer->sdPlayer.playpump);

	if(avPlayer->sdPlayer.playpump)
		NEXUS_Playpump_Close(avPlayer->sdPlayer.playpump);

	DBG_APP(("%s: NEXUSAPP_Playpump_Stop  SD ok !\n",__FUNCTION__));

	if(avPlayer->audioDecoder)
		NEXUS_AudioDecoder_Stop(avPlayer->audioDecoder);
	
	if(avPlayer->audio2Decoder)
		NEXUS_AudioDecoder_Stop(avPlayer->audio2Decoder);	

	if(avPlayer->audioplayer.audioDecoder)
		NEXUS_AudioDecoder_Stop(avPlayer->audioplayer.audioDecoder);

	NEXUSAPP_Playpump_Windows_Disconnect(&avPlayer->hdPlayer);
	NEXUSAPP_Playpump_Windows_Disconnect(&avPlayer->sdPlayer);

	NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioDac_GetConnector(avPlayer->platformConfig.outputs.audioDacs[0]));
	NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(avPlayer->audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));

	if(avPlayer->audioDecoder)
		NEXUS_AudioDecoder_Close(avPlayer->audioDecoder);
	
	if(avPlayer->audio2Decoder)
		NEXUS_AudioDecoder_Close(avPlayer->audio2Decoder);
	
	if(avPlayer->audioplayer.audioDecoder)
		NEXUS_AudioDecoder_Close(avPlayer->audioplayer.audioDecoder);	

	if(avPlayer->hdPlayer.videoDecoder)
		NEXUS_VideoDecoder_Close(avPlayer->hdPlayer.videoDecoder);

	if(avPlayer->sdPlayer.videoDecoder)
		NEXUS_VideoDecoder_Close(avPlayer->sdPlayer.videoDecoder);

	if(avPlayer->display.winHd_0)
		NEXUS_VideoWindow_Close(avPlayer->display.winHd_0);

	if(avPlayer->display.winHd_1)
		NEXUS_VideoWindow_Close(avPlayer->display.winHd_1);

	if(avPlayer->display.winSd_0)
		NEXUS_VideoWindow_Close(avPlayer->display.winSd_0);

	if(avPlayer->gfx)
		NEXUS_Graphics2D_Close(avPlayer->gfx);


	for(i  = 0 ; i < 7 ; i++)
	{
		if(avPlayer->framebuff[i].surface)
			NEXUS_Surface_Destroy(avPlayer->framebuff[i].surface);
	}

	if(avPlayer->display.display_hd)
		NEXUS_Display_Close(avPlayer->display.display_hd);

	if(avPlayer->display.display_sd)
		NEXUS_Display_Close(avPlayer->display.display_sd);

	DBG_APP((" close all app succ! \n "));

	NEXUS_Platform_Uninit();

}
/***********************************************************************************************/
int NEXUSAPP_Debug_State_Get(void)
{
	return nexus_debug_opt;
}

void NEXUSAPP_Debug_State_Set(int state)
{
	nexus_debug_opt = state;
}

/***********************************************************************************************/
int HM_Nexus_Process_Pid_Get(char *path)
{
	FILE *fp = NULL;
	char szBuf[40];
	int iPid = 0;	

	if(access(path,F_OK))
		return -1;
	
	fp = fopen(path,"rb");
	if(!fp)
		return -1;
	
	fseek(fp,0,0);
	memset(szBuf,0x00,sizeof(szBuf));
	if(fread(szBuf,1,sizeof(szBuf),fp)<=0)
		return -1;
	
	iPid = atoi(szBuf);

	return iPid;	
}

void gpio_interrupt(void *context, int param)
{
	BSTD_UNUSED(context);
	BSTD_UNUSED(param);
	
	DBG_APP(("got interrupt\n"));

	return;	
}

static void *stanby_thread(void *context)
{
	NEXUS_GpioHandle pin;
	NEXUS_GpioSettings gpioSettings;
	unsigned key_pinNum = 3;	
	NEXUS_GpioStatus gpioStatus;
	NEXUS_PlatformStandbySettings tTempNexusStandbySettings;

	BSTD_UNUSED(context);

	NEXUS_Gpio_GetDefaultSettings(NEXUS_GpioType_eAonStandard, &gpioSettings);
	gpioSettings.mode = NEXUS_GpioMode_eInput;
	gpioSettings.interruptMode = NEXUS_GpioInterrupt_eRisingEdge;
	gpioSettings.interrupt.callback = gpio_interrupt;
	pin = NEXUS_Gpio_Open(NEXUS_GpioType_eAonStandard, key_pinNum, &gpioSettings);

	while(1)
	{	
		NEXUS_Gpio_GetStatus(pin, &gpioStatus);
		if(0 == gpioStatus.value)
		{
			BKNI_Sleep(20);

			NEXUS_Gpio_GetStatus(pin, &gpioStatus);
			if (0 == gpioStatus.value)
			{
				while (0 == gpioStatus.value)
				{
					BKNI_Sleep(100);

					NEXUS_Gpio_GetStatus(pin, &gpioStatus);
				}
				
				NEXUS_Platform_GetStandbySettings(&tTempNexusStandbySettings);
				tTempNexusStandbySettings.wakeupSettings.gpio = true;
				NEXUS_Platform_P_SetWakeupDevices(&tTempNexusStandbySettings);

				
				DBG_APP(("StandBy...\n"));
				system("sync");
				system("echo 1 > /sys/devices/platform/brcmstb/halt_mode");
				system("halt");

				break;				
			}			
		}

		BKNI_Sleep(500);
	}

	return (void*)0;
}

#if 0
static void *reboot_thread(void *context)
{
	int count=0;
	
	BSTD_UNUSED(context);

	while(1)
	{
		BKNI_Sleep(60000);
		count++;
		if(count >=5)
		{
			system("reboot");
			break;
		}
	}
	
	return (void*)0;
}
#endif

/*
	NEXUSAPP_If_Song_Exist:
	check if the song exist before delete action
	input: fileno 
	return value: 1 file exist 0 file not exist
				
*/
int NEXUSAPP_If_Song_Exist(const char* fileno)
{
	char buf[512];
	char pcmd[512];
	FILE *fp;
	int ret;
	
	memset(pcmd, 0, sizeof(pcmd));
	memset(buf, 0, sizeof(buf));
	sprintf(pcmd, "find /mnt/song_A /mnt/song_B /mnt/song_C /mnt/song_D /mnt/song_E /mnt/song_F /mnt/song_G -type f -name %s* ", fileno);

	fp = popen(pcmd, "r");
	if (fp)
	{
		ret = fread(buf, 1, 50, fp);
		if (ret)
		{
			pclose(fp);
			return 1;
		}
	}

	return 0;
}

/*
	NEXUSAPP_Delete_Song:
	delete song from disk
	input: fileno
	return value: 0 command exec succ -1 command fail
*/

int NEXUSAPP_Delete_Song(const char* fileno)
{
	char cmdbuf[512];

	memset(cmdbuf, 0, sizeof(cmdbuf));
	sprintf(cmdbuf, "find /mnt/song_A /mnt/song_B /mnt/song_C /mnt/song_D /mnt/song_E /mnt/song_F /mnt/song_G -type f -name %s* |xargs rm -f", fileno);

	return system(cmdbuf);

}

int notify(int iAuthMode, HM_ATSHA204_AUTH_RESULT_S* pstAuthResult)
{
	DBG_APP(("iAuthMode = %d, AuthResult = %d, iVersioning=%d \n", iAuthMode, pstAuthResult->iAuthResult, pstAuthResult->iVersioning));

	if (pstAuthResult->iAuthResult == HM_SECURITY_SUCC)
	{
		printf("security check passed\n");
		g_secure_pass_flag = 1;
		g_iVersion = pstAuthResult->iVersioning;
	}
		
	return 0;
}

static int NEXUSAPP_Security_Init(void)
{
	unsigned int uiResult = 0;
	HM_SECURITY_INIT_S stInit;

	memset(&stInit, 0x00, sizeof(HM_SECURITY_INIT_S));

	/* 循环的安全认证模式 */
	
	stInit.stAtsha204Init.iAuthMode = HM_ATSHA204_AUTH_MODE_CYCLE;

	/* 安全认证结果通知 函数钩子 */
	stInit.stAtsha204Init.pfuncNotify = notify;
	
	uiResult = hm_security_init(&stInit);
	
	DBG_APP(("%s,  result = %d\r\n", __FUNCTION__, uiResult));

	return uiResult;

}

static int NEXUSAPP_ClearLogFile(void)
{
	char cmd[512];

	sprintf(cmd, "filelist=$(ls /mnt/system/log-* > /dev/null 2>&1);for i in $filelist; do [ $i != '/mnt/system/log-%s' ] && rm -f $i;done", g_current_date);
	return system(cmd);
}

#if 0
static int NEXUSAPP_TestLogFile(void)
{
	char cmd[512];

	sprintf(cmd, "echo `date` >> /root/testlog;cat /proc/mounts |grep /dev/sda1 >> /root/testlog; echo `ls -lah /mnt/system/data/song.tab` >> /root/testlog");
	return system(cmd);
}
#endif

/*************************************************
  Function:    	NEXUSAPP_Ctrl_Start
  Description:	accept new client for ctrl center .
  Input:		1 . av_playpump *avp	:  playpump player info struct	
  Output:						
  Return:		-1	: error out
  			0	: succ out
  			1	: no found hplay
  other : 		
*************************************************/
int HM_Nexus_Player_Init(av_playpump *avPlayer)
{
	
	int iPid = 0 , iErrorCount = 0;
	pthread_t threadHd;
	pthread_t threadOsd;

	pthread_attr_t attr;
	
	int rc;
	pthread_t standby;

	putenv("force_vsync=y");
    	putenv("sync_disabled=1");

	DBG_APP(("%s\n", HMPLAYER_VERSION));

	signal(SIGPIPE, signalTransportProc);
	signal(EFFECT_k9001_SIGNED, signalTransportProc);
	signal(PLAYER_DEBUG_SIGNED, signalTransportProc);
	
	NEUXSAPP_SystemFile_Creat();

	NEXUSAPP_Plaupump_Ready(avPlayer);

	/* check k9001 effect dev */
	NEXUSAPP_Effect_Check(avPlayer);

	NEXUSAPP_Playpump_Pthread_Init(avPlayer);

	NEXUSAPP_Platform_Init(avPlayer);
	
	rc = NEXUSAPP_Security_Init();
	if (rc != HM_SECURITY_SUCC)
		return -1;

	NEXUSAPP_SysTime_Setup();

	NEXUSAPP_ClearLogFile();

	adddisk_semid=sem_open("/adddisk", O_RDWR);	
	if (SEM_FAILED == adddisk_semid)
	{
		DBG_APP(("fail to get adddisk sem\n"));
		return -1;
	}

	rmdisk_semid=sem_open("/rmdisk", O_RDWR);
	if (SEM_FAILED == rmdisk_semid)
	{
		DBG_APP(("fail to get rmdisk sem\n"));
		return -1;
	}

	song_searched_semid=sem_open("/songsearch", O_RDWR);	
	if (SEM_FAILED == song_searched_semid)
	{
		DBG_APP(("fail to get song searched sem\n"));
		return -1;
	}

	song_finish_semid=sem_open("/songfinish", O_RDWR);	
	if (SEM_FAILED == song_finish_semid)
	{
		DBG_APP(("fail to get song finished sem\n"));
		return -1;
	}

#if 0
	rc = sqlite3_open("/mnt/system/data/song_total.db", &db);
	if (rc != SQLITE_OK)
	{
		printf("open sqlite database fail\n");
	}
#endif

#if 0
	rc = sqlite3_exec(db, "attach 'song_total.db' as song_total", NULL, 0, 0);
	if (rc != SQLITE_OK)
	{
		printf("[%s-%d]sqlite3_exec fail\n", __FUNCTION__, __LINE__);
	}
#endif

#if 0
	rc = sqlite3_get_table(db, "select * from SONG",  &selectResult, &nResult, &mResult, 0);
	if (rc != SQLITE_OK)
	{
		printf("[%s-%d]sqlite3_exec fail\n", __FUNCTION__, __LINE__);
	}

	if(nResult > 0)
	{
		printf("nResult:%d\n", nResult);
	}

	rc = sqlite3_close(db);
	if (rc != SQLITE_OK)
	{
		printf("sqlite database close fail\n");
	}
#endif

	NEXUSAPP_SystemInfo_Init(avPlayer);

	NEXUSAPP_StcChannel_Init(avPlayer,PLAYER_PLAYPUMP_HD);
	NEXUSAPP_StcChannel_Init(avPlayer,PLAYER_PLAYPUMP_SD);
	
	NEXUSAPP_VideoDecoder_Open(avPlayer,PLAYER_PLAYPUMP_HD);
	NEXUSAPP_VideoDecoder_Open(avPlayer,PLAYER_PLAYPUMP_SD);

	NEXUSAPP_Playpump_Windows_Connect(&avPlayer->hdPlayer);
	NEXUSAPP_Playpump_Windows_Connect(&avPlayer->sdPlayer);
	NEXUSAPP_AudioPlayer_Ready(avPlayer);


/*  HD PLAYER THREAD */
	lib_hdThreadContext.avp = avPlayer;
	lib_hdThreadContext.playpumpID = PLAYER_PLAYPUMP_HD;

	pthread_attr_init (&attr);
	pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&threadHd,&attr,NEXUSAPP_Start_Hd_Pthread,&lib_hdThreadContext);
	pthread_attr_destroy (&attr);

	NEXUSAPP_DPlayer_Ready(avPlayer);

/*  OSD THREAD */
	pthread_attr_init (&attr);
	pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&threadOsd,&attr, NEXUSAPP_Osd_Start_Pthread,avPlayer);
	pthread_attr_destroy (&attr);

/*  PIZ PLAYER THREAD */
	NEXUSAPP_PizPlayer_Ready(avPlayer);

/*  CTRL SEC THREAD */
	NEXUSAPP_Ctrl_Sec_Init(&lib_ctrl_sec_SocketInfo);
/*  MODY BY HH , pthread paremter protect */
	lib_ctrl_sec_info.avp = avPlayer;
	lib_ctrl_sec_info.socket_info = &lib_ctrl_sec_SocketInfo;
	NEXUSAPP_Ctrl_Sec_Start(&lib_ctrl_sec_info);

/*	CTRL THREAD */
	NEXUSAPP_Ctrl_Init(&lib_ctrl_SocketInfo);
	lib_ctrl_info.start = false ;
	lib_ctrl_info.avp = avPlayer;
	lib_ctrl_info.socket_info = &lib_ctrl_SocketInfo;
	
	NEXUSAPP_Ctrl_Start(&lib_ctrl_info);
	
	rc = pthread_create(&standby, NULL, stanby_thread, NULL);
	if (rc != 0)
		DBG_APP(("!!!!stanby_thread err, ret:%d\n", rc));

	while(!lib_ctrl_info.start && iErrorCount < 10)
	{
		sleep(1);
		iErrorCount ++;
	}

	/* must be cloese all pthread out !!!!*/
	if(!lib_ctrl_info.start)
		goto api_error_out;
	
	iPid = HM_Nexus_Process_Pid_Get(MAINPID);

	if(iPid <= 0 )
		return 1;

	kill(iPid,HPLAY_PID_SIGNED);
	kill(iPid,HPLAY_PID_SIGNED);

	DBG_APP(("%s: Aplication thread init succ !\n",__FUNCTION__));

    	return 0;

api_error_out:

	unlink(PID);
	
	NEXUSAPP_Playpump_Static_Set(avPlayer,PLAYER_PLAYPUMP_HD,PLAYER_CTRL_EXIT);
	NEXUSAPP_Playpump_Static_Set(avPlayer,PLAYER_PLAYPUMP_SD,PLAYER_CTRL_EXIT);
	
	DBG_APP(("%s: Aplication destroy thread!\n",__FUNCTION__));
	
	pthread_kill(threadHd,SIGNAL_EXIT);
	pthread_kill(threadOsd,SIGNAL_EXIT);

	pthread_join(threadHd,NULL);
	pthread_join(threadOsd,NULL);

	NEXUSAPP_SystemInfo_UnInit(avPlayer);

	memset(avPlayer,0x00,sizeof(av_playpump));

	return -1;
	
}

#endif

