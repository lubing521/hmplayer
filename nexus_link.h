#ifndef NEXUS_LINK_H
#define NEXUS_LINK_H
/***********************************************************************************************/
#include "nexus_player.h"
/***********************************************************************************************/
#define OverFlow 	-1  	/*定义OverFlow表示内存溢出*/   
#define OK        	0  		/*定义OK表示成功 */  
#define Error    	-2  	/*定义操作失败的返回值*/   
/***********************************************************************************************/
#define DATA_FLAG_MAX 256
/***********************************************************************************************/
typedef struct {
	fbuff	fb;
	unsigned short index;
	unsigned short iCount;
	unsigned short pyindex;
	unsigned short wx;
	unsigned short hy;
	unsigned char	flag[DATA_FLAG_MAX];
	char	name[40];
	NEXUS_Rect dest_rect;
	bool	start;
	int		loop;
	int		nowloop;
}data_t;
/***********************************************************************************************/
typedef struct myNode_t
{
	data_t data;   
	struct myNode_t *next;
} nexusNode, *nexusLinkList;
/***********************************************************************************************/
void 		NEXUSAPP_LINK_Init_myLinkList(nexusLinkList *Head_pointer);
void 		NEXUSAPP_LINK_SetNull_myLinkList(nexusLinkList *Head_pointer); 
int 		NEXUSAPP_LINK_Insert_First(nexusLinkList *Head_pointer, data_t *x);  
int 		NEXUSAPP_LINK_Delete_myLinkList(nexusLinkList *Head_pointer, char *name) ;
int 		NEXUSAPP_LINK_Length_myLinkList(nexusLinkList Head);
nexusLinkList 	NEXUSAPP_LINK_Location_FindName(nexusLinkList Head,char *name);
nexusLinkList 	NEXUSAPP_LINK_SetSeek_myLinkList(nexusLinkList Head , int no);
/***********************************************************************************************/
#endif
