#include "SpdLogger.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string>
#include "CommunicateProtocol.h"

INT32 BuildSockAddr(const INT8* pcIP, UINT16 usPort, struct sockaddr_in& tSockAddr)
{
	/*构造一个IPv4协议的socket地址结构*/

	bzero(&tSockAddr, sizeof(tSockAddr));

	if (pcIP == NULL)
	{
		ERROR("Invalid input argument, 'pcIP' is NULL");
		return -1;
	}

	tSockAddr.sin_family = AF_INET;
	inet_pton(AF_INET, pcIP, &tSockAddr.sin_addr.s_addr);
	tSockAddr.sin_port = htons(usPort);

	return 0;
}

UINT8 GetXORCode(VOID* ptrData, UINT32 unLength)
{
	/*BCC异或校验，输入数据首地址ptrData，需校验的数据长度unLength，字节为单位*/

	if(unLength == 0)
	{
		INFO("data length is 0");
	}

	UINT8 ucRtnCode = 0;
	UINT8* ptrTmp = (UINT8*)ptrData;
	while(unLength > 0)
	{
		ucRtnCode ^= *ptrTmp;

		ptrTmp++;
		unLength--;
	}

	return ucRtnCode;
}


INT32 CheckMsg(VOID* ptrBuff, UINT32 unMsgLen)
{
	/*消息校验*/

	if( ptrBuff == NULL )
	{
		ERROR("Invalid input argument, 'ptrBuff' is NULL");
		return -1;
	}

	if(unMsgLen <= 0)
	{
		ERROR("Invalid input argument, 'unMsgLen' is equal to or less than 0");
		return -2;
	}

	UINT8* ptr = (UINT8*)ptrBuff;

	//头标志校验
	UINT32 unHeadFlag = *(UINT32*)ptr;
	if(unHeadFlag != HEAD_FLAG)
	{
		ERROR("invalid head flag(0x{:08X}), it must be 0x{:08X}", unHeadFlag, HEAD_FLAG);
		return -3;
	}

	//尾标志校验
	UINT32 unTailFlag = *(UINT32*)(ptr+unMsgLen-sizeof(UINT32));
	if(unTailFlag != TAIL_FLAG)
	{
		ERROR("invalid tail flag(0x{:08X}), it must be 0x{:08X}", unTailFlag, TAIL_FLAG);
		return -4;
	}

	//BCC校验
	UINT8 ucCheckCode = GetXORCode(ptr, unMsgLen-sizeof(MsgSuffixInfo));
	UINT8 ucCheckCodeRef = *(UINT8*)( ptr + unMsgLen - sizeof(MsgSuffixInfo));
	if( ucCheckCode != ucCheckCodeRef )
	{
		ERROR("check failed, check result:0x{:02X}, check code in MSG:0x{:02X}", ucCheckCode, ucCheckCodeRef);
		return -5;
	}

	//校验通过
	return 0;
}

INT32 BuildMsgPrefix(UINT32 unHeadFlag, UINT16 usDataLen, UINT32 unComCnt, UINT16 usSrcID, UINT16 usDestID, UINT16 usMsgID, MsgPrefixInfo& tMsgPrefix)
{
	/*构造报文前缀*/

	bzero(&tMsgPrefix, sizeof(MsgPrefixInfo));
	
	tMsgPrefix.unHeadFlag = unHeadFlag;	
	tMsgPrefix.usSrcID = usSrcID;
	tMsgPrefix.usDstID = usDestID;	
	tMsgPrefix.unComunicatCnt = unComCnt;
	tMsgPrefix.usMsgLen = usDataLen + MSG_PREFIX_LEN; //数据长度+前缀长度
	tMsgPrefix.usMsgID = usMsgID;	
	return 0;
}

