#ifndef _IVI_COMMUNICATION_H_
#define _IVI_COMMUNICATION_H_
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <time.h>
#include "TBoxDataPool.h"
#include "VoiceCall.h"
#include "simcom_common.h"
#include "WiFiControl.h"
#include "dsi_netctrl.h"
#include "LTEModuleAtCtrl.h"
#include <sys/syslog.h>
#include <sys/prctl.h>



#define IVI_DEBUG_EN  0

#if IVI_DEBUG_EN
	#define IVI_LOG(format,...) printf("### IVI ### %s, %s[%d] "format"\n",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)
	#define IVI_ERROR(format,...) fprintf(stderr, "### IVI ### %s, %s[%d] "format"\n",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)
	#define IVI(format,...) printf(format,##__VA_ARGS__)
#else
	#define IVI_LOG(format,...)
	#define IVI_ERROR(format,...)
	#define IVI(format,...)
#endif




#define IVI_SERVER            "192.168.100.1"
#define IVI_PORT              20000
#define LISTEN_BACKLOG        1
#define MAX_EVENT_NUMBER      10
#define BUFFER_SIZE           100
#define IVICONNECTNUMBER      20

/* Protocol Related */
#define MESSAGE_HEADER_ID     0x464C432E
#define TEST_FLAG             0


typedef struct
{
	uint8_t MessageHeaderID[4];
	uint8_t TestFlag;
	uint16_t MsgSize;
	uint16_t DispatcherMsgLen;
	uint8_t MsgSecurityVer;
}MSG_HEADER_ST;

typedef struct
{ 
	uint8_t uplinkCounter;
	uint8_t downlinkCounter;
}MSG_COUNTER_ST;

typedef struct
{
	uint32_t EventCreationTime;
	uint8_t EventID;
	uint16_t ApplicationID;
	uint16_t MessageID;
	MSG_COUNTER_ST MsgCounter;
	uint16_t ApplicationDataLength;
	uint16_t Result;
}DISPATCHER_MSG_ST;

typedef enum ApplicationID
{
	CallStateReport = 0x2101,
	NetworkStateQuery = 0x2201,
	VehicleVINCodeQuery = 0x2301,
	TelephoneNumQuery = 0x2401,
	EcallStateReport = 0x2501,
	TBOXInfoQuery = 0x2601,
	WIFIInfoQuery = 0x2701,
}AID;


typedef enum Call_State
{ 
	TBOX_CallCommandReq = 1,
	TBOX_CallCommandResp = 2,
	TBOX_CallStateReport = 3,
}CALL_STATE_E;

typedef enum Network_State
{ 
	TBOX_NetworkStateReq = 1,
	TBOX_NetworkStateResp = 2,
}NETWORK_STATE_E;

typedef enum Vin_Code
{ 
	TBOX_VINCodeReq = 1,
	TBOX_VINCodeResp = 2,
}VIN_CODE_E;

typedef enum Telephoe_Query
{ 
	TBOX_PhoneNumberReq = 1,
	TBOX_PhoneNumberResp = 2,
}TELEPHOE_QUERY_E;

typedef enum Tbox_Wifi
{ 
	GET_TBOX_WIFI = 1,
	SET_TBOX_WIFI = 2,
}TBOX_WIFI_E;




class IVI_Communication
{
public:
    IVI_Communication();
    ~IVI_Communication();
    static void *CheckHeartBeat(void *arg);
    int IVI_Communication_Init();
    int socketConnect();
	void et_process(struct epoll_event *events, int number, int epoll_fd, int socketfd);
    int set_non_block(int fd);
    void add_socketFd(int epoll_fd, int fd);

    uint8_t checkSum(uint8_t *pData, int len);
    uint8_t checkSum_BCC(uint8_t *pData, uint16_t len);
    uint8_t unpack_Protocol_Analysis(uint8_t *pData, int len);

	uint16_t received_and_check_data(int fd, uint8_t *buff, int size);

	uint8_t TBOX_Call_State_Report(uint8_t *pData, int len, uint16_t msgType);
	uint8_t TBOX_Network_State_Query();
	uint8_t TBOX_Vehicle_VINCode_Query();
	uint8_t TBOX_Telephone_Num_Query();
	uint8_t TBOX_Ecall_State_Report();
	uint8_t TBOX_General_Info_Query();	
	uint8_t TBOX_WIFI_Info_Query(uint8_t *pData, int len, uint16_t msgType);

	//get seconds from 1970 to now
	uint32_t Get_Seconds_from_1970();
	uint8_t pack_hande_data_and_send(uint16_t MsgID, uint8_t state);
	uint16_t pack_Protocol_Data(uint8_t *pData, int len, DISPATCHER_MSG_ST *dispatchMsg, uint8_t state);
	uint8_t pack_TBOX_Data_Report(uint8_t TestFlag, uint16_t MsgID, uint8_t callState);
	uint8_t TBOX_Voicall_State(uint8_t *pData);


private:
    int sockfd;
    int accept_fd;
    int HeartBeatTimes;
    pthread_t CheckHeartBeatID;
    struct sockaddr_in serverAddr;
	uint8_t MsgSecurityVerion;
    char incomingPhoneNum[20];
    uint8_t voiceCallState;
    uint8_t callID;
    bool setWifiState;

};

extern TBoxDataPool *dataPool;
extern LTEModuleAtCtrl *LteAtCtrl;


#endif

