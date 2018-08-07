﻿#ifndef _MCUUART_H#define _MCUUART_H#include <stdio.h>#include <sys/types.h>#include <sys/stat.h>#include <stdlib.h>#include <string.h>#include <unistd.h>#include <fcntl.h>#include <termios.h>#include <errno.h>#include <pthread.h>#include <stdint.h>#include <time.h>#include "common.h"#include "TBoxDataPool.h"#include "LTEModuleAtCtrl.h"#include "GBT32960.h"#include "FAWACP.h"#include "VoiceCall.h"#include "Message.h"#include "DataCall.h"#include "WDSControl.h"#include "NASControl.h"#include "dsi_netctrl.h"#ifndef MCU_DEBUG	#define MCU_DEBUG 1#endif#if MCU_DEBUG 		#define MCULOG(format,...) printf("== MCU == FILE: %s, FUN: %s, LINE: %d "format"\n",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)	#define MCU_NO(format,...) printf(format,##__VA_ARGS__)#else		#define MCULOG(format,...)	#define MCU_NO(format,...)#endif#define MCU_UART_DEVICE	        "/dev/ttyHS1"    /* for mcu uart *///#define BLUETOOTH_UART_DEVICE   "/dev/ttyHS0"     /* for bluetooth */#define PLC_UPGRADE_FILE        "/data/plc.bin"#define MCU_UPGRADE_FILE        "/data/MCU.bin"#define RM_MCU_FILE             "rm -rf /data/MCU.bin"#define LTE_FILE_NAME           "/data/LteUpgrade.bin"#define RM_LTE_FILE             "rm -rf /data/LteUpgrade.bin"#define MCU_UART_SPEED	        460800#define MCU_UART_STOP_BITS      1#define MCU_UART_DATA_BITS      8#define MCU_UART_CHECK_BIT      'n'#define MCU_UART_TIMEOUT_MSECONDS  50#define BUFF_LEN                1024#define CONTENT_MAX_LEN         200#define START_SYNC_CMD          0x00			//从机同步参数指令#define MCU_GENERAL_RESP        0x01		 //从机通用应答#define TBOX_GENERAL_RESP       0x81		 //主机通用应答#define HEART_BEAT_CMD          0x10		   //从机车况信息上报#define TBOX_REPORT_4G_STATE    0x85	  //主机状态汇报#define TBOX_REPLY_MESSAGES_ID  0x8B	//主机应答短信发送消息#define TBOX_REMOTECTRL_CMD		0x04	//从机控制应答#define MCU_SND_MESSAGES_ID     0x0B	//从机发送短信消息#define TEXT_TO_SPEECH_ID       0x0C		   //从机发送语音消息#define TBOX_SEND_UPGRADE_CMD   0x8a#define TBOX_RECV_MCU_APPLY_FOR 0x86#define MCU_SEND_COMPLETE       0x0A/* mcu upgrade tbox*/#define MCU_SND_UPGRADE_INFO    0x0D#define TBOX_REPLY_ID           0x8E#define MCU_SND_UPGRADE_DATA    0x0E#define MCU_SND_UPGRADE_CMPL    0x8D     //COMPLETE/**************************************************************************** Function Name: callBack_reportDate* Function Introduction:*               callback function, report the data which need by client *               per second. * Parameter description:*	            pData  : data buffer.*               length : data length* Function return value: *                        void* Data : 2017.10.10****************************************************************************/typedef void (*callBack_reportDate)(uint8_t *pData, uint16_t length);typedef void (* callback_ReplayRemoteCtrl)(void);typedef uint16_t (* callback_EventCmd)(uint8_t, AcpAppID_E);//TBOX事件触发信号结构typedef struct{	int s_TimeOutFileAlarm; //定时熄火信号	//车辆告警及提醒	int s_DoorIntrusAlarm;	//车门入侵告警	int s_VehMoveAlarm;		//车辆异动提醒	int s_VehHealthRemAlarm;//剩余保养里程	int s_LowOilAlarm;		//低油量报警	int s_VehOutFileAlarm;	//紧急熄火	//紧急救援数据上报	int s_VehCollideAlarm;	//车辆碰撞	int s_LoadRescueAlarm;	//道路救援上报	//车况上报事件触发信号	int s_VehAwakeEvent;	 //车辆唤醒事件上报	int s_VehPowerEvent;	 //车辆上电事件上报	int s_VehStartEvent;		//车辆启动事件上报	int s_TravelStartEvent;	  	//行程开始事件上报	int s_RapidDeceStartEvent;	//急减速开始事件上报	int s_RapidDeceEndEvent;	//急减速结束事件上报	int s_RapidAcceStartEvent;	//急加速开始事件上报	int s_RapidAcceEndEvent;	//急加速结束事件上报	int s_RapidTurnStartEvent;	//急转弯开始事件上报	int s_RapidTurnEndEvent;	//急转弯结束事件上报	int s_OverSpeedStartEvent;  //超速开始事件上报	int s_OverSpeedEndEvent;//超速结束事件上报	int s_VehOutFireEvent;	//车辆熄火事件上报	int s_TravelEndEvent;	//行程结束事件上报	int s_VehDownPowerEvent;//车辆下电事件上报	int s_FailEvent;	  	//故障事件上报	int s_VehsleepEvent;	  //车辆睡眠事件上报}SIG_Event_t;class mcuUart{public:    mcuUart();	static mcuUart * m_mcuUart;	int mcuUartInit();	int setUartSpeed(int fd, int speed);	int setUartParity(int fd, int databits, int stopbits, int parity);	int mcuUartReceiveData();	int registerCallback_reportDate(callBack_reportDate func);	uint32_t checkMcuUartData(unsigned char *pData, unsigned int size);	int unpackMcuUartData(uint8_t *pData, unsigned int datalen);	unsigned int Crc16Check(unsigned char* pData, uint32_t len);	int unpack_syncParameter(unsigned char *pData, unsigned int len);	int unpack_updatePositionInfo(unsigned char *pData, unsigned int len);	int setSystemTime(struct tm *pTm);	int unpack_updateGBT32960PositionInfo(unsigned char *pData, unsigned int len);	int unpack_updateTimingInfo(unsigned char *pData, unsigned int len);	int unpack_updateFAWACPPositionInfo(unsigned char *pData, unsigned int len);	int unpack_updateFAWACPVehCondInfo(unsigned char* pData, unsigned int len);	int unpack_text_messages(unsigned char* pData, unsigned int len); 	int unpack_tts_voice(unsigned char* pData, unsigned int len);	int packDataWithRespone(uint8_t responeCmd, uint8_t subCmd, uint8_t *needToPackData, uint16_t len, uint16_t serialNumber);	int packProtocolData(uint8_t responeCmd, uint8_t subCmd, uint8_t *needToPackData, uint16_t len, uint16_t serialNum);	int escape_mcuUart_data(unsigned char *pData, int len);	uint16_t pack_report4GState(unsigned char *pBuff, int length);	uint16_t pack_successMark(unsigned char *pBuff, uint8_t *pData, uint16_t len);	void reportEventCmd(callback_EventCmd cb_EventCmd, uint8_t MsgType, AcpAppID_E AppID);	static void *CheckEventThread(void *arg);	void ReplayRemoteCtrl(callback_ReplayRemoteCtrl cb_TspRemoteCtrl);	static int cb_RemoteConfigCmd(uint8_t SubitemCode, uint16_t SubitemVal);	static int cb_RemoteCtrlCmd(uint8_t SubitemCode, uint8_t SubitemCodeParam, uint8_t SubitemTotal);	int unpack_RemoteCtrl(unsigned char *pData, uint16_t datalen);	void close_uart();	/*检测PWM波形*/	static void *CheckPWMThread(void *arg);	/* For tbox upgrade MCU */	/*****************************************************************************	* Function Name : pack_mcuUart_upgrade_data	* Description	: pack upgrade data. 	* Input 		: unsigned char cmd:升级指令	                  bool isReceivedDataCorrect:是否收到了数据	                  int mcuOrPlcFlag:升级mcu或者plc	                                   0: MCU	                                   1: PLC 	* Output		: None	* Return		: 0:success	* Auther		: ygg	* Date			: 2018.04.10	*****************************************************************************/	int pack_mcuUart_upgrade_data(unsigned char cmd, bool isReceivedDataCorrect, int mcuOrPlcFlag);	unsigned short int pack_upgrade_cmd(unsigned char *pData, int len, int mcuOrPlcFlag);	unsigned short int pack_upgrade_data(unsigned char *pData, int len, bool isReceivedDataCorrect, int mcuOrPlcFlag);	void mcu_apply_for_data(unsigned char* pData, uint32_t len);	unsigned char dataPacketOffsetAddr[4];	unsigned char dataPacketLen[2];	unsigned char fileName[64];	/* For MCU upgrade tbox */	int unpack_MCU_SND_Upgrade_Info(unsigned char* pData, unsigned int len);	int unpack_MCU_SND_Upgrade_Data(unsigned char* pData, unsigned int len);	int unpack_MCU_SND_Upgrade_CMPL();	int calculate_files_CRC(char *fileName, uint32_t *crc);	//unsigned int crc32Check(unsigned char *buff, unsigned int size);	unsigned int crc32Check(unsigned int crc, unsigned char *buff, unsigned int size);	int storageFile(uint8_t *fileName, uint8_t *pData, uint32_t len);	uint32_t dataSize;	uint32_t crc32;	char tboxVersion[30];    private:	int fd;	SIG_Event_t SigEvent;	pthread_t CheckEventThreadId;	pthread_t CheckPWMThreadId;	uint8_t Subitemnumber;	callBack_reportDate reportDataFunc;	};extern TBoxDataPool *dataPool;extern LTEModuleAtCtrl *LteAtCtrl;#endif