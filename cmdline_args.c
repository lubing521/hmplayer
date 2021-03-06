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
 * $brcm_Workfile: cmdline_args.c $
 * $brcm_Revision: 109 $
 * $brcm_Date: 2/22/12 1:52p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/utils/cmdline_args.c $
 * 
 * 109   2/22/12 1:52p vsilyaev
 * SW7425-2258: Improved handling of MJPEG decoders
 * 
 * 108   2/15/12 11:34a erickson
 * SW7405-5610: use endptr instead of strtol to verify output
 * 
 * 107   2/14/12 12:46p vsilyaev
 * SW7425-2258: Added Motion Jpeg codec type
 * 
 * 106   2/7/12 7:34p jgarrett
 * SW7425-2136: Adding FLAC support
 * 
 * 105   2/2/12 4:47p jgarrett
 * SW7425-2268: Adding voice conferencing codecs
 * 
 * 104   1/31/12 5:07p rjlewis
 * SW7425-1136: Added HDMI support (and some cleanup).
 * 
 * 103   1/26/12 1:32p erickson
 * SW7425-2250: default to NEXUS_StcChannelAutoModeBehavior_eVideoMaster
 * 
 * 102   1/19/12 11:51a erickson
 * SW7346-629: add -cmp_crc and -avd_crc options
 * 
 * 101   1/11/12 6:13p vsilyaev
 * SW7425-2112: Added option to limit playback rate
 * 
 * 100   1/4/12 3:18p jgarrett
 * SW7231-500: Merge to main branch
 * 
 * SW7231-500/1   12/29/11 3:00p jgarrett
 * SW7231-500: Enabling probe for IEEE-1394 LPCM audio
 * 
 * 99   12/29/11 2:11p jgarrett
 * SW7231-500: Removing branched argument
 * 
 * 98   12/29/11 1:14p jgarrett
 * SW7231-500: Adding IEEE-1394 LPCM
 * 
 * 97   12/12/11 11:56a jgarrett
 * SW7425-1406: Adding vorbis support
 * 
 * 96   11/18/11 11:16a jgarrett
 * SW7405-5071: Adding support for WMATS playback
 * 
 * 95   10/25/11 11:36a vsilyaev
 * SWDTV-8388: Added NEXUS_TransportType_eOgg type
 * 
 * 94   10/7/11 4:17p vsilyaev
 * SW7420-2086: Added MPEG1 Program Stream type
 * 
 * 93   9/15/11 3:56p jgarrett
 * SWDTV-4772: Adding support for dvi_adpcm type
 * 
 * 92   9/9/11 3:52p jgarrett
 * SW7422-408: Adding ADPCM support
 * 
 * 91   8/31/11 1:36p vsilyaev
 * SW7422-406: Removed dereference of relocated fields
 * 
 * 90   8/26/11 5:57p jgarrett
 * SW7344-160: Adding DTS-Legacy support
 * 
 * 89   8/18/11 6:21p vsilyaev
 * SW7425-1086, SW7425-966: added support for LeftRightEnhancedResolution
 * 
 * 88   8/18/11 5:23p vsilyaev
 * SW7425-651: Removed deprecated control over the encoder delay
 * 
 * 87   8/15/11 3:11p vsilyaev
 * SW7425-938: Added mapping for SH263 and VP8 video codecs
 * 
 * 86   7/13/11 8:39p vsilyaev
 * SWDTV-7897: Added code to wrap PCM file with the WAV header
 * 
 * 85   7/7/11 5:52p jtna
 * SW7425-869: added runtime option to override
 *  NEXUS_VideoDecoderSourceOrientation
 * 
 * 84   4/22/11 5:49p vsilyaev
 * SW7425-416: Added option to override display orientation
 * 
 * 83   4/20/11 10:58a jgarrett
 * SW7422-408: Adding AMR
 * 
 * 82   4/8/11 5:19p vsilyaev
 * SW7425-14: Fixed online help
 * 
 * 81   3/24/11 8:06p vsilyaev
 * SW7422-406: Fixed parsing of record parameters
 * 
 * 80   3/23/11 5:41p vsilyaev
 * SW7422-406: Unified parsing of command line parameters
 * 
 * 79   3/15/11 2:15p erickson
 * SW7346-101: fix dss playback
 * 
 * 78   3/11/11 3:49p vsilyaev
 * SW7425-159: add encode profile/level etc settings; add clean shutdown
 *  sequence for transcoder; add sample usage for transcoder;
 * 
 * SW7425-159/1   3/9/11 8:05p hongtaoz
 * SW7425-159: add encode profile/level etc settings; add clean shutdown
 *  sequence for transcoder; add sample usage for transcoder;
 * 
 * 77   3/11/11 3:35p vsilyaev
 * SW7422-14: Added support for decoding SVC/MVC streams
 * 
 * 76   3/4/11 3:18p jgarrett
 * SW7422-146: Adding multichannel audio support
 * 
 * 75   3/4/11 1:22p erickson
 * SW7125-833: add NEXUS_VideoWindowContentMode_ePanScanWithoutCorrection
 *
 * 74   2/22/11 7:53p vsilyaev
 * SW7422-107: Added FLV mapping
 *
 * 73   2/16/11 2:32p vsilyaev
 * SW7425-39, SW7425-40: updated to match new API
 *
 * SW7425_bringup/2   2/15/11 9:52p hongtaoz
 * SW7425-40: adapted to new API;
 *
 * 72   1/20/11 5:51p vsilyaev
 * SW7425-69: Can't use nexus API in cmdline_args
 *
 * 71   1/20/11 11:09a vsilyaev
 * SW7422-216: Added vp6 video coded
 *
 * 70   12/22/10 1:15p vsilyaev
 * SW7425-39, SW7425-40: merged hdmi_input source and transcoder util
 *  test;
 *
 * SW7425_bringup/1   12/22/10 12:42a hongtaoz
 * SW7425-39, SW7425-40: add hdmi_input source and transcoder util test;
 *
 * 69   11/30/10 2:47p vsilyaev
 * SW7422-65: Fixed mapping of SVC/MVC video codecs
 *
 * 68   11/24/10 1:49p vsilyaev
 * SW3548-3106: Fixed position of usage for 'start_paused'
 *
 * 67   11/17/10 3:34p jgarrett
 * SW7408-137: Adding HDMI output options
 *
 * 66   11/12/10 11:31a vsilyaev
 * SW7422-64: Added basic 3D display format types
 *
 * 65   11/3/10 7:10p jgarrett
 * SW35230-91: Renamed NEXUS_AudioCodec_eRealAudioLbr to _eCook
 *
 * 64   10/27/10 1:50p vsilyaev
 * SW7468-69: Added mapping for cook audio streams
 *
 * 63   10/22/10 12:17p vsilyaev
 * SW3548-3106: Added option to start playback in a paused state
 *
 * 62   9/28/10 6:44p vsilyaev
 * SW7422-65: Added SVC types
 *
 * 61   9/8/10 3:22p vsilyaev
 * SW7468-129: Split help message
 *
 * 60   9/8/10 12:09p vsilyaev
 * SW7468-129: Added video decoder on ZSP
 *
 * 59   8/31/10 2:44p erickson
 * SWGIGGSVIZIO-57: add -fixed_bitrate option to set
 *  NEXUS_PlaybackMode_eFixedBitrate
 *
 * 58   8/10/10 12:14p erickson
 * SW7405-4735: merge
 *
 * SW7405-4735/1   8/9/10 3:21p jtna
 * SW7405-4735: rename opts.pids to opts.otherPids. allow pid 0 in
 *  opts.otherPids. start only one playback during allpass record.
 *
 * 57   8/9/10 8:17a erickson
 * SW3548-3045: bound array that is being dereferenced with an enum
 *
 * 56   8/9/10 6:32a erickson
 * SW3548-3042: add more help for record, add default -video_type for
 *  record
 *
 * 55   8/3/10 1:30p jhaberf
 * SW35230-844: Added 1080p support
 *
 * 54   7/14/10 6:12p vsilyaev
 * SW3556-1131: Added basic support for CDXA format
 *
 * 53   5/7/10 4:18p erickson
 * SWDEPRECATED-3783: default -probe for playback w/ no -video and no -
 *  audio
 *
 * 52   5/6/10 3:45p vsilyaev
 * SW7405-3773: Added support for demuxing fragments from fragmented MP4
 *  container
 *
 * 51   5/5/10 10:43a vsilyaev
 * SW7405-1260: Added settings for size of the audio decoder buffer
 *
 * 50   3/10/10 6:59p jtna
 * SW3556-1051: default back to timestamp reordering at host, until
 *  decoder reordering contract is re-established
 *
 * 49   2/23/10 4:50p vsilyaev
 * SW3556-913: Added code that monitors state of the playback file and
 *  restarts playback (from last good position) in case of error
 *
 * 48   2/22/10 5:33p vsilyaev
 * SW3556-913: Added option to plug  Custom File I/O routines to inject
 *  errors
 *
 * 47   2/16/10 11:58a jtna
 * SW3556-1051: make decoder reordering the default mode
 *
 * 46   2/12/10 5:56p jtna
 * SW3556-1051: added option to control playback timestamp reordering
 *
 * 45   1/28/10 11:58a jtna
 * SW7405-3260: -astm option is only used in decode app, not playback app
 *
 * 44   1/20/10 5:11p erickson
 * SW7550-159: update record util for threshold and buffer control
 *
 * 43   1/12/10 3:52p jtna
 * SW3548-2715: propagate video frame rate for AVI streams
 *
 * 42   1/11/10 6:54p jtna
 * SW3556-982: propagate pcr pid to playback util
 *
 * 41   1/6/10 11:27a erickson
 * SW3556-958: select correct NEXUS_TransportTimestampType for MPEG2TS
 *
 * 40   12/30/09 3:19p vsilyaev
 * SW7408-17: Added options to select PCM and compressed audio outputs
 *
 * 39   12/30/09 2:13p erickson
 * SW7550-128: add closed caption feature (-cc on)
 *
 * 38   12/9/09 1:42p vsilyaev
 * SW7408-1: Added option to pass codec/stream types as a number
 *
 * 37   12/8/09 2:31p gmohile
 * SW7408-1 : Add defines for nexus had frontend
 *
 * 36   11/25/09 6:39p vsilyaev
 * SWDEPRECATED-3586: Fixed handling of ES streams (bogus start of both
 *  audio and video decoders for ES streams)
 *
 * 35   11/25/09 5:24p katrep
 * SW7405-2740: Add support for WMA pro drc
 *
 * 34   10/8/09 6:03p jgarrett
 * SW7405-3064: Adding DRA
 *
 * 33   10/8/09 5:45p jgarrett
 * SW3548-2188: Adding AMR
 *
 * 32   10/8/09 10:43a jtna
 * SWDEPRECATED-3793: clarify -acceptnull option for record
 *
 * 31   8/18/09 4:36p vsilyaev
 * PR 56809: Added option to control handling of video errors
 *
 * 30   7/15/09 7:27p vsilyaev
 * PR 55653: Added WAV format
 *
 * 29   7/13/09 4:11p vsilyaev
 * PR 46190: Fixed mapping for PCM audio
 *
 * 28   6/25/09 11:33a erickson
 * PR54342: use media probe to determine if there's an index to use
 *
 * 27   6/19/09 5:20p vsilyaev
 * PR 56169: Added option to set max decode rate
 *
 * 26   6/18/09 4:30p jtna
 * PR54802: add frontend support to record
 *
 * 25   6/16/09 5:13p jtna
 * PR54802: added record
 *
 * 24   6/8/09 7:06a erickson
 * PR55617: add user-specific aspectRatio
 *
 * 23   6/1/09 3:39p erickson
 * PR48944: if scart1composite is on, revert from panel defaults
 *
 * 22   5/22/09 5:21p vsilyaev
 * PR 55376 PR 52344: Added option to enable processing of AVC(H.264)
 *  Level 5.1 video
 *
 * 21   3/24/09 10:38a erickson
 * PR48944: default ES to video or audio if audio_type is set or not set
 *
 * 20   3/18/09 10:30a erickson
 * PR52350: add wxga/fha support with 50/60 hz option
 *
 * 19   3/17/09 5:41p vsilyaev
 * PR 46190: Adding mappings to the PCM audio codec
 *
 * 18   3/10/09 10:55a erickson
 * PR52946: reconcile ts_timestamp help and actual cmdline param
 *
 * 17   3/6/09 9:33a erickson
 * PR51743: added -ar and -graphics options, default DTV apps to panel
 *  output
 *
 * 16   2/27/09 5:05p vsilyaev
 * PR 52634: Added code to handle MPEG-2 TS streams with timesampts (e.g.
 *  192 byte packets)
 *
 * 15   2/20/09 2:06p vsilyaev
 * PR 51467: Added option to set size of the video decoder buffer
 *
 * 14   2/5/09 2:08p erickson
 * PR51151: update
 *
 * 13   2/5/09 1:49p erickson
 * PR51151: added media probe option for playback
 *
 * 12   1/26/09 11:26a vsilyaev
 * PR 51579: Added stream_processing and auto_bitrate options
 *
 * 11   1/22/09 7:48p vsilyaev
 * PR 50848: Don't use globals for the command line options
 *
 * 10   1/20/09 4:28p erickson
 * PR48944: add -mad and -display_format options
 *
 * 9   1/8/09 10:34p erickson
 * PR48944: add more options
 *
 * 8   1/8/09 9:36p erickson
 * PR50757: added NEXUS_VideoFrameRate support, both as a start setting
 *  and status
 *
 * 7   1/6/09 12:45a erickson
 * PR50763: added -bof, -eof options. added playback position to status.
 *  fix mkv, mp4.
 *
 * 6   1/5/09 12:49p erickson
 * PR50763: update for mkv testing
 *
 * 5   12/3/08 3:55p erickson
 * PR48944: update
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
#include "nexus_platform.h"
#include "nexus_core_utils.h"
#include "cmdline_args.h"
#include "bmedia_probe.h"
#include "bmpeg2ts_probe.h"
#include "bmedia_cdxa.h"
#if B_HAS_ASF
#include "basf_probe.h"
#endif
#if B_HAS_AVI
#include "bavi_probe.h"
#endif
#if NEXUS_HAS_VIDEO_ENCODER
#include "nexus_video_encoder_types.h"
#include "nexus_stream_mux.h"
#endif
#include "bfile_stdio.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

