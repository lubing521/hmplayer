/***********************************************************************************************/
#ifndef NEXUS_TRAMSPORT_H
#define NEXUS_TRAMSPORT_H
/***********************************************************************************************/
#include <sys/un.h>
#include <netinet/in.h>
/***********************************************************************************************/
#define LINE_CHECK 		"LINECHECK"
#define LINE_CHECK_OK	"LINECHECKOK"
/***********************************************************************************************/
typedef struct socket_t
{
	struct sockaddr_in sin;
	struct sockaddr_in pin;
	int listen_fd;
	int port;
}socket_t;
/***********************************************************************************************/
typedef struct unix_socket_t
{
	struct sockaddr_un  un;
	int listen_fd;
	char szName[64];
}unixsock;
/*************************************************
  Function:      	signalTransportProc
  Description:   get socket transport signal , deal with SIGPIPE signal
  Input:          
  Output:         
  Return:         
*************************************************/
void signalTransportProc(int sig);
/*************************************************
  Function:    	socket_tcp_server_init
  Description:	tcp server init ,  create server socket and start listen
  Input:		1 . port 			  : listen port   
  Output:         	1 . socket_t *socket : socket info struct
  Return:            1 .  0 	:  succeed
  			2 .   -1 	:  socket init error ( create error ,bind error , listen error )
*************************************************/
int socket_tcp_server_init(socket_t *socket, int port);
/*************************************************
  Function:    	socket_tcp_server_open
  Description:	tcp server accept one new client , and set opt
  Input:		1 . socket_t *socket : socket info struct
  Output:         	
  Return:            1 .  >0 	:  socket fd
  			2 .   -1 	:  open error
*************************************************/
int socket_tcp_server_open(socket_t *socket);
/*************************************************
  Function:    	socket_tcp_server_close
  Description:	tcp server close one client
  Input:		1 . int *sock_fd : socket fd
  Output:         	1 . int *sock_fd : if close fd succeed, socket fd will be set -1
  Return:            1 .  0 	:  succeed
*************************************************/
int socket_tcp_server_close(int *sock_fd);
/*************************************************
  Function:    	socket_tcp_server_send
  Description:	send tcp data with socket
  Input:		1 . int sock_fd 		: socket fd
  			2 . char *szSend 	: send data buffer
  			3 . int iLen 		: send data size
  Output:         	
  Return:            1 .  0 	:  succeed
  			2 .   -1 	:  error
*************************************************/
int socket_tcp_server_send( int sock_fd , char *szSend , int iLen);
/*************************************************
  Function:    	socket_tcp_server_read
  Description:	receive tcp data from socket
  Input:		1 . int sock_fd 				: socket fd
  Output:		1 . char *szBuff			: receive data buffer
  			2 . unsigned long *iBuffLen	: receive data size
  Return:            1 .  0 	:  succeed
  			2 .   -1 	:  error
*************************************************/
int socket_tcp_server_read(int sock_fd , char *szBuff, unsigned long *iBuffLen);
/*************************************************
  Function:    	socket_tcp_server_read
  Description:	receive tcp data from socket
  Input:		1 . int sock_fd 				: socket fd
  			2 . unsigned int usTimeout	: time out , unit is second
  Output:		1 . char *szBuff			: receive data buffer
  			2 . unsigned long *iBuffLen	: receive data size
  Return:            1 .  0 	:  succeed
  			2 .   -1 	:  error
*************************************************/
int socket_tcp_server_read_timeout(int sock_fd , char *szBuff, unsigned long *iBuffLen ,unsigned int usTimeout);
/*************************************************
  Function:    	socket_tcp_server_check_link
  Description:	check socket link , no need recv data used
  Input:		1 . int sock_fd 				: socket fd
  			2 . unsigned int usTimeout	: time out , unit is second
  Return:            1 .  0 	:   timeout ,link ok!
  			2 .  1 	:   link down!!!
*************************************************/
int socket_tcp_server_check_link(int sockfd , unsigned long usTimeout);
int socket_unix_server_open(unixsock *socket);
int socket_unix_server_init(unixsock *socket_info);

int nexus_debug_sock_open(const char *addr);
int nexus_debug_sock_send(const char *text, ...);

/*************************************************/

#endif

