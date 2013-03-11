#ifndef NEXUS_RTC_C
#define NEXUS_RTC_C

/************************************************************************************/
#include "nexus_player.h"
/************************************************************************************/
char g_current_date[10];
/************************************************************************************/
/* tInputGpioPinNum:		46---73(89)
tInputGpioMode:			0-- input,
						1-- push pull output,						
						2-- open drain output.	*/
unsigned int NEXUSAPP_GpioCreate(int tInputGpioPinNum,unsigned char tInputGpioMode)
{
	NEXUS_GpioHandle tTempGpio;
	NEXUS_GpioSettings tTempGpioSettings;

	NEXUS_Gpio_GetDefaultSettings(NEXUS_GpioType_eStandard, &tTempGpioSettings);
	switch( tInputGpioMode )
	{
	case 0:
		tTempGpioSettings.mode = NEXUS_GpioMode_eInput;
		break;
	case 1:
		tTempGpioSettings.mode = NEXUS_GpioMode_eOutputPushPull;
		break;
	case 2:
		tTempGpioSettings.mode = NEXUS_GpioMode_eOutputOpenDrain;
		break;
	}
	tTempGpio = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, tInputGpioPinNum, &tTempGpioSettings);
	return (unsigned int)tTempGpio;
}


void NEXUSAPP_GpioClose(unsigned int tInputGpioHandle)
{
	if( tInputGpioHandle==0 )
		return;
	NEXUS_Gpio_Close( (NEXUS_GpioHandle)tInputGpioHandle );
}

/*tInputGpioMode:			0-- input,
						1-- push pull output,						
						2-- open drain output.	*/
void NEXUSAPP_GpioSetPinMode(unsigned int tInputGpioHandle,int tInputGpioMode)
{
	NEXUS_GpioSettings tTempGpioSettings;

	if( tInputGpioHandle==0 )
		return;
	NEXUS_Gpio_GetSettings((NEXUS_GpioHandle)tInputGpioHandle,&tTempGpioSettings);
	switch( tInputGpioMode )
	{
	case 0:
		tTempGpioSettings.mode = NEXUS_GpioMode_eInput;
		break;
	case 1:
		tTempGpioSettings.mode = NEXUS_GpioMode_eOutputPushPull;
		break;
	case 2:
		tTempGpioSettings.mode = NEXUS_GpioMode_eOutputOpenDrain;
		break;
	}
	NEXUS_Gpio_SetSettings((NEXUS_GpioHandle)tInputGpioHandle,&tTempGpioSettings);
}


void NEXUSAPP_GpioSet(unsigned int tInputGpioHandle,int tInputHighOrNot)
{
	NEXUS_GpioSettings tTempGpioSettings;

	if( tInputGpioHandle==0 )
		return;
	NEXUS_Gpio_GetSettings((NEXUS_GpioHandle)tInputGpioHandle,&tTempGpioSettings);
	if( tInputHighOrNot )
		tTempGpioSettings.value=NEXUS_GpioValue_eHigh;
	else
		tTempGpioSettings.value=NEXUS_GpioValue_eLow;
	NEXUS_Gpio_SetSettings((NEXUS_GpioHandle)tInputGpioHandle,&tTempGpioSettings);
}

int NEXUSAPP_GpioGet(unsigned int tInputGpioHandle)
{
       NEXUS_GpioStatus tTempGpioStatus;

	if( tInputGpioHandle==0 )
		return -1;
	NEXUS_Gpio_GetStatus((NEXUS_GpioHandle)tInputGpioHandle,&tTempGpioStatus);
	if( tTempGpioStatus.value )
		return 1;
	return 0;
}

/******************Setup Timer****************************/
unsigned int tTimerIo_Reset,tTimerIo_Clk,tTimerIo_Dat;

void NEXUSAPP_Timer_IoDelay(int tInputCnt)
{
	int i;
	for(i=0;i<tInputCnt;i++)
		usleep(10);
}

void NEXUSAPP_TimerInit(void)
{
	tTimerIo_Reset=NEXUSAPP_GpioCreate(82,1);
	tTimerIo_Clk=NEXUSAPP_GpioCreate(83,1);
	tTimerIo_Dat=NEXUSAPP_GpioCreate(81,1);

	NEXUSAPP_GpioSet(tTimerIo_Reset,1);
	NEXUSAPP_GpioSet(tTimerIo_Clk,1);
	NEXUSAPP_GpioSet(tTimerIo_Dat,1);
}

void NEXUSAPP_TimerFree(void)
{
	NEXUSAPP_GpioClose(tTimerIo_Reset);
	NEXUSAPP_GpioClose(tTimerIo_Clk);
	NEXUSAPP_GpioClose(tTimerIo_Dat);
}