NEXUS_VideoCodec b_videocodec2nexus(bvideo_codec settop_value);
NEXUS_AudioCodec b_audiocodec2nexus(baudio_format settop_value);
NEXUS_TransportType b_mpegtype2nexus(bstream_mpeg_type settop_value);

const namevalue_t g_videoFormatStrs[] = {
    {"ntsc",      NEXUS_VideoFormat_eNtsc},
    {"480i",      NEXUS_VideoFormat_eNtsc},
    {"pal",       NEXUS_VideoFormat_ePal},
    {"576i",      NEXUS_VideoFormat_ePal},
    {"1080i",     NEXUS_VideoFormat_e1080i},
    {"720p",      NEXUS_VideoFormat_e720p},
    {"480p",      NEXUS_VideoFormat_e480p},
    {"576p",      NEXUS_VideoFormat_e576p},
    {"1080p",     NEXUS_VideoFormat_e1080p},
    {"720p3D",    NEXUS_VideoFormat_e3D_720p},
    {"1080p3D",   NEXUS_VideoFormat_e3D_1080p_24Hz},
    {NULL, 0}
};

const namevalue_t g_videoFrameRateStrs[NEXUS_VideoFrameRate_eMax+1] = {
    {"0",      NEXUS_VideoFrameRate_eUnknown},
    {"23.976", NEXUS_VideoFrameRate_e23_976},
    {"24",     NEXUS_VideoFrameRate_e24},
    {"25",     NEXUS_VideoFrameRate_e25},
    {"29.97",  NEXUS_VideoFrameRate_e29_97},
    {"30",     NEXUS_VideoFrameRate_e30},
    {"50",     NEXUS_VideoFrameRate_e50},
    {"59.94",  NEXUS_VideoFrameRate_e59_94},
    {"60",     NEXUS_VideoFrameRate_e60},
    {NULL, 0}
};

static const namevalue_t g_transportTypeStrs[] = {
    {"ts",  NEXUS_TransportType_eTs},
    {"pes", NEXUS_TransportType_eMpeg2Pes},
    {"es",  NEXUS_TransportType_eEs},
    {"vob", NEXUS_TransportType_eVob},
    {"mp4", NEXUS_TransportType_eMp4},
    {"mkv", NEXUS_TransportType_eMkv},
    {"avi", NEXUS_TransportType_eAvi},
    {"asf", NEXUS_TransportType_eAsf},
    {"wav", NEXUS_TransportType_eWav},
    {"mp4f", NEXUS_TransportType_eMp4Fragment},
    {"rmff", NEXUS_TransportType_eRmff},
    {"flv", NEXUS_TransportType_eFlv},
    {"ogg", NEXUS_TransportType_eOgg},
    {"dss", NEXUS_TransportType_eDssEs},
    {"dss_es", NEXUS_TransportType_eDssEs}, /* alias. used for SD. */
    {"dss_pes", NEXUS_TransportType_eDssPes}, /* used for HD */
    {"mp1", NEXUS_TransportType_eMpeg1Ps},
    {NULL, 0}
};

