/******************************************************************************
 *    (c)2008-2012 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: cmdline_args.h $
 * $brcm_Revision: 48 $
 * $brcm_Date: 1/19/12 11:51a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/utils/cmdline_args.h $
 * 
 * 48   1/19/12 11:51a erickson
 * SW7346-629: add -cmp_crc and -avd_crc options
 * 
 * 47   1/11/12 6:13p vsilyaev
 * SW7425-2112: Added option to limit playback rate
 * 
 * 46   12/20/11 1:43p vsilyaev
 * SW7425-2034: Used 32-bit type for program id (pid) types
 * 
 * 45   8/18/11 5:23p vsilyaev
 * SW7425-651: Removed deprecated control over the encoder delay
 * 
 * 44   7/13/11 8:39p vsilyaev
 * SWDTV-7897: Added code to wrap PCM file with the WAV header
 * 
 * 43   7/7/11 5:53p jtna
 * SW7425-869: added runtime option to override
 *  NEXUS_VideoDecoderSourceOrientation
 * 
 * 42   4/22/11 5:49p vsilyaev
 * SW7425-416: Added option to override display orientation
 * 
 * 41   3/23/11 5:41p vsilyaev
 * SW7422-406: Unified parsing of command line parameters
 * 
 * 40   3/11/11 3:48p vsilyaev
 * SW7425-159: add encode profile/level etc settings; add clean shutdown
 *  sequence for transcoder; add sample usage for transcoder;
 * 
 * SW7425-159/1   3/9/11 8:05p hongtaoz
 * SW7425-159: add encode profile/level etc settings; add clean shutdown
 *  sequence for transcoder; add sample usage for transcoder;
 * 
 * 38   3/4/11 3:18p jgarrett
 * SW7422-146: Adding multichannel audio support
 * 
 * 37   2/22/11 3:41p vsilyaev
 * SW7425-40: adapted to new API;
 * 
 * SW7425_bringup/2   2/15/11 9:52p hongtaoz
 * SW7425-40: adapted to new API;
 * 
 * 36   12/22/10 1:15p vsilyaev
 * SW7425-39, SW7425-40: merged hdmi_input source and transcoder util
 *  test;
 * 
 * SW7425_bringup/1   12/22/10 12:42a hongtaoz
 * SW7425-39, SW7425-40: add hdmi_input source and transcoder util test;
 * 
 * 35   11/17/10 3:34p jgarrett
 * SW7408-137: Adding HDMI output options
 * 
 * 34   10/22/10 12:17p vsilyaev
 * SW3548-3106: Added option to start playback in a paused state
 * 
 * 33   9/8/10 12:05p vsilyaev
 * SW7468-129: Added video decoder on ZSP
 * 
 * SW7468-129/2   3/8/10 1:03p vsilyaev
 * SW7468-129: Added decoder index to the record options
 * 
 * SW7468-129/1   3/5/10 7:31p vsilyaev
 * SW7468-129: Added hooks for soft video decoder
 * 
 * 32   8/31/10 2:44p erickson
 * SWGIGGSVIZIO-57: add -fixed_bitrate option to set
 *  NEXUS_PlaybackMode_eFixedBitrate
 *
 * 31   8/10/10 12:14p erickson
 * SW7405-4735: merge
 *
 * SW7405-4735/1   8/9/10 3:21p jtna
 * SW7405-4735: rename opts.pids to opts.otherPids. allow pid 0 in
 *  opts.otherPids. start only one playback during allpass record.
 *
 * 30   7/14/10 6:12p vsilyaev
 * SW3556-1131: Added basic support for CDXA format
 *
 * 29   5/5/10 10:43a vsilyaev
 * SW7405-1260: Added settings for size of the audio decoder buffer
 *
 * 28   2/23/10 4:50p vsilyaev
 * SW3556-913: Added code that monitors state of the playback file and
 *  restarts playback (from last good position) in case of error
 *
 * 27   2/22/10 5:33p vsilyaev
 * SW3556-913: Added option to plug  Custom File I/O routines to inject
 *  errors
 *
 * 26   2/12/10 5:56p jtna
 * SW3556-1051: added option to control playback timestamp reordering
 *
 * 25   1/20/10 5:08p erickson
 * SW7550-159: update record util for threshold and buffer control
 *
 * 24   12/30/09 3:19p vsilyaev
 * SW7408-17: Added options to select PCM and compressed audio outputs
 *
 * 23   12/30/09 2:13p erickson
 * SW7550-128: add closed caption feature (-cc on)
 *
 * 22   12/8/09 2:31p gmohile
 * SW7408-1 : Add defines for nexus had frontend
 *
 * 21   11/25/09 5:24p katrep
 * SW7405-2740: Add support for WMA pro drc
 *
 * 20   8/18/09 4:36p vsilyaev
 * PR 56809: Added option to control handling of video errors
 *
 * 19   6/19/09 5:20p vsilyaev
 * PR 56169: Added option to set max decode rate
 *
 * 18   6/18/09 4:30p jtna
 * PR54802: add frontend support to record
 *
 * 17   6/16/09 5:13p jtna
 * PR54802: added record
 *
 * 16   6/8/09 7:06a erickson
 * PR55617: add user-specific aspectRatio
 *
 * 15   5/22/09 5:21p vsilyaev
 * PR 55376 PR 52344: Added option to enable processing of AVC(H.264)
 *  Level 5.1 video
 *
 * 14   3/18/09 10:26a erickson
 * PR52350: add wxga/fha support with 50/60 hz option
 *
 * 13   3/6/09 9:33a erickson
 * PR51743: added -ar and -graphics options, default DTV apps to panel
 *  output
 *
 * 12   2/27/09 5:05p vsilyaev
 * PR 52634: Added code to handle MPEG-2 TS streams with timesampts (e.g.
 *  192 byte packets)
 *
 * 11   2/20/09 2:06p vsilyaev
 * PR 51467: Added option to set size of the video decoder buffer
 *
 * 10   1/26/09 11:26a vsilyaev
 * PR 51579: Added stream_processing and auto_bitrate options
 *
 * 9   1/22/09 7:48p vsilyaev
 * PR 50848: Don't use globals for the command line options
 *
 * 8   1/20/09 4:28p erickson
 * PR48944: add -mad and -display_format options
 *
 * 7   1/8/09 10:34p erickson
 * PR48944: add more options
 *
 * 6   1/8/09 9:36p erickson
 * PR50757: added NEXUS_VideoFrameRate support, both as a start setting
 *  and status
 *
 * 5   1/6/09 12:45a erickson
 * PR50763: added -bof, -eof options. added playback position to status.
 *  fix mkv, mp4.
 *
 * 4   11/20/08 12:50p erickson
 * PR48944: update
 *
 * 3   11/19/08 1:30p erickson
 * PR48944: update
 *
 * 2   11/17/08 2:19p erickson
 * PR48944: update
 *
 * 1   11/17/08 12:34p erickson
 * PR48944: add utils
 *
 *****************************************************************************/