void NEXUSAPP_Timer_WriteByte(unsigned char tInputByte)
{
	unsigned char i;
	
	NEXUSAPP_GpioSetPinMode(tTimerIo_Dat,1);
	NEXUSAPP_Timer_IoDelay(1);
	for(i=0;i<8;i++)
	{
		NEXUSAPP_GpioSet(tTimerIo_Clk,0);
		NEXUSAPP_Timer_IoDelay(1);
		if( tInputByte&0x01 )
			NEXUSAPP_GpioSet(tTimerIo_Dat,1);
		else
			NEXUSAPP_GpioSet(tTimerIo_Dat,0);
		NEXUSAPP_Timer_IoDelay(1);
		NEXUSAPP_GpioSet(tTimerIo_Clk,1);
		NEXUSAPP_Timer_IoDelay(1);
		tInputByte>>=1;
	}
}

unsigned char NEXUSAPP_Timer_ReadByte(void)
{
	unsigned char i;
	unsigned char tTempRet=0;
	
	NEXUSAPP_GpioSetPinMode(tTimerIo_Dat,0);
	NEXUSAPP_Timer_IoDelay(1);
	for(i=0;i<8;i++)
	{
		NEXUSAPP_GpioSet(tTimerIo_Clk,0);
		NEXUSAPP_Timer_IoDelay(1);
		tTempRet>>=1;
		if( NEXUSAPP_GpioGet(tTimerIo_Dat) )
			tTempRet|=0x80;
		NEXUSAPP_Timer_IoDelay(1);
		NEXUSAPP_GpioSet(tTimerIo_Clk,1);
		NEXUSAPP_Timer_IoDelay(1);
	}
	return tTempRet;
}

unsigned char NEXUSAPP_Timer_ReadReg(unsigned char tInputAddr)
{
	unsigned char tTempRet;

	NEXUSAPP_GpioSet(tTimerIo_Reset,0);
	NEXUSAPP_Timer_IoDelay(100);
	NEXUSAPP_GpioSet(tTimerIo_Clk,0);
	NEXUSAPP_Timer_IoDelay(100);
	NEXUSAPP_GpioSet(tTimerIo_Reset,1);
	NEXUSAPP_Timer_IoDelay(1);
	NEXUSAPP_Timer_WriteByte(tInputAddr);
	tTempRet=NEXUSAPP_Timer_ReadByte();
	NEXUSAPP_GpioSet(tTimerIo_Reset,0);
	NEXUSAPP_Timer_IoDelay(1);
	NEXUSAPP_GpioSet(tTimerIo_Reset,0);
	NEXUSAPP_Timer_IoDelay(1);
	return tTempRet;
}

void NEXUSAPP_Timer_WriteReg(unsigned char tInputAddr,unsigned char tInputData)
{
	NEXUSAPP_GpioSet(tTimerIo_Reset,0);
	NEXUSAPP_Timer_IoDelay(100);
	NEXUSAPP_GpioSet(tTimerIo_Clk,0);
	NEXUSAPP_Timer_IoDelay(100);
	NEXUSAPP_GpioSet(tTimerIo_Reset,1);
	NEXUSAPP_Timer_IoDelay(1);
	NEXUSAPP_Timer_WriteByte(tInputAddr);
	NEXUSAPP_Timer_WriteByte(tInputData);
	NEXUSAPP_GpioSet(tTimerIo_Reset,0);
	NEXUSAPP_Timer_IoDelay(1);
	NEXUSAPP_GpioSet(tTimerIo_Clk,0);
	NEXUSAPP_Timer_IoDelay(1);
}

unsigned char bcd2byte(unsigned char bcd)
{
	unsigned char r = 0;
	r = (bcd>>4)&0x0f;
	r *= 10;
	r += bcd&0x0f;
	return r;
}

unsigned char byte2bcd(unsigned char b)
{
	unsigned char r = 0;
	r = b/10;
	r <<= 4;
	r += b%10;
	return r;
}

/*
pInputArray: [0], Year - 2000, 2 digits
		     [1], Month
		     [2], Day
		     [3], Hour
		     [4], Minute
		     [5], Second

*/
void NEXUSAPP_TimerSet(unsigned char *pInputArray)
{
	int i;
	unsigned char pTempBuffer[7];

	NEXUSAPP_Timer_WriteReg(0x8e,0x00);	// wr enable 
	NEXUSAPP_Timer_WriteReg(0x90,0xa0);	// charge 
	NEXUSAPP_Timer_WriteReg(0x8e,0x80);

	pTempBuffer[0] = byte2bcd(pInputArray[5]);	
	pTempBuffer[1] = byte2bcd(pInputArray[4]);
	pTempBuffer[2] = byte2bcd(pInputArray[3]);
	pTempBuffer[3] = byte2bcd(pInputArray[2]);
	pTempBuffer[4] = byte2bcd(pInputArray[1]);
	pTempBuffer[6] = byte2bcd(pInputArray[0]);

	NEXUSAPP_Timer_WriteReg(0x8e,0x00);
	for(i=0;i<7;i++)
		NEXUSAPP_Timer_WriteReg(0x80+i*2,pTempBuffer[i]);
	NEXUSAPP_Timer_WriteReg(0x8e,0x80);

}