static const namevalue_t g_videoCodecStrs[] = {
    {"mpeg2", NEXUS_VideoCodec_eMpeg2},
    {"mpeg", NEXUS_VideoCodec_eMpeg2},
    {"mpeg1", NEXUS_VideoCodec_eMpeg1},
    {"avc", NEXUS_VideoCodec_eH264},
    {"h264", NEXUS_VideoCodec_eH264},
    {"svc", NEXUS_VideoCodec_eH264_Svc},
    {"mvc", NEXUS_VideoCodec_eH264_Mvc},
    {"h263", NEXUS_VideoCodec_eH263},
    {"h263", NEXUS_VideoCodec_eH263},
    {"avs", NEXUS_VideoCodec_eAvs},
    {"vc1", NEXUS_VideoCodec_eVc1},
    {"vc1sm", NEXUS_VideoCodec_eVc1SimpleMain},
    {"divx", NEXUS_VideoCodec_eMpeg4Part2},
    {"mpeg4", NEXUS_VideoCodec_eMpeg4Part2},
    {"divx311", NEXUS_VideoCodec_eDivx311},
    {"divx3", NEXUS_VideoCodec_eDivx311},
    {"rv40", NEXUS_VideoCodec_eRv40},
    {"vp6", NEXUS_VideoCodec_eVp6},
    {"vp7", NEXUS_VideoCodec_eVp7},
    {"vp8", NEXUS_VideoCodec_eVp8},
    {"spark", NEXUS_VideoCodec_eSpark},
    {"mjpeg", NEXUS_VideoCodec_eMotionJpeg},
    {NULL, 0}
};

#if NEXUS_HAS_VIDEO_ENCODER
static const namevalue_t g_videoCodecProfileStrs[] = {
	{"simple", NEXUS_VideoCodecProfile_eSimple},
	{"main", NEXUS_VideoCodecProfile_eMain},
	{"high", NEXUS_VideoCodecProfile_eHigh},
	{"advanced", NEXUS_VideoCodecProfile_eAdvanced},
	{"jizhun", NEXUS_VideoCodecProfile_eJizhun},
	{"snrscl", NEXUS_VideoCodecProfile_eSnrScalable},
	{"spatialscl", NEXUS_VideoCodecProfile_eSpatiallyScalable},
	{"advsimple", NEXUS_VideoCodecProfile_eAdvancedSimple},
	{"baseline", NEXUS_VideoCodecProfile_eBaseline},
	
    {NULL, 0}
};

static const namevalue_t g_videoCodecLevelStrs[] = {
	{"0.0", NEXUS_VideoCodecLevel_e00},
	{"1.0", NEXUS_VideoCodecLevel_e10},
	{"1.b", NEXUS_VideoCodecLevel_e1B},
	{"1.1", NEXUS_VideoCodecLevel_e11},
	{"1.2", NEXUS_VideoCodecLevel_e12},
	{"1.3", NEXUS_VideoCodecLevel_e13},
	{"2.0", NEXUS_VideoCodecLevel_e20},
	{"2.1", NEXUS_VideoCodecLevel_e21},
	{"2.2", NEXUS_VideoCodecLevel_e22},
	{"3.0", NEXUS_VideoCodecLevel_e30},
	{"3.1", NEXUS_VideoCodecLevel_e31},
	{"3.2", NEXUS_VideoCodecLevel_e32},
	{"4.0", NEXUS_VideoCodecLevel_e40},
	{"4.1", NEXUS_VideoCodecLevel_e41},
	{"4.2", NEXUS_VideoCodecLevel_e42},
	{"5.0", NEXUS_VideoCodecLevel_e50},
	{"5.1", NEXUS_VideoCodecLevel_e51},
	{"6.0", NEXUS_VideoCodecLevel_e60},
	{"6.2", NEXUS_VideoCodecLevel_e62},
	{"low", NEXUS_VideoCodecLevel_eLow},
	{"main", NEXUS_VideoCodecLevel_eMain},
	{"high", NEXUS_VideoCodecLevel_eHigh},
	{"high1440", NEXUS_VideoCodecLevel_eHigh1440},
    {NULL, 0}
};
#endif

static const namevalue_t g_audioCodecStrs[] = {
    {"mpeg", NEXUS_AudioCodec_eMpeg},
    {"mp3", NEXUS_AudioCodec_eMp3},
    {"ac3", NEXUS_AudioCodec_eAc3},
    {"ac3plus", NEXUS_AudioCodec_eAc3Plus},
    {"aac", NEXUS_AudioCodec_eAac},
    {"aacplus", NEXUS_AudioCodec_eAacPlus},
    {"wma", NEXUS_AudioCodec_eWmaStd},
    {"wmastd", NEXUS_AudioCodec_eWmaStd},
    {"wmats", NEXUS_AudioCodec_eWmaStdTs},
    {"wmapro", NEXUS_AudioCodec_eWmaPro},
    {"pcm", NEXUS_AudioCodec_ePcmWav},
    {"dra", NEXUS_AudioCodec_eDra},
    {"dts_legacy", NEXUS_AudioCodec_eDtsLegacy},
    {"dts", NEXUS_AudioCodec_eDts},
    {"dtshd", NEXUS_AudioCodec_eDtsHd},
    {"cook", NEXUS_AudioCodec_eCook},
    {"amr", NEXUS_AudioCodec_eAmr},
    {"adpcm", NEXUS_AudioCodec_eAdpcm},
    {"vorbis", NEXUS_AudioCodec_eVorbis},
    {"lpcm_1394", NEXUS_AudioCodec_eLpcm1394},
    {"g711", NEXUS_AudioCodec_eG711},
    {"g726", NEXUS_AudioCodec_eG726},
    {"g729", NEXUS_AudioCodec_eG729},
    {"g723.1", NEXUS_AudioCodec_eG723_1},
    {"flac", NEXUS_AudioCodec_eFlac},
    {NULL, 0}
};

static const namevalue_t g_stcChannelMasterStrs[] = {
    {"first", NEXUS_StcChannelAutoModeBehavior_eFirstAvailable},
    {"video", NEXUS_StcChannelAutoModeBehavior_eVideoMaster},
    {"audio", NEXUS_StcChannelAutoModeBehavior_eAudioMaster},
    {NULL, 0}
};

static const namevalue_t g_endOfStreamActionStrs[] = {
    {"loop",  NEXUS_PlaybackLoopMode_eLoop},
    {"pause", NEXUS_PlaybackLoopMode_ePause},
    {"play",  NEXUS_PlaybackLoopMode_ePlay},
    {NULL, 0}
};

static const namevalue_t g_tsTimestampType[] = {
    {"none", NEXUS_TransportTimestampType_eNone},
    {"mod300", NEXUS_TransportTimestampType_eMod300},
    {"binary", NEXUS_TransportTimestampType_eBinary},
    {NULL, 0}
};

static const namevalue_t g_contentModeStrs[] = {
    {"zoom", NEXUS_VideoWindowContentMode_eZoom},
    {"box", NEXUS_VideoWindowContentMode_eBox},
    {"panscan", NEXUS_VideoWindowContentMode_ePanScan},
    {"full", NEXUS_VideoWindowContentMode_eFull},
    {"nonlinear", NEXUS_VideoWindowContentMode_eFullNonLinear},
    /* {"panscan_no_cor", NEXUS_VideoWindowContentMode_ePanScanWithoutCorrection}, */
    {NULL, 0}
};

static const namevalue_t g_panelStrs[] = {
    {"wxga", 0},
    {"wxga50", 1},
    {"fhd", 2},
    {"fhd50", 3},
    {"off", 0xff},
    {NULL, 0}
};

#if NEXUS_HAS_FRONTEND
static const namevalue_t g_vsbModeStrs[] = {
    {"8", NEXUS_FrontendVsbMode_e8},
    {"16", NEXUS_FrontendVsbMode_e16},
    {NULL, 0}
};