#ifndef CMDLINE_ARGS_H__
#define CMDLINE_ARGS_H__

#include "nexus_types.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_pid_channel.h"
#include "nexus_stc_channel.h"
#include "nexus_playback.h"
#include "bmedia_pcm.h"

#if NEXUS_HAS_VIDEO_ENCODER
#include "nexus_video_encoder.h"
#endif

typedef struct {
    const char *name;
    int value;
} namevalue_t;

struct common_opts_t {
    NEXUS_TransportType transportType;
    unsigned videoPid, pcrPid, audioPid, extVideoPid, audio2Pid;
    unsigned videoCdb;
    unsigned audioCdb;
    unsigned videoDecoder;
    unsigned stream_duration;/*lidj:total time of stream*/
    NEXUS_VideoCodec videoCodec;
    NEXUS_VideoCodec extVideoCodec;
    NEXUS_AudioCodec audioCodec, audio2Codec;
    NEXUS_VideoFormat displayFormat;
    NEXUS_DisplayType displayType;
    NEXUS_VideoWindowContentMode contentMode;
    NEXUS_TransportTimestampType tsTimestampType;
    NEXUS_VideoDecoderTimestampMode decoderTimestampMode;
    NEXUS_VideoFrameRate videoFrameRate;
    NEXUS_AspectRatio aspectRatio;
    NEXUS_VideoOrientation  displayOrientation;
    NEXUS_VideoDecoderSourceOrientation sourceOrientation;
    struct {
        unsigned x, y;
    } sampleAspectRatio;