void NEXUSAPP_TimerGet(unsigned char *pOutputArray)
{
	unsigned char pTempBuffer[7];
	int i;

	memset(pTempBuffer,0,7);
	for(i=0;i<7;i++)
	{
		pTempBuffer[i]=NEXUSAPP_Timer_ReadReg(0x80+i*2+1);
	}
	
	pOutputArray[0]=bcd2byte(pTempBuffer[6]);
	pOutputArray[1]=bcd2byte(pTempBuffer[4]);
	pOutputArray[2]=bcd2byte(pTempBuffer[3]);
	pOutputArray[3]=bcd2byte(pTempBuffer[2]);
	pOutputArray[4]=bcd2byte(pTempBuffer[1]);
	pOutputArray[5]=bcd2byte(pTempBuffer[0]);	
}

/*
NEXUSAPP_SysTime_Setup: Set System Time
	
*/
void NEXUSAPP_SysTime_Setup(void)
{
	unsigned char pTempBuffer[7];
	char buf[128];

	memset(pTempBuffer, 0, sizeof(pTempBuffer));	

	NEXUSAPP_TimerInit();
	NEXUSAPP_TimerGet(pTempBuffer);
	
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "date -s \"%d-%d-%d %d:%d:%d\"", pTempBuffer[0]+2000, pTempBuffer[1],pTempBuffer[2],pTempBuffer[3],pTempBuffer[4],pTempBuffer[5]);
	system(buf);	

	sprintf(g_current_date, "%d-%02d-%02d", pTempBuffer[0]+2000, pTempBuffer[1], pTempBuffer[2]);

	NEXUSAPP_TimerFree();
}

/*
pInputArray: [0], Year - 2000, 2 digits
		     [1], Month
		     [2], Day
		     [3], Hour
		     [4], Minute
		     [5], Second

*/
int NEXUSAPP_SetTime(unsigned char *pInputArray)
{
	int tYear, tMonth, tDay;
	char buf[128];

	if (pInputArray[0] > 99)
	{
		DBG_APP(("Input YEAR not valid, should be [0-99]\n"));
		return -1;
	}

	if (pInputArray[1] < 1 || pInputArray[1] > 12)
	{
		DBG_APP(("Input MONTH not valid, should be [1-12]\n"));
		return -1;
	}

	if (pInputArray[2] < 1 || pInputArray[2] > 31)
	{
		DBG_APP(("Input DAY not valid, should be [1-31]\n"));
			
		return -1;
	}
	else
	{
		tMonth = pInputArray[1];
		tDay = pInputArray[2];
		tYear=pInputArray[0] + 2000;

		if (tMonth == 2)
		{
			if ((tYear%4) == 0)
			{
				if (tDay > 29)
				{
					DBG_APP(("Input DAY not valid, Year:%d,Feb should not excess 29\n", tYear));
					return -1;
				}
			}
			else
			{
				if (tDay > 28)
				{
					DBG_APP(("Input DAY not valid, Year:%d,Feb should not excess 28\n", tYear));
					return -1;
				}
			}
		}		
	}

	if (pInputArray[3] > 23)
	{
		DBG_APP(("Input HOUR not valid, should be [0-23]\n"));
		return -1;
	}

	if (pInputArray[4] > 59)
	{
		DBG_APP(("Input MINUTE not valid, should be [0-59]\n"));
		return -1;
	}

	if (pInputArray[5] > 59)
	{
		DBG_APP(("Input SECOND not valid, should be [0-59]\n"));
		return -1;
	}

	NEXUSAPP_TimerInit();

	NEXUSAPP_TimerSet(pInputArray);
	
	NEXUSAPP_TimerFree();

	memset(buf, 0, sizeof(buf));
	sprintf(buf, "date -s \"%d-%d-%d %d:%d:%d\"", pInputArray[0]+2000, pInputArray[1],pInputArray[2],pInputArray[3],pInputArray[4],pInputArray[5]);
	system(buf);
	
	return 0;
}

/*
pOutputArray: [0], Year - 2000, 2 digits
		     [1], Month
		     [2], Day
		     [3], Hour
		     [4], Minute
		     [5], Second

*/
int NEXUSAPP_GetTime(unsigned char *pOutputArray)
{
	NEXUSAPP_TimerInit();

	NEXUSAPP_TimerGet(pOutputArray);
	
	NEXUSAPP_TimerFree();

	return 0;
}


#endif