static const namevalue_t g_qamModeStrs[] = {
    {"16", NEXUS_FrontendQamMode_e16},
    {"32", NEXUS_FrontendQamMode_e32},
    {"64", NEXUS_FrontendQamMode_e64},
    {"128", NEXUS_FrontendQamMode_e128},
    {"256", NEXUS_FrontendQamMode_e256},
    {"512", NEXUS_FrontendQamMode_e512},
    {"1024", NEXUS_FrontendQamMode_e1024},
    {"2048", NEXUS_FrontendQamMode_e2048},
    {"4096", NEXUS_FrontendQamMode_e4096},
    {"Auto_64_256", NEXUS_FrontendQamMode_eAuto_64_256},
    {NULL, 0}
};

static const namevalue_t g_satModeStrs[] = {
    {"dvb", NEXUS_FrontendSatelliteMode_eDvb},
    {"dss", NEXUS_FrontendSatelliteMode_eDss},
    {NULL, 0}
};
#endif

static const namevalue_t g_videoErrorHandling[] = {
    {"none", NEXUS_VideoDecoderErrorHandling_eNone},
    {"picture", NEXUS_VideoDecoderErrorHandling_ePicture},
    {"prognostic", NEXUS_VideoDecoderErrorHandling_ePrognostic},
    {NULL, 0}
};

static const namevalue_t g_videoOrientation[] = {
    {"2D", NEXUS_VideoOrientation_e2D},
    {"LeftRight", NEXUS_VideoOrientation_e3D_LeftRight},   
    {"3D", NEXUS_VideoOrientation_e3D_LeftRight},
    {"OverUnder", NEXUS_VideoOrientation_e3D_OverUnder},
    {NULL, 0}
};

static const namevalue_t g_sourceOrientation[] = {
    {"2D", NEXUS_VideoDecoderSourceOrientation_e2D},
    {"LeftRight", NEXUS_VideoDecoderSourceOrientation_e3D_LeftRight},
    {"OverUnder", NEXUS_VideoDecoderSourceOrientation_e3D_OverUnder},
    {"LeftRightFull", NEXUS_VideoDecoderSourceOrientation_e3D_LeftRightFullFrame},
    {"RightLeftFull", NEXUS_VideoDecoderSourceOrientation_e3D_RightLeftFullFrame},
    {"LeftRightEnhanced", NEXUS_VideoDecoderSourceOrientation_e3D_LeftRightFullFrame},
    {NULL, 0}    
};

static unsigned lookup(const namevalue_t *table, const char *name)
{
    unsigned i;
    unsigned value;
    char *endptr;
    for (i=0;table[i].name;i++) {
        if (!strcasecmp(table[i].name, name)) {
            return table[i].value;
        }
    }
    value = strtol(name, &endptr, 0);
    if(!endptr || *endptr) { /* if valid, *endptr = '\0' */
        value = table[0].value;
    }
    printf("Unknown cmdline param '%s', using %u as value\n", name, value);
    return value;
}

static void print_list(const namevalue_t *table)
{
    unsigned i;
    const char *sep=" {";
    for (i=0;table[i].name;i++) {
        /* skip aliases */
        if (i > 0 && table[i].value == table[i-1].value) continue;
        printf("%s%s",sep,table[i].name);
        sep = ",";
    }
    printf("}");
}

static void print_list_option(const char *name, const namevalue_t *list)
{
    printf("  -%s ", name);
    print_list(list);
    printf("\n");
}

static void print_usage_common(void)
{
    printf(
        "  -h|--help  - this usage information\n"
        "  -pcr PID   - defaults to video PID\n"
        "  -mpeg_type");
    print_list(g_transportTypeStrs);
    printf("\n  -video PID\n");
    print_list_option("video_type",g_videoCodecStrs);
    print_list_option("audio_type",g_audioCodecStrs);
    printf("  -audio PID\n");
    print_list_option("frame_rate",g_videoFrameRateStrs);
    print_list_option("display_format",g_videoFormatStrs);
    print_list_option("ar",g_contentModeStrs);
    print_list_option("panel",g_panelStrs);
    printf(
#if NEXUS_NUM_COMPOSITE_OUTPUTS
        "  -composite {on|off}\n"
#endif
#if NEXUS_NUM_COMPONENT_OUTPUTS
        "  -component {on|off}\n"
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
        "  -hdmi {on|off}\n"
#endif
#if NEXUS_NUM_SCART_INPUTS
        "  -scart1composite {on|off}\n"
#endif
        "  -cc {on|off} - enable closed caption routing and output\n"
        "  -probe - use media probe to discover stream format for playback (defaults on if no video or audio pid)\n"
        );
    printf(
        "  -video_cdb KBytes - size of compressed video buffer, in KBytes\n"
        "  -audio_cdb KBytes - size of compressed audio buffer, in KBytes\n"
        "  -avc51 - Enable AVC (H.264) Level 5.1 decoding\n"
        "  -max_decoder_rate rate - Set decoder max decoder rate\n"
        "  -compressed_audio - Also output compressed audio\n"
        "  -multichannel_audio - Also output multichannel audio on hdmi\n"
        "  -no_decoded_audio - Don't output decoded (PCM) audio\n"
        "  -video_decoder index - Selects video decoder index\n"
        "  -pcm sr,channels,bits - Play LPCM file\n"
        );
    print_list_option("ts_timestamp",g_tsTimestampType);
    print_list_option("video_error_handling", g_videoErrorHandling);
    printf(
        "  -detect_avc_extension - Detect SVC/MVC extensions\n"
        "  -ext_video PID\n"
        "  -max_playback_data_rate - Set limit for playback data rate (Kbps)\n"
        );
    print_list_option("ext_video_type", g_videoCodecStrs);
    print_list_option("display_3d", g_videoOrientation);
    print_list_option("source_3d", g_sourceOrientation);
    return;
}

void print_usage(const char *app)
{
    printf("%s usage:\n", app);
    print_usage_common();
    printf(
        "  -stream_processing - enable extra stream processing for playback\n"
        "  -auto_bitrate - enable bitrate detection for playback\n"
        "  -fixed_bitrate X - provide fixed bitrate to playback (units bits per second)\n"
        "  -graphics    - add a graphics plane\n"
        );
    printf(
        "  -astm        - enable Astm (adaptive system time management). used only for decode app\n"
        "  -sync        - enable SyncChannel (high-precision lipsync)\n"
#if HOST_REORDER /* disabled for now */
        "  -host_reorder - handle timestamp reordering at host and disable at decoder\n"
#endif
        "  -stctrick    - use STC trick modes instead of decoder trick modes\n"
        "  -start_paused - Start playback in a paused state\n"
        );
    print_list_option("master", g_stcChannelMasterStrs);
    print_list_option("bof", g_endOfStreamActionStrs);
    print_list_option("eof", g_endOfStreamActionStrs);
    return;
}

struct cmdline_parse_common_state  {
    bool isVideoEs;
};

