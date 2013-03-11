#ifndef NEXUS_LINK_C
#define NEXUS_LINK_C
/***********************************************************************************************/
#include <stdio.h>   
#include <stdlib.h>   
#include <string.h>
#include "nexus_link.h"
/***********************************************************************************************/
/*  
 * 	构建头结点不存储数据的空表
 */  
void NEXUSAPP_LINK_Init_myLinkList(nexusLinkList *Head_pointer)    
{   
	*Head_pointer = NULL;   
}  
/***********************************************************************************************/
/*  插入一个元素（头插）  */  
int NEXUSAPP_LINK_Insert_First(nexusLinkList *Head_pointer, data_t *x)   
{   
	nexusNode *p; 
	
	p = (nexusNode *) malloc(sizeof(nexusNode));   
	if (p == NULL)   
		goto error_out;
	
	memcpy(&p->data,x,sizeof(data_t));
	   
	p->next = *Head_pointer;   
	*Head_pointer = p;   
	   
	return OK; 

error_out:
	 return OverFlow;
	
}  
/***********************************************************************************************/
nexusLinkList NEXUSAPP_LINK_Location_FindName(nexusLinkList Head,char *name)   
{   
	nexusLinkList p;   

	p = Head;   
	
	while(p != NULL)   
	{   		
		if(!strcmp(p->data.name,name))
		{
			break;
		}
		p = p->next;   
	}   
	
	return p;   
} 
/***********************************************************************************************/
/*  
 * 	删除指定的元素  
*/  
int NEXUSAPP_LINK_Delete_myLinkList(nexusLinkList *Head_pointer, char *name)   
{   
	nexusNode *p, *q =NULL;   

	p = *Head_pointer;   
	if (!strcmp(p->data.name,name))
	{   
		*Head_pointer = (*Head_pointer)->next;   
		free(p);   
		goto succ_out;
	}   
	else  
	{   
		q = p; p = p->next;  
		while(p != NULL)   
		{   
			if (!strcmp(p->data.name,name) ) 
			{   
				q->next = p->next;   
				free(p);   
				goto succ_out;   
			}   
			q = p; p = p->next;   
		}   
	}   

	return Error; 

succ_out:
	return OK;  
  
}  
/***********************************************************************************************/
/*  
 * 	清空链表  
 */  
void NEXUSAPP_LINK_SetNull_myLinkList(nexusLinkList *Head_pointer)   
{   
	nexusLinkList p, q;   

	p = *Head_pointer;   

	while (p != NULL)   
	{   
		q = p;   
		p = p->next;   
		free(q);
	}

}  
/***********************************************************************************************/
/* 
* 	计算链表的长度 
*/ 
int NEXUSAPP_LINK_Length_myLinkList(nexusLinkList Head) 
{ 
	nexusLinkList p = Head; 
	int sum = 0; 

	
	while(p != NULL) 
	{ 
		sum++; 
		p = p->next;
		
	} 

	return sum; 
} 
/***********************************************************************************************/
/*
*	定位到链表元素
*/
nexusLinkList NEXUSAPP_LINK_SetSeek_myLinkList(nexusLinkList Head , int no) 
{ 
	nexusLinkList p = Head; 
	int sum = 0;

	while(p != NULL) 
	{ 
		
		if(sum >= no)
			return p;

		sum++;
		p = p->next; 
	}

	return p; 
} 
/***********************************************************************************************/
#endif
