#include "CommunicateProtocol.h"
#include "global.h"
#include "SpdLogger.h"

INT32 ParseMsgDPRcvDisp(VOID* ptrIN, UINT32 unMsgLen);


VOID* DPRcvDispCMD(VOID*)
{
	/*数据处理接收显控命令*/

	//建立套接字
	INT32 fd;
	if((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		ERROR("socket error");
	
	//配置数处接收显控命令的套接字结构
	struct sockaddr_in tDPAddr;
	bzero(&tDPAddr, sizeof(tDPAddr));
	BuildSockAddr(ANY_IP, DP_RCV_PORT_FOR_DISP, tDPAddr);

	//套接字与监听端口绑定
	if (bind(fd, (struct sockaddr *)&tDPAddr, sizeof(tDPAddr)) < 0)
	{
		ERROR("Bind failed.");
		exit(1);
	}

	//显控套接字地址结构(From)
	struct sockaddr_in tDispAddrFrom;
	UINT32 unFromLen = sizeof(tDispAddrFrom);

	INT8* aRcvBuffer = new INT8 [BUFFER_NUM*BUFFER_SIZE];
	INT8 *pRcvBuffer = NULL;
	UINT32 unRcvCnt = 0;
	LONG LMsgLen = 0;
	INT32 nRtn = 0;
	
	//数据接收
	while (true)
	{

		pRcvBuffer = aRcvBuffer + unRcvCnt % BUFFER_NUM * BUFFER_SIZE;

		LMsgLen = recvfrom(fd, pRcvBuffer, BUFFER_SIZE, 0, (struct sockaddr *)&tDispAddrFrom, &unFromLen);

		unRcvCnt++;

		if(LMsgLen < 0)
		{
			WARN("receive error, length of message received must be greater than 0, but got {}", LMsgLen);
			continue;
		}

		//消息校验
		if( (nRtn = CheckMsg(pRcvBuffer, LMsgLen)) < 0 )
		{
			WARN("message check failed");
			continue;
		}

		//数据解析
		ParseMsgDPRcvDisp(pRcvBuffer, LMsgLen);

	}

	delete [] aRcvBuffer;

	return NULL;
}

INT32 ParseMsgDPRcvDisp(VOID* ptrIN, UINT32 unMsgLen)
{
	/*解析显控下发至数据处理的控制命令*/

	if(ptrIN == NULL)
	{
		ERROR("Invalid input argument, pointer is NULL");
		return -1;
	}

	if(unMsgLen == 0)
	{
		WARN("message length is 0");
		return -2;
	}
	
	UINT8* ptr = (UINT8*)ptrIN;

	UINT16 usMsgID = *(UINT16*)(ptr+sizeof(MsgPrefixInfo)-sizeof(UINT16));
	UINT8 ucIDForTrackMannual = 0;

	switch(usMsgID)
	{

	case TRACK_MANNUAL_MSG_ID: //手动跟踪
		ucIDForTrackMannual = *(UINT8*)(ptr+sizeof(MsgPrefixInfo));
		if(ucIDForTrackMannual == 0)
		{
			ERROR("invalid track ID({}) for mannual tracking", ucIDForTrackMannual);
			return -3;
		}
		pthread_mutex_lock(&mutexIDForTrackMannual);
		g_qucIDForTrackMannual.push(ucIDForTrackMannual);
		pthread_mutex_unlock(&mutexIDForTrackMannual);
		break;

	default:
		ERROR("invalid MSG ID(0x{:04X})", usMsgID);
		return -4;
	}

	return 0;
}


