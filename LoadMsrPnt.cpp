#include "DataTypeDef.h"
#include "SpdLogger.h"
#include "CommunicateProtocol.h"
#include "global.h"
#include "DPRcvMsrPnt.h"
#include <stdio.h>
#include <unistd.h>     // usleep

#define MSG_LEN_OFFSET 12 //移动至消息长度的偏移量

VOID* LoadMsrPnt(VOID*)
{
	INT8 acDataPath[] = "./MsrPntData/MsrPnt20260331151902.dat";
	INT32 nNum = 0, nRtn = 0;

	UINT8* aucBuff = new UINT8 [BUFFER_SIZE];

	FILE* pf = NULL;
	if( (pf = fopen(acDataPath, "rb")) == NULL )
	{
		ERROR("fopen error, data path:{}", acDataPath);
		exit(1);
	}
	
	// std::cout << "enter a number to trigger msr pnt reading, 0 for auto, 1 for manual:";
	// std::cin >> nNum;
	// std::cout << std::endl;

	UINT32 unPacketLen = 0, unReadCnt = 0;
	MsgPrefixInfo tMsgPrefix;
	bzero(&tMsgPrefix, sizeof(MsgPrefixInfo));

	SPReportDataInBuffInfo* ptSPRprtBuff = NULL;
	
	UINT32 unHead1 = 0, unHead2 = 0;

	while(!feof(pf))
	{
		if(nNum == 0)
			usleep(5e2);
		else
		{
			std::cout << "enter any number to read next msr pnt packet:";
			std::cin >> nNum;
			std::cout << std::endl;			
		}

		//验证头标志
		if( (nRtn = fread(&unHead1, sizeof(UINT32), 1, pf)) != 1 )
		{
			ERROR("fread error, try to read {} bytes, but got {} bytes", 1, nRtn);
			g_bIsRunning = false;
			break;
		}

		//验证头标志
		if( (nRtn = fread(&unHead2, sizeof(UINT32), 1, pf)) != 1 )
		{
			ERROR("fread error, try to read {} bytes, but got {} bytes", 1, nRtn);
			exit(1);
		}

		if(unHead1 != 0xFA55FA55 || unHead2 != 0xBB03BB02)
			continue;

		//修正指针位置
		if( (nRtn = fseek(pf, -8, SEEK_CUR)) != 0)
		{
			ERROR("fseek error");
			exit(1);
		}

		//读取包头，获取数据长度
		if( (nRtn = fread(aucBuff, sizeof(MsgPrefixInfo), 1, pf)) != 1 )
		{
			ERROR("fread error, try to read {} bytes, but got {} bytes", 1, nRtn);
			exit(1);
		}

		memcpy(&tMsgPrefix, aucBuff, sizeof(MsgPrefixInfo));
		if(tMsgPrefix.unHeadFlag != HEAD_FLAG)
		{
			ERROR("MstPnt data error, got invalid head flag(0x{:08X})", tMsgPrefix.unHeadFlag);
			exit(1);
		}

		//修正流指针至初始位置
		if (fseek(pf, -1*(sizeof(MsgPrefixInfo)), SEEK_CUR) != 0)
		{
			ERROR("fseek error");
			exit(1);
		}

		//读取一包数据
		unPacketLen = tMsgPrefix.usMsgLen + sizeof(MsgSuffixInfo);
		if( (nRtn = fread(aucBuff, unPacketLen, 1, pf)) != 1 )
		{
			ERROR("fread error, try to read {} bytes, but got {} bytes", 1, nRtn);
			exit(1);
		}

		//数据解析
		if(g_qptSPRprtBuffFree.empty())
			INFO("No free buff to save measure point, retry...");
		ptSPRprtBuff = g_qptSPRprtBuffFree.pop();
		ParseMsgDPRcvSPMsrPnt(aucBuff, ptSPRprtBuff, unReadCnt);

		//数据缓存
		g_qptSPRprtData.push(ptSPRprtBuff);
		
		unReadCnt++;
	}
		
	delete [] aucBuff;
	return NULL;
}
