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

		//debug
		if(ptSPRprtBuff->ucAntennaIdx == 1 && ptSPRprtBuff->ucBeamCoordX == 0 && ptSPRprtBuff->ucBeamCoordY == 2)
		{
			g_qptSPRprtBuffFree.push(ptSPRprtBuff);
			unReadCnt++;
			continue;
		}
		else if(ptSPRprtBuff->ucAntennaIdx == 1 && ptSPRprtBuff->ucBeamCoordX == 1 && ptSPRprtBuff->ucBeamCoordY == 2)
		{
			ptSPRprtBuff->enumBeamType = LEFT_EDGE;
		}

		//debug,处理参数调整
		ptSPRprtBuff->tDPAlgrthmPara.dAziCentroidTH = AZI_3dB/10.0*3;
		ptSPRprtBuff->tDPAlgrthmPara.dEleCentroidTH = ELE_3dB/10.0*3;
		ptSPRprtBuff->tDPAlgrthmPara.dRngCentroidTH = RNG_CELL*2;
		ptSPRprtBuff->tDPAlgrthmPara.dVrCentroidTH  = 5;
		ptSPRprtBuff->tDPAlgrthmPara.ucTermntPntNumTH = 5;
		ptSPRprtBuff->tDPAlgrthmPara.ucTermntWindwLenTH = 5;
		ptSPRprtBuff->tDPAlgrthmPara.ucConfirmPntNumTH = 3;
		ptSPRprtBuff->tDPAlgrthmPara.ucConfirmWindwLenTH = 3;
		ptSPRprtBuff->tDPAlgrthmPara.ucCancleInitPntNumTH = 3;
		ptSPRprtBuff->tDPAlgrthmPara.dModelNoise = 1;
		ptSPRprtBuff->tDPAlgrthmPara.dVrMin = 0.1;
		ptSPRprtBuff->tDPAlgrthmPara.dVrMax = 120;
		ptSPRprtBuff->tDPAlgrthmPara.dEleAssocTH = ELE_3dB/10.0*3;
		ptSPRprtBuff->tDPAlgrthmPara.dAltAssocTH = 10;
		ptSPRprtBuff->tDPAlgrthmPara.dGamma = 6;
		ptSPRprtBuff->tDPAlgrthmPara.dDefAngleForTmpTrackTH = 60;
		ptSPRprtBuff->tDPAlgrthmPara.dDefAngleForTgtTrackTH = 180;
		ptSPRprtBuff->tDPAlgrthmPara.dConfidenceTH = 0.5;

		ptSPRprtBuff->tDPAlgrthmPara.ucStandardInitPntNumTH = 4;
		ptSPRprtBuff->tDPAlgrthmPara.ucStandardInitWindwLenTH = 5;

		ptSPRprtBuff->tDPAlgrthmPara.dQuickInitRngTH = 3500;
		ptSPRprtBuff->tDPAlgrthmPara.ucQuickInitPntNumTH = 3;
		ptSPRprtBuff->tDPAlgrthmPara.ucQuickInitWindwLenTH = 4;
		ptSPRprtBuff->tDPAlgrthmPara.dQuickInitAziAssocTH = AZI_3dB/10.0*8;
		ptSPRprtBuff->tDPAlgrthmPara.dQuickInitVrAssocTH  = 15;

		ptSPRprtBuff->tDPAlgrthmPara.dHighConfVrTH = 5;   //高置信度速度阈值
		ptSPRprtBuff->tDPAlgrthmPara.dHighConfRngTH = 1000;  //高置信度距离阈值
        ptSPRprtBuff->tDPAlgrthmPara.ucSlowInitPntNumTH = 6;
        ptSPRprtBuff->tDPAlgrthmPara.ucSlowInitWindwLenTH = 8;
		ptSPRprtBuff->tDPAlgrthmPara.dSlowInitAziAssocTH = 1.6; //方位关联阈值-低置信度航迹
		ptSPRprtBuff->tDPAlgrthmPara.dSlowInitVrAssocTH = 7; //多普勒速度关联阈值-低置信度航迹
		ptSPRprtBuff->tDPAlgrthmPara.dRngDiffTH = ptSPRprtBuff->tDPAlgrthmPara.dHighConfVrTH*0.8; //距离微分阈值-低置信度航迹
		ptSPRprtBuff->tDPAlgrthmPara.dStdRngTH = 15;  //距离标准差阈值-低置信度航迹
		ptSPRprtBuff->tDPAlgrthmPara.dStdAziTH = 0.8;  //方位角标准差阈值-低置信度航迹
		ptSPRprtBuff->tDPAlgrthmPara.dStdEleTH = 0.6;  //俯仰角标准差阈值-低置信度航迹
		ptSPRprtBuff->tDPAlgrthmPara.dStdVrTH = 1.5;  //多普勒速度标准差阈值-低置信度航迹
		ptSPRprtBuff->tDPAlgrthmPara.dStdSNRTH = 1.5;  //信噪比标准差阈值-低置信度航迹
		ptSPRprtBuff->tDPAlgrthmPara.adSNRAccumTH[2] = 28.2;
		ptSPRprtBuff->tDPAlgrthmPara.adSNRAccumTH[3] = 30.73;
		ptSPRprtBuff->tDPAlgrthmPara.adSNRAccumTH[4] = 32.67;
		ptSPRprtBuff->tDPAlgrthmPara.adSNRAccumTH[5] = 34.26;
		ptSPRprtBuff->tDPAlgrthmPara.adSNRAccumTH[6] = 35.57;
		ptSPRprtBuff->tDPAlgrthmPara.adSNRAccumTH[7] = 36.73;
		ptSPRprtBuff->tDPAlgrthmPara.adSNRAccumTH[8] = 37.88;
		//数据缓存
		g_qptSPRprtData.push(ptSPRprtBuff);
		
		unReadCnt++;
	}
		
	delete [] aucBuff;
	return NULL;
}
