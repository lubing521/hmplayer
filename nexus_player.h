#ifndef NEXUS_PLAYER_H
#define NEXUS_PLAYER_H
/***********************************************************************************************/
/*nexus header*/
#include "nexus_platform.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_audio_input.h"
#include "nexus_component_output.h"
#include "nexus_playpump.h"

#include "nexus_playback.h"
#include "nexus_audio_mixer.h"
#include "nexus_file.h"

#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_output.h"
#include "nexus_core_utils.h"
#include "nexus_graphics2d.h"
#include "nexus_surface.h"
#include "nexus_display.h"
#include "cmdline_args.h"
/***********************************************************************************************/
/*system header*/
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
/***********************************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
/***********************************************************************************************/
/*app header*/
#include <pthread.h>
#include "nexus_bmz.h"
#include "nexus_xpiz.h"
#include "nexus_transport.h"
#include "nexusio_mftp_io.h"
/***********************************************************************************************/
#define SIGNAL_EXIT	60
#define EFFECT_k9001_SIGNED	91
#define PLAYER_DEBUG_SIGNED	98
#define PID "/mnt/ramdisk/HMPLAY.pid"
#define PARA_ASSERT(cond)  {if(!cond) return;}
#define PARA_ASSERT_VALUE(cond,x)  {if(!cond) return x;}
/***********************************************************************************************/
#define DEBUG 1
#define MSGDEBUG 1
#define DISPLAYMSG 1
#define OSDDEBUG 0
#define IODEBUG 1
#define AUDIOPLAYERDEBUG 0
/***********************************************************************************************/
#define MSG_DISPLAY(x)  {if(DISPLAYMSG)printf x;}
#define MSG_APP(x)  {if(MSGDEBUG)printf x ;}
#define DBG_APP(x)  {if(DEBUG)printf x ;}
/*#define DBG_APP(x)  {if(DEBUG)nexus_debug_sock_send x ;}*/
#define DBG_OSD(x)  {if(OSDDEBUG)printf x ;}
#define DBG_AUDIO_PLAYER(x)  {if(AUDIOPLAYERDEBUG)printf x ;}
#define DBG_IO(x)  {if(IODEBUG)nexus_debug_sock_send x ;}
/***********************************************************************************************/
#define HD_PLAYER_PORT 65535
#define SD_PLAYER_PORT 65534
#define HD_SD_PLAYER_CTCL_PORT 65533
#define SEC_PLAYER_CTCL_PORT 65532
/***********************************************************************************************/
#define MAX_READ (188*1024)
#define PLAYER_USERDATA_SIZE (3*1024*1024)
/***********************************************************************************************/
#define PLAYER_PLAYPUMP_HD	0
#define PLAYER_PLAYPUMP_SD	1
/***********************************************************************************************/
/*player ctrl cmd*/
#define PLAYER_CTRL_PLAY 						0  	/*	 play 					*/
#define PLAYER_CTRL_PAUSE						1  	/*	 pause					*/
#define PLAYER_CTRL_STOP 						2  	/*	 stop					*/
#define PLAYER_CTRL_VOICE_VOLUME_SET			3  	/*	 voice volume ctrl			*/
#define PLAYER_CTRL_AUDIO_CHANNEL_SET		4  	/*	 audio channel ctrl			*/
#define PLAYER_CTRL_AUDIO_MUL_CHANNEL_SET	5  	/*	 audio mulit channel switch	*/
#define PLAYER_CTRL_AUDIO_SOURCE_SET			6  	/*	 audio stream input select	*/
#define PLAYER_CTRL_DISPLAY_MODE				7  	/*	 display mode	 			*/
#define PLAYER_CTRL_PIP_WINDOW_VISIBLE		8  	/*	 pip window visible			*/
#define PLAYER_CTRL_PIP_WINDOW_INFO			9	/*	 pip window info			*/
#define PLAYER_CTRL_PLAY_END					200  	/*	 play end					*/
#define PLAYER_CTRL_PLAY_READY				201  	/*	 player ready				*/
#define PLAYER_CTRL_PLAY_TRICKMODE			202		/*    play trickmode */
/***********************************************************************************************/
/*osd ctrl cmd*/
#define PLAYER_CTRL_OSD_PIZ					10	/*	 osd piz ctrl 				*/
#define PLAYER_CTRL_OSD_TEXT_STATIC			11	/*	 osd text static display		*/
#define PLAYER_CTRL_OSD_TEXT_ROLL				12	/*	 osd text roll display		*/
#define PLAYER_CTRL_OSD_PIC_FLASH				13	/*	 osd pic flash				*/
#define PLAYER_CTRL_EXIT						14

