/*
* Copyright (c) 2012,福建海媚数码科技有限公司

* All rights reserved.
*
* 文件标识：
* 摘 要：	
*
* 当前版本：1.0
* 作 者：	wuhx
* 开始日期: 2012年11月16日
* 完成日期：
* 备注:
* 修改记录: 
* 
*/





#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif /* __cplusplus */
#endif /* __cplusplus */





#ifndef __HM_SECURITY_PUB___
#define __HM_SECURITY_PUB___



#define HM_ATSHA204_MAC_SIZE						6				/* mac地址长度 */
#define HM_ATSHA204_IPADDR_SIZE						15				/* ip地址长度 */



#ifndef NULL

#define NULL ((void*) 0)

#endif

#ifndef FALSE

#define FALSE ((int)0)

#endif

#ifndef TRUE

#define TRUE ((int)1)

#endif





/* 返回结果 */

typedef enum hm_security_result
{
	HM_SECURITY_SUCC = 0,							/* 成功 */
	HM_SECURITY_FAIL = 2000,						/* 失败 */
	HM_SECURITY_ERROR_PARAM,						/* 参数错误 */
	HM_SECURITY_ERROR_ATSHA204_TYPE,				/* ATSHA204类型错误 */
	HM_SECURITY_ERROR_ATSHA204_AUTH_MODE,			/* ATSHA204认证模式错误 */
	HM_SECURITY_ERROR_ATSHA204_NOTIFY,				/* ATSHA204,初始化时notify函数指针为空 */
	HM_SECURITY_ERROR_ATSHA204_MAGIC,				/* ATSHA204,digest magic error */
	HM_SECURITY_ERROR_ATSHA204_DIGEST_UNUSE,		/* ATSHA204,digest unuse */
	HM_SECURITY_ERROR_ATSHA204_DIGEST_CRC,			/* ATSHA204,digest crc error */
	HM_SECURITY_ERROR_ATSHA204_DIGEST_COMPARE,		/* ATSHA204,digest compare fail */
	HM_SECURITY_ERROR_ATSHA204_SN,					/* ATSHA204,SN error */
	HM_SECURITY_ERROR_ATSHA204_KEYID,				/* ATSHA204,key id error */
	HM_SECURITY_ERROR_ATSHA204_I2C_ENABLE,			/* ATSHA204,i2c enable error */											
										
	HM_SECURITY_ERROR_ATSHA204_LOCK_01,				/* 配置unlock	数据lock	处理: 出错处理, 提示芯片被lock,不能使用 */
	HM_SECURITY_ERROR_ATSHA204_LOCK_10,				/* 配置lock	数据unlock */
	HM_SECURITY_ERROR_ATSHA204_LOCK_11,				/* 配置lock	数据lock	处理: 出错处理, 提示芯片被lock,不能使用 */
	HM_SECURITY_ERROR_ATSHA204_SLOT_CONFIG,			/* slot config 配置错误 */
	HM_SECURITY_ERROR_ATSHA204_NO_CHIP,				/* 没有加密芯片 */
	HM_SECURITY_ERROR_ATSHA204_NO_DIGEST,			/* 没有摘要信息 */
	HM_SECURITY_ERROR_ATSHA204_LOCK_10_CONFIG,		/* 配置lock	数据unlock同时配置错误 */
	HM_SECURITY_ERROR_ATSHA204_LOCK_11_CONFIG,		/* 配置lock	数据lock同时配置错误 */
	HM_SECURITY_ERROR_MALLOC,						/* malloc失败 */
	HM_SECURITY_ERROR_FILE_FAIL,					/* 文件操作失败 */
	HM_SECURITY_ERROR_FILE_NULL,					/* 文件为空 */
	HM_SECURITY_ERROR_FILE_LOAD,					/* 文件加载失败 */
	HM_SECURITY_ERROR_FILE_SAVE,					/* 文件保存失败 */
	HM_SECURITY_ERROR_FILE_SIZE,					/* 文件大小错误 */
	HM_SECURITY_ERROR_CRC,							/* crc错误 */
	HM_SECURITY_ERROR_CMD,							/* 发行命令错误 */
	HM_SECURITY_ERROR_RELEASE,						/* 发行错误 */
	HM_SECURITY_ERROR_SOCKET_CONNECT,				/* socket connect fail */
	HM_SECURITY_ERROR_BUTT							/* 无效的结果值 */
	
}HM_SECURITY_RESULT_E;


/* 安全认证模式 */

typedef enum hm_atsha204_auth_mode
{
	HM_ATSHA204_AUTH_MODE_NULL		= 0,		/* 无效模式 */
	HM_ATSHA204_AUTH_MODE_NORMAL	= 1,		/* 正常安全模式, 只在初始化时进行安全认证一次 */
	HM_ATSHA204_AUTH_MODE_CYCLE		= 2,		/* 循环认证模式, 在一个线程中进行循环的安全认证 */
	HM_ATSHA204_AUTH_MODE_BUTT					/* 无效模式 */
	
}HM_ATSHA204_AUTH_MODE_E;






/* 安全认证通知事件, 告知应用安全认证的结果, 当应用判断到结果不成功时, 可以退出系统 */

typedef int (*PF_ATSHA204_NOTIFY)(int iAuthMode, int iResult);


/* atsha204加密芯片初始化结构体 */

typedef struct hm_atsha204_init
{
	int			iAuthMode;					/* 安全认证模式 见HM_ATSHA204_AUTH_MODE_E */
	PF_ATSHA204_NOTIFY		pfuncNotify;				/* 安全认证通知事件, 告知应用安全认证的结果,结果见HM_SECURITY_RESULT_E。 当应用判断到结果不成功时, 可以退出系统 */
	
}HM_ATSHA204_INIT_S;




/* 安全模块初始化参数结构体 */

typedef struct hm_security_init
{
	HM_ATSHA204_INIT_S stAtsha204Init;
	
	
}HM_SECURITY_INIT_S;



/* 设置重试次数， 用在当发行客户端 连接发行服务器失败时，重复尝试连接发行服务器的次数, 系统默认100次, 整个尝试连接时间周期为5分钟  */
int hm_security_retry_num_set(int iNum);

/* 设置加密芯片间隔多久认证一次， 时间单位 为秒， 系统默认30秒认证一次 */
int hm_security_mac_delay_time(int iDelayTime);



/* 显示安全模块的版本号 */

unsigned char *hm_security_version(void);

/* 设置调试信息开关 */
void hm_security_debug_set(int value);

/* 设置错误信息开关 */
void hm_security_error_set(int value);


/* 配置发行服务器的ip地址和端口， 端口一般都设置为30000 */
int hm_security_server_addr_cfg(unsigned char *pucIpAddr, unsigned int uiPort);


/* 安全模块初始化 */
int hm_security_init(HM_SECURITY_INIT_S *pstInit);





#endif /* __HM_SECURITY_PUB___ */



#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

