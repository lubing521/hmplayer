#ifndef NEXUSIO_MFTP_CS_C
#define NEXUSIO_MFTP_CS_C
/*****************************************************************************/
#include <sys/ioctl.h>
#include "nexusio_mftp_cs.h"
#include "../nexus_ctrl.h"
/*****************************************************************************/
char * strzcpy(char *d_str, const char *s_str, size_t s_max)
{
	if(NULL != d_str && NULL != s_str)
	{
		char  *p_max =  d_str + s_max;
		char  *d_ptr =  d_str;

		while(*s_str && d_ptr < p_max)
		{
			*d_ptr = *s_str;

			d_ptr ++;
			s_str ++;
		}

		*d_ptr = '\0';
	}

	return d_str;
}
/*****************************************************************************/
int nexusio_socket_tcp_client_recv(sock_t sock, void *buff, int size, int opts)
{
	fd_set read_sets;
	int iMaxFd , status;
	unsigned long iRet = 0;
	unsigned long iPos =0;
	unsigned long iReadLen = 0;
	char *pbuf = (char *)buff;

	if(sock <= 0)
		return -1;

	iReadLen = size;
	
	while(1)
	{
		FD_ZERO(&read_sets);
		FD_SET(sock,&read_sets);
		iMaxFd = sock + 1;
		status = select(iMaxFd,&read_sets,NULL,NULL,NULL);
		switch(status)
		{
		    case -1:
			goto error_out;
		    case 0:
			continue;
		    default:
		        if(FD_ISSET(sock,&read_sets))
		        {
				iRet = recv(sock,pbuf+iPos,iReadLen,opts);
				if(iRet <=0)
				{
					MSG_APP(("socket_tcp_recv: read error !\n"));
					goto error_out;	
				}
			
				if(iRet==iReadLen)
					goto succ_out;
				
				if(iRet<iReadLen)
				{
					iPos+=iRet ; 
					iReadLen -= iRet;
					continue;
				}
		        }
		        break;
		}    
		}	

succ_out:
	return iPos + iRet;
error_out:
	MSG_APP(("socket_tcp_recv: select timeout\n"));
	return -1;	
}
/*****************************************************************************/
int nexusio_socket_tcp_client_send(sock_t sock, void *buff, int size, int opts)
{
	int ret, size1, fd_max, len;
	fd_set wfds;
	struct timeval tv;
	char *pbuf = (char *)buff;

	if(sock<=0)
		return -1;
	
	size1 = size;
	while (size1 > 0) 
	{
		fd_max = sock;
		FD_ZERO(&wfds);
		FD_SET(sock, &wfds);
		tv.tv_sec = 0;
		tv.tv_usec = 100 * 1000;
		ret = select(fd_max + 1, NULL, &wfds, NULL, &tv);
		if (ret > 0 && FD_ISSET(sock, &wfds)) 
		{
			len = send(sock, pbuf, size1,opts);
			if (len < 0) 
				return -1;

			size1 -= len;
			pbuf += len;
		} 
		else if (ret < 0)
			return -1;

	}
	return size;	
}
/*****************************************************************************/
int nexusio_socket_tcp_client_connect(sock_t *sock, void *addr, int wait)
{
	int tempsock;

	if((tempsock = socket(AF_INET, SOCK_STREAM, 0)) > 0)
	{
		int nonblock;

		nonblock = 1;
		ioctl(tempsock, FIONBIO, &nonblock);

		if(connect(tempsock, (struct sockaddr *)addr, sizeof(struct sockaddr)) < 0)
		{
			struct timeval tv;
			fd_set         fd;

			struct linger  lg;
			socklen_t      sz;
			int            er;
			int            fg;

			tv.tv_sec  = wait;
			tv.tv_usec = 0;

			FD_ZERO(&fd);
			FD_SET(tempsock, &fd);

			if((er = select(tempsock +1, NULL, &fd, NULL, &tv)) < 1)
				goto CNNT_FAIL;

			sz = sizeof(int);

			if(getsockopt(tempsock, SOL_SOCKET, SO_ERROR, &er, &sz))
				goto CNNT_FAIL;

			if(er != 0)
				goto CNNT_FAIL;

			fg = 1;
			setsockopt(tempsock, IPPROTO_TCP, TCP_NODELAY, &fg, sizeof(fg));

			lg.l_onoff  = 1;
			lg.l_linger = 0;
			setsockopt(tempsock, SOL_SOCKET , SO_LINGER  , &lg, sizeof(lg));
		}

		nonblock = 0;
		ioctl(tempsock, FIONBIO, &nonblock);

		*sock = tempsock;
		return 1;

CNNT_FAIL:

		close(tempsock);
		tempsock = (-1);
	}

	*sock = tempsock;
	return 0;
}
/*****************************************************************************/
int nexusio_socket_tcp_client_cleanup(sock_t *sock)
{
	if(*sock > 0)
	{
		shutdown(*sock, 2);

		close(*sock);
		*sock = (-1);

		return 1;
	}

	return 0;
}
/*****************************************************************************/
int nexusio_mftp_udp_ready(int *sock)
{
	if(*sock < 0)
		*sock = socket(AF_INET, SOCK_DGRAM, 0);

	if(*sock < 0)
		return(0);

	fcntl(*sock, F_SETFL,
	fcntl(*sock, F_GETFL) & (~O_NONBLOCK));

	return(1);
}
/*****************************************************************************/
int nexusio_mftp_udp_clean(int *sock)
{
	if(*sock < 0)
		return(0);

	close(*sock);
	*sock = (-1);

	return(1);
}
/*****************************************************************************/
int nexusio_mftp_udp_find_peer(int  sock, SADDR *saddr, int *found, void *peer_hadd)
{
	IADDR  iaddr;
	size_t addrz;

	if(sock >= 0)
	{
		memset(&iaddr, 0, addrz = sizeof(iaddr));

		iaddr.sin_family = AF_INET;
		iaddr.sin_port = htons(NEXUSIO_UDP_MFTP_RECV_KEEP);
		iaddr.sin_addr.s_addr =  inet_addr("127.0.0.1");

		struct nexusio_mftp_pack send_pack;
		struct nexusio_mftp_pack recv_pack;

		send_pack.xhead.xcmmd = NEXUSIO_MFTP_CBD_FIND_PEER;
		send_pack.xhead.xargv = NEXUSIO_MFTP_CBD_FIND_PEER;
		send_pack.xhead.Hsize = 0;
		send_pack.xhead.Lsize = 6;

		memcpy(send_pack.xdata.svar, peer_hadd, 6);

		if(sendto(sock, &send_pack, 10,MSG_NOSIGNAL, (SADDR *)&iaddr, addrz) < 10)
			return 0;

		nexusio_mftp_sock_recv_wait(sock , 2000);

		if(recvfrom(sock, &recv_pack , 8192 , MSG_NOSIGNAL, (SADDR *)&iaddr,&addrz) < 10)
			return 0;

		if(recv_pack.xhead.xcmmd == NEXUSIO_MFTP_CBD_FIND_PEER)
		{
			if((*found = recv_pack.xhead.xargv) != 0)
			{
				memcpy(saddr, recv_pack.xdata.svar + 6 , 16);
			}

			return 1;
		}
	}

	return(0);
}
/*****************************************************************************/
int nexusio_mftp_udp_find_srvr(int  sock, SADDR *addrs, int *count)
{
	IADDR  iaddr;
	size_t addrz;

	if(sock >= 0)
	{
		memset(&iaddr, 0, addrz = sizeof(iaddr));

		iaddr.sin_family = AF_INET;
		iaddr.sin_port = htons(NEXUSIO_UDP_MFTP_RECV_KEEP);
		iaddr.sin_addr.s_addr =  inet_addr("127.0.0.1");

		struct nexusio_mftp_pack send_pack;
		struct nexusio_mftp_pack recv_pack;

		send_pack.xhead.xcmmd = NEXUSIO_MFTP_CBD_FIND_SRVR;
		send_pack.xhead.xargv = NEXUSIO_MFTP_CBD_SRVR_ADDR;
		send_pack.xhead.Hsize = 0;
		send_pack.xhead.Lsize = 0;
		
		if(sendto(sock, &send_pack, 4,MSG_NOSIGNAL, (SADDR *)&iaddr, addrz) < 4)
			return 0;

		DBG_IO(("nexusio_mftp_udp_find_srvr --> sendto \n "));

		nexusio_mftp_sock_recv_wait(sock, 1000);

		if(recvfrom(sock, &recv_pack, 8192,MSG_NOSIGNAL, (SADDR *)&iaddr,&addrz) < 8)
			return 0;

		DBG_IO(("nexusio_mftp_udp_find_srvr --> recvfrom \n "));
		
		if(recv_pack.xhead.xcmmd == NEXUSIO_MFTP_CBD_FIND_SRVR
		&& recv_pack.xhead.xargv == NEXUSIO_MFTP_CBD_SRVR_ADDR)
		{
			memcpy(addrs, recv_pack.xdata.addr_list.array.saddr,
			sizeof(SADDR)* recv_pack.xdata.addr_list.count);

			*count = recv_pack.xdata.addr_list.count;

			return 1;
		}
	}

	return(0);
}
/*****************************************************************************/
int nexusio_mftp_udp_find_song(int  sock, SADDR *addrs, int *count, char *song_code)
{
	IADDR  iaddr;
	size_t addrz;

	if(sock >= 0)
	{
		memset(&iaddr, 0, addrz = sizeof(iaddr));

		iaddr.sin_family = AF_INET;
		iaddr.sin_port = htons(NEXUSIO_UDP_MFTP_RECV_KEEP);
		iaddr.sin_addr.s_addr =  inet_addr("127.0.0.1");

		struct nexusio_mftp_pack send_pack;
		struct nexusio_mftp_pack recv_pack;

		send_pack.xhead.xcmmd = NEXUSIO_MFTP_CBD_FIND_SONG;
		send_pack.xhead.xargv = NEXUSIO_MFTP_CBD_SRVR_ADDR;
		send_pack.xhead.Hsize = 0;
		send_pack.xhead.Lsize = 32;

		strzcpy(send_pack.xdata.svar, song_code, 31);
		
		if(sendto(sock, &send_pack, 36,MSG_NOSIGNAL, (SADDR *)&iaddr, addrz) < 36)
			return 0;

		nexusio_mftp_sock_recv_wait(sock, 2000);

		if(recvfrom(sock, &recv_pack, 8192,MSG_NOSIGNAL, (SADDR *)&iaddr,&addrz) <  8)
			return 0;

		DBG_IO(("nexusio_mftp_udp_find_song -->\n "));
		
		if(recv_pack.xhead.xcmmd == NEXUSIO_MFTP_CBD_FIND_SONG
		&& recv_pack.xhead.xargv == NEXUSIO_MFTP_CBD_SRVR_ADDR)
		{
			DBG_IO(("nexusio_mftp_udp_find_song --> copy addrs\n "));
			
			memcpy(addrs, recv_pack.xdata.addr_list.array.saddr,
			sizeof(SADDR)* recv_pack.xdata.addr_list.count);

			*count = recv_pack.xdata.addr_list.count;

			DBG_IO(("nexusio_mftp_udp_find_song , addr_list.count =%d\n" , *count));

			return 1;
		}

	}

	return(0);
}
/*****************************************************************************/
int nexusio_mftp_udp_read_etha(int  sock, void *haddr)
{
	IADDR  iaddr;
	size_t addrz;

	if(sock >= 0)
	{
		memset(&iaddr, 0, addrz = sizeof(iaddr));

		iaddr.sin_family = AF_INET;
		iaddr.sin_port = htons(NEXUSIO_UDP_MFTP_RECV_KEEP);
		iaddr.sin_addr.s_addr =  inet_addr("127.0.0.1");

		struct nexusio_mftp_pack send_pack;
		struct nexusio_mftp_pack recv_pack;

		send_pack.xhead.xcmmd = NEXUSIO_MFTP_CBD_READ_ETHA;
		send_pack.xhead.xargv = 0;
		send_pack.xhead.Hsize = 0;
		send_pack.xhead.Lsize = 0;

		if(sendto(sock, &send_pack, 4,MSG_NOSIGNAL, (SADDR *)&iaddr, addrz) <  4)
			return 0;

		nexusio_mftp_sock_recv_wait(sock, 1000);

		if(recvfrom(sock, &recv_pack, 8192,MSG_NOSIGNAL, (SADDR *)&iaddr,&addrz) < 10)
			return 0;

		if(recv_pack.xhead.xcmmd == NEXUSIO_MFTP_CBD_READ_ETHA)
		{
			memcpy(haddr, recv_pack.xdata.svar, 6);
			return 1;
		}
	}

	return(0);
}
/*****************************************************************************/
int nexusio_mftp_udp_load_file(int  sock, char *fname, void *fdata, int fsize)
{
	int tsize;
	IADDR  iaddr;
	size_t addrz;
	struct nexusio_mftp_pack send_pack;
	struct nexusio_mftp_pack recv_pack;

	if(sock >= 0)
	{
		memset(&iaddr, 0, addrz = sizeof(iaddr));

		iaddr.sin_family = AF_INET;
		iaddr.sin_port = htons(NEXUSIO_UDP_MFTP_RECV_KEEP);
		iaddr.sin_addr.s_addr =  inet_addr("127.0.0.1");

		send_pack.xhead.xcmmd = NEXUSIO_MFTP_CBD_LOAD_FILE;
		send_pack.xhead.xargv = 0;
		send_pack.xhead.Hsize = 0;
		send_pack.xhead.Lsize = 0;

		strzcpy(send_pack.xdata.svar, fname, 255);

		if(sendto(sock, &send_pack, 260,MSG_NOSIGNAL, (SADDR *)&iaddr, addrz) < 260)
			return -2;

		nexusio_mftp_sock_recv_wait(sock, 2000);

		if(recvfrom(sock, &recv_pack, 8192,MSG_NOSIGNAL, (SADDR *)&iaddr,&addrz) < 260)
			return -3;

		if(recv_pack.xhead.xcmmd != NEXUSIO_MFTP_CBD_LOAD_FILE)
			return -4;

		if(strcmp(send_pack.xdata.svar, recv_pack.xdata.svar))
			return -5;

		if(recv_pack.xhead.xargv == 1)
		{
			tsize  = recv_pack.xhead.Hsize & 255;
			tsize <<= 8;
			tsize |= recv_pack.xhead.Lsize & 255;

			if(fsize > tsize)
				fsize = tsize;

			memcpy(fdata, recv_pack.xdata.svar + 256, fsize);

			return(fsize);
		}

		return -6;
	}

	return -1;
}
/*****************************************************************************/
int nexusio_mftp_udp_save_file(int  sock, char *fname, void *fdata, int fsize)
{
        IADDR  iaddr;
	size_t addrz;
	struct nexusio_mftp_pack send_pack;
	struct nexusio_mftp_pack recv_pack;		

	if(sock >= 0)
	{
		memset(&iaddr, 0, addrz = sizeof(iaddr));

		iaddr.sin_family = AF_INET;
		iaddr.sin_port = htons(NEXUSIO_UDP_MFTP_RECV_KEEP);
		iaddr.sin_addr.s_addr =  inet_addr("127.0.0.1");

		send_pack.xhead.xcmmd = NEXUSIO_MFTP_CBD_SAVE_FILE;
		send_pack.xhead.xargv = 0;
		send_pack.xhead.Hsize = (fsize >> 8);
		send_pack.xhead.Lsize = (fsize >> 0);

		strzcpy(send_pack.xdata.svar, fname, 255);

		memcpy(send_pack.xdata.svar + 256, fdata, fsize);

		if(sendto(sock, &send_pack, fsize + 260,MSG_NOSIGNAL, (SADDR *)&iaddr, addrz) < 260)
			return -2;

		nexusio_mftp_sock_recv_wait(sock, 1000);

		if(recvfrom(sock, &recv_pack, 8192,MSG_NOSIGNAL, (SADDR *)&iaddr,&addrz) < 260)
			return -3;

		if(recv_pack.xhead.xcmmd != NEXUSIO_MFTP_CBD_SAVE_FILE)
			return -4;

		if(strcmp(send_pack.xdata.svar, recv_pack.xdata.svar))
			return -5;

		if(recv_pack.xhead.xargv == 1)
			return fsize;

		return -6;
	}

	return -1;
}
/*****************************************************************************/
int nexusio_mftp_tcp_auto_cnnt(int *sock, SADDR *addrs, int count, char  clnt_code)
{
	nexusio_socket_tcp_client_cleanup((sock_t *)sock);

	int ii;

	for(ii = 0; ii < count; ii ++)
	{
		if(nexusio_socket_tcp_client_connect((sock_t *)sock, addrs + ii, 5))
		{
			char tmpbuff[4];

			tmpbuff[0] = NEXUSIO_MFTP_CREG;
			tmpbuff[1] = clnt_code;
			tmpbuff[2] = 0;
			tmpbuff[3] = 0;

			nexusio_socket_tcp_client_send(*sock, tmpbuff, 4, MSG_NOSIGNAL);

			return 1;
		}
	}
	
	return 0;
}
/*****************************************************************************/
int nexusio_mftp_tcp_shut_down(int *sock)
{
	return nexusio_socket_tcp_client_cleanup((sock_t *)sock);
}
/*****************************************************************************/
int nexusio_mftp_tcp_ctrl_test(int sock)
{
	char pack_head[4];

	if(sock >= 0)
	{
		pack_head[0] = NEXUSIO_MFTP_CTRL;
		pack_head[1] = NEXUSIO_MFTP_KTEST;
		pack_head[2] = 0;
		pack_head[3] = 0;

		if(nexusio_socket_tcp_client_send(sock, pack_head, 4, MSG_NOSIGNAL) < 4)
			return(-1);

		if(nexusio_socket_tcp_client_recv(sock, pack_head, 4, MSG_NOSIGNAL) < 4)
			return(-1);
		else
			return( 1);
	}

	return(-4);
}
/*****************************************************************************/
int nexusio_mftp_tcp_ctrl_null(int sock)
{
	char pack_head[4];

	if(sock >= 0)
	{
		pack_head[0] = NEXUSIO_MFTP_CTRL;
		pack_head[1] = NEXUSIO_MFTP_NULL;
		pack_head[2] = 0;
		pack_head[3] = 0;

		if(nexusio_socket_tcp_client_send(sock, pack_head, 4, MSG_NOSIGNAL) < 4)
			return(-1);
		else
			return( 1);
	}

	return(-4);
}
/*****************************************************************************/
int nexusio_mftp_tcp_file_open(int sock, char *filename,int iLen, char open_mode)
{
	char pack_buff[260];

	if(sock >= 0)
	{
		pack_buff[0] = NEXUSIO_MFTP_OPEN;
		pack_buff[1] = open_mode;
		pack_buff[2] = 1;
		pack_buff[3] = 0;

		strzcpy(pack_buff + 4, filename, 255);

		if(nexusio_socket_tcp_client_send(sock, pack_buff, 260, MSG_NOSIGNAL) < 260)
			return(-1);

		if(nexusio_socket_tcp_client_recv(sock, pack_buff, 260, MSG_NOSIGNAL) < 260)
			return(-1);

		if(pack_buff[0] != NEXUSIO_MFTP_OPEN)
			return(-2);

		if(pack_buff[1] <  NEXUSIO_MFTP_FMIN
		|| pack_buff[1] >  NEXUSIO_MFTP_FMAX)
			return(-3);

		memset(filename,0x00,iLen);
		memcpy(filename, pack_buff + 4, iLen>255?255:iLen);

		return(pack_buff[1]);
	}
	else
		return(-4);
}
/*****************************************************************************/
int nexusio_mftp_tcp_file_read(int sock, char filenumb, char *pack_buff)
{
	if(sock >= 0)
	{
		char pack_head[4];
		long data_size;

		pack_head[0] = NEXUSIO_MFTP_READ;
		pack_head[1] = filenumb;
		pack_head[2] = 0;
		pack_head[3] = 0;

		if(nexusio_socket_tcp_client_send(sock, &pack_head, 4, MSG_NOSIGNAL) < 4)
			return(-1);

		pack_head[0] = 0;
		pack_head[1] = 0;

		if(nexusio_socket_tcp_client_recv(sock, &pack_head, 4, MSG_NOSIGNAL) < 4)
			return(-1);

		if(pack_head[0] != NEXUSIO_MFTP_READ)
			return(-2);

		if(pack_head[1] <  NEXUSIO_MFTP_FMIN
		|| pack_head[1] >  NEXUSIO_MFTP_FMAX)
			return(-3);

		if(nexusio_socket_tcp_client_recv(sock, pack_buff, 49152, MSG_NOSIGNAL) < 49152)
			return(-1);

		data_size   = pack_head[2] & 255;
		data_size <<= 8;
		data_size  |= pack_head[3] & 255;

		return(data_size);
	}
	else
		return(-4);
}
/*****************************************************************************/
int nexusio_mftp_tcp_file_writ(int sock, char filenumb, char *buff, int size)
{
	if(sock >= 0 && size >= 0)
	{
		struct nexusio_mftp_pack pack;

		if(size > 49152)
			size = 49152;

		pack.xhead.xcmmd = NEXUSIO_MFTP_WRIT;
		pack.xhead.xargv = filenumb;
		pack.xhead.Hsize = NEXUSIO_MFTP_HSIZE(size);
		pack.xhead.Lsize = NEXUSIO_MFTP_LSIZE(size);

		memcpy(pack.xdata.svar, buff, size);

		if(nexusio_socket_tcp_client_send(sock, &pack, 49156, MSG_NOSIGNAL) < 49156)
			return(-1);

		if(nexusio_socket_tcp_client_recv(sock , &pack , 4 , MSG_NOSIGNAL) <     4)
			return(-1);

		if(pack.xhead.xcmmd != NEXUSIO_MFTP_WRIT)
			return(-2);

		if(pack.xhead.xargv <  NEXUSIO_MFTP_FMIN
		|| pack.xhead.xargv >  NEXUSIO_MFTP_FMAX)
			return(-3);

		return(size);
	}
	else
		return(-4);
}
/*****************************************************************************/
int nexusio_mftp_tcp_file_seek(int sock, char  filenumb, int seek_post, int orig)
{
	if(sock >= 0)
	{
		struct nexusio_mftp_pack pack;

		pack.xhead.xcmmd = NEXUSIO_MFTP_SEEK;
		pack.xhead.xargv = filenumb;
		pack.xhead.Hsize = 0;
		pack.xhead.Lsize = 8;

		pack.xdata.ivar[0] = seek_post;
		pack.xdata.ivar[1] = orig;

		if(nexusio_socket_tcp_client_send(sock, &pack, 12, MSG_NOSIGNAL) < 12)
			return(-1);

		if(nexusio_socket_tcp_client_recv(sock, &pack,  4, MSG_NOSIGNAL) <  4)
			return(-1);

		DBG_IO(( " nexusio_mftp_tcp_file_seek : %d\n ", pack.xhead.xcmmd));
		
		if(pack.xhead.xcmmd != NEXUSIO_MFTP_SEEK)
			return(-2);

		if(pack.xhead.xargv <  NEXUSIO_MFTP_FMIN
		|| pack.xhead.xargv >  NEXUSIO_MFTP_FMAX)
			return(-3);

		if(nexusio_socket_tcp_client_recv(sock, &pack.xdata.ivar, 4, MSG_NOSIGNAL) < 4)
			return(-1);

		return(pack.xdata.ivar[0]);
	}
	else
		return(-4);
}
/*****************************************************************************/
int nexusio_mftp_tcp_file_shut(int sock, char filenumb)
{
	if(sock >= 0)
	{
		char pack_head[4];

		pack_head[0] = NEXUSIO_MFTP_SHUT;
		pack_head[1] = filenumb;
		pack_head[2] = 0;
		pack_head[3] = 0;

		if(nexusio_socket_tcp_client_send(sock, &pack_head, 4, MSG_NOSIGNAL) < 4)
			return(-1);

		if(nexusio_socket_tcp_client_recv(sock, &pack_head, 4, MSG_NOSIGNAL) < 4)
			return(-1);

		if(pack_head[0] != NEXUSIO_MFTP_SHUT)
			return(-2);

		if(pack_head[1]  < NEXUSIO_MFTP_FMIN
		|| pack_head[1]  > NEXUSIO_MFTP_FMAX)
			return(-3);

		return( 1);
	}
	else
		return(-4);
}
/*****************************************************************************/
int nexusio_mftp_tcp_file_prefetch_start(int sock, char filenumb)
{
	if(sock >= 0)
	{
		char pack_head[4];

		pack_head[0] = NEXUSIO_MFTP_READ;
		pack_head[1] = filenumb;
		pack_head[2] = 0;
		pack_head[3] = 0;

		if(nexusio_socket_tcp_client_send(sock, &pack_head, 4, MSG_NOSIGNAL) < 4)
			return(-1);
		else
			return( 1);
	}

	return(-4);
}
/*****************************************************************************/
int nexusio_mftp_tcp_file_prefetch_clean(int sock)
{
	if(sock >= 0)
	{
		struct nexusio_mftp_pack pack;

		if(nexusio_socket_tcp_client_recv(sock, &pack, 49156 , MSG_NOSIGNAL) < 49156)
			return(-1);

		if(pack.xhead.xcmmd != NEXUSIO_MFTP_READ)
			return(-2);

		if(pack.xhead.xargv <  NEXUSIO_MFTP_FMIN
		|| pack.xhead.xargv >  NEXUSIO_MFTP_FMAX)
			return(-3);
		else
			return( 1);
	}

	return(-4);
}
/*****************************************************************************/
int nexusio_mftp_sock_recv_dump(int sock, int size)
{
	char *buf = malloc(size);

	size_t asize;
	size_t dsize;

	while(ioctl(sock, FIONREAD, &dsize) == 0)
	{
		if(dsize < 1)
		{
			free(buf);
			return(1);
		}

		struct sockaddr addr;

		asize = sizeof(addr);

		recvfrom(sock, buf, size, MSG_NOSIGNAL, &addr, &asize);
	}

	free(buf);

	return(0);
}
/*****************************************************************************/
int nexusio_mftp_sock_recv_wait(int sock, int msec)
{
	fd_set fd_Read;

	FD_ZERO(&fd_Read);
	FD_SET(sock, &fd_Read);

	struct timeval tv_Wait;

	tv_Wait.tv_sec = msec / 1000;
	tv_Wait.tv_usec = (msec % 1000) * 1000;

	return(select(sock + 1, &fd_Read, NULL, NULL, &tv_Wait));
}
/*****************************************************************************/
int nexusio_mftp_sock_send_wait(int sock, int msec)
{
	fd_set fd_Writ;

	FD_ZERO(&fd_Writ);
	FD_SET(sock, &fd_Writ);

	struct timeval tv_Wait;

	tv_Wait.tv_sec = msec / 1000;
	tv_Wait.tv_usec = (msec % 1000) * 1000;

	return(select(sock + 1, NULL, &fd_Writ, NULL, &tv_Wait));
}
/*****************************************************************************/
#endif/*MFTP_CS_C*/
