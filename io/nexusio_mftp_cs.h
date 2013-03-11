#ifndef NEXUSIO_MFTP_CS_H
#define NEXUSIO_MFTP_CS_H
/*****************************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <netdb.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <net/if.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
/*****************************************************************************/
#ifndef sock_t
typedef int sock_t;
#endif
/*****************************************************************************/
#ifndef XADDR_TYPE
#define XADDR_TYPE
typedef char EADDR[6];
typedef struct sockaddr SADDR;
typedef struct sockaddr_in IADDR;
#endif/*XADDR_TYPE*/
/*****************************************************************************/
#ifndef SHUT2
#define SHUT2(s) shutdown(s, 2)
#endif/*SHUT2*/
/*****************************************************************************/
#define NEXUSIO_TCP_MFTP_SRVR_PORT 2004
#define NEXUSIO_UDP_MFTP_MISC_PORT 2005
#define NEXUSIO_UDP_MFTP_SEND_KEEP 2015
#define NEXUSIO_UDP_MFTP_RECV_KEEP 2025
/*****************************************************************************/
/*TCP main command set*/
#define NEXUSIO_MFTP_NULL 0

#define NEXUSIO_MFTP_CREG 1
#define NEXUSIO_MFTP_QUIT 2

#define NEXUSIO_MFTP_OPEN 3
#define NEXUSIO_MFTP_SHUT 4

#define NEXUSIO_MFTP_READ 5
#define NEXUSIO_MFTP_WRIT 6
#define NEXUSIO_MFTP_SEEK 7

#define NEXUSIO_MFTP_CTRL 8
#define NEXUSIO_MFTP_LIST 9

/*UDP main command set*/
#define NEXUSIO_MFTP_UFIND 23
#define NEXUSIO_MFTP_ULOAD 25
#define NEXUSIO_MFTP_USAVE 26
#define NEXUSIO_MFTP_UCTRL 28
#define NEXUSIO_MFTP_UMONI 29

/*CBD main command set*/
#define NEXUSIO_MFTP_CBD_FIND_SRVR 99
#define NEXUSIO_MFTP_CBD_FIND_SONG 98
#define NEXUSIO_MFTP_CBD_FIND_PEER 97

#define NEXUSIO_MFTP_CBD_SAVE_FILE 86
#define NEXUSIO_MFTP_CBD_LOAD_FILE 85
#define NEXUSIO_MFTP_CBD_READ_ETHA 84

#define NEXUSIO_MFTP_CBD_REQ_ADDR 96
#define NEXUSIO_MFTP_CBD_RES_ADDR 95

#define NEXUSIO_MFTP_CBD_SRVR_ADDR 1 /*for find_srvr & find_song*/
#define NEXUSIO_MFTP_CBD_SRVR_BURT 2 /*for find_srvr & find_song*/

/*open file : mode*/
#define NEXUSIO_MFTP_ORDP 0
#define NEXUSIO_MFTP_ORDO 1
#define NEXUSIO_MFTP_OWRO 2
#define NEXUSIO_MFTP_OWRD 3
#define NEXUSIO_MFTP_OCWR 7

/*open file : rang*/
#define NEXUSIO_MFTP_FMIN 1
#define NEXUSIO_MFTP_FMAX 7

/*ctrl sub_command*/
#define NEXUSIO_MFTP_KTEST 1
#define NEXUSIO_MFTP_KHDSZ 2 /*get hard-disk size */
#define NEXUSIO_MFTP_KSYSV 3 /*get system version */
#define NEXUSIO_MFTP_KTIME 4 /*get date time */
#define NEXUSIO_MFTP_KVTOT 5 /*voice to text */