/*player ctrl cmd add*/
#define PLAYER_CTRL_PLAYER_SILENT				15  	/*	 player silent				*/

/*player ctrl mw cmd add 2011_01_19 */
#define PLAYER_CTRL_MWALL_PICSET				16  	/*	 media wall pic send to tmp famebuff	*/
#define PLAYER_CTRL_MWALL_DFSET				17  	/*	 media wall default rect set			*/
#define PLAYER_CTRL_MWALL_DFDISPLAY			18  	/*	 media wall display default 			*/
#define PLAYER_CTRL_MWALL_PBLIT				19  	/*	 media wall blit rect copy			*/
#define PLAYER_CTRL_MWALL_RETURN				20  	/*	 media wall blit return last pic			*/
#define PLAYER_CTRL_MWALL_ClEAN				21  	/*	 media wall clean					*/
#define PLAYER_CTRL_MAX						202  	/*	 CTRL CMD MAX					*/
/***********************************************************************************************/
#define PLAYER_AUDIO_VOLUME_DB_MAX 		100
#define PLAYER_AUDIO_VOLUME_DB_NORMAL 	100
#define PLAYER_AUDIO_VOLUME_DB_MIN 		0
/***********************************************************************************************/
typedef enum PLAYER_PipWindwos_Show
{
	PLAYER_Pip_eShow = 0,		 	/* show pip window */
	PLAYER_Pip_eClose,		 	/* close pip window */
	PLAYER_Pip_eMax
} PLAYER_PipWindwos_Show;
/***********************************************************************************************/
typedef enum PLAYER_AudioSource
{
	PLAYER_AudioSource_eHd = 0,		 	/* Select Hd steam audio source output*/
	PLAYER_AudioSource_eSd,		 		/* Select Sd steam audio source output*/
	PLAYER_AudioSource_eMax
} PLAYER_AudioSource;
/***********************************************************************************************/
 typedef enum PLAYER_AudioChannelMode
{
	PLAYER_AudioChannelMode_eStereo =0 ,  	/* Normal ordering, left-> left, right->right */
	PLAYER_AudioChannelMode_eLeft,    		/* Duplicate left channel on left & right */
	PLAYER_AudioChannelMode_eRight,   		/* Duplicate right channel on left & right */
	PLAYER_AudioChannelMode_eSwapped, 	/* Reverse ordering, left->right, right->left */
	PLAYER_AudioChannelMode_eMax
} PLAYER_AudioChannelMode;
/***********************************************************************************************/
typedef struct av_stream_info_t
{
	int iFlag;
	int transportType;
	int videoCodec;
	int audioCodec;
	int bitRate;		/*  actually bitRate / 1024  */
	int ifHd;
	int trackMode;
	unsigned int pid_video;
	unsigned int pid_audio_1;
	unsigned int pid_audio_2;
	unsigned int pid_audio_3;
	unsigned int pid_audio_4;
	unsigned int pid_audio_5;
	char szSongName[128];
}av_stream_info;
/***********************************************************************************************/
typedef struct av_pidChannel_t
{
	unsigned int pid;
	NEXUS_PidChannelHandle pidChannel;
}av_pidChannel;
/***********************************************************************************************/
typedef struct av_player_t
{
	NEXUS_PlaypumpHandle playpump;
	NEXUS_PlaybackHandle playback;
	NEXUS_FilePlayHandle file;
	NEXUS_TransportType transportType;
	NEXUS_VideoDecoderHandle videoDecoder;
	NEXUS_VideoDecoderStartSettings videoProgram;
	NEXUS_AudioDecoderStartSettings audioProgram;
	NEXUS_AudioDecoderStartSettings audio2Program;
	NEXUS_StcChannelHandle stcChannel;
	NEXUS_VideoWindowHandle win_0;
	NEXUS_VideoWindowHandle win_1;
	BKNI_EventHandle Event;
	pthread_mutex_t lock;

	av_pidChannel videoPidChannel;
	av_pidChannel audioPidChannel[5];
	
	NEXUSIOProtocol *myIo;
	nexusIoContext ioContext;
	
	int socketFd;
	int state;
	int rate;
	int playerId;
	unsigned int depthCtrl;
	unsigned int uiLastdepth;
	unsigned int uiDisplay3DMode;
	unsigned int uiloopCount;
	bool decoderStart;
	bool audioSelected;
	int audiomode;
	int ifcompressedAudio;
}av_player;