static int cmdline_parse_common(int offset, int argc, const char *argv[], struct common_opts_t *opts, struct cmdline_parse_common_state *state, void (*usage)(const char *) )
{
    int i;

    if (offset==0) {
        memset(state,0,sizeof(*state));
        offset = 1;
        state->isVideoEs = true;
        memset(opts,0,sizeof(*opts));
        opts->transportType = NEXUS_TransportType_eTs;
        opts->extVideoCodec = NEXUS_VideoCodec_eNone;
        opts->videoCodec = NEXUS_VideoCodec_eMpeg2;
        opts->audioCodec = NEXUS_AudioCodec_eMpeg;
        opts->contentMode = NEXUS_VideoWindowContentMode_eFull;
        opts->compressedAudio = false;
        opts->multichannelAudio = false;
        opts->detectAvcExtension = false;
        opts->decodedAudio = true;
        opts->playpumpTimestampReordering = true;
        opts->videoDecoder = 0;
        opts->tsTimestampType = NEXUS_TransportTimestampType_eNone;
        opts->videoFrameRate = NEXUS_VideoFrameRate_eUnknown;
        opts->displayOrientation = NEXUS_VideoOrientation_e2D;
        opts->sourceOrientation = NEXUS_VideoDecoderSourceOrientation_e2D;
        opts->pcm = false;
        bpcm_file_config_init(&opts->pcm_config);
        opts->useCompositeOutput = true;
        opts->useComponentOutput = true;
        opts->useHdmiOutput = true;
        opts->displayFormat = NEXUS_VideoFormat_eNtsc;
        opts->displayType = NEXUS_DisplayType_eAuto;
    }

    for (i=offset;i<argc;i++) {
        if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
            usage(argv[0]);
            return -1;
        }
        else if (!strcmp(argv[i], "-pcr") && i+1<argc) {
            opts->pcrPid = strtoul(argv[++i], NULL, 0);
        }
        else if (!strcmp(argv[i], "-mpeg_type") && i+1<argc) {
            opts->transportType=lookup(g_transportTypeStrs, argv[++i]);
        }
        else if (!strcmp(argv[i], "-display_format") && i+1<argc) {
            opts->displayFormat=lookup(g_videoFormatStrs, argv[++i]);
            if (opts->displayFormat >= NEXUS_VideoFormat_e480p) {
                opts->useCompositeOutput = false;
            }
        }
        else if (!strcmp(argv[i], "-ar") && i+1<argc) {
            opts->contentMode = lookup(g_contentModeStrs, argv[++i]);
        }
        else if (!strcmp(argv[i], "-video") && i+1<argc) {
            opts->videoPid = strtoul(argv[++i], NULL, 0);
        }
        else if (!strcmp(argv[i], "-ext_video") && i+1<argc) {
            opts->extVideoPid = strtoul(argv[++i], NULL, 0);
        }
        else if (!strcmp(argv[i], "-audio") && i+1<argc) {
            opts->audioPid = strtoul(argv[++i], NULL, 0);
            state->isVideoEs = false;
        }
        else if (!strcmp(argv[i], "-video_type") && i+1<argc) {
            opts->videoCodec=lookup(g_videoCodecStrs, argv[++i]);
        }
        else if (!strcmp(argv[i], "-ext_video_type") && i+1<argc) {
            opts->extVideoCodec=lookup(g_videoCodecStrs, argv[++i]);
        }
        else if (!strcmp(argv[i], "-audio_type") && i+1<argc) {
            opts->audioCodec=lookup(g_audioCodecStrs, argv[++i]);
            state->isVideoEs = false;
        }
        /* Note: leave in these options even if the hardware doesn't exist so we don't break any scripts that don't know what is available */
        else if (!strcmp(argv[i], "-composite") && i+1<argc) {
            opts->useCompositeOutput = strcasecmp(argv[++i], "off");
        }
        else if (!strcmp(argv[i], "-component") && i+1<argc) {
            opts->useComponentOutput = strcasecmp(argv[++i], "off");
        }
        else if (!strcmp(argv[i], "-hdmi") && i+1<argc) {
            opts->useHdmiOutput = strcasecmp(argv[++i], "off");
        }
        else if (!strcmp(argv[i], "-scart1composite") && i+1<argc) {
            opts->useScart1CompositeOutput = strcasecmp(argv[++i], "off");
            opts->usePanelOutput = false;
            opts->displayFormat = NEXUS_VideoFormat_ePal;
        }
        else if (!strcmp(argv[i], "-video_cdb") && i+1<argc) {
            opts->videoCdb = strtoul(argv[++i], NULL, 0);
        }
        else if (!strcmp(argv[i], "-audio_cdb") && i+1<argc) {
            opts->audioCdb = strtoul(argv[++i], NULL, 0);
        }
        else if (!strcmp(argv[i], "-probe")) {
            opts->probe = true;
        }
        else if (!strcmp(argv[i], "-panel") && i+1<argc) {
            unsigned paneltype = lookup(g_panelStrs, argv[++i]);

            /* for now, use output_resolution env to get into platform */
            switch (paneltype) {
            case 0:
                opts->usePanelOutput = true;
                opts->displayFormat = NEXUS_VideoFormat_eCustom0;
                setenv("output_resolution", "wxga", true);
                setenv("bvn_usage", "config1", true);
                break;
            case 1:
                opts->usePanelOutput = true;
                opts->displayFormat = NEXUS_VideoFormat_eCustom1;
                setenv("output_resolution", "wxga", true);
                setenv("bvn_usage", "config1", true);
                break;
            case 2:
                opts->usePanelOutput = true;
                opts->displayFormat = NEXUS_VideoFormat_eCustom0;
                setenv("output_resolution", "fhd", true);
                setenv("bvn_usage", "config2", true);
                break;
            case 3:
                opts->usePanelOutput = true;
                opts->displayFormat = NEXUS_VideoFormat_eCustom1;
                setenv("output_resolution", "fhd", true);
                setenv("bvn_usage", "config2", true);
                break;
            /* TODO: XGA 60 & 50 */
            default:
            case 4:
                opts->usePanelOutput = false; /* off */
                break;
            }
        }
        else if (!strcmp(argv[i], "-mad")) {
            opts->mad = true;
        }
        else if (!strcmp(argv[i], "-compressed_audio")) {
            opts->compressedAudio = true;
        }
        else if (!strcmp(argv[i], "-multichannel_audio")) {
            opts->multichannelAudio = true;
        }
        else if (!strcmp(argv[i], "-video_decoder") && i+1<argc) {
            opts->videoDecoder= strtoul(argv[++i], NULL, 0);
        }
        else if (!strcmp(argv[i], "-frame_rate") && i+1<argc) {
            opts->videoFrameRate=lookup(g_videoFrameRateStrs, argv[++i]);
        }
        else if (!strcmp(argv[i], "-ts_timestamp") && i+1<argc) {
            opts->tsTimestampType=lookup(g_tsTimestampType, argv[++i]);
        }
        else if (!strcmp(argv[i], "-detect_avc_extension")) {
            opts->detectAvcExtension = true;
        }
        else if (!strcmp(argv[i], "-host_reorder")) {
            opts->playpumpTimestampReordering = true;
        }
        else if (!strcmp(argv[i], "-no_decoded_audio")) {
            opts->decodedAudio = false;
        }
        else if (!strcmp(argv[i], "-display_3d") && i+1<argc) {
            opts->displayOrientation=lookup(g_videoOrientation, argv[++i]);
        } 
        else if (!strcmp(argv[i], "-source_3d") && i+1<argc) {
            opts->sourceOrientation=lookup(g_sourceOrientation, argv[++i]);
        } 
        else if (!strcmp(argv[i], "-pcm") && i+1<argc) {
            unsigned channel_count = opts->pcm_config.channel_count;
            unsigned sample_size = opts->pcm_config.sample_size;
            unsigned sample_rate = opts->pcm_config.sample_rate;

            sscanf(argv[++i], "%u,%u,%u", &sample_rate, &channel_count, &sample_size);
            opts->pcm_config.channel_count = channel_count;
            opts->pcm_config.sample_size = sample_size;
            opts->pcm_config.sample_rate = sample_rate;
            opts->pcm = true;
        } 
        else {
            return i;
        }
    }

    if (opts->usePanelOutput) {
        opts->displayType = NEXUS_DisplayType_eLvds;
        opts->useComponentOutput = false;
        opts->useCompositeOutput = false;
    }
    else {
        opts->displayType = NEXUS_DisplayType_eAuto;
    }

    /* this allows the user to set: "-mpeg_type es -video_type mpeg" and forget the "-video 1" option */
    if (opts->transportType == NEXUS_TransportType_eEs && !opts->videoPid && !opts->audioPid) {
        if (state->isVideoEs) {
            opts->videoPid = 1;
        }
        else {
            opts->audioPid = 1;
        }
    }

    return 0;
}

