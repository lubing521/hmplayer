/*
mody by hh , add time out return for socket_tcp_server_read_timeout fuc 2012_02_01
*/
#ifndef NEXUS_TRAMSPORT_C
#define NEXUS_TRAMSPORT_C
/*****************************************************************************/
#include <stddef.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <net/if.h>
 #include <sys/ioctl.h>

#include "nexus_player.h"
#include "nexus_transport.h"
/*****************************************************************************/
static int nexus_debug_sock = -1;
static int nexus_debug_mode = -1;
static struct sockaddr_in nexus_debug_addr;
/*****************************************************************************/
void signalTransportProc(int sig)
{
        switch(sig)
        {
		case SIGPIPE:
			DBG_APP(("SIGPIPE"));
			break;
		case EFFECT_k9001_SIGNED:
			DBG_APP(("K9001 SIGNEL"));
			break;
		case PLAYER_DEBUG_SIGNED:
			if(NEXUSAPP_Debug_State_Get())
				NEXUSAPP_Debug_State_Set(0);
			else
				NEXUSAPP_Debug_State_Set(1);
			DBG_APP(("PLAYER_DEBUG_SIGNED signal!"));
			break;
		default:
			DBG_APP(("Unknow signal!"));		
			break;
        }
}
/*****************************************************************************/
int socket_unix_server_init(unixsock *socket_info)
{
	
	int n = 0;

	if(strlen(socket_info->szName) <= 0)
		return -1;

	socket_info->listen_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (-1 == socket_info->listen_fd)
	{
		MSG_APP(("call to socket\n"));
		return -1;
	}
	
	unlink(socket_info->szName); /* in case it already exists */

	memset(&socket_info->un, 0, sizeof(struct sockaddr_un));
	socket_info->un.sun_family = AF_UNIX;
	strcpy(socket_info->un.sun_path, socket_info->szName);
	n = offsetof(struct sockaddr_un, sun_path) + strlen(socket_info->szName);
	
	if (bind(socket_info->listen_fd, (struct sockaddr *)&socket_info->un, n)<0)
	{
		MSG_APP(("call to bind\n"));
		return -1;
	}
	
	n = listen(socket_info->listen_fd, 1);
	if (-1 == n)
	{
		MSG_APP(("call to listen\n"));
		return -1;
	}
	
	MSG_APP(("Accepting connections...\n"));

	return 0;
}
/*****************************************************************************/
int socket_unix_server_open(unixsock *socket)
{	
	int address_size = sizeof(struct sockaddr_un);
	int nRecvBuf = 8*1024;
	int sockfd;

	sockfd = accept(socket->listen_fd, (struct sockaddr *)&socket->un, (socklen_t *)&address_size);
	MSG_APP(("Accepting one connection ,start read!,socket =%d,name=%s\n",sockfd,socket->szName));

	setsockopt(sockfd,SOL_SOCKET,SO_RCVBUF,(const char*)&nRecvBuf,sizeof(int));
	
	unlink(socket->un.sun_path);
	
	if(sockfd > 0)
		return sockfd;
	else
		return -1;
}
/*****************************************************************************/
int socket_tcp_server_init(socket_t *socket_info, int port)
{
	
	int n = 0;
	int nRecvBuf=8*1024;
	int iReused = 1;

	bzero(&socket_info->sin, sizeof(socket_info->sin));
	socket_info->sin.sin_family = AF_INET;
	socket_info->sin.sin_addr.s_addr = INADDR_ANY;
	socket_info->sin.sin_port = htons(port);

	bzero(&socket_info->pin, sizeof(socket_info->pin));

	socket_info->listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == socket_info->listen_fd)
	{
		MSG_APP(("call to socket\n"));
		return -1;
	}

	setsockopt(socket_info->listen_fd,SOL_SOCKET,SO_REUSEADDR , &iReused,sizeof(int));

	n = bind(socket_info->listen_fd, (struct sockaddr *)&socket_info->sin, sizeof(socket_info->sin));
	if (-1 == n)
	{
		MSG_APP(("call to bind\n"));
		return -1;
	}

	setsockopt(socket_info->listen_fd,SOL_SOCKET,SO_RCVBUF,(const char*)&nRecvBuf,sizeof(int));
	
	n = listen(socket_info->listen_fd, 2);
	if (-1 == n)
	{
		MSG_APP(("call to listen\n"));
		return -1;
	}
	MSG_APP(("Accepting connections...\n"));

	socket_info->port = port;
	return 0;

}
/*****************************************************************************/
int socket_tcp_server_open(socket_t *socket)
{	
	int address_size = sizeof(socket->pin);
	int nRecvBuf=8*1024;
	int sockfd;

	sockfd = accept(socket->listen_fd, (struct sockaddr *)&socket->pin, (socklen_t *)&address_size);
	MSG_APP(("Accepting one connection ,start read!,socket =%d,port=%d\n",sockfd,socket->port));
	
	if(sockfd > 0)
	{
		setsockopt(sockfd,SOL_SOCKET,SO_RCVBUF,(const char*)&nRecvBuf,sizeof(int));
		return sockfd;
	}
	else
		return -1;
}
/*****************************************************************************/
int socket_tcp_server_close(int *sock_fd)
{
	if(*sock_fd <= 0)
		return 0;

	close(*sock_fd);
	*sock_fd = -1;
	
	return 0;
}
/*****************************************************************************/
int socket_tcp_server_send(int sock_fd , char *buf, int size)
{
	int ret, size1, fd_max, len;
	fd_set wfds;
	struct timeval tv;

	if(sock_fd<=0)
		return -1;
	
	size1 = size;
	while (size1 > 0) 
	{
		fd_max = sock_fd;
		FD_ZERO(&wfds);
		FD_SET(sock_fd, &wfds);
		tv.tv_sec = 0;
		tv.tv_usec = 100 * 1000;
		ret = select(fd_max + 1, NULL, &wfds, NULL, &tv);
		if (ret > 0 && FD_ISSET(sock_fd, &wfds)) 
		{
			len = send(sock_fd, buf, size1,MSG_NOSIGNAL);
			if (len < 0) 
				return -1;

			size1 -= len;
			buf += len;
		} 
		else if (ret < 0)
			return -1;

	}
	return 0;
}
/*****************************************************************************/
int socket_tcp_server_read(int sock_fd , char *szBuff, unsigned long *iBuffLen)
{

	fd_set read_sets;
	int iMaxFd , status;
	unsigned long iRet = 0;
	unsigned long iPos =0;
	unsigned long iReadLen = 0;

	if(sock_fd<=0)
		return -1;
	
	iReadLen = *iBuffLen;
	while(1)
	{
		FD_ZERO(&read_sets);
		FD_SET(sock_fd,&read_sets);
		iMaxFd=sock_fd+1;
		status = select(iMaxFd,&read_sets,NULL,NULL,NULL);
		switch(status)
		{
		    case -1:
			DBG_APP(("socket_tcp_recv: select error\n"));	
			goto error_out;
		    case 0:
			continue;
		    default:
		        if( FD_ISSET(sock_fd,&read_sets))
		        	{
				iRet = recv(sock_fd,szBuff+iPos,iReadLen,0);
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
/*	MSG_DISPLAY(("succ out : recv data : %lu , hope : %lu \n",iPos+iRet,*iBuffLen));*/
	*iBuffLen = iPos + iRet;
	return 0;
error_out:
/*	MSG_DISPLAY(("error out : recv data : %lu , hope : %lu \n",iPos+iRet,*iBuffLen));*/
	*iBuffLen = 0;
	MSG_APP(("socket_tcp_recv: select timeout\n"));
	return -1;
}
/*****************************************************************************/
int socket_tcp_server_read_im(int sock_fd , char *szBuff, unsigned long *iBuffLen)
{

	fd_set read_sets;
	int iMaxFd , status;
	struct timeval tv;
	unsigned long iRet = 0;
	unsigned long iReadLen = 0;

	iReadLen = *iBuffLen;
	
	FD_ZERO(&read_sets);
	FD_SET(sock_fd,&read_sets);
	iMaxFd=sock_fd+1;    	

	tv.tv_sec = 0;
	tv.tv_usec = 50000;
	status = select(iMaxFd,&read_sets,NULL,NULL,&tv);
	switch(status)
	{
	    case -1:
		DBG_APP(("socket_tcp_recv: select error\n"));	
		return -1;
	    case 0:
		return 0;
	    default:
	        if( FD_ISSET(sock_fd,&read_sets))
	        	{
			iRet = recv(sock_fd,szBuff,iReadLen,0);
			if(iRet <=0)
			{
				MSG_APP(("socket_tcp_recv: read error !\n"));
				return -1;	
			}
			*iBuffLen = iRet;
			return 0;	
	        }
	        break;
	}    
	
	MSG_APP(("socket_tcp_recv: select timeout\n"));
	return 0;
}
/*****************************************************************************/
int socket_tcp_server_read_timeout(int sock_fd , char *szBuff, unsigned long *iBuffLen ,unsigned int usTimeout)
{
	fd_set read_sets;
	int iMaxFd , status;
	struct timeval tv;
	unsigned long iRet = 0;
	unsigned long iPos =0;
	unsigned long iReadLen = 0;

	if(sock_fd<=0)
		return -1;
	
	iReadLen = *iBuffLen;
	while(1)
	{
		FD_ZERO(&read_sets);
		FD_SET(sock_fd,&read_sets);
		iMaxFd=sock_fd+1;	
		tv.tv_sec = usTimeout;
		tv.tv_usec = 0;
		status = select(iMaxFd,&read_sets,NULL,NULL,&tv);
		switch(status)
		{
		    case -1:
			DBG_APP(("socket_tcp_recv: select error\n"));	
			goto error_out;
		    case 0:
			DBG_APP(("socket_tcp_recv: select time out!\n"));		
			goto time_out;
		    default:
		        if( FD_ISSET(sock_fd,&read_sets))
		        {
				iRet = recv(sock_fd,szBuff+iPos,iReadLen,0);
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
	*iBuffLen = iPos + iRet;
	return 0;
error_out:
	*iBuffLen = 0;
	return -1;
/* mody by hh , add time out return 2012_02_01*/ 	
time_out:
	*iBuffLen = 0;
	return -2;
}

/*****************************************************************************/
int socket_tcp_server_check_link(int sockfd , unsigned long timeout)
{
	int status;
	fd_set read_sets;
	struct timeval tv;
	char szbuf[100];

	if(sockfd <= 0)
		return 1;

	tv.tv_sec = 0;
	tv.tv_usec = timeout;

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
				if(recv(sockfd,szbuf,sizeof(szbuf),0) <= 0)
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
/*****************************************************************************/
int nexus_debug_sock_open(const char *addr)
{
       /*bind*/
	struct sockaddr_in bind_addr;

	if(1)
	{
		/*sock*/
		nexus_debug_sock = socket(AF_INET, SOCK_DGRAM, 0);

		memset(&bind_addr, 0, sizeof(bind_addr));

		bind_addr.sin_family = AF_INET;
		bind_addr.sin_addr.s_addr = INADDR_ANY;

		if(bind(nexus_debug_sock, (struct sockaddr *)
		  &bind_addr, sizeof(bind_addr)) != 0)
		{
			close(nexus_debug_sock);
			nexus_debug_sock = (-1);

			return 0;
		}

		/*addr*/
		if(addr == NULL)
		{
			struct ifreq ifr;

			memset(&ifr, 0, sizeof(ifr));

			strcpy(ifr.ifr_name, "eth0");

			ifr.ifr_addr.sa_family = AF_INET;

			if(ioctl(nexus_debug_sock, SIOCGIFBRDADDR, &ifr) == 0)
			{
				nexus_debug_mode = 2;
				 
				memcpy(&bind_addr, &(ifr.ifr_addr), sizeof(bind_addr));

				memset(&nexus_debug_addr, 0, sizeof(nexus_debug_addr));

				nexus_debug_addr.sin_family = AF_INET;
				nexus_debug_addr.sin_port = htons(2022);
				nexus_debug_addr.sin_addr.s_addr = bind_addr.sin_addr.s_addr;

				setsockopt(nexus_debug_sock, SOL_SOCKET, SO_BROADCAST, &nexus_debug_mode, 4);
			}
			else
			{
				nexus_debug_mode = 1;

				memset(&nexus_debug_addr, 0, sizeof(nexus_debug_addr));

				nexus_debug_addr.sin_family = AF_INET;
				nexus_debug_addr.sin_port = htons(2022);
				nexus_debug_addr.sin_addr.s_addr = INADDR_BROADCAST;

				setsockopt(nexus_debug_sock, SOL_SOCKET, SO_BROADCAST, &nexus_debug_mode, 4);
			}
		}
		else
		{

			memset(&nexus_debug_addr, 0, sizeof(nexus_debug_addr));

			nexus_debug_addr.sin_family = AF_INET;
			nexus_debug_addr.sin_port = htons(2022);
			nexus_debug_addr.sin_addr.s_addr = inet_addr(addr);
		}
	}

	return 2;
}
/*****************************************************************************/
int nexus_debug_sock_send(const char *text, ...)
{
	if(NEXUSAPP_Debug_State_Get() && nexus_debug_sock > 0 )
	{
		va_list ap;

		char cstr[1024];

		va_start(ap, text);

		vsnprintf(cstr, 1022, text, ap);

		va_end(ap);

		sendto(nexus_debug_sock, cstr, strlen(cstr) -1, MSG_NOSIGNAL,
			(struct sockaddr *)&nexus_debug_addr, sizeof(nexus_debug_addr));

		return 1;
	}

	return 0;
}

/*****************************************************************************/
#endif
