#ifndef NEXUSIO_CONVERTER_C
#define NEXUSIO_CONVERTER_C
/*****************************************************************************/
#include "nexusio_converter.h"
#include "nexusio_mftp_io.h"
/*****************************************************************************/
/*
static void my_avlog(void *c, int l, const char *f, va_list v)
{
}
*/
/*****************************************************************************/
/*
int nexusio_stream_conver_ready(void)
{
	NEXUSIOProtocol *nexusio_protocol = nexusio_mftp_prtocol_get();

	if(nexusio_protocol == NULL )
		return -1;

	av_register_all();
	av_log_set_callback(my_avlog);
	av_register_protocol(nexusio_protocol);

	return 0;
}
*/
/*****************************************************************************/
int nexusio_stream_conver_demux_close(nexusio_demux *pzone)
{
	if(pzone->p_av_ic)
		av_close_input_file(pzone->p_av_ic);

	memset(pzone, 0, sizeof(nexusio_demux));

	return 0;

}
/*****************************************************************************/
int nexusio_stream_conver_demux_fopen(char *fname , nexusio_demux *demux)
{
	uint32_t i ;
	AVCodecContext *avcc;
	
	nexusio_stream_conver_demux_close(demux);

	printf("stream_conver_demux_fopen : song = %s \n",fname);
	
	demux->i_v0_si = -1;
	demux->i_a1_si = -1;
	demux->i_a2_si = -1;

	
	if(av_open_input_file(&demux->p_av_ic, fname,demux->p_av_if, 0,  demux->p_av_fp) < 0)
		goto FAIL_QUIT_0;

	if(av_find_stream_info(demux->p_av_ic) < 0)
		goto FAIL_QUIT_1;

	printf("open file  ok , nb = %d , type = %s\n",demux->p_av_ic->nb_streams , demux->p_av_ic->iformat->name);
	
	for(i = 0; i < demux->p_av_ic->nb_streams; i ++)
	{
		avcc = demux->p_av_ic->streams[i]->codec;
		
		switch(demux->p_av_ic->streams[i]->codec->codec_type)
		{
			case CODEC_TYPE_VIDEO:
				demux->i_v0_si  = i ; 
				demux->p_v0_cc = avcc;
				demux->i_vo_pid = demux->p_av_ic->streams[i]->id;
				break;
			case CODEC_TYPE_AUDIO:
				if(demux->i_a1_si < 0)
				{
					demux->i_a1_si  = i ;
					demux->p_a1_cc = avcc;
					demux->i_a1_pid = demux->p_av_ic->streams[i]->id;					
				}
				else
				if(demux->i_a2_si < 0)
				{
					demux->i_a2_si  = i ;
					demux->p_a2_cc = avcc;
					demux->i_a2_pid = demux->p_av_ic->streams[i]->id;
				}
				default:
					break;
		}
	
	}

	return 0;

FAIL_QUIT_1:
	printf("av_find_stream_info error out !\n");
	av_close_input_file(demux->p_av_ic);
FAIL_QUIT_0:

	printf("av_open_input_file error out !\n");
	
	memset(demux, 0, sizeof(nexusio_demux));

	demux->i_v0_si = -1;
	demux->i_a1_si = -1;
	demux->i_a2_si = -1;

	return -1;
}
/*****************************************************************************
int nexusio_stream_conver_check_link(int sockfd , int *iRecv)
{
	int status;
	fd_set read_sets;
	struct timeval tv;
	char szbuf[100];

	if(sockfd<=0)
		return 0;

	tv.tv_sec = 0;
	tv.tv_usec = 200000;		

	while(1)
	{
		FD_ZERO(&read_sets);
		FD_SET(sockfd,&read_sets);

		status = select(sockfd+1 , &read_sets,NULL,NULL,&tv);
		switch(status)
		{
		    case -1:
			goto link_done;
		    case 0:
			goto time_out;
		    default:
		        if( FD_ISSET(sockfd,&read_sets))
		        	{
				*iRecv = recv(sockfd,szbuf,sizeof(szbuf),0);
				if(*iRecv <= 0)
					goto link_done;	
		        }
		        break;
		}    
	}
	
link_done:
	return 1;
time_out:
	return 0;
}
*/
/*****************************************************************************/

/*
int stream_conver_init(bc7405_pstream *stream, const char *fname)
{
	t_stream_info stream_head;
	stream_conver_info *info = NULL;

	if(stream == NULL || fname == NULL)
		return 0;
	
	info = &stream->info;
	
	stream_head.struct_flag = 1;
	
	debug_sock_send("stream_conver_init in\n");

	if(info->conver_state != CONVERTER_CONVER_STATE_NONE)
		stream_conver_close(info);

	debug_sock_send("stream_conver_demux_fopen start, song = %s\n",fname);
	
	if(stream_conver_demux_fopen(info->demux , fname) == 0)
		goto demux_error_out;

	debug_sock_send("stream_conver_demux_fopen ok\n");

	if(stream_conver_demux_set_stream_type(info->demux ,  &stream_head))
		goto demux_error_out;

	if(stream_conver_demux_set_video_type(info->demux , &stream_head))
		goto demux_error_out;
	
	debug_sock_send("check video  ok\n");

	if(stream_conver_demux_set_audio_type(info->demux , &stream_head))
		goto demux_error_out;
	
	debug_sock_send("check video  ok\n");
	
	if(stream_conver_demux_set_stream_info(info->demux , &stream_head))
		goto demux_error_out;

	strcpy(stream_head.szSongName , fname);
	
	debug_sock_send("add stream!\n");

	if(!stream_conver_check_dev(stream , &stream_head))
	{
		debug_sock_send("stream_conver_check_dev error!\n");
		stream_conver_close(info);
	}
	
	info->v_stream_sock = stream->v_stream_sock;
	
	stream_conver_demux_close(info->demux);

	return 1;

demux_error_out:
	debug_sock_send("demux_error_out\n");
	stream_conver_demux_close(info->demux);
	return 0;
}
*/
/*****************************************************************************/
#endif/*BC7405_USTREAM_C*/