int cmdline_parse(int argc, const char *argv[], struct util_opts_t *opts)
{
    struct cmdline_parse_common_state  state;
    int i;
    int rc=-1;
    BSTD_UNUSED(argc);
    BSTD_UNUSED(argv);

    memset(opts,0,sizeof(*opts));
    opts->stcChannelMaster = NEXUS_StcChannelAutoModeBehavior_eVideoMaster;
    opts->beginningOfStreamAction = NEXUS_PlaybackLoopMode_eLoop;
    opts->endOfStreamAction = NEXUS_PlaybackLoopMode_eLoop;
    opts->videoErrorHandling = NEXUS_VideoDecoderErrorHandling_eNone;
    opts->customFileIo = false;
    opts->playbackMonitor = false;
    opts->startPaused = false;
    opts->maxPlaybackDataRate = 0;

    for (i=0;i<argc;i++)
    {
        rc = cmdline_parse_common(i, argc, argv, &opts->common, &state, print_usage);
        if(rc==0) break;
        if(rc<0) return rc;

        /* unknown option */
        i = rc;
        if (!strcmp(argv[i], "-bof") && i+1<argc) {
            opts->beginningOfStreamAction=lookup(g_endOfStreamActionStrs, argv[++i]);
        }
        else if (!strcmp(argv[i], "-eof") && i+1<argc) {
            opts->endOfStreamAction=lookup(g_endOfStreamActionStrs, argv[++i]);
        }
        else if (!strcmp(argv[i], "-master") && i+1<argc) {
            opts->stcChannelMaster=lookup(g_stcChannelMasterStrs, argv[++i]);
        }
        else if (!strcmp(argv[i], "-cc") && i+1<argc) {
            opts->closedCaptionEnabled = strcasecmp(argv[++i], "off");
        }
        else if (!strcmp(argv[i], "-stctrick")) {
            opts->stcTrick = true;
        }
        else if (!strcmp(argv[i], "-astm")) {
            opts->astm = true;
        }
        else if (!strcmp(argv[i], "-sync")) {
            opts->sync = true;
        }
        else if (!strcmp(argv[i], "-custom_file_io")) {
            opts->customFileIo=true;
        }
        else if (!strcmp(argv[i], "-playback_monitor")) {
            opts->playbackMonitor=true;
        }
        else if (!strcmp(argv[i], "-stream_processing")) {
            opts->streamProcessing = true;
        }
        else if (!strcmp(argv[i], "-auto_bitrate")) {
            opts->autoBitrate = true;
        }
        else if (!strcmp(argv[i], "-fixed_bitrate") && i+1<argc) {
            opts->fixedBitrate = strtoul(argv[++i], NULL, 0);
        }
        else if (!strcmp(argv[i], "-graphics")) {
            opts->graphics = true;
        }
        else if (!strcmp(argv[i], "-avc51")) {
            opts->avc51= true;
        }
        else if (!strcmp(argv[i], "-max_decoder_rate") && i+1<argc) {
            opts->maxDecoderRate=atof(argv[++i])*NEXUS_NORMAL_PLAY_SPEED;
        }
        else if (!strcmp(argv[i], "-video_error_handling") && i+1<argc) {
            opts->videoErrorHandling=lookup(g_videoErrorHandling, argv[++i]);
        }
        else if (!strcmp(argv[i], "-start_paused")) {
            opts->startPaused = true;
        }
        else if (!strcmp(argv[i], "-max_playback_data_rate") && i+1<argc) {
            opts->maxPlaybackDataRate=atof(argv[++i])*1000;
        }
        else if (!strcmp(argv[i], "-cmp_crc")) {
            opts->cmp_crc = true;
        }
        else if (!strcmp(argv[i], "-avd_crc")) {
            opts->avd_crc = true;
        }
        else if (!opts->filename) {
            opts->filename = argv[i];
        }
        else if (!opts->indexname) {
            opts->indexname = argv[i];
        }
        else {
            printf("unknown parameter: '%s'\n", argv[i]);
            return -1;
        }
    }
    if(rc!=0) {
        cmdline_parse_common(i, argc, argv, &opts->common, &state, print_usage); /* allow common parser to execute finishing touches */
    }
            
    /* default -probe for playback if no -video or -audio option */
    if (opts->filename && !opts->common.videoPid && !opts->common.audioPid) {
        opts->common.probe = true;
    }
    return 0;
}

void print_usage_record(const char *app)
{
    printf("%s usage: nexus %s [-options] datafile [indexfile]", app, app);
    print_usage_common();
    printf(
        "  -decode{on|off} - A/V decode of source during record");
    printf(
        "\n  -streamer       - (default)"
        "\n  -playfile       - playback file to be used as source");
#if NEXUS_HAS_FRONTEND
    printf("\n  -vsb");
    print_list(g_vsbModeStrs);
    printf("\n  -qam");
    print_list(g_qamModeStrs);
    printf("\n  -sat");
    print_list(g_satModeStrs);
#endif
#if NEXUS_HAS_HDMI_INPUT
    printf("\n  -hdmi_input");
#endif
#if NEXUS_HAS_VIDEO_ENCODER
    printf("\n  -encode              - enable video encoder");
    printf("\n  -encode_video_type");
    print_list(g_videoCodecStrs);
    printf("\n  -video_codec_profile");
    print_list(g_videoCodecProfileStrs);
    printf("\n  -video_codec_level");
    print_list(g_videoCodecLevelStrs);
    printf("\n  -encode_format");
    print_list(g_videoFormatStrs);
    printf("\n  -encode_frame_rate");
    print_list(g_videoFrameRateStrs);
    printf("\n  -video_bit_rate MAX - video encoder max bit rate");
    printf("\n  -gop_frameP num_of_P_between_Is");
    printf("\n  -gop_frameB num_of_B_between_IPs ");
    printf("\n  -mux_latency_tolerance MSEC   - Stream MUX latency tolerance in number of msecs");
    printf("\n  -encode_audio");
    printf("\n  -audio_bit_rate");
    printf("\n  -encode_audio_type");
    print_list(g_audioCodecStrs);
#endif
    printf(
        "\n  -freq           - tuner frequency in MHz"
        "\n  -probe          - use media probe to discover stream format for playback"
        "\n  -allpass        - allpass record"
        "\n  -acceptnull     - record null packets in source stream (applies only to allpass record)"
        "\n  -data_buffer_size           - CDB size in bytes"
        "\n  -data_data_ready_threshold  - CDB interrupt threshold in bytes");
    printf(
        "\n  -index_buffer_size          - ITB size in bytes"
        "\n  -index_data_ready_threshold - ITB interrupt threshold in bytes"
        "\n  -video_decoder index - Selects video decoder index"
        "\n");
    return;
}

