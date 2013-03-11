#ifndef NEXUSIO_CONVERTER_H
#define NEXUSIO_CONVERTER_H
/*****************************************************************************/
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
/*****************************************************************************/
#define CONVERTER_DEMUX_STATE_OPENED	1
#define CONVERTER_DEMUX_STATE_STANDBY	2
/*****************************************************************************/
typedef struct nexusio_demux_t
{
	AVFormatParameters *p_av_fp;
	AVFormatContext    *p_av_ic;
	AVInputFormat      *p_av_if;
	
	AVCodecContext     *p_v0_cc;
	int	i_v0_si;
	unsigned int	i_vo_pid;

	AVCodecContext     *p_a1_cc;
	int	i_a1_si;
	unsigned int	i_a1_pid;

	AVCodecContext     *p_a2_cc;
	int	i_a2_si;
	unsigned int	i_a2_pid;

	char stream_type[128];
}   nexusio_demux;

/*****************************************************************************/
int nexusio_stream_conver_ready(void);
int nexusio_stream_conver_demux_close(nexusio_demux *pzone);
int nexusio_stream_conver_demux_fopen(char *fname , nexusio_demux *demux);
/*****************************************************************************/
#endif