/*moni sub_command*/
#define NEXUSIO_MFTP_MBURT 1
/*pack size*/
#define NEXUSIO_MFTP_HSIZE(B) (B >> 8)
#define NEXUSIO_MFTP_LSIZE(B) (B >> 0)
/*****************************************************************************/
typedef struct
{
	long count;

	char ident[32];

	union
	{
		struct sockaddr    saddr[32];
		struct sockaddr_in iaddr[32];
	}array;

}   nexusio_sock_addr_list;
/*****************************************************************************/
typedef struct  /*4252 bytes*/
{
    char file_name[ 256];
    char file_data[4096];
}nexusio_mftp_uftp_pack;
/*****************************************************************************/
typedef struct  /*16 bytes*/
{
	unsigned long srvr_saddr;
	unsigned long keep_timer;
	unsigned long clnt_count;
	unsigned long send_speed;
}nexusio_mftp_burt_info;
/*****************************************************************************/
typedef struct
{
	long count;
	char ident[32];
	nexusio_mftp_burt_info array[32];
}nexusio_mftp_burt_list;
/*****************************************************************************/
typedef struct  /*52 bytes*/
{
	char dev_name[32];
	char dev_enab;
	unsigned open_count;
	unsigned open_total;
	unsigned song_count;
	unsigned pick_count;
}nexusio_mftp_disc_info;
/*****************************************************************************/
typedef struct
{
	long count;
	char ident[32];
	nexusio_mftp_disc_info array[32];
}nexusio_mftp_disc_list;
/*****************************************************************************/
typedef struct
{
	char song_crid[20];
	char song_numb[8];
	long song_type;
	long play_time;
	long stop_time;
}nexusio_qtab_play_done;
/*****************************************************************************/
typedef struct
{
	long func_mask;
	long auth_date;
	long curr_date;
	long auth_client;
	long auth_number;
}nexusio_qtab_auth_info;
/*****************************************************************************/
struct nexusio_mftp_pack      /* 4 + 48k bytes */
{
	struct
	{
		unsigned char   xcmmd;
		unsigned char   xargv;

		unsigned char   Hsize;
		unsigned char   Lsize;

	}xhead;

	union
	{
		long ivar[12288];
		char svar[49152];

		nexusio_mftp_uftp_pack uftp;
		nexusio_sock_addr_list addr_list;
		nexusio_mftp_burt_list burt_list;
		nexusio_mftp_disc_list disc_list;
		nexusio_qtab_play_done qtab_done;
		nexusio_qtab_auth_info qtab_auth;
	}xdata;
};
/*****************************************************************************/
int nexusio_mftp_udp_ready(int *sock);
int nexusio_mftp_udp_clean(int *sock);

int nexusio_mftp_udp_find_peer(int  sock, SADDR *saddr, int *found, void *peer_hadd);

int nexusio_mftp_udp_find_srvr(int  sock, SADDR *addrs, int *count);
int nexusio_mftp_udp_find_song(int  sock, SADDR *addrs, int *count, char *song_code);

int nexusio_mftp_udp_read_etha(int  sock, void *haddr);
int nexusio_mftp_udp_load_file(int  sock, char *fname, void *fdata, int fsize);
int nexusio_mftp_udp_save_file(int  sock, char *fname, void *fdata, int fsize);

int nexusio_mftp_tcp_auto_cnnt(int *sock, SADDR *addrs, int  count, char  clnt_code);
int nexusio_mftp_tcp_shut_down(int *sock);

int nexusio_mftp_tcp_ctrl_test(int  sock);
int nexusio_mftp_tcp_ctrl_null(int  sock);

int nexusio_mftp_tcp_file_open(int sock, char *filename,int iLen, char open_mode);
int nexusio_mftp_tcp_file_read(int  sock, char  filenumb, char *pack_buff);
int nexusio_mftp_tcp_file_writ(int  sock, char  filenumb, char *pack_buff, int size);
int nexusio_mftp_tcp_file_seek(int  sock, char  filenumb, int   seek_post, int orig);
int nexusio_mftp_tcp_file_shut(int  sock, char  filenumb);
/*****************************************************************************/
int nexusio_mftp_tcp_file_prefetch_start(int sock, char filenumb);
int nexusio_mftp_tcp_file_prefetch_clean(int sock);
/*****************************************************************************/
int nexusio_mftp_sock_recv_dump(int sock, int size);
int nexusio_mftp_sock_recv_wait(int sock, int msec);
int nexusio_mftp_sock_send_wait(int sock, int msec);
/*****************************************************************************/
#endif/*MFTP_CS_H*/