int cmdline_parse_record(int argc, const char *argv[], struct util_opts_record_t *opts)
{
    int i;
    int rc=-1;
    struct cmdline_parse_common_state  state;

    memset(opts,0,sizeof(*opts));
#if NEXUS_HAS_FRONTEND
    opts->vsbMode = NEXUS_FrontendVsbMode_eMax;
    opts->qamMode = NEXUS_FrontendQamMode_eMax;
    opts->satMode = NEXUS_FrontendSatelliteMode_eMax;
#endif
#if NEXUS_HAS_HDMI_INPUT
    opts->hdmiInput = false;
#endif
#if NEXUS_HAS_VIDEO_ENCODER
    opts->encoder = false;
    opts->encodeFormat = NEXUS_VideoFormat_e480p;
    opts->videoTranscodec = NEXUS_VideoCodec_eH264;
    opts->videoProfile    = NEXUS_VideoCodecProfile_eBaseline;
    opts->videoLevel      = NEXUS_VideoCodecLevel_e31;
    opts->encodeFrameRate = NEXUS_VideoFrameRate_e29_97;
    opts->variableFrameRate = true;
    opts->maxVideoBitRate = 8*1000*1000;
    opts->gopFrameP  = 0;
    opts->gopFrameB  = 0;
    opts->muxLatencyTolerance = 20; /* 20 msec */
    opts->audioEncode = false;
    opts->audioBitRate = 48000;
    opts->audioTranscodec = NEXUS_AudioCodec_eAac;
#endif
    opts->decode = true;

    for (i=0;i<argc;i++)
    {
        rc = cmdline_parse_common(i, argc, argv, &opts->common, &state, print_usage);
        if(rc==0) break;
        if(rc<0) return rc;

        /* unknown option */
        i = rc;
        if (!strcmp(argv[i], "-pids") && i+1<argc) {
            unsigned j;
            char *cur, pid[16+1];
            const char *prev;

            /* parse the comma-separated list of pids */
            j=0;
            prev=argv[i+1];
            while (j<MAX_RECORD_PIDS) {
                if ((cur=strchr(prev,','))) {
                    if ((cur-prev)/sizeof(*cur)>16) { continue; }
                    strncpy(pid,prev,(cur-prev)/sizeof(*cur));
                    opts->otherPids[j] = strtoul(pid, NULL, 0);
                    j++;
                    prev = cur+1;
                }
                else {
                    strncpy(pid,prev,sizeof(pid));
                    opts->otherPids[j] = strtoul(pid, NULL, 0);
                    j++;
                    break;
                }
                pid[0]='\0';
            }
            opts->numOtherPids = j;
            i++;
        }
        else if (!strcmp(argv[i], "-decode") && i+1<argc) {
            opts->decode = strcasecmp(argv[++i], "off");
        }
        else if (!strcmp(argv[i], "-streamer")) {
            opts->streamer = true;
        }
        else if (!strcmp(argv[i], "-playfile")) {
            opts->playfname = argv[++i];
            printf("-playfile %s\n", opts->playfname);
        }
#if NEXUS_HAS_FRONTEND
        else if (!strcmp(argv[i], "-vsb")) {
            opts->vsbMode = lookup(g_vsbModeStrs, argv[++i]);
        }
        else if (!strcmp(argv[i], "-qam")) {
            opts->qamMode = lookup(g_qamModeStrs, argv[++i]);
        }
        else if (!strcmp(argv[i], "-sat")) {
            opts->satMode = lookup(g_satModeStrs, argv[++i]);
        }
        else if (!strcmp(argv[i], "-freq")) {
            opts->freq = strtoul(argv[++i], NULL, 0);
        }
#endif
#if NEXUS_HAS_HDMI_INPUT
        else if (!strcmp(argv[i], "-hdmi_input")) {
            opts->hdmiInput = true;
        }
#endif
#if NEXUS_HAS_VIDEO_ENCODER
        else if (!strcmp(argv[i], "-encode")) {
            opts->encoder = true;
        }
        else if (!strcmp(argv[i], "-encode_audio")) {
            opts->audioEncode = true;
        }
        else if (!strcmp(argv[i], "-audio_bit_rate")) {
            opts->audioBitRate    = strtoul(argv[++i], NULL, 0);
        }
        else if (!strcmp(argv[i], "-encode_audio_type")) {
            opts->audioTranscodec = lookup(g_audioCodecStrs, argv[++i]);
        }
        else if (!strcmp(argv[i], "-encode_format")) {
            opts->encodeFormat = lookup(g_videoFormatStrs, argv[++i]);
        }
        else if (!strcmp(argv[i], "-encode_video_type")) {
            opts->videoTranscodec = lookup(g_videoCodecStrs, argv[++i]);
        }
        else if (!strcmp(argv[i], "-video_codec_profile")) {
            opts->videoProfile = lookup(g_videoCodecProfileStrs, argv[++i]);
        }
        else if (!strcmp(argv[i], "-video_codec_level")) {
            opts->videoLevel = lookup(g_videoCodecLevelStrs, argv[++i]);
        }
        else if (!strcmp(argv[i], "-encode_frame_rate")) {
            opts->encodeFrameRate = lookup(g_videoFrameRateStrs, argv[++i]);
        }
        else if (!strcmp(argv[i], "-video_bit_rate")) {
            opts->maxVideoBitRate    = strtoul(argv[++i], NULL, 0);
        }
        else if (!strcmp(argv[i], "-gop_frameP")) {
            opts->gopFrameP = strtoul(argv[++i], NULL, 0);
        }
        else if (!strcmp(argv[i], "-gop_frameB")) {
            opts->gopFrameB = strtoul(argv[++i], NULL, 0);
        }
        else if (!strcmp(argv[i], "-mux_latency_tolerance")) {
            opts->muxLatencyTolerance    = strtoul(argv[++i], NULL, 0);
        }
#endif
        else if (!strcmp(argv[i], "-allpass")) {
            opts->allpass = true;
        }
        else if (!strcmp(argv[i], "-acceptnull")) {
            opts->acceptNullPackets = true;
        }
        else if (!strcmp(argv[i], "-data_buffer_size") && i+1<argc) {
            opts->data.bufferSize = strtoul(argv[++i], NULL, 0);
        }
        else if (!strcmp(argv[i], "-data_data_ready_threshold") && i+1<argc) {
            opts->data.dataReadyThreshold = strtoul(argv[++i], NULL, 0);
        }
        else if (!strcmp(argv[i], "-index_buffer_size") && i+1<argc) {
            opts->index.bufferSize = strtoul(argv[++i], NULL, 0);
        }
        else if (!strcmp(argv[i], "-index_data_ready_threshold") && i+1<argc) {
            opts->index.dataReadyThreshold = strtoul(argv[++i], NULL, 0);
        }
        else if (!opts->recfname) {
            opts->recfname = argv[i];
        }
        else if (!opts->recidxname) {
            opts->recidxname = argv[i];
        }
        else {
            printf("unknown param %s\n", argv[i]);
            return -1;
        }
    }

    if(rc!=0) {
        cmdline_parse_common(i, argc, argv, &opts->common, &state, print_usage); /* allow common parser to execute finishing touches */
    }

    if (!opts->recidxname) { /* if index is specified, require video codec to be explicit. otherwise, default to MPEG2 */
        opts->common.videoCodec = NEXUS_VideoCodec_eMpeg2;
    }
    return 0;
}

int cmdline_probe(struct common_opts_t *opts, const char *filename, const char **indexname)
{
    int rc = 0;

    if (opts->probe) {
        /* use media probe to set values */
        bmedia_probe_t probe = NULL;
        bmedia_probe_config probe_config;
        const bmedia_probe_stream *stream = NULL;
        const bmedia_probe_track *track = NULL;
        bfile_io_read_t fd = NULL;
        bpcm_file_t pcm_file = NULL;
        bool foundAudio = false, foundVideo = false;
        FILE *fin;
        char stream_info[512];

        probe = bmedia_probe_create();

        opts->videoCodec = NEXUS_VideoCodec_eUnknown;
        opts->audioCodec = NEXUS_AudioCodec_eUnknown;

        fin = fopen64(filename,"rb");
        if (!fin) {
            printf("can't open media file '%s' for probing\n", filename);
            rc = -1;
            goto done;
        }

        fd = bfile_stdio_read_attach(fin);
        if(opts->pcm) {
            pcm_file = bpcm_file_create(fd, &opts->pcm_config);
            BDBG_ASSERT(pcm_file);
        }

        bmedia_probe_default_cfg(&probe_config);
        probe_config.file_name = filename;
        probe_config.type = bstream_mpeg_type_unknown;
        stream = bmedia_probe_parse(probe, pcm_file?bpcm_file_get_file_interface(pcm_file):fd, &probe_config);

        if(stream && stream->type == bstream_mpeg_type_cdxa) {
            bcdxa_file_t cdxa_file;
            bmedia_stream_to_string(stream, stream_info, sizeof(stream_info));
            printf( "Media Probe:\n" "%s\n\n", stream_info);
            cdxa_file = bcdxa_file_create(fd);
            if(cdxa_file) {
                const bmedia_probe_stream *cdxa_stream;
                cdxa_stream = bmedia_probe_parse(probe, bcdxa_file_get_file_interface(cdxa_file), &probe_config);
                bcdxa_file_destroy(cdxa_file);
                if(cdxa_stream) {
                    bmedia_probe_stream_free(probe, stream);
                    stream = cdxa_stream;
                    opts->cdxaFile = true;
                }
            }
        }
        if (pcm_file) {
            bpcm_file_destroy(pcm_file);
        }

        /* now stream is either NULL, or stream descriptor with linked list of audio/video tracks */
        bfile_stdio_read_detach(fd);

        fclose(fin);
        if(!stream) {
            printf("media probe can't parse stream '%s'\n", filename);
            rc = -1;
            goto done;
        }

        /* if the user has specified the index, don't override */
        if (indexname && !*indexname) {
            if (stream->index == bmedia_probe_index_available || stream->index == bmedia_probe_index_required) {
                *indexname = filename;
            }
        }

        bmedia_stream_to_string(stream, stream_info, sizeof(stream_info));
        printf(
            "Media Probe:\n"
            "%s\n\n",
            stream_info);

	opts->stream_duration = stream->duration;
       opts->transportType = b_mpegtype2nexus(stream->type);

        if (stream->type == bstream_mpeg_type_ts) {
            if ((((bmpeg2ts_probe_stream*)stream)->pkt_len) == 192) {
                if(opts->tsTimestampType == NEXUS_TransportTimestampType_eNone) {
                    opts->tsTimestampType = NEXUS_TransportTimestampType_eMod300;
                }
            }
        }

        for(track=BLST_SQ_FIRST(&stream->tracks);track;track=BLST_SQ_NEXT(track, link)) {
            switch(track->type) {
                case bmedia_track_type_audio:
                    if(track->info.audio.codec != baudio_format_unknown /*&& !foundAudio*/) {
				if(opts->audioPid && opts->audioCodec)
				{
					opts->audio2Pid = track->number;
					opts->audio2Codec = b_audiocodec2nexus(track->info.audio.codec);
				}
				else
				{
                        		opts->audioPid = track->number;
                        		opts->audioCodec = b_audiocodec2nexus(track->info.audio.codec);
                        		foundAudio = true;
				}
                    }
                    break;
                case bmedia_track_type_video:
                    if(track->info.video.codec == bvideo_codec_h264_svc || track->info.video.codec == bvideo_codec_h264_mvc) {
                        if(opts->detectAvcExtension) {
                            opts->extVideoPid = track->number;
                            opts->extVideoCodec = b_videocodec2nexus(track->info.video.codec);
                        }
                        break;
                    } else if (track->info.video.codec != bvideo_codec_unknown && !foundVideo) {
                        opts->videoPid = track->number;
                        opts->videoCodec = b_videocodec2nexus(track->info.video.codec);
                        foundVideo = true;
                        /* timestamp reordering can be done at the host or decoder.
                           to do it at the decoder, disable it at the host and use media_probe to
                           determine the correct decoder timestamp mode */
                        if (opts->playpumpTimestampReordering == false) {
                            opts->decoderTimestampMode = track->info.video.timestamp_order;
                        }

#if B_HAS_ASF
                        if (stream->type == bstream_mpeg_type_asf) {
                            basf_probe_track *asf_track = (basf_probe_track *)track;
                            if (asf_track->aspectRatioValid) {
                                opts->aspectRatio = NEXUS_AspectRatio_eSar;
                                opts->sampleAspectRatio.x = asf_track->aspectRatio.x;
                                opts->sampleAspectRatio.y = asf_track->aspectRatio.y;
                            }
                            if(asf_track->dynamicRangeControlValid) {
                                opts->dynamicRangeControlValid = true;
                                opts->dynamicRangeControl.peakReference = asf_track->dynamicRangeControl.peakReference;
                                opts->dynamicRangeControl.peakTarget = asf_track->dynamicRangeControl.peakTarget;
                                opts->dynamicRangeControl.averageReference = asf_track->dynamicRangeControl.averageReference;
                                opts->dynamicRangeControl.averageTarget = asf_track->dynamicRangeControl.averageTarget;
                            }
                        }
#endif
                    }
                    break;
                case bmedia_track_type_pcr:
                    opts->pcrPid = track->number;
                    break;
                default:
                    break;
            }
        }

#if B_HAS_AVI
        if (stream->type == bstream_mpeg_type_avi && ((bavi_probe_stream *)stream)->video_framerate && opts->videoFrameRate==0) {
            NEXUS_LookupFrameRate(((bavi_probe_stream *)stream)->video_framerate, &opts->videoFrameRate);
        }
#endif

done:
        if (probe) {
            if (stream) {
                bmedia_probe_stream_free(probe, stream);
            }
            bmedia_probe_destroy(probe);
        }
    }

    return rc;
}