    bool useCompositeOutput;
    bool useComponentOutput;
    bool useHdmiOutput;
    bool usePanelOutput;
    bool useScart1CompositeOutput;
    bool mad;
    bool compressedAudio;
    bool multichannelAudio;
    bool decodedAudio;
    bool probe;
    bool cdxaFile;
    bool detectAvcExtension;
    bool playpumpTimestampReordering;
    bool pcm;
    bpcm_file_config pcm_config;
    /* asf wma drc */
#if B_HAS_ASF
    bool dynamicRangeControlValid;
    struct {
        unsigned peakReference;
        unsigned peakTarget;
        unsigned averageReference;
        unsigned averageTarget;
    } dynamicRangeControl;
#endif
};

struct util_opts_t {
    struct common_opts_t common;
    const char *filename;
    const char *indexname;
    bool stcTrick;
    bool astm;
    bool sync;
    bool streamProcessing;
    bool autoBitrate;
    bool closedCaptionEnabled;
    bool startPaused;
    bool avc51;
    unsigned fixedBitrate; /* non-zero */
    NEXUS_StcChannelAutoModeBehavior stcChannelMaster;
    NEXUS_PlaybackLoopMode beginningOfStreamAction;
    NEXUS_PlaybackLoopMode endOfStreamAction;
    NEXUS_VideoDecoderErrorHandling videoErrorHandling;
    bool graphics;
    bool customFileIo;
    bool playbackMonitor;
    unsigned maxDecoderRate;
    unsigned maxPlaybackDataRate;
    bool avd_crc;
    bool cmp_crc;
};

#define MAX_RECORD_PIDS 16
struct util_opts_record_t {
    struct common_opts_t common;
    /* record options */
    unsigned short otherPids[MAX_RECORD_PIDS];
    unsigned numOtherPids;
    struct {
        unsigned bufferSize;
        unsigned dataReadyThreshold;
    } data, index;

    /* display */
    bool allpass;
    bool acceptNullPackets;
    bool decode;

    /* output */
    const char *recfname;
    const char *recidxname;

    /* source */
    bool streamer;
    const char *playfname;
#if NEXUS_HAS_FRONTEND
    NEXUS_FrontendVsbMode vsbMode;
    NEXUS_FrontendQamMode qamMode;
    NEXUS_FrontendSatelliteMode satMode;
    unsigned freq; /* in MHz */
#endif
#if NEXUS_HAS_HDMI_INPUT
    bool hdmiInput;
#endif
#if NEXUS_HAS_VIDEO_ENCODER
    bool encoder;
    NEXUS_VideoFormat encodeFormat;
    unsigned maxVideoBitRate;
    NEXUS_VideoFrameRate encodeFrameRate;
    bool variableFrameRate;
    unsigned gopFrameP;
    unsigned gopFrameB;
    NEXUS_VideoCodec videoTranscodec;
	NEXUS_VideoCodecProfile     videoProfile;
	NEXUS_VideoCodecLevel       videoLevel;
    NEXUS_AudioCodec audioTranscodec;
    bool                        audioEncode;
    unsigned                    audioBitRate;
    unsigned muxLatencyTolerance;
#endif

};

extern const float g_frameRateValues[NEXUS_VideoFrameRate_eMax];
extern const namevalue_t g_videoFrameRateStrs[];

/*
cmdline_parse should be called before NEXUS_Platform_Init
returns non-zero if app should exit
*/
int cmdline_parse(int argc, const char *argv[], struct util_opts_t *opts);
int cmdline_parse_record(int argc, const char *argv[], struct util_opts_record_t *opts);

/*
cmdline_probe should be called after NEXUS_Platform_Init
*/
int cmdline_probe(struct common_opts_t *opts, const char *filename, const char **indexname);

/*
print_usage can be called if cmdline_parse fails
*/
void print_usage(const char *app /* argv[0] */);
void print_usage_record(const char *app /* argv[0] */);

#endif
