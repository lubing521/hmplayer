/*
* Copyright (c) 2012,������������Ƽ����޹�˾

* All rights reserved.
*
* �ļ���ʶ��
* ժ Ҫ��	
*
* ��ǰ�汾��1.0
* �� �ߣ�	wuhx
* ��ʼ����: 2012��11��16��
* ������ڣ�
* ��ע:
* �޸ļ�¼: 
* 
*/





#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif /* __cplusplus */
#endif /* __cplusplus */





#ifndef __HM_SECURITY_PUB___
#define __HM_SECURITY_PUB___



#define HM_ATSHA204_MAC_SIZE						6				/* mac��ַ���� */
#define HM_ATSHA204_IPADDR_SIZE						15				/* ip��ַ���� */



#ifndef NULL

#define NULL ((void*) 0)

#endif

#ifndef FALSE

#define FALSE ((int)0)

#endif

#ifndef TRUE

#define TRUE ((int)1)

#endif





/* ���ؽ�� */

typedef enum hm_security_result
{
	HM_SECURITY_SUCC = 0,							/* �ɹ� */
	HM_SECURITY_FAIL = 2000,						/* ʧ�� */
	HM_SECURITY_ERROR_PARAM,						/* �������� */
	HM_SECURITY_ERROR_ATSHA204_TYPE,				/* ATSHA204���ʹ��� */
	HM_SECURITY_ERROR_ATSHA204_AUTH_MODE,			/* ATSHA204��֤ģʽ���� */
	HM_SECURITY_ERROR_ATSHA204_NOTIFY,				/* ATSHA204,��ʼ��ʱnotify����ָ��Ϊ�� */
	HM_SECURITY_ERROR_ATSHA204_MAGIC,				/* ATSHA204,digest magic error */
	HM_SECURITY_ERROR_ATSHA204_DIGEST_UNUSE,		/* ATSHA204,digest unuse */
	HM_SECURITY_ERROR_ATSHA204_DIGEST_CRC,			/* ATSHA204,digest crc error */
	HM_SECURITY_ERROR_ATSHA204_DIGEST_COMPARE,		/* ATSHA204,digest compare fail */
	HM_SECURITY_ERROR_ATSHA204_SN,					/* ATSHA204,SN error */
	HM_SECURITY_ERROR_ATSHA204_KEYID,				/* ATSHA204,key id error */
	HM_SECURITY_ERROR_ATSHA204_I2C_ENABLE,			/* ATSHA204,i2c enable error */											
										
	HM_SECURITY_ERROR_ATSHA204_LOCK_01,				/* ����unlock	����lock	����: ������, ��ʾоƬ��lock,����ʹ�� */
	HM_SECURITY_ERROR_ATSHA204_LOCK_10,				/* ����lock	����unlock */
	HM_SECURITY_ERROR_ATSHA204_LOCK_11,				/* ����lock	����lock	����: ������, ��ʾоƬ��lock,����ʹ�� */
	HM_SECURITY_ERROR_ATSHA204_SLOT_CONFIG,			/* slot config ���ô��� */
	HM_SECURITY_ERROR_ATSHA204_NO_CHIP,				/* û�м���оƬ */
	HM_SECURITY_ERROR_ATSHA204_NO_DIGEST,			/* û��ժҪ��Ϣ */
	HM_SECURITY_ERROR_ATSHA204_LOCK_10_CONFIG,		/* ����lock	����unlockͬʱ���ô��� */
	HM_SECURITY_ERROR_ATSHA204_LOCK_11_CONFIG,		/* ����lock	����lockͬʱ���ô��� */
	HM_SECURITY_ERROR_MALLOC,						/* mallocʧ�� */
	HM_SECURITY_ERROR_FILE_FAIL,					/* �ļ�����ʧ�� */
	HM_SECURITY_ERROR_FILE_NULL,					/* �ļ�Ϊ�� */
	HM_SECURITY_ERROR_FILE_LOAD,					/* �ļ�����ʧ�� */
	HM_SECURITY_ERROR_FILE_SAVE,					/* �ļ�����ʧ�� */
	HM_SECURITY_ERROR_FILE_SIZE,					/* �ļ���С���� */
	HM_SECURITY_ERROR_CRC,							/* crc���� */
	HM_SECURITY_ERROR_CMD,							/* ����������� */
	HM_SECURITY_ERROR_RELEASE,						/* ���д��� */
	HM_SECURITY_ERROR_SOCKET_CONNECT,				/* socket connect fail */
	HM_SECURITY_ERROR_BUTT							/* ��Ч�Ľ��ֵ */
	
}HM_SECURITY_RESULT_E;


/* ��ȫ��֤ģʽ */

typedef enum hm_atsha204_auth_mode
{
	HM_ATSHA204_AUTH_MODE_NULL		= 0,		/* ��Чģʽ */
	HM_ATSHA204_AUTH_MODE_NORMAL	= 1,		/* ������ȫģʽ, ֻ�ڳ�ʼ��ʱ���а�ȫ��֤һ�� */
	HM_ATSHA204_AUTH_MODE_CYCLE		= 2,		/* ѭ����֤ģʽ, ��һ���߳��н���ѭ���İ�ȫ��֤ */
	HM_ATSHA204_AUTH_MODE_BUTT					/* ��Чģʽ */
	
}HM_ATSHA204_AUTH_MODE_E;






/* ��ȫ��֤֪ͨ�¼�, ��֪Ӧ�ð�ȫ��֤�Ľ��, ��Ӧ���жϵ�������ɹ�ʱ, �����˳�ϵͳ */

typedef int (*PF_ATSHA204_NOTIFY)(int iAuthMode, int iResult);


/* atsha204����оƬ��ʼ���ṹ�� */

typedef struct hm_atsha204_init
{
	int			iAuthMode;					/* ��ȫ��֤ģʽ ��HM_ATSHA204_AUTH_MODE_E */
	PF_ATSHA204_NOTIFY		pfuncNotify;				/* ��ȫ��֤֪ͨ�¼�, ��֪Ӧ�ð�ȫ��֤�Ľ��,�����HM_SECURITY_RESULT_E�� ��Ӧ���жϵ�������ɹ�ʱ, �����˳�ϵͳ */
	
}HM_ATSHA204_INIT_S;




/* ��ȫģ���ʼ�������ṹ�� */

typedef struct hm_security_init
{
	HM_ATSHA204_INIT_S stAtsha204Init;
	
	
}HM_SECURITY_INIT_S;



/* �������Դ����� ���ڵ����пͻ��� ���ӷ��з�����ʧ��ʱ���ظ��������ӷ��з������Ĵ���, ϵͳĬ��100��, ������������ʱ������Ϊ5����  */
int hm_security_retry_num_set(int iNum);

/* ���ü���оƬ��������֤һ�Σ� ʱ�䵥λ Ϊ�룬 ϵͳĬ��30����֤һ�� */
int hm_security_mac_delay_time(int iDelayTime);



/* ��ʾ��ȫģ��İ汾�� */

unsigned char *hm_security_version(void);

/* ���õ�����Ϣ���� */
void hm_security_debug_set(int value);

/* ���ô�����Ϣ���� */
void hm_security_error_set(int value);


/* ���÷��з�������ip��ַ�Ͷ˿ڣ� �˿�һ�㶼����Ϊ30000 */
int hm_security_server_addr_cfg(unsigned char *pucIpAddr, unsigned int uiPort);


/* ��ȫģ���ʼ�� */
int hm_security_init(HM_SECURITY_INIT_S *pstInit);





#endif /* __HM_SECURITY_PUB___ */



#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