typedef struct audio_player_t
{
	NEXUS_PlaypumpHandle playpump;
	NEXUS_PlaybackHandle playback;
	NEXUS_StcChannelHandle stcChannel;
	NEXUS_AudioDecoderHandle audioDecoder;
	NEXUS_AudioDecoderStartSettings audioProgram;
	NEXUS_PidChannelHandle audioPidChannel;
	NEXUS_FilePlayHandle file;
	bool start;
}audio_player;
/***********************************************************************************************/
typedef struct xxpiz_info_t
{
	int state;
	unsigned int pizSize;
	unsigned int pizRecSize;
	NEXUS_Rect newPosition;
	NEXUS_Rect position;
	char *pizBuf;
	char pizPath[80];
	struct nexus_xxpiz_t axpiz;
}xxpiz_info;
/***********************************************************************************************/
typedef struct av_osdPiz_t
{
	xxpiz_info pizHd;
	xxpiz_info pizSd;
	pthread_mutex_t pizLock;
}av_osd_piz;
/***********************************************************************************************/
typedef struct av_osd_piz_player_t
{
	FILE *fp;
	int iState;
	unsigned int uiIndex;
	unsigned int uiSize;
	unsigned int uiRecSize;
	NEXUS_Rect position;
	char szName[48];
	pthread_mutex_t pizPlayerLock;
}av_osd_piz_player;
/***********************************************************************************************/
typedef struct av_osd_text_t
{
	int state;
	int fbId;
	NEXUS_Rect position;
	unsigned short textWidth;
	unsigned short textHeight;
	t_osd_picture_ragb pic;
	unsigned int compressType;
	unsigned int picSize;
	unsigned int picRecSize;
	unsigned int picDisplayMode;	
	char szContext[1024];
	pthread_mutex_t staticLock;
}av_osd_text;
/***********************************************************************************************/
typedef struct av_osd_text_roll_t
{
	int state;
	NEXUS_Rect sourceRect;
	NEXUS_Rect destRect;
	NEXUS_Rect displayRect;
	NEXUS_Rect bufferRect;
	unsigned short pixStep;
	unsigned short textWidth;
	unsigned short maxWidth;
	t_osd_picture_ragb pic;
	NEXUS_Rect position;
	unsigned int picWidth;
	unsigned int picHeight;
	unsigned int compressType;
	unsigned int picRecSize;
	unsigned int picSize;
	unsigned int displayCount;
	unsigned int NewCount;
	pthread_mutex_t RollLock;
}av_osd_text_roll;
/***********************************************************************************************/
typedef struct av_osd_mw_t
{
	int mwState;
	NEXUS_Rect default_rect;
	pthread_mutex_t mwStateLock;
	pthread_mutex_t mwActionLock;
}av_osd_mw;
/***********************************************************************************************/
typedef struct framebuff_t
{
	NEXUS_SurfaceHandle surface;
	NEXUS_SurfaceMemory mem;
	NEXUS_SurfaceCreateSettings createSettings;
}fbuff;
/***********************************************************************************************/
typedef struct osd_t
{
	av_osd_piz 	piz;
	av_osd_mw	mw;
	av_osd_text textStatic;
	av_osd_text_roll 	textRoll;
	av_osd_piz_player	pizPlayer;
	pthread_mutex_t osdLock;
}osd_node;
/***********************************************************************************************/
typedef struct display_t{
	int mode;
	bool pipVisible;
	NEXUS_Rect pipInfo;
	NEXUS_DisplayHandle display_hd;
	NEXUS_DisplayHandle display_sd;
	NEXUS_VideoWindowHandle winHd_0;
	NEXUS_VideoWindowHandle winHd_1;
	NEXUS_VideoWindowHandle winSd_0;
	NEXUS_VideoWindowHandle winSd_1;
}display_info;
/***********************************************************************************************/
typedef struct av_playpump_t
{
	NEXUS_PlatformConfiguration platformConfig;
	NEXUS_AudioDecoderHandle audioDecoder;
	NEXUS_AudioDecoderHandle audio2Decoder;/*lidj:added 20120915*/
	NEXUS_AudioMixerHandle mixer;

	bool audioDecoderStart;
	bool audio2DecoderStart;

	/* note audio silent state */
	int audioSilent;

	int audioVolume;
	int audioChangeState;
	
	/* use audio silent spdif out conctrl */
	/* when stop playpump ,set ctrl flag ,next play check it*/
	int audioSilentCtrl;
	
	NEXUS_Graphics2DHandle gfx;
	NEXUS_Graphics2DFillSettings fillSettings;
	NEXUS_Graphics2DSettings graphics2DSettings;
	NEXUS_Graphics2DBlitSettings blitSettings;
	BKNI_EventHandle g2dEvent;
	pthread_mutex_t g2dFillLock;

	/*add by LQQ; Date:2011-11-09*/
	void (*Hdmihotplug_callback)(void *pParam, int iParam);

	/* use for vga mode , 0 = 1080i+720p , 1 = 480+480+720*/
	int framebuffIndex;
	int framebuffFlag;
	
	fbuff framebuff[7];
	osd_node osd;
	
	av_player 	hdPlayer;
	av_player 	sdPlayer;
	audio_player	audioplayer;
	display_info display;

	
	bool ctrlPthreadStart;
	bool ctrlSecPthreadStart;
	int socketCtrlFd;
	int socketCtrlSecFd;
	pthread_mutex_t ctrlLock;
	pthread_mutex_t playerLock;
	pthread_mutex_t playerPizLock;
	
	bool uiWinNorm;
	bool uiIfHdRun;	
}av_playpump;
/***********************************************************************************************/
typedef struct av_pthread_context_t
{
	int playpumpID;
	av_playpump *avp;
}av_pthread_context;
/***********************************************************************************************/
typedef struct ctrl_start_info_t
{
	av_playpump *avp;
	socket_t *socket_info;
	bool start;
}ctrl_start_info;
/***********************************************************************************************/
int  HM_Nexus_Player_Init(av_playpump *avPlayer);
int 	NEXUSAPP_Debug_State_Get(void);
void NEXUSAPP_Debug_State_Set(int state);
void NEXUSAPP_Effect_Set(int mode);
void NEXUSAPP_Signal_Exit(int sig);
void NEXUSAPP_SystemInfo_Init(struct av_playpump_t *avPlayer);
void NEXUSAPP_SystemInfo_UnInit(struct av_playpump_t * avPlayer);
int NEXUSAPP_Find_Stream_Info(char *filename);/*check stream info for adding song, input: filename, return num of audio stream*/
int NEXUSAPP_Find_Outside_SongDisk(void);
int NEXUSAPP_Search_Songs(void);
int NEXUSAPP_Standby(void);
int NEXUSAPP_Ctrl_DisplayMode_Set(struct av_playpump_t *avp,int mode);
int NEXUSAPP_DisplayMode_Set(int mode);
void NEXUSAPP_Playpump_Codec_Get(char *fname , av_stream_info *streamInfo );
int  	NEXUSAPP_Playpump_Codec_Set(av_player *player , av_stream_info *streamInfo );
int NEXUSAPP_Util_Opts_Init(struct util_opts_t *opts);
int NEXUSAPP_SetTime(unsigned char *pInputArray);
int NEXUSAPP_GetTime(unsigned char *pOutputArray);
void NEXUSAPP_SysTime_Setup(void);
int NEXUSAPP_PreRecord(void);
int NEXUSAPP_PostRecord(void);


/***********************************************************************************************/
#endif