struct {
    NEXUS_VideoCodec nexus;
    bvideo_codec settop;
} g_videoCodec[] = {
    {NEXUS_VideoCodec_eUnknown, bvideo_codec_none},
    {NEXUS_VideoCodec_eUnknown, bvideo_codec_unknown},
    {NEXUS_VideoCodec_eMpeg1, bvideo_codec_mpeg1},
    {NEXUS_VideoCodec_eMpeg2, bvideo_codec_mpeg2},
    {NEXUS_VideoCodec_eMpeg4Part2, bvideo_codec_mpeg4_part2},
    {NEXUS_VideoCodec_eH263, bvideo_codec_h263},
    {NEXUS_VideoCodec_eH264, bvideo_codec_h264},
    {NEXUS_VideoCodec_eH264_Svc, bvideo_codec_h264_svc},
    {NEXUS_VideoCodec_eH264_Mvc, bvideo_codec_h264_mvc},
    {NEXUS_VideoCodec_eVc1, bvideo_codec_vc1},
    {NEXUS_VideoCodec_eVc1SimpleMain, bvideo_codec_vc1_sm},
    {NEXUS_VideoCodec_eDivx311, bvideo_codec_divx_311},
    {NEXUS_VideoCodec_eRv40, bvideo_codec_rv40},
    {NEXUS_VideoCodec_eVp6, bvideo_codec_vp6},
    {NEXUS_VideoCodec_eVp8, bvideo_codec_vp8},
    {NEXUS_VideoCodec_eSpark, bvideo_codec_spark},
    {NEXUS_VideoCodec_eMotionJpeg, bvideo_codec_mjpeg},
    {NEXUS_VideoCodec_eAvs, bvideo_codec_avs}
};

struct {
    NEXUS_AudioCodec nexus;
    baudio_format settop;
} g_audioCodec[] = {
   {NEXUS_AudioCodec_eUnknown, baudio_format_unknown},
   {NEXUS_AudioCodec_eMpeg, baudio_format_mpeg},
   {NEXUS_AudioCodec_eMp3, baudio_format_mp3},
   {NEXUS_AudioCodec_eAac, baudio_format_aac},
   {NEXUS_AudioCodec_eAacPlus, baudio_format_aac_plus},
   {NEXUS_AudioCodec_eAacPlusAdts, baudio_format_aac_plus_adts},
   {NEXUS_AudioCodec_eAacPlusLoas, baudio_format_aac_plus_loas},
   {NEXUS_AudioCodec_eAc3, baudio_format_ac3},
   {NEXUS_AudioCodec_eAc3Plus, baudio_format_ac3_plus},
   {NEXUS_AudioCodec_eDts, baudio_format_dts},
   {NEXUS_AudioCodec_eLpcmHdDvd, baudio_format_lpcm_hddvd},
   {NEXUS_AudioCodec_eLpcmBluRay, baudio_format_lpcm_bluray},
   {NEXUS_AudioCodec_eDtsHd, baudio_format_dts_hd},
   {NEXUS_AudioCodec_eWmaStd, baudio_format_wma_std},
   {NEXUS_AudioCodec_eWmaPro, baudio_format_wma_pro},
   {NEXUS_AudioCodec_eLpcmDvd, baudio_format_lpcm_dvd},
   {NEXUS_AudioCodec_eAvs, baudio_format_avs},
   {NEXUS_AudioCodec_eAmr, baudio_format_amr},
   {NEXUS_AudioCodec_eDra, baudio_format_dra},
   {NEXUS_AudioCodec_eCook, baudio_format_cook},
   {NEXUS_AudioCodec_ePcmWav, baudio_format_pcm},
   {NEXUS_AudioCodec_eAdpcm, baudio_format_adpcm},
   {NEXUS_AudioCodec_eAdpcm, baudio_format_dvi_adpcm},
   {NEXUS_AudioCodec_eVorbis, baudio_format_vorbis},
   {NEXUS_AudioCodec_eLpcm1394, baudio_format_lpcm_1394},
   {NEXUS_AudioCodec_eFlac, baudio_format_flac},
};

struct {
    NEXUS_TransportType nexus;
    unsigned settop;
} g_mpegType[] = {
    {NEXUS_TransportType_eTs, bstream_mpeg_type_unknown},
    {NEXUS_TransportType_eEs, bstream_mpeg_type_es},
    {NEXUS_TransportType_eTs, bstream_mpeg_type_bes},
    {NEXUS_TransportType_eMpeg2Pes, bstream_mpeg_type_pes},
    {NEXUS_TransportType_eTs, bstream_mpeg_type_ts},
    {NEXUS_TransportType_eDssEs, bstream_mpeg_type_dss_es},
    {NEXUS_TransportType_eDssPes, bstream_mpeg_type_dss_pes},
    {NEXUS_TransportType_eVob, bstream_mpeg_type_vob},
    {NEXUS_TransportType_eAsf, bstream_mpeg_type_asf},
    {NEXUS_TransportType_eAvi, bstream_mpeg_type_avi},
    {NEXUS_TransportType_eMpeg1Ps, bstream_mpeg_type_mpeg1},
    {NEXUS_TransportType_eMp4, bstream_mpeg_type_mp4},
    {NEXUS_TransportType_eMkv, bstream_mpeg_type_mkv},
    {NEXUS_TransportType_eWav, bstream_mpeg_type_wav},
    {NEXUS_TransportType_eMp4Fragment, bstream_mpeg_type_mp4_fragment},
    {NEXUS_TransportType_eRmff, bstream_mpeg_type_rmff},
    {NEXUS_TransportType_eFlv, bstream_mpeg_type_flv},
    {NEXUS_TransportType_eOgg, bstream_mpeg_type_ogg},
    {NEXUS_TransportType_eFlac, bstream_mpeg_type_flac}
};

#define CONVERT(g_struct) \
    unsigned i; \
    for (i=0;i<sizeof(g_struct)/sizeof(g_struct[0]);i++) { \
        if (g_struct[i].settop == settop_value) { \
            return g_struct[i].nexus; \
        } \
    } \
    printf("unable to find value %d in %s\n", settop_value, #g_struct); \
    return g_struct[0].nexus

NEXUS_VideoCodec b_videocodec2nexus(bvideo_codec settop_value)
{
    CONVERT(g_videoCodec);
}

NEXUS_AudioCodec b_audiocodec2nexus(baudio_format settop_value)
{
    CONVERT(g_audioCodec);
}

NEXUS_TransportType b_mpegtype2nexus(bstream_mpeg_type settop_value)
{
    CONVERT(g_mpegType);
}

