#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <net/if.h>
#include <dirent.h>
#include "CommunicateProtocol.h"
#include "SpdLogger.h"
#include "global.h"
#include "SinCosTan.h"
#include  <sys/stat.h> 

INT32 SaveMsrPntPacket(VOID* ptrIN, UINT32 unLength, UINT8 ucSaveFlag);
time_t GetUTCTimeSec(INT32 nYear, INT32 nMonth, INT32 nDay, INT32 nHour, INT32 nMin, INT32 nScnd);
INT32 ParseMsgDPRcvSPMsrPnt(UINT8* pRcvBuffer, SPReportDataInBuffInfo* ptSPRprtDataBuff, UINT32 unRcvCnt);


VOID* DPRcvMsrPnt(VOID*)
{
	/*数据处理接收信处点迹*/

	INT32 nRtn = 0;

	//建立套接字
	INT32 fd;
	if( (fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 )
		ERROR("socket error.");

	//配置数据处理套接字地址结构的IP和端口号
	struct sockaddr_in tDPAddr;
	bzero(&tDPAddr, sizeof(tDPAddr));
	BuildSockAddr(ANY_IP, DP_RCV_PORT_FOR_SP, tDPAddr);

	//套接字与监听端口绑定
	if (bind(fd, (struct sockaddr *)&tDPAddr, sizeof(tDPAddr)) < 0)
	{
		ERROR("bind failed");
		exit(1);
	}

	UINT8* aucRcvBuffer = new UINT8 [BUFFER_NUM*BUFFER_SIZE];
	UINT8* pucRcvBuffer = NULL;
	struct sockaddr_in tSPAddr;
	UINT32 unFromLen = sizeof(tSPAddr), unRcvCnt = 0;
	LONG LMsgLen = 0;
	SPReportDataInBuffInfo *ptSPRprtBuff = NULL;

	while(true)
	{
		pucRcvBuffer = aucRcvBuffer + unRcvCnt % BUFFER_NUM * BUFFER_SIZE;
		LMsgLen = recvfrom(fd, pucRcvBuffer, BUFFER_SIZE, 0, (struct sockaddr *)&tSPAddr, &unFromLen);

		if(LMsgLen < 0)
		{
			WARN("recvfrom error, the received message length is less than 0({})", LMsgLen);
			continue;
		}

		//消息校验
		if( (nRtn = CheckMsg(pucRcvBuffer, LMsgLen)) < 0 )
		{
			WARN("msr pnt message check failed");
			continue;
		}

		//点迹数据保存
		SaveMsrPntPacket(pucRcvBuffer, LMsgLen, 1);

		//申请缓冲空间
		if(g_qptSPRprtBuffFree.empty())
		{
			WARN("No free buff to save measure point, retry...");
		}
		ptSPRprtBuff = g_qptSPRprtBuffFree.pop();
			
		//数据解析
		if ( (nRtn = ParseMsgDPRcvSPMsrPnt(pucRcvBuffer, ptSPRprtBuff, unRcvCnt) ) < 0)
        {
            g_qptSPRprtBuffFree.push(ptSPRprtBuff);
            continue;
        }

		//debug
//		INFO("Rcv CPI:{}, microsec:{}", ptSPRprtBuff->unCPI, ptSPRprtBuff->tDate.unMicroSec);

		//debug,处理参数调整
		ptSPRprtBuff->tDPAlgrthmPara.dAziCentroidTH = AZI_3dB/10.0*3;
		ptSPRprtBuff->tDPAlgrthmPara.dEleCentroidTH = ELE_3dB/10.0*3;
		ptSPRprtBuff->tDPAlgrthmPara.dRngCentroidTH = RNG_CELL*2;
		ptSPRprtBuff->tDPAlgrthmPara.dVrCentroidTH  = 4;
		ptSPRprtBuff->tDPAlgrthmPara.ucTermntPntNumTH = 5;
		ptSPRprtBuff->tDPAlgrthmPara.ucTermntWindwLenTH = 5;
		ptSPRprtBuff->tDPAlgrthmPara.ucConfirmPntNumTH = 3;
		ptSPRprtBuff->tDPAlgrthmPara.ucConfirmWindwLenTH = 3;
		ptSPRprtBuff->tDPAlgrthmPara.ucCancleInitPntNumTH = 3;
		ptSPRprtBuff->tDPAlgrthmPara.dModelNoise = 0.5;
		ptSPRprtBuff->tDPAlgrthmPara.dVrMin = 0.1;
		ptSPRprtBuff->tDPAlgrthmPara.dVrMax = 120;
		ptSPRprtBuff->tDPAlgrthmPara.dEleAssocTH = ELE_3dB/10.0*5;
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
		ptSPRprtBuff->tDPAlgrthmPara.dQuickInitAziAssocTH = AZI_3dB/10.0*15;
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

		//数据置入缓冲区
		g_qptSPRprtData.push(ptSPRprtBuff);
	
		unRcvCnt++;	

	}
}

INT32 ParseMsgDPRcvSPMsrPnt(UINT8* pRcvBuffer, SPReportDataInBuffInfo* ptSPRprtDataBuff, UINT32 unRcvCnt)
{
	/*从接收缓冲区中拷贝数据至缓冲队列*/

	if(pRcvBuffer == NULL)
	{
		ERROR("invalid argument, 'pRcvBuffer' == NULL");
		return -1;
	}

	if(ptSPRprtDataBuff == NULL)
	{
		ERROR("invalid argument, 'ptSPRprtDataBuff' == NULL");
		return -2;
	}	

	bzero(ptSPRprtDataBuff, sizeof(SPReportDataInBuffInfo));

	//控制表解析
	CtrlListMsg tCtrlList;
	bzero(&tCtrlList, sizeof(CtrlListMsg));
	memcpy(&tCtrlList, pRcvBuffer+sizeof(MsgPrefixInfo), sizeof(CtrlListMsg));

	if (tCtrlList.ucBandWidthID == 0)
	{
		ptSPRprtDataBuff->dB = 6e6;
	}
	else if (tCtrlList.ucBandWidthID == 1)
	{
		ptSPRprtDataBuff->dB = 15e6;
	}
	else
	{
		ERROR("invalid bandwidth index:{}", tCtrlList.ucBandWidthID);
	}

	if (tCtrlList.ucFcIdx <= MAX_FC_ID)
	{
		ptSPRprtDataBuff->dFc = (9.2+0.02*tCtrlList.ucFcIdx)*1e9;
	}
	else
	{
		ERROR("invalid fc index:{}", tCtrlList.ucFcIdx);
	}

	ptSPRprtDataBuff->dPRT = tCtrlList.usPRT_1 * 0.1 * 1e-6;
	ptSPRprtDataBuff->unPulseNum = tCtrlList.usPulseNum;
	ptSPRprtDataBuff->enumWorkMode = static_cast<WorkFashionInfo>(tCtrlList.ucWorkMode);
	ptSPRprtDataBuff->enumScheduleEvent = static_cast<ScheduleEventInfo>(tCtrlList.ucScheduleType);
	ptSPRprtDataBuff->dAziMajor = tCtrlList.sAziMajor_2 * 1e-2;
	ptSPRprtDataBuff->dEleMajor = tCtrlList.sEleMajor_2 * 1e-2;
	ptSPRprtDataBuff->usTgtID = tCtrlList.usTASID;
	ptSPRprtDataBuff->tDate.usYear = tCtrlList.usYear;
	ptSPRprtDataBuff->tDate.ucMon = tCtrlList.ucMon;
	ptSPRprtDataBuff->tDate.ucDay = tCtrlList.ucDay;
	ptSPRprtDataBuff->tDate.ucHour = tCtrlList.ucHour;
	ptSPRprtDataBuff->tDate.ucMin = tCtrlList.ucMin;
	ptSPRprtDataBuff->tDate.ucSec = tCtrlList.ucSec;
	ptSPRprtDataBuff->tDate.unMicroSec = tCtrlList.unMicroSec;
	ptSPRprtDataBuff->dTime = tCtrlList.ucHour*3600+tCtrlList.ucMin*60+tCtrlList.ucSec + tCtrlList.unMicroSec*1e-6;
	ptSPRprtDataBuff->unCPI = tCtrlList.unCPI;
	ptSPRprtDataBuff->tDPAlgrthmPara.ucQuickInitWindwLenTH = tCtrlList.ucQuickInitWindwLenTH;
	ptSPRprtDataBuff->tDPAlgrthmPara.ucQuickInitPntNumTH = tCtrlList.ucQuickInitPntNumTH;
	ptSPRprtDataBuff->tDPAlgrthmPara.ucTermntWindwLenTH = tCtrlList.ucTermntWindwLenTH;
	ptSPRprtDataBuff->tDPAlgrthmPara.ucTermntPntNumTH = tCtrlList.ucTermntPntNumTH;
	ptSPRprtDataBuff->tDPAlgrthmPara.dModelNoise = tCtrlList.usModelNoise_2 * 1e-2;
	ptSPRprtDataBuff->tDPAlgrthmPara.dVrMin = tCtrlList.usVrMin_1 * 1e-1;
	ptSPRprtDataBuff->tDPAlgrthmPara.dVrMax = tCtrlList.usVrMax_1 *1e-1; 
	ptSPRprtDataBuff->tDPAlgrthmPara.dQuickInitAziAssocTH = tCtrlList.usAziAssocTH_1 * 1e-1;
	ptSPRprtDataBuff->tDPAlgrthmPara.dEleAssocTH = tCtrlList.usEleAssocTH_1 *1e-1;
	ptSPRprtDataBuff->tDPAlgrthmPara.dQuickInitVrAssocTH = tCtrlList.usVrAssocTH_1 * 1e-1;
	ptSPRprtDataBuff->tDPAlgrthmPara.dGamma = tCtrlList.usGamma_1 *1e-1;
	ptSPRprtDataBuff->tDPAlgrthmPara.dRngCentroidTH = tCtrlList.ucRngCentroidTH;
	ptSPRprtDataBuff->tDPAlgrthmPara.dAziCentroidTH = tCtrlList.ucAziCentroidTH_1 * 1e-1;
	ptSPRprtDataBuff->tDPAlgrthmPara.dEleCentroidTH = tCtrlList.ucEleCentroidTH_1 * 1e-1;
	ptSPRprtDataBuff->tDPAlgrthmPara.dVrCentroidTH = tCtrlList.ucVrCentroidTH_1 * 1e-1;

	ptSPRprtDataBuff->tAttitude.dYaw = tCtrlList.unYaw_3 * 1e-3;
	ptSPRprtDataBuff->tAttitude.dPitch = tCtrlList.nPitch_3 * 1e-3;
	ptSPRprtDataBuff->tAttitude.dRoll = tCtrlList.sRoll_2 * 1e-2;
	ptSPRprtDataBuff->tLLA.dLat = tCtrlList.lLat_9 * 1e-9;
	ptSPRprtDataBuff->tLLA.dLon = tCtrlList.lLon_9 * 1e-9;
	ptSPRprtDataBuff->tLLA.dAlt = tCtrlList.nAlt_3 * 1e-3;


	if (abs(tCtrlList.cBeamType) > 1) {
		ERROR("invalid argument, 'tCtrlList.cBeamType({})', it can only be -1, 0, 1.", tCtrlList.cBeamType);
		return -3;
	}
	ptSPRprtDataBuff->enumBeamType = (enum BeamTypeInfo)tCtrlList.cBeamType;
	ptSPRprtDataBuff->ucBeamCoordX = tCtrlList.ucBeamCoordX;
	ptSPRprtDataBuff->ucBeamCoordY = tCtrlList.ucBeamCoordY;
	ptSPRprtDataBuff->ucAntennaIdx = *(UINT8*)(pRcvBuffer+RADAR_INDEX_OFFSET);
	ptSPRprtDataBuff->ucRadarID = *(UINT8*)(pRcvBuffer+RADAR_INDEX_OFFSET);

	//点迹数据解析
	UINT16 usPntNum = *(UINT16*)(pRcvBuffer+MSR_PNT_NUM_OFFSET);
	SPSendMsrPntInfo* ptSPSendMstPnt = (SPSendMsrPntInfo*)(pRcvBuffer + MSR_PNT_NUM_OFFSET + sizeof(UINT16));
	UINT16 usCnt = 0;
	for(UINT16 i = 0; i < usPntNum; i++)
	{
//		ptSPSendMstPnt->fSNR_CFAR < 14 ||
//		if(fabs(ptSPSendMstPnt->fVr) <= 2 || ptSPSendMstPnt->fSNR_CFAR < 12)
//		{
//			ptSPSendMstPnt++;
//			continue;
//		}

		ptSPRprtDataBuff->atMsrPnrArr[usCnt].unIdx = i;
		ptSPRprtDataBuff->atMsrPnrArr[usCnt].fRng = ptSPSendMstPnt->fDist;
		ptSPRprtDataBuff->atMsrPnrArr[usCnt].unRngCell = ptSPSendMstPnt->unRngCell;
		ptSPRprtDataBuff->atMsrPnrArr[usCnt].fAzi = ptSPSendMstPnt->fAzi;
		ptSPRprtDataBuff->atMsrPnrArr[usCnt].fAziMajor = ptSPSendMstPnt->fAziMajor;
		ptSPRprtDataBuff->atMsrPnrArr[usCnt].fEle = ptSPSendMstPnt->fEle;
		ptSPRprtDataBuff->atMsrPnrArr[usCnt].fAlt = ptSPSendMstPnt->fAlt;
		ptSPRprtDataBuff->atMsrPnrArr[usCnt].fEleMajor = ptSPSendMstPnt->fEleMajor;
		ptSPRprtDataBuff->atMsrPnrArr[usCnt].fVr = ptSPSendMstPnt->fVr;
		ptSPRprtDataBuff->atMsrPnrArr[usCnt].unDplCell = ptSPSendMstPnt->unDplCell;
		ptSPRprtDataBuff->atMsrPnrArr[usCnt].fAmp = ptSPSendMstPnt->fAmp;
		ptSPRprtDataBuff->atMsrPnrArr[usCnt].fSNR = ptSPSendMstPnt->fSNR_CFAR;
		ptSPRprtDataBuff->atMsrPnrArr[usCnt].tDate = ptSPRprtDataBuff->tDate;
		ptSPRprtDataBuff->atMsrPnrArr[usCnt].dTime = ptSPRprtDataBuff->dTime;
		ptSPRprtDataBuff->atMsrPnrArr[usCnt].dPRT = ptSPRprtDataBuff->dPRT;
		ptSPRprtDataBuff->atMsrPnrArr[usCnt].unCPI = ptSPRprtDataBuff->unCPI;
		ptSPRprtDataBuff->atMsrPnrArr[usCnt].fConfidence = ptSPSendMstPnt->fConfidence;

		ptSPSendMstPnt++;
		usCnt++;

		if(usCnt >= MAX_MSR_NUM) break;
	}
	ptSPRprtDataBuff->unMsrNum = usCnt;

	return 0;
}


INT32 SaveMsrPntPacket(VOID* ptrIN, UINT32 unLength, UINT8 ucSaveFlag)
{
	/*将控制表和点迹数据写入文件, ucSaveFlag为写文件开关*/

	if(ptrIN == NULL)
	{
		ERROR("invalid argument, 'ptrIN' == NULL")	;
		return -1;
	}

	if(unLength == 0)
	{
		ERROR("data length is 0");
		return -2;
	}
	
	if(ucSaveFlag == 0)
	{
		return 1;
	}

	INT8* ptr = (INT8*)ptrIN;
	INT32 nRtn = 0;
	static FILE* pf = NULL;
	static char acFilePath[256] = "";
	static BOOL bCreateFileFlag = true;


	//获取时间
	time_t timeNowUTCSec = time(0);
	struct tm * ptLocalTime = localtime(&timeNowUTCSec);

	//创建路径，编辑文件名
	//static 类型的bCreateFileFlag确保文件仅创建一次
	if(bCreateFileFlag)
	{
		if( opendir("./MsrPntData") == NULL)
		{
			if( mkdir("./MsrPntData", 0777) < 0) //路径权限0777，文件所有者有rwx权限
			{
				ERROR("mkdir error");
				return -2;
			}
		}

		sprintf(acFilePath, "./MsrPntData/MsrPnt%d%02d%02d%02d%02d%02d.dat", ptLocalTime->tm_year+1900, ptLocalTime->tm_mon+1, ptLocalTime->tm_mday, ptLocalTime->tm_hour, ptLocalTime->tm_min, ptLocalTime->tm_sec);

		bCreateFileFlag = false;
	}

	//获取点迹数量
	UINT16 usPntNum = *(UINT16*)(ptr+MSR_PNT_NUM_OFFSET);

	//有点迹保存数据包
//	if( usPntNum == 0) return -3;


	//打开输入流
	if( (pf = fopen(acFilePath, "ab")) == NULL)
	{
		ERROR("fopen error, file path:{}", acFilePath);
		return -4;
	}

	//写入数据
	if( (nRtn = fwrite(ptr, unLength, 1, pf)) != 1 )
	{
		ERROR("fwrite error, the length is expected to {}, but got {}", 1, nRtn);
		return -5;
	}

	//关闭输入流
	fclose(pf);

	return 0;
}

time_t GetUTCTimeSec(INT32 nYear, INT32 nMonth, INT32 nDay, INT32 nHour, INT32 nMin, INT32 nScnd)
{
	/*将年-月-日-时-分-秒格式的UTC时间转换为累计秒数*/

	struct tm tRowTime = {0};

	//年、月预处理
	tRowTime.tm_year  = nYear - 1900;
	tRowTime.tm_mon   = nMonth - 1;
	tRowTime.tm_mday  = nDay;
	tRowTime.tm_hour  = nHour;
	tRowTime.tm_min   = nMin;
	tRowTime.tm_sec   = nScnd;

	//转换
	time_t Date2Scnds = mktime(&tRowTime) - 18000;//日期格式转换为相对时间

	return Date2Scnds;
}

