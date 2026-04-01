#include "DPFunc.h"

#include "StructDef.h"
#include "SpdLogger.h"
#include "global.h"
#include "SinCosTan.h"
#include "mkl.h"
#include <vector>
#include <algorithm>
#include <iterator>
#include "dirent.h"
#include  <sys/stat.h> 
#include <map>

DOUBLE GetTimeCost(const struct timeval& tBgn, const struct timeval tEnd)
{
	/*计算两个时间点之间的时间间隔，单位:秒*/

	DOUBLE dRes = (tEnd.tv_sec - tBgn.tv_sec) + (tEnd.tv_usec - tBgn.tv_usec)*1e-6;

	return dRes;
}

INT32 CheckAlgorithmPara(AlgorithmParaInfo* ptAlgorithmPara)
{
	/*算法参数检查*/

	if(ptAlgorithmPara->ucQuickInitPntNumTH == 0)
	{
		ERROR("invalid argument, 'ptAlgorithmPara->ucQuickInitPntNumTH == 0'");
		return -1;
	}

	if(ptAlgorithmPara->ucQuickInitWindwLenTH == 0)
	{
		ERROR("invalid argument, 'ptAlgorithmPara->ucQuickInitWindwLenTH' == 0");
		return -2;
	}

	if(ptAlgorithmPara->ucTermntPntNumTH == 0)
	{
		ERROR("invalid argument, 'ptAlgorithmPara->ucTermntPntNumTH' == 0");
		return -3;
	}

	if(ptAlgorithmPara->ucTermntWindwLenTH == 0)
	{
		ERROR("invalid argument, 'ptAlgorithmPara->ucTermntWindwLenTH' == 0");
		return -4;
	}

	if(ptAlgorithmPara->dGamma == 0)
	{
		ERROR("invalid argument, 'ptAlgorithmPara->dGamma == 0'");
		return -5;
	}

	if(ptAlgorithmPara->ucQuickInitPntNumTH > ptAlgorithmPara->ucQuickInitWindwLenTH)
	{
		ERROR("invalid argument, 'ptAlgorithmPara->ucQuickInitPntNumTH({}) > ptAlgorithmPara->ucQuickInitWindwLenTH({})'",
				ptAlgorithmPara->ucQuickInitPntNumTH, ptAlgorithmPara->ucQuickInitWindwLenTH);
		return -6;
	}

	if(ptAlgorithmPara->ucTermntPntNumTH > ptAlgorithmPara->ucTermntWindwLenTH)
	{
		ERROR("invalid argument, 'ptAlgorithmPara->ucTermntPntNumTH({}) > ptAlgorithmPara->ucTermntWindwLenTH({})'",
				ptAlgorithmPara->ucTermntPntNumTH, ptAlgorithmPara->ucTermntWindwLenTH);
		return -7;
	}

	if(ptAlgorithmPara->dVrMin > ptAlgorithmPara->dVrMax)
	{
		ERROR("invalid argument, 'ptAlgorithmPara->dVrMin({}) > ptAlgorithmPara->dVrMax({})'",
				ptAlgorithmPara->dVrMin, ptAlgorithmPara->dVrMax);
		return -8;
	}

	return 0;
}

INT32 AlgorithmParaUpdt(SPReportDataInBuffInfo* ptSPRprtData, AlgorithmParaInfo& tAlgorithmPara)
{
	/*更新算法参数*/

	if(ptSPRprtData == NULL)
	{
		ERROR("invalid argument, 'ptSPRprtData' == NULL");
		return -1;
	}

	INT32 nRtn = 0;

	if( (nRtn = CheckAlgorithmPara(&(ptSPRprtData->tDPAlgrthmPara)) ) < 0 )
	{
		ERROR("algorithm parameter check failed.");
		return -2;
	}

	memcpy(&tAlgorithmPara, &(ptSPRprtData->tDPAlgrthmPara), sizeof(AlgorithmParaInfo));	

	return 0;
}

INT32 CheckSysPara(SPReportDataInBuffInfo* ptSPRprtData)
{
	/*检查系统参数*/

	if(ptSPRprtData == NULL)
	{
		ERROR("invalid argument, 'ptSPRprtData' == NULL");
		return -1;
	}

	if(ptSPRprtData->dB <= 0)
	{
		ERROR("invalid argument, 'ptSPRprtData->dB({}) <= 0'", ptSPRprtData->dB);
		return -2;
	}

	if(ptSPRprtData->dFc <= 0)
	{
		ERROR("invalid argument, 'ptSPRprtData->dFc({}) <= 0'", ptSPRprtData->dFc);
		return -3;
	}

	if(ptSPRprtData->dPRT <=0)
	{
		ERROR("invalid argument, 'ptSPRprtData->dPRT({}) <= 0'", ptSPRprtData->dPRT);
		return -4;
	}

	if(ptSPRprtData->unPulseNum == 0)
	{
		ERROR("invalid argument, 'ptSPRprtData->unPulseNum == 0'");
		return -5;
	}

	//if(ptSPRprtData->usCPINumOneCircle == 0)
	//{
	//	ERROR("invalid argument, 'ptSPRprtData->usCPINumOneCircle == 0'");
	//	return -6;
	//}

	return 0;
}

INT32 SysParaUpdt(SPReportDataInBuffInfo* ptSPRprtData, SysParaInfo& tSysPara)
{
	/*系统参数更新*/

	if(ptSPRprtData == NULL)
	{
		ERROR("invalid argument, 'ptSPRprtData == NULL'");
		return -1;
	}

	INT32 nRtn = 0;

	if( (nRtn = CheckSysPara(ptSPRprtData)) < 0 )
	{
		ERROR("system para check failed");
		return -2;

	}

	tSysPara.dB = ptSPRprtData->dB;
	tSysPara.dAzi3dB = AZI_3dB;
	tSysPara.dEle3dB = ELE_3dB;
	tSysPara.usCPINumOneCircle = ptSPRprtData->usCPINumOneCircle;
	tSysPara.dFc = ptSPRprtData->dFc;
	tSysPara.dLambda = CVEL / tSysPara.dFc;
	tSysPara.dPRT = ptSPRprtData->dPRT;
	tSysPara.usPulseNum = ptSPRprtData->unPulseNum;
	tSysPara.enumWorkMode = ptSPRprtData->enumWorkMode;
	tSysPara.enumScheduleEvent = ptSPRprtData->enumScheduleEvent;
	tSysPara.usTgtID = ptSPRprtData->usTgtID;
	tSysPara.tRadarArrayAttitude.dYaw = 0;//ptSPRprtData->tAttitude.dYaw;
	tSysPara.tRadarArrayAttitude.dPitch = 0;//ptSPRprtData->tAttitude.dPitch;
	tSysPara.tRadarArrayAttitude.dRoll = 0;//ptSPRprtData->tAttitude.dRoll;
	tSysPara.ucAntennaIdx = ptSPRprtData->ucAntennaIdx;
	tSysPara.enumBeamType = ptSPRprtData->enumBeamType;
	tSysPara.ucBeamCoordX = ptSPRprtData->ucBeamCoordX;
	tSysPara.ucBeamCoordY = ptSPRprtData->ucBeamCoordY;

	return 0;
}


INT32 CpyMsrPnt(SPReportDataInBuffInfo* ptSPRprtData, OneCPIMsrInfo& tOneCPIMsrPnt)
{
	/*点迹拷贝*/
	if(ptSPRprtData == NULL)
	{
		ERROR("invalid argument, 'ptSPRprtData == NULL'");
		return -1;
	}

	//tOneCPIMsrPnt.unCircleCnt = ptSPRprtData->unCPI / ptSPRprtData->usCPINumOneCircle;
	tOneCPIMsrPnt.unCPI = ptSPRprtData->unCPI;
	tOneCPIMsrPnt.tDate = ptSPRprtData->tDate;
	tOneCPIMsrPnt.dTime = ptSPRprtData->dTime;
	tOneCPIMsrPnt.dPRT = ptSPRprtData->dPRT;
	tOneCPIMsrPnt.unPulseNum = ptSPRprtData->unPulseNum;
	tOneCPIMsrPnt.unMsrPntNum = ptSPRprtData->unMsrNum;
	for(UINT16 i = 0; i < tOneCPIMsrPnt.unMsrPntNum; i++)
	{
		tOneCPIMsrPnt.atMsrPntArr[i].unIdx = ptSPRprtData->atMsrPnrArr[i].unIdx;
		tOneCPIMsrPnt.atMsrPntArr[i].dRng  = ptSPRprtData->atMsrPnrArr[i].fRng;
		tOneCPIMsrPnt.atMsrPntArr[i].dAzi  = ptSPRprtData->atMsrPnrArr[i].fAzi;
		tOneCPIMsrPnt.atMsrPntArr[i].dEle  = ptSPRprtData->atMsrPnrArr[i].fEle;
		tOneCPIMsrPnt.atMsrPntArr[i].dAlt  = ptSPRprtData->atMsrPnrArr[i].fAlt;
		tOneCPIMsrPnt.atMsrPntArr[i].dVr   = ptSPRprtData->atMsrPnrArr[i].fVr;
		tOneCPIMsrPnt.atMsrPntArr[i].dPRT  = ptSPRprtData->dPRT;
		tOneCPIMsrPnt.atMsrPntArr[i].dAmp  = ptSPRprtData->atMsrPnrArr[i].fAmp;
		tOneCPIMsrPnt.atMsrPntArr[i].dSNR  = ptSPRprtData->atMsrPnrArr[i].fSNR;
		bzero(&(tOneCPIMsrPnt.atMsrPntArr[i].tMsrStdDev), sizeof(MsrStdDevInfo));
		tOneCPIMsrPnt.atMsrPntArr[i].tDate   = ptSPRprtData->tDate;
		tOneCPIMsrPnt.atMsrPntArr[i].dTime = ptSPRprtData->dTime;
		tOneCPIMsrPnt.atMsrPntArr[i].unCPI = ptSPRprtData->unCPI;
		tOneCPIMsrPnt.atMsrPntArr[i].dAziMajor = ptSPRprtData->atMsrPnrArr[i].fAziMajor;
		tOneCPIMsrPnt.atMsrPntArr[i].dEleMajor = ptSPRprtData->atMsrPnrArr[i].fEleMajor;
		tOneCPIMsrPnt.atMsrPntArr[i].ucAntennaIdx = ptSPRprtData->ucAntennaIdx;
		tOneCPIMsrPnt.atMsrPntArr[i].enumBeamType = ptSPRprtData->enumBeamType;
		tOneCPIMsrPnt.atMsrPntArr[i].ucBeamCoordX = ptSPRprtData->ucBeamCoordX;
		tOneCPIMsrPnt.atMsrPntArr[i].ucBeamCoordY = ptSPRprtData->ucBeamCoordY;
		tOneCPIMsrPnt.atMsrPntArr[i].unRngCell = ptSPRprtData->atMsrPnrArr[i].unRngCell;
		tOneCPIMsrPnt.atMsrPntArr[i].unDplCell = ptSPRprtData->atMsrPnrArr[i].unDplCell;
		bzero(tOneCPIMsrPnt.atMsrPntArr[i].adRctMsr, sizeof(DOUBLE)*MSR_DIM);
		bzero(tOneCPIMsrPnt.atMsrPntArr[i].adRctMsrCov, sizeof(DOUBLE)*MSR_DIM*MSR_DIM);
		tOneCPIMsrPnt.atMsrPntArr[i].ucAssocFlag    = 0;
		tOneCPIMsrPnt.atMsrPntArr[i].ucCentroidFlag = 0;
		tOneCPIMsrPnt.atMsrPntArr[i].dConfidence    = ptSPRprtData->atMsrPnrArr[i].fConfidence;
		tOneCPIMsrPnt.atMsrPntArr[i].untar_type    = ptSPRprtData->atMsrPnrArr[i].untar_type;
	}

	return 0;
}

std::map<BeamInfo, SPReportDataInBuffInfo*, CompareBeam>::iterator GetIter1stofBeamCoordXInMap(std::map<BeamInfo, SPReportDataInBuffInfo*, CompareBeam>* pmDataMap, BeamInfo tBeam)
{
	//返回数据map中第一个满足方位指向满足要求的迭代器

	std::map<BeamInfo, SPReportDataInBuffInfo*, CompareBeam>::iterator iter = pmDataMap->begin();

	BOOL bFlag = false;

	for(; iter != pmDataMap->end(); iter++)
	{
		if(iter->first.ucBeamCoordX == tBeam.ucBeamCoordX)
		{
			bFlag = true;
			break;
		}
	}

	if(!bFlag)
	{
		//		WARN("no such BeamCoordX({}) in map, find failed", tBeam.ucBeamCoordX);
		iter = pmDataMap->end();
	}

	return iter;
}


INT32 CpyMsrPntOneCoordX(std::map<BeamInfo, SPReportDataInBuffInfo*, CompareBeam>* pmDataMap, BeamInfo tBeam, OneAziMajorMsrInfo& tOneAziMajorMsrPnt)
{
	//拷贝一个方位指向所有俯仰波束中的数据

	if(pmDataMap->empty())
	{
		ERROR("invalid argument, 'mDataMap' is empty");
		return -1;
	}

	//搜索指定方位数据在map中的位置
	auto iter = GetIter1stofBeamCoordXInMap(pmDataMap, tBeam);
	if(iter == pmDataMap->end())
	{
		ERROR("no such BeamCoordX({}) in map, copy failed", tBeam.ucBeamCoordX);
		return -2;
	}

	//拷贝数据
	tOneAziMajorMsrPnt.ucAntennaIdx = iter->second->ucAntennaIdx;
	tOneAziMajorMsrPnt.dTime = iter->second->dTime;
	tOneAziMajorMsrPnt.tDate = iter->second->tDate;
	tOneAziMajorMsrPnt.ucBeamCoordX = tBeam.ucBeamCoordX;
	tOneAziMajorMsrPnt.unMsrPntNum = 0;
	tOneAziMajorMsrPnt.enumEdgeTypeInData = MIDDLE;
	BOOL bFlag = false;

	for(; iter != pmDataMap->end(); iter++)
	{
		if(iter->first.ucBeamCoordX != tBeam.ucBeamCoordX)
			continue;

		//debug
		if(iter->second->unCPI == 27629+1380327)	
			INFO("debug");

		for(UINT16 i = 0; i < iter->second->unMsrNum; i++)
		{
			if(iter->second->atMsrPnrArr[i].ucCentroidFlag == 1) continue;

			if(tOneAziMajorMsrPnt.unMsrPntNum >= MAX_MSR_NUM_ONE_AZIMAJOR)
			{
				WARN("msr num exceeds the max num({}), discard the ramaining msr", MAX_MSR_NUM_ONE_AZIMAJOR);
				break;
			}

			tOneAziMajorMsrPnt.atMsrPntArr[tOneAziMajorMsrPnt.unMsrPntNum].unIdx = iter->second->atMsrPnrArr[i].unIdx;
			tOneAziMajorMsrPnt.atMsrPntArr[tOneAziMajorMsrPnt.unMsrPntNum].dRng = iter->second->atMsrPnrArr[i].fRng;
			tOneAziMajorMsrPnt.atMsrPntArr[tOneAziMajorMsrPnt.unMsrPntNum].dAzi = iter->second->atMsrPnrArr[i].fAzi;
			tOneAziMajorMsrPnt.atMsrPntArr[tOneAziMajorMsrPnt.unMsrPntNum].dEle = iter->second->atMsrPnrArr[i].fEle;
			tOneAziMajorMsrPnt.atMsrPntArr[tOneAziMajorMsrPnt.unMsrPntNum].dAlt = iter->second->atMsrPnrArr[i].fAlt;
			tOneAziMajorMsrPnt.atMsrPntArr[tOneAziMajorMsrPnt.unMsrPntNum].dVr  = iter->second->atMsrPnrArr[i].fVr;
			tOneAziMajorMsrPnt.atMsrPntArr[tOneAziMajorMsrPnt.unMsrPntNum].dPRT = iter->second->atMsrPnrArr[i].dPRT;
			tOneAziMajorMsrPnt.atMsrPntArr[tOneAziMajorMsrPnt.unMsrPntNum].dAmp = iter->second->atMsrPnrArr[i].fAmp;
			tOneAziMajorMsrPnt.atMsrPntArr[tOneAziMajorMsrPnt.unMsrPntNum].dSNR = iter->second->atMsrPnrArr[i].fSNR;
			bzero(&(tOneAziMajorMsrPnt.atMsrPntArr[tOneAziMajorMsrPnt.unMsrPntNum].tMsrStdDev), sizeof(MsrStdDevInfo));
			tOneAziMajorMsrPnt.atMsrPntArr[tOneAziMajorMsrPnt.unMsrPntNum].tDate = iter->second->atMsrPnrArr[i].tDate;
			tOneAziMajorMsrPnt.atMsrPntArr[tOneAziMajorMsrPnt.unMsrPntNum].dTime = iter->second->atMsrPnrArr[i].dTime;
			tOneAziMajorMsrPnt.atMsrPntArr[tOneAziMajorMsrPnt.unMsrPntNum].unCPI = iter->second->atMsrPnrArr[i].unCPI;
			tOneAziMajorMsrPnt.atMsrPntArr[tOneAziMajorMsrPnt.unMsrPntNum].dAziMajor = iter->second->atMsrPnrArr[i].fAziMajor;
			tOneAziMajorMsrPnt.atMsrPntArr[tOneAziMajorMsrPnt.unMsrPntNum].dEleMajor = iter->second->atMsrPnrArr[i].fEleMajor;
			tOneAziMajorMsrPnt.atMsrPntArr[tOneAziMajorMsrPnt.unMsrPntNum].ucAntennaIdx = iter->second->ucAntennaIdx;
			tOneAziMajorMsrPnt.atMsrPntArr[tOneAziMajorMsrPnt.unMsrPntNum].enumBeamType = iter->second->enumBeamType;
			tOneAziMajorMsrPnt.atMsrPntArr[tOneAziMajorMsrPnt.unMsrPntNum].ucBeamCoordX = iter->second->ucBeamCoordX;
			tOneAziMajorMsrPnt.atMsrPntArr[tOneAziMajorMsrPnt.unMsrPntNum].ucBeamCoordY = iter->second->ucBeamCoordY;
			tOneAziMajorMsrPnt.atMsrPntArr[tOneAziMajorMsrPnt.unMsrPntNum].unRngCell = iter->second->atMsrPnrArr[i].unRngCell;
			tOneAziMajorMsrPnt.atMsrPntArr[tOneAziMajorMsrPnt.unMsrPntNum].unDplCell = iter->second->atMsrPnrArr[i].unDplCell;
			bzero(tOneAziMajorMsrPnt.atMsrPntArr[tOneAziMajorMsrPnt.unMsrPntNum].adRctMsr, sizeof(DOUBLE)*MSR_DIM);
			bzero(tOneAziMajorMsrPnt.atMsrPntArr[tOneAziMajorMsrPnt.unMsrPntNum].adRctMsrCov, sizeof(DOUBLE)*MSR_DIM*MSR_DIM);
			tOneAziMajorMsrPnt.atMsrPntArr[tOneAziMajorMsrPnt.unMsrPntNum].ucAssocFlag = 0;
			tOneAziMajorMsrPnt.atMsrPntArr[tOneAziMajorMsrPnt.unMsrPntNum].ucCentroidFlag = 0;
			tOneAziMajorMsrPnt.atMsrPntArr[tOneAziMajorMsrPnt.unMsrPntNum].dConfidence = iter->second->atMsrPnrArr[i].fConfidence;
			tOneAziMajorMsrPnt.atMsrPntArr[tOneAziMajorMsrPnt.unMsrPntNum].untar_type  = iter->second->atMsrPnrArr[i].untar_type;

			//记录当前方位坐标数据中包含的边界信息
			if (!bFlag && abs(iter->second->enumBeamType) == 1)
			{
				tOneAziMajorMsrPnt.enumEdgeTypeInData = iter->second->enumBeamType;
				bFlag = true;
			}

			tOneAziMajorMsrPnt.unMsrPntNum++;
		}
	}

	return 0;
}


INT32 GetClusterCenter(std::vector<UINT32>& vunClusterIdx, MsrPntInfo atMsrPnt[], enum CentroidFashionInfo enumCentroidFashion, MsrPntInfo& tClusterCentor)
{
	/*计算凝聚簇的质心*/

	if(vunClusterIdx.empty())
	{
		ERROR("invalid input argument 'vunClusterIdx', it cant't be empty");
		return -1;
	}

	DOUBLE dNrm = 0, dAmp = 0, dAmpMax = -1 * INF;
	DOUBLE dDistCentor = 0, dAziCentor = 0, dEleCentor = 0, dAltCentor = 0, dVrCentor = 0, dTimeCentor = 0;
	UINT32 unMsrIdxAmpMax = 0;

	switch(enumCentroidFashion)
	{
	case MAX_AMP:
		for (std::vector<UINT32>::iterator iter = vunClusterIdx.begin(); iter != vunClusterIdx.end(); iter++)
		{
			if(atMsrPnt[*(iter)].dAmp > dAmpMax)
			{
				dAmpMax = atMsrPnt[*iter].dAmp;
				unMsrIdxAmpMax = *iter;
			}
		}
		tClusterCentor = atMsrPnt[unMsrIdxAmpMax];
		break;

	case AVERAGE:
		for (std::vector<UINT32>::iterator iter = vunClusterIdx.begin(); iter != vunClusterIdx.end(); iter++)
		{
			//归一化系数，幅度之和
			dAmp = atMsrPnt[*(iter)].dAmp;
			dNrm += dAmp;

			//当前点迹贡献
			dDistCentor += atMsrPnt[*(iter)].dRng * dAmp;
			dAziCentor += atMsrPnt[*(iter)].dAzi * dAmp;
			dEleCentor += atMsrPnt[*(iter)].dEle * dAmp;
			dAltCentor += atMsrPnt[*(iter)].dAlt*dAmp;
			dVrCentor += atMsrPnt[*(iter)].dVr * dAmp;
			dTimeCentor += atMsrPnt[*(iter)].dTime * dAmp;

			//记录当前幅度最大值
			if (atMsrPnt[*(iter)].dAmp > dAmpMax)
			{
				dAmpMax = atMsrPnt[*(iter)].dAmp;
				unMsrIdxAmpMax = *(iter);
			}
		}

		tClusterCentor = atMsrPnt[unMsrIdxAmpMax];
		tClusterCentor.dRng = dDistCentor / dNrm;
		tClusterCentor.dAzi  = dAziCentor / dNrm;
		tClusterCentor.dEle  = dEleCentor / dNrm;
		tClusterCentor.dAlt  = dAltCentor / dNrm;
		tClusterCentor.dVr   = dVrCentor / dNrm;
		tClusterCentor.dTime = dTimeCentor / dNrm;
		break;

	default:
		//ERROR("invalid argument, 'enumCentroidFashion'=={}, it can only be 'MAX_AMP'({}) or 'AVERAGE'({})", enumCentroidFashion, MAX_AMP, AVERAGE);
		break;
	}

	return 0;
}

INT32 GetMsrPntStdDev(DOUBLE dBand, DOUBLE dAzi3dB, DOUBLE dEle3dB, DOUBLE dFc, DOUBLE dPRT, UINT32 unPluseNum, DOUBLE dSNRdB, enum MsrStdEstFashionInfo enumMsrStdEstFashion, MsrStdDevInfo& tMsrPntStdDev)
{
	/*计算量测点测量精度*/

	if (dBand <= 0 || dAzi3dB <= 0 || dEle3dB <= 0 || dFc <= 0 || dPRT <= 0 || unPluseNum == 0 || dSNRdB <= 0)
	{
		ERROR("invalid argument, arguments must be greater than 0");
		return -1;
	}

	DOUBLE dCnst = sqrt(2*pow(10, 0.1 * dSNRdB));

	DOUBLE ddR = CVEL / 2.0 / dBand;
	DOUBLE dLambda = CVEL / dFc;

	DOUBLE dResdnceTime = dPRT *unPluseNum;
	DOUBLE dVdplUint = CVEL / dFc / dPRT / 2 / unPluseNum;//一个多普勒通道对应的多普勒速度

	switch(enumMsrStdEstFashion)
	{
	case STATIC:
		tMsrPntStdDev.dSgmR = RNG_STD;
		tMsrPntStdDev.dSgmAzi = AZI_STD;
		tMsrPntStdDev.dSgmEle = ELE_STD;
		tMsrPntStdDev.dSgmVr = dVdplUint*RATIO_OF_VR_STD_TO_DOPPLER_UNIT;
		break;

	case DYNAMIC:
		tMsrPntStdDev.dSgmR = ddR / dCnst;
		tMsrPntStdDev.dSgmAzi = dAzi3dB / dCnst;
		tMsrPntStdDev.dSgmEle = dEle3dB / dCnst;
		tMsrPntStdDev.dSgmVr = dLambda / 2.0 / dResdnceTime / dCnst;
		break;

	default:
		//ERROR("invalid arg 'enumMsrStdEstFashion'({}), it can only be STATIC({}) or DYNAMIC({})", enumMsrStdEstFashion, STATIC, DYNAMIC);
		break;
	}

	return 0;
}

INT32 CentroidMsrPnt(OneCPIMsrInfo& tOneFrmMsr, DOUBLE dSNRRef, DOUBLE dB, DOUBLE dAzi3dB, DOUBLE dEle3dB, DOUBLE dFc, DOUBLE dPRT, UINT32 unPulseNum, enum MsrStdEstFashionInfo enumMsrStdEstFashion, AlgorithmParaInfo tAlgrthmPara)
{
	/*波束内点迹凝聚--BFS*/

	if (dB <= 0 || dAzi3dB <= 0 || dEle3dB <= 0 || dFc <= 0 || dPRT <= 0 || unPulseNum == 0)
	{
		ERROR("invalid argument");
		return -1;
	}

	if (tOneFrmMsr.unMsrPntNum == 0)
	{
		return 1;
	}

	//计算参考测量精度
	MsrStdDevInfo tMsrStdDevRef = { 0 };
	GetMsrPntStdDev(dB, dAzi3dB, dEle3dB, dFc, dPRT, unPulseNum, dSNRRef, enumMsrStdEstFashion, tMsrStdDevRef);

	//凝聚标记辅助数组
	UINT32* aunCentroidFlag = new UINT32[(tOneFrmMsr.unMsrPntNum)];
	memset(aunCentroidFlag, 0, sizeof(UINT32) * tOneFrmMsr.unMsrPntNum);

	/*遍历点迹数组*/
	UINT32 unMsrPntNumCentroid = 0;
	for (UINT32 i = 0; i < tOneFrmMsr.unMsrPntNum; i++)
	{
		if (aunCentroidFlag[i] == 1) continue;

		//初始化当前簇的索引集
		std::vector<UINT32> vunClusterIdx;

		//凝聚标记
		aunCentroidFlag[i] = 1;
		vunClusterIdx.emplace_back(i);
		tOneFrmMsr.atMsrPntArr[i].ucCentroidFlag = 1;

		//判断是否满足凝聚条件
		for (UINT32 j = i + 1; j < tOneFrmMsr.unMsrPntNum; j++)
		{
			if (aunCentroidFlag[j] == 1)
				continue;
			if (fabs(tOneFrmMsr.atMsrPntArr[i].dVr - tOneFrmMsr.atMsrPntArr[j].dVr) > tAlgrthmPara.dVrCentroidTH)
				continue;
			if (fabs(tOneFrmMsr.atMsrPntArr[i].dRng - tOneFrmMsr.atMsrPntArr[j].dRng) > tAlgrthmPara.dRngCentroidTH)
				continue;
			if (fabs(tOneFrmMsr.atMsrPntArr[i].dAzi - tOneFrmMsr.atMsrPntArr[j].dAzi) > tAlgrthmPara.dAziCentroidTH)
				continue;
			if (fabs(tOneFrmMsr.atMsrPntArr[i].dEle - tOneFrmMsr.atMsrPntArr[j].dEle) > tAlgrthmPara.dEleCentroidTH)
				continue;

			aunCentroidFlag[j] = 1;
			vunClusterIdx.emplace_back(j);
			tOneFrmMsr.atMsrPntArr[j].ucCentroidFlag = 1;
		}

		//凝聚后点迹原地存储
		GetClusterCenter(vunClusterIdx, tOneFrmMsr.atMsrPntArr, MAX_AMP, tOneFrmMsr.atMsrPntArr[unMsrPntNumCentroid]);

		//点迹数量计数
		unMsrPntNumCentroid++;
	}
	tOneFrmMsr.unMsrPntNum = unMsrPntNumCentroid;

	delete[] aunCentroidFlag;
	return 0;
}


INT32 RAE2XYZUnBias(MsrPntInfo& tMsrPnt, UINT32 unMsrDim)
{
	/*量测点从RAE坐标无偏转换至XYZ坐标*/

	DOUBLE fRng = tMsrPnt.dRng;
	DOUBLE fEle = tMsrPnt.dEle;
	DOUBLE fAzix = 90 - tMsrPnt.dAzi;

	DOUBLE fCnst = pow(PI / 180.0, 2);

	DOUBLE fSgmEle2 = pow(tMsrPnt.tMsrStdDev.dSgmEle, 2);
	DOUBLE fCoeffEle = exp(-0.5 * fSgmEle2 * fCnst);
	DOUBLE fCoeff2Ele = exp(-2 * fSgmEle2 * fCnst);

	DOUBLE fSgmAzi2 = pow(tMsrPnt.tMsrStdDev.dSgmAzi, 2);
	DOUBLE fCoeffAzi = exp(-0.5 * fSgmAzi2 * fCnst);
	DOUBLE fCoeff2Azi = exp(-2 * fSgmAzi2 * fCnst);

	DOUBLE fx = 0, fy = 0, fz = 0;
	DOUBLE CovXX = 0, CovXY = 0, CovXZ = 0, CovYY = 0, CovYZ = 0, CovZZ = 0;

	switch (unMsrDim)
	{
	case 2:
		fx = fRng * cosd(fEle) * cosd(fAzix) * fCoeffEle * fCoeffAzi;
		fy = fRng * cosd(fEle) * sind(fAzix) * fCoeffEle * fCoeffAzi;
		tMsrPnt.adRctMsr[0] = fx;
		tMsrPnt.adRctMsr[1] = fy;

		fCnst = (pow(fRng, 2) + pow(tMsrPnt.tMsrStdDev.dSgmR, 2));

		CovXX = -1 * fx * fx + fCnst * 0.25 * (1 + fCoeff2Azi * cosd(2 * fAzix)) * (1 + fCoeff2Ele * cosd(2 * fEle));
		CovXY = -1 * fx * fy + fCnst * 0.25 * fCoeff2Azi * sind(2 * fAzix) * (1 + fCoeff2Ele * cosd(2 * fEle));
		CovYY = -1 * fy * fy + fCnst * 0.25 * (1 - fCoeff2Azi * cosd(2 * fAzix)) * (1 + fCoeff2Ele * cosd(2 * fEle));

		tMsrPnt.adRctMsrCov[0] = CovXX; tMsrPnt.adRctMsrCov[1] = CovXY;
		tMsrPnt.adRctMsrCov[2] = CovXY; tMsrPnt.adRctMsrCov[3] = CovYY;
		break;

	case 3:
		fx = fRng * cosd(fEle) * cosd(fAzix) * pow(fCoeffEle,-1) * pow(fCoeffAzi,-1);
		fy = fRng * cosd(fEle) * sind(fAzix) * pow(fCoeffEle,-1) * pow(fCoeffAzi,-1);
		fz = fRng * sind(fEle) * pow(fCoeffEle,-1);
		tMsrPnt.adRctMsr[0] = fx;
		tMsrPnt.adRctMsr[1] = fy;
		tMsrPnt.adRctMsr[2] = fz;

		fCnst = (pow(fRng, 2) + pow(tMsrPnt.tMsrStdDev.dSgmR, 2));

		CovXX = fx*fx - 2*pow(fRng*cosd(fEle)*cosd(fAzix),2) + fCnst * 0.25 * (1 + fCoeff2Azi * cosd(2 * fAzix)) * (1 + fCoeff2Ele * cosd(2 * fEle));
		CovXY = fx*fy - 2*(fRng*fRng*cosd(fEle)*cosd(fEle)*sind(fAzix)*cosd(fAzix))+ fCnst * 0.25 * fCoeff2Azi * sind(2 * fAzix) * (1 + fCoeff2Ele * cosd(2 * fEle));
		CovYY = fy*fy - 2*pow(fRng*cosd(fEle)*sind(fAzix),2) + fCnst * 0.25 * (1 - fCoeff2Azi * cosd(2 * fAzix)) * (1 + fCoeff2Ele * cosd(2 * fEle));
		CovXZ = (pow(fCoeffAzi,-1)*pow(fCoeffEle,-2)-pow(fCoeffAzi,-1)-fCoeffAzi)*(fRng*fRng*cosd(fAzix)*sind(fEle)*cosd(fEle)) + fCnst * 0.50 * fCoeffAzi * fCoeff2Ele * cosd(fAzix) * sind(2 * fEle);
		CovYZ = (pow(fCoeffAzi,-1)*pow(fCoeffEle,-2)-pow(fCoeffAzi,-1)-fCoeffAzi)*(fRng*fRng*sind(fAzix)*sind(fEle)*cosd(fEle)) + fCnst * 0.50 * fCoeffAzi * fCoeff2Ele * sind(fAzix) * sind(2 * fEle);
		CovZZ = fz*fz- 2*pow(fRng*sind(fEle),2) + fCnst * 0.50 * (1 - fCoeff2Ele * cosd(2 * fEle));

		tMsrPnt.adRctMsrCov[0] = CovXX; tMsrPnt.adRctMsrCov[1] = CovXY; tMsrPnt.adRctMsrCov[2] = CovXZ;
		tMsrPnt.adRctMsrCov[3] = CovXY; tMsrPnt.adRctMsrCov[4] = CovYY; tMsrPnt.adRctMsrCov[5] = CovYZ;
		tMsrPnt.adRctMsrCov[6] = CovXZ; tMsrPnt.adRctMsrCov[7] = CovYZ; tMsrPnt.adRctMsrCov[8] = CovZZ;
		break;

	default:
		ERROR("invalid argument, 'unMsrDim({})' must be 2 or 3", unMsrDim);
		break;
	}

	return 0;
}


INT32 ConvertMsrPnt(SysParaInfo* ptSysPara, OneCircleMsrInfo& tOneCircleMsrPnt)
{
	/*量测转换*/
	for(UINT32 i = 0; i < tOneCircleMsrPnt.unMsrPntNum; i++)
	{
		GetMsrPntStdDev(ptSysPara->dB, ptSysPara->dAzi3dB, ptSysPara->dEle3dB, ptSysPara->dFc, tOneCircleMsrPnt.atMsrPntArr[i].dPRT, ptSysPara->usPulseNum, tOneCircleMsrPnt.atMsrPntArr[i].dSNR, STATIC, tOneCircleMsrPnt.atMsrPntArr[i].tMsrStdDev);

		RAE2XYZUnBias(tOneCircleMsrPnt.atMsrPntArr[i], MSR_DIM);
	}

	return 0;
}

INT32 ConvertMsrPntOneCPI(SysParaInfo* ptSysPara, OneCPIMsrInfo& tOneCPIMsrPnt)
{
	/*量测转换-一个CPI*/
	for(UINT32 i = 0; i < tOneCPIMsrPnt.unMsrPntNum; i++)
	{
		GetMsrPntStdDev(ptSysPara->dB, ptSysPara->dAzi3dB, ptSysPara->dEle3dB, ptSysPara->dFc, tOneCPIMsrPnt.atMsrPntArr[i].dPRT, ptSysPara->usPulseNum, tOneCPIMsrPnt.atMsrPntArr[i].dSNR, STATIC, tOneCPIMsrPnt.atMsrPntArr[i].tMsrStdDev);

		RAE2XYZUnBias(tOneCPIMsrPnt.atMsrPntArr[i], MSR_DIM);
	}

	return 0;
}

INT32 ConvertMsrPntOneAziMajor(SysParaInfo* ptSysPara, OneAziMajorMsrInfo& tOneAziMajorMsrPnt)
{
	/*量测转换-一个CPI*/
	for(UINT32 i = 0; i < tOneAziMajorMsrPnt.unMsrPntNum; i++)
	{
		GetMsrPntStdDev(ptSysPara->dB, ptSysPara->dAzi3dB, ptSysPara->dEle3dB, ptSysPara->dFc, tOneAziMajorMsrPnt.atMsrPntArr[i].dPRT, ptSysPara->usPulseNum, tOneAziMajorMsrPnt.atMsrPntArr[i].dSNR, STATIC, tOneAziMajorMsrPnt.atMsrPntArr[i].tMsrStdDev);

		RAE2XYZUnBias(tOneAziMajorMsrPnt.atMsrPntArr[i], MSR_DIM);
	}

	return 0;
}


BOOL TmpTrackAssocMsrPntRAE(std::list<TmpTrackInfo>::iterator iter, MsrPntInfo* ptMsrPnt)
{
	/*暂态航迹RAE与点迹关联*/

	if(ptMsrPnt == NULL)
	{
		ERROR("invalid argument, 'ptMsrPnt == NULL'");
		return false;
	}

	BOOL bFlag = fabs(iter->qtMsrArr[iter->cIdxLastNonEmpty].dVr - (-10)) <= 5 && fabs(iter->qtMsrArr[iter->cIdxLastNonEmpty].dRng - 1000) <= 0 
				&&iter->qtMsrArr[iter->cIdxLastNonEmpty].dAzi > 10 && iter->qtMsrArr[iter->cIdxLastNonEmpty].dAzi < 20 && ptMsrPnt->dRng < 1e3 
				 && fabs(ptMsrPnt->dVr - (-10)) <= 5 && ptMsrPnt->dAzi > 10 && ptMsrPnt->dAzi < 20;

	DOUBLE dt = ptMsrPnt->dTime - iter->dLstUpdTime;
	if(dt <= 0 && bFlag)
	{
		ERROR("time error, measure's time({:.3f}) must be greater than tmp track time({:.3f})", ptMsrPnt->dTime, iter->dLstUpdTime);
		return false;
	}	

	if(bFlag)
		INFO("debug");

	// fabs(iter->qtMsrArr[iter->cIdxLastNonEmpty].dVr) >= 12 && fabs(iter->qtMsrArr[iter->cIdxLastNonEmpty].dVr) <= 15 && fabs(iter->qtMsrArr[iter->cIdxLastNonEmpty].dAzi - 274) <= 2
	//  && fabs(ptMsrPnt->dVr) >= 12 && fabs((ptMsrPnt->dVr))<= 15 && fabs((ptMsrPnt->dAzi - 274)) <= 2;//false;

	//多普勒速度约束
	if( fabs(ptMsrPnt->dVr) < g_tAlgorithmPara.dVrMin )
		return false;

	//置信度约束
	// if (ptMsrPnt->dConfidence < g_tAlgorithmPara.dConfidenceTH)
	// 	return false;

	//最大最小速度约束
	DOUBLE dDeltaRng = fabs(ptMsrPnt->dRng - iter->qtMsrArr.at(iter->cIdxLastNonEmpty).dRng);
	//	if( dDeltaRng < g_tAlgorithmPara.dVrMin*dt || dDeltaRng > (g_tAlgorithmPara.dVrMax*dt+3*RNG_STD))
	if( dDeltaRng > (g_tAlgorithmPara.dVrMax*dt+3*RNG_STD))
	{
		//debug
		if(bFlag)
			INFO("[1] dDeltaRng:{:.4f}, VrMin*dt:{:.4f}, VrMax*dt:{:.4f}", dDeltaRng, g_tAlgorithmPara.dVrMin*dt, g_tAlgorithmPara.dVrMax*dt);
		return false;
	}


	//距离微分与多普勒速度一致性约束
	DOUBLE dRngDiffVr = -1*(ptMsrPnt->dRng - iter->qtMsrArr.at(iter->cIdxLastNonEmpty).dRng) / dt;
	DOUBLE dDeltaVr = fabs(ptMsrPnt->dVr - dRngDiffVr);
	DOUBLE dDeltaVrTH = sqrt( pow(ptMsrPnt->tMsrStdDev.dSgmVr, 2) + ( pow(ptMsrPnt->tMsrStdDev.dSgmR, 2)+pow(iter->qtMsrArr.at(iter->cIdxLastNonEmpty).tMsrStdDev.dSgmR, 2) )/pow(dt, 2));
	if(dDeltaVr > 3*dDeltaVrTH)
	{
		//debug
		if(bFlag)
			INFO("[2] dRngDiffVr:{:.4f}, ptMsrPnt->dVr:{:.4f}, 3*dDeltaVrTH:{:.4f}", dRngDiffVr, ptMsrPnt->dVr, 3*dDeltaVrTH);
		return false;
	}


	//多普勒速度抖动约束
	DOUBLE dInitVrAssocTH = 0;
	dDeltaVr = fabs(ptMsrPnt->dVr - iter->qtMsrArr.at(iter->cIdxLastNonEmpty).dVr);
	if (iter->enumInitMode == QUICK)
		dInitVrAssocTH = g_tAlgorithmPara.dQuickInitVrAssocTH;
	else if (iter->enumInitMode == STANDARD)
		dInitVrAssocTH = g_tAlgorithmPara.dQuickInitVrAssocTH;
	else if (iter->enumInitMode == SLOW)
		dInitVrAssocTH = g_tAlgorithmPara.dSlowInitVrAssocTH;
	else {
		ERROR("invalid argument, 'iter->enumInitMode'{}, it must {} or {}", iter->enumInitMode, QUICK, SLOW);
		exit(1);
	}
	if(dDeltaVr > dInitVrAssocTH)
	{
		//debug
		if(bFlag)
			INFO("[3] dDeltaVr:{:.4f}, VrTH:{:.4f}", dDeltaVr, dInitVrAssocTH);
		return false;
	}


	//方位角抖动约束
	DOUBLE dDeltaAzi = fabs(ptMsrPnt->dAzi - iter->qtMsrArr.at(iter->cIdxLastNonEmpty).dAzi);
	if (dDeltaAzi >180 )
		dDeltaAzi = 360 - dDeltaAzi;
	DOUBLE dAziAssocTH = 0;
	if (iter->enumInitMode == QUICK)
		dAziAssocTH = g_tAlgorithmPara.dQuickInitAziAssocTH;
	else if (iter->enumInitMode == STANDARD)
		dAziAssocTH = g_tAlgorithmPara.dQuickInitAziAssocTH;
	else if (iter->enumInitMode == SLOW)
		dAziAssocTH = g_tAlgorithmPara.dSlowInitAziAssocTH;
	else {
		ERROR("invalid argument, 'iter->enumInitMode'{}, it must {} or {}", iter->enumInitMode, QUICK, SLOW);
		exit(1);
	}
	if(dDeltaAzi > dAziAssocTH)
	{
		//debug
		if(bFlag)
			INFO("[4] dDeltaAzi:{:.4f}, AziTH:{:.4f}",dDeltaAzi,dAziAssocTH);
		return false;
	}

	//俯仰角抖动约束
	DOUBLE dDeltaEle = fabs(ptMsrPnt->dEle - iter->qtMsrArr.at(iter->cIdxLastNonEmpty).dEle);
	DOUBLE dDeltaAlt = fabs(ptMsrPnt->dAlt - iter->qtMsrArr.at(iter->cIdxLastNonEmpty).dAlt);
	if(dDeltaEle > g_tAlgorithmPara.dEleAssocTH && dDeltaAlt > g_tAlgorithmPara.dAltAssocTH)
	{
		//debug
		if(bFlag)
			INFO("(5) ID:{} dDeltaEle:{:.4f}, EleTH:{:.4f}, dDeltaAlt:{:.4f}, AltTH:{:.4f}",
				iter->usPseudoTrackID, dDeltaEle, g_tAlgorithmPara.dEleAssocTH, dDeltaAlt, g_tAlgorithmPara.dAltAssocTH);
		return false;
	}

	//航向角约束
	if(iter->usPntNum > 1)
	{
		DOUBLE* adPosDiffTrack = new DOUBLE [MSR_DIM];
		memcpy(adPosDiffTrack, iter->qtMsrArr.at(iter->cIdxLastNonEmpty).adRctMsr, sizeof(DOUBLE)*MSR_DIM);
		cblas_daxpy(MSR_DIM, -1, iter->qtMsrArr.at(iter->cIdxLast2NonEmpty).adRctMsr, 1, adPosDiffTrack, 1);

		DOUBLE* adPosDiffPnt = new DOUBLE [MSR_DIM];
		memcpy(adPosDiffPnt, ptMsrPnt->adRctMsr, sizeof(DOUBLE)*MSR_DIM);
		cblas_daxpy(MSR_DIM, -1, iter->qtMsrArr.at(iter->cIdxLastNonEmpty).adRctMsr, 1, adPosDiffPnt, 1);

		DOUBLE ddot = cblas_ddot(MSR_DIM, adPosDiffTrack, 1, adPosDiffPnt, 1);
		DOUBLE dNormDiffTack = cblas_dnrm2(MSR_DIM, adPosDiffTrack, 1);
		DOUBLE dNormDiffPnt = cblas_dnrm2(MSR_DIM, adPosDiffPnt, 1);
		DOUBLE dAngle = acosd(ddot / dNormDiffTack / dNormDiffPnt);

		delete [] adPosDiffTrack;
		delete [] adPosDiffPnt;

		if(dAngle > g_tAlgorithmPara.dDefAngleForTmpTrackTH)
		{
			//debug
//			if(bFlag)
//				INFO("[6] dAngle:{:.4f}, ANGLE_TH:{}", dAngle, g_tAlgorithmPara.dDefAngleForTmpTrackTH);
			return false;
		}
	}

	return true;
}

DOUBLE get_cross_range(double dv, double dt, double dr)
{
	//计算横向距离对应的角度

	double dcross_range = 0;

	if (dr <= 0)
	{
		ERROR("invalid argument, 'dr==0'");
		return dcross_range;
	}

	dcross_range = (dv * dt / dr) / PI * 180.0;

	return dcross_range;
}

BOOL TgtTrackAssocMsrPntRAE(std::list<TgtTrackPntInfo>::iterator iter, MsrPntInfo* ptMsrPnt)
{
	/*目标航迹RAE与点迹关联*/

	if(ptMsrPnt == NULL)
	{
		ERROR("invalid argument, 'ptMsrPnt == NULL'");
		return false;
	}

	DOUBLE dt = ptMsrPnt->dTime - iter->dLstUpdTime;
	if(dt <= 0)
	{
		// ERROR("time error, measure's time({}) must be greater than target track(ID:{}) time({})", ptMsrPnt->dTime, iter->usTrackID, iter->dLstUpdTime);
		return false;
	}


	BOOL bFlag = iter->usTrackID == 19 && ptMsrPnt->unCPI == 1645305;
	if(bFlag)
		INFO("debug");
	
	// fabs(iter->tRAEest.dVrEst) >= 9 && fabs(iter->tRAEest.dVrEst) <= 15 && fabs(iter->tRAEest.dAziEst - (-86)) < 5  && fabs(ptMsrPnt->dVr) >= 9 && fabs((ptMsrPnt->dVr))<= 15 && fabs(ptMsrPnt->dAzi - 274) < 5;

	//最大最小速度约束
	DOUBLE dDeltaRng = fabs(ptMsrPnt->dRng - iter->tRAEest.dRngEst);
	if( dDeltaRng > (g_tAlgorithmPara.dVrMax*dt+3*RNG_STD) || dDeltaRng < g_tAlgorithmPara.dVrMin*dt)
		//	if( dDeltaRng < g_tAlgorithmPara.dVrMin*dt || dDeltaRng > g_tAlgorithmPara.dVrMax*dt)
	{
		//debug
		if(bFlag)
			INFO("(1) ID:{} dDeltaRng:{:.2f}, VrMax*dt:{:.2f}, dt:{:.4f}, track_range:{:.2f}, pnt_range:{:.2f}",
					iter->usTrackID, dDeltaRng, g_tAlgorithmPara.dVrMax*dt, dt, iter->tRAEest.dRngEst, ptMsrPnt->dRng);
		return false;
	}

	//距离微分与多普勒速度一致性约束
	DOUBLE dRngDiffVr = -1 * (ptMsrPnt->dRng - iter->tRAEest.dRngEst) / dt;
	DOUBLE dDeltaVr0 = fabs(ptMsrPnt->dVr - dRngDiffVr);
	DOUBLE dDeltaVrTH = sqrt(pow(VR_STD, 2) + (2 * pow(RNG_STD, 2)) / pow(dt, 2));
	if (dDeltaVr0 > 3 * dDeltaVrTH)
	{
		// debug
		if(bFlag)
			INFO("(2) ID:{}, dRngDiffVr:{:.2f}, 3*dDeltaVrTH:{:.2f}", iter->usTrackID, dRngDiffVr, 3 * dDeltaVrTH);
		return false;
	}

	//多普勒速度抖动约束
	DOUBLE dDeltaVr = fabs(ptMsrPnt->dVr - iter->tRAEest.dVrEst);
	if(dDeltaVr > g_tAlgorithmPara.dQuickInitVrAssocTH)
	{
		//debug
		if(bFlag)
			INFO("(3)  ID:{} dDeltaVr:{:.2f}, VrTH:{:.2f}, PntVr:{:.2f}, TrackVr:{:.2f}",
					iter->usTrackID, dDeltaVr, g_tAlgorithmPara.dQuickInitVrAssocTH, ptMsrPnt->dVr, iter->tRAEest.dVrEst);
		return false;
	}

	//方位角抖动约束
	DOUBLE dDeltaAzi = fabs(ptMsrPnt->dAzi - iter->tRAEest.dAziEst);
	if (dDeltaAzi > 180)
		dDeltaAzi = 360 - dDeltaAzi;
	double dcross_range_max = get_cross_range(g_tAlgorithmPara.dVrMax, dt, ptMsrPnt->dRng);
	if(dDeltaAzi > g_tAlgorithmPara.dQuickInitAziAssocTH || dDeltaAzi > dcross_range_max)
	{
		if(bFlag)
			INFO("(4) ID:{} dDeltaAzi:{:.2f}, AziTH:{:.2f}, cross_range_max:{:.2f}",
					iter->usTrackID, dDeltaAzi,g_tAlgorithmPara.dQuickInitAziAssocTH, dcross_range_max);
		return false;
	}

	//俯仰角抖动约束
	DOUBLE dDeltaEle = fabs(ptMsrPnt->dEle - iter->tRAEest.dEleEst);
	DOUBLE dDeltaAlt = fabs(ptMsrPnt->dAlt - iter->tRAEest.dAltEst);
	if(dDeltaEle > g_tAlgorithmPara.dEleAssocTH && dDeltaAlt > g_tAlgorithmPara.dAltAssocTH)
	{
		if(bFlag)
			INFO("(5) ID:{} dDeltaEle:{:.2f}, EleTH:{:.2f}, dDeltaAlt:{:.2f}, AltTH:{:.2f}",
					iter->usTrackID, dDeltaEle, g_tAlgorithmPara.dEleAssocTH, dDeltaAlt, g_tAlgorithmPara.dAltAssocTH);
		return false;
	}

	//航向角约束
	DOUBLE* adPosDiffPnt = new DOUBLE [MSR_DIM];
	DOUBLE* adTrackDiff = new DOUBLE [MSR_DIM];
	DOUBLE* adTrackPos = new DOUBLE [MSR_DIM];

	adTrackDiff[0] = iter->adXest[0]-iter->adXestLast2[0];
	adTrackDiff[1] = iter->adXest[3]-iter->adXestLast2[3];
	adTrackDiff[2] = iter->adXest[6]-iter->adXestLast2[6];

	memcpy(adPosDiffPnt, ptMsrPnt->adRctMsr, sizeof(DOUBLE)*MSR_DIM);
	adTrackPos[0] = iter->adXest[0]; adTrackPos[1] = iter->adXest[3]; adTrackPos[2] = iter->adXest[6];
	cblas_daxpy(MSR_DIM, -1, adTrackPos, 1, adPosDiffPnt, 1);

	DOUBLE ddot = cblas_ddot(MSR_DIM-1, adTrackDiff, 1, adPosDiffPnt, 1);
	DOUBLE dNormTrackDiff = cblas_dnrm2(MSR_DIM-1, adTrackDiff, 1);
	DOUBLE dNormDiffPnt = cblas_dnrm2(MSR_DIM-1, adPosDiffPnt, 1);
	DOUBLE dAngle = acosd(ddot / dNormTrackDiff / dNormDiffPnt);

	delete [] adTrackDiff;
	delete [] adPosDiffPnt;
	delete [] adTrackPos;

	if(dAngle > g_tAlgorithmPara.dDefAngleForTgtTrackTH)
	{
		//debug
		if(bFlag)
			INFO("(6) ID:{} dAngle:{:.2f}, ANGLETH:{}", iter->usTrackID, dAngle, g_tAlgorithmPara.dDefAngleForTgtTrackTH);
		return false;

	}

	//多普勒速度约束
	//	if( fabs(ptMsrPnt->dVr) < g_tAlgorithmPara.dVrMin )
	//	{
	//		//debug
	//		if(bFlag)
	//			INFO("(7) ID:{} abs(Vr):{:.2f}, VrTH:{}", iter->usTrackID, ptMsrPnt->dVr,  g_tAlgorithmPara.dVrMin);
	//		return false;
	//	}

	return true;
}

INT32 GetDiffStateCov(MsrPntInfo* pMsrPntLast, MsrPntInfo* pMsrPntLast2, DOUBLE adP0[])
{
	/*计算差分初始化目标状态的协方差矩阵*/

	if(pMsrPntLast == NULL || pMsrPntLast2 == NULL)
	{
		ERROR("Input argument 'pMsrPntLast' or 'pMsrPntLsst2' is NULL");
		return 1;
	}

	DOUBLE fdt = pMsrPntLast->dTime - pMsrPntLast2->dTime;

	DOUBLE adP0Mat[STATE_DIM][STATE_DIM];
	bzero(adP0Mat, sizeof(adP0Mat));

	//位置向量自协方差
	UINT32 unCnt = 0;
	for (UINT32 i = 0; i < STATE_DIM; i += 3)
	{
		for (UINT32 j = 0; j < STATE_DIM; j += 3)
		{
			adP0Mat[i][j] = pMsrPntLast->adRctMsrCov[unCnt];
			unCnt++;
		}
	}

	//速度向量自协方差
	unCnt = 0;
	DOUBLE fCnst = pow(fdt, 2);
	for (UINT32 i = 1; i < STATE_DIM; i += 3)
	{
		for (UINT32 j = 1; j < STATE_DIM; j += 3)
		{
			adP0Mat[i][j] = (pMsrPntLast->adRctMsrCov[unCnt] + pMsrPntLast2->adRctMsrCov[unCnt]) / fCnst;
			unCnt++;
		}
	}

	//加速度向量自协方差
	unCnt = 0;
	fCnst = pow(fdt, 4);
	for (UINT32 i = 2; i < STATE_DIM; i += 3)
	{
		for (UINT32 j = 2; j < STATE_DIM; j += 3)
		{
			adP0Mat[i][j] = (3 * pMsrPntLast->adRctMsrCov[unCnt] + 3 * pMsrPntLast2->adRctMsrCov[unCnt]) / fCnst;
			unCnt++;
		}
	}

	//位置、速度互协方差
	unCnt = 0;
	fCnst = fdt;
	for (UINT32 i = 0; i < STATE_DIM; i += 3)
	{
		for (UINT32 j = 1; j < STATE_DIM; j += 3)
		{
			adP0Mat[i][j] = pMsrPntLast->adRctMsrCov[unCnt] / fCnst;
			adP0Mat[j][i] = adP0Mat[i][j];
			unCnt++;
		}
	}

	//位置、加速度互协方差
	unCnt = 0;
	fCnst = pow(fdt, 2);
	for (UINT32 i = 0; i < STATE_DIM; i += 3)
	{
		for (UINT32 j = 2; j < STATE_DIM; j += 3)
		{
			adP0Mat[i][j] = pMsrPntLast->adRctMsrCov[unCnt] / fCnst;
			adP0Mat[j][i] = adP0Mat[i][j];
			unCnt++;
		}
	}

	//速度、加速度互协方差
	unCnt = 0;
	fCnst = pow(fdt, 3);
	for (UINT32 i = 1; i < STATE_DIM; i += 3)
	{
		for (UINT32 j = 2; j < STATE_DIM; j += 3)
		{
			adP0Mat[i][j] = (pMsrPntLast->adRctMsrCov[unCnt] + 2 * pMsrPntLast2->adRctMsrCov[unCnt]) / fCnst;
			adP0Mat[j][i] = adP0Mat[i][j];
			unCnt++;
		}
	}

	//拷贝至一维数组中
	memcpy(adP0, adP0Mat, sizeof(adP0Mat));

	return 0;
}


INT32 DiffInitState(MsrPntInfo* pMsrPntLast, MsrPntInfo* pMsrPntLast2, DOUBLE afX0[], DOUBLE afP0[])
{
	/*两个量测点迹差分初始化目标状态*/

	//差分计算目标速度向量，构造目标状态
	DOUBLE afDiff[MSR_DIM] = { 0 };
	memcpy(afDiff, pMsrPntLast->adRctMsr, sizeof(afDiff));
	cblas_daxpy(MSR_DIM, -1, (pMsrPntLast2->adRctMsr), 1, afDiff, 1);

	DOUBLE ddt = pMsrPntLast->dTime - pMsrPntLast2->dTime;

	DOUBLE afX0Tmp[STATE_DIM] = { pMsrPntLast->adRctMsr[0], afDiff[0] / ddt , 0, pMsrPntLast->adRctMsr[1], afDiff[1] / ddt, 0, pMsrPntLast->adRctMsr[2], afDiff[2] / ddt, 0 };

	memcpy(afX0, afX0Tmp, sizeof(afX0Tmp));

	//计算状态向量的协方差矩阵
	GetDiffStateCov(pMsrPntLast, pMsrPntLast2, afP0);

	return 0;
}

INT32 GetStateTransMat(DOUBLE ddt, DOUBLE adF[])
{
	/*计算CA模型的状态转移矩阵*/

	DOUBLE adFTmp[STATE_DIM][STATE_DIM] = { {0} };
	for (INT32 i = 0; i < STATE_DIM; i++)
	{
		adFTmp[i][i] = 1;
	}

	DOUBLE dCnst = 0.5 * pow(ddt, 2);
	adFTmp[0][1] = ddt;      adFTmp[3][4] = ddt;      adFTmp[6][7] = ddt;
	adFTmp[0][2] = dCnst;    adFTmp[3][5] = dCnst;    adFTmp[6][8] = dCnst;
	adFTmp[1][2] = ddt;      adFTmp[4][5] = ddt;      adFTmp[7][8] = ddt;

	memcpy(adF, adFTmp, sizeof(adFTmp));

	return 0;
}

INT32 GetModelNoiseCov(DOUBLE ddt, DOUBLE dModelNoise, DOUBLE adQ[])
{
	/*计算CA模型的模型噪声协方差矩阵，基于kronecker积特性，将x轴的情况类推至y、z轴*/
	/*在x轴，按行分别构造x坐标、x速度、x加速度*/

	DOUBLE adQTmp[STATE_DIM][STATE_DIM] = { {0} };

	//轴坐标维
	for (INT32 i = 0; i < STATE_DIM; i += 3)
	{
		adQTmp[i][i] = dModelNoise * pow(ddt, 5) / 20;
		adQTmp[i][i + 1] = dModelNoise * pow(ddt, 4) / 8;
		adQTmp[i][i + 2] = dModelNoise * pow(ddt, 3) / 6;
	}

	//速度维
	for (INT32 i = 1; i < STATE_DIM; i += 3)
	{
		adQTmp[i][i - 1] = dModelNoise * pow(ddt, 4) / 8;
		adQTmp[i][i] = dModelNoise * pow(ddt, 3) / 3;
		adQTmp[i][i + 1] = dModelNoise * pow(ddt, 2) / 2;
	}

	//加速度维
	for (INT32 i = 2; i < STATE_DIM; i += 3)
	{
		adQTmp[i][i - 2] = dModelNoise * pow(ddt, 3) / 6;
		adQTmp[i][i - 1] = dModelNoise * pow(ddt, 2) / 2;
		adQTmp[i][i] = dModelNoise * ddt;
	}

	memcpy(adQ, adQTmp, sizeof(adQTmp));

	return 0;
}

INT32 GetTransformCov(DOUBLE adA[], INT32 nRowNumA, DOUBLE adCovin[], INT32 nRowNumCovin, DOUBLE adNoiseCov[], DOUBLE adCovOut[])
{
	/*状态变换的协方差矩阵，afCovOut = A*Covin*A'+NoiseCov, Covin和CovOut均为方阵*/

	//debug
	//double adAcpy[STATE_DIM*STATE_DIM] = {0};
	//memcpy(adAcpy, adA, sizeof(adAcpy));
	//double adCovinCpy[STATE_DIM*STATE_DIM] = {0};
	//memcpy(adCovinCpy, adCovin, sizeof(adCovinCpy));

	//afTmp = A*Covin
	DOUBLE adTmp[STATE_DIM*STATE_DIM] = { 0 };
	cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, nRowNumA, nRowNumCovin, nRowNumCovin, 1, adA, nRowNumCovin, adCovin, nRowNumCovin, 0, adTmp, nRowNumCovin);

	//afCovOut = afTmp*afA' + afNoiseCov
	memcpy(adCovOut, adNoiseCov, sizeof(DOUBLE)*nRowNumA*nRowNumA);
	cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasTrans, nRowNumA, nRowNumA, nRowNumCovin, 1, adTmp, nRowNumCovin, adA, nRowNumCovin, 1, adCovOut, nRowNumA);

	return 0;
}

INT32 TowMsrPntExtend(MsrPntInfo* pMsrPntLast, MsrPntInfo* pMsrPntLast2, DOUBLE fdt, DOUBLE afXpreOut[], DOUBLE afPpreOut[])
{
	/*基于两个量测点迹外推航迹，计算外推航迹点及其协方差矩阵*/

	//差分构造初始状态
	DOUBLE afX0[STATE_DIM] = { 0 };
	DOUBLE afP0[STATE_DIM * STATE_DIM] = { 0 };
	DiffInitState(pMsrPntLast, pMsrPntLast2, afX0, afP0);

	//状态预测,Xpre = F*X0
	DOUBLE afF[STATE_DIM * STATE_DIM] = { 0 };
	GetStateTransMat(fdt, afF);
	DOUBLE afXpre[STATE_DIM] = { 0 };
	cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, STATE_DIM, 1, STATE_DIM, 1, afF, STATE_DIM, afX0, 1, 0, afXpre, 1);
	memcpy(afXpreOut, afXpre, sizeof(afXpre));

	//预测状态协方差，线性变换后的协方差,	Ppre = F*P0*F'+Q
	DOUBLE afPpre[STATE_DIM * STATE_DIM] = { 0 };
	DOUBLE afModelNoiseCov[STATE_DIM * STATE_DIM] = { 0 };
	GetModelNoiseCov(fdt, g_tAlgorithmPara.dModelNoise, afModelNoiseCov);
	GetTransformCov(afF, STATE_DIM, afP0, STATE_DIM, afModelNoiseCov, afPpre);
	memcpy(afPpreOut, afPpre, sizeof(afPpre));

	return 0;
}

INT32 GetJacobiH(UINT32 unMsrDim, UINT32 unStateDim, DOUBLE adH[])
{
	/*计算量测方程雅可比矩阵*/

	memset(adH, 0, sizeof(DOUBLE) * unMsrDim * unStateDim);
	for (UINT32 i = 0; i < unMsrDim; i++)
	{
		//H[i][i*MsrDim] = 1;
		adH[i * unStateDim + i * unMsrDim] = 1;
	}

	return 0;
}

DOUBLE Msr2GateDist(MsrPntInfo* pMsrPnt, DOUBLE adZpre[], DOUBLE adS[])
{
	/*计算量测点迹到波门中心的关联统计距离
		方式①仅用afS对角线元素归一化距离
		方式②利用矩阵求逆计算马氏距离*/

	DOUBLE adDiff[MSR_DIM] = { 0 };
	memcpy(adDiff, pMsrPnt->adRctMsr, sizeof(adDiff));
	cblas_daxpy(MSR_DIM, -1, adZpre, 1, adDiff, 1);

	/*fDist = sum(afDiff[i]^2/afS[i][i])*/
	//DOUBLE dDist = 0;
	//for (UINT32 i = 0; i < MSR_DIM; i++)
	//{
	//	dDist += pow(adDiff[i], 2) / adS[i*MSR_DIM + i];
	//}

	/*fDist = afDiff * inv(afS) * afDiff*/
	IPIVTYPE Ipiv[MSR_DIM] = { 0 };
	DOUBLE adSLU[MSR_DIM * MSR_DIM] = { 0 };
	memcpy(adSLU, adS, sizeof(adSLU));
	LAPACKE_dgetrf(LAPACK_ROW_MAJOR, MSR_DIM, MSR_DIM, adSLU, MSR_DIM, Ipiv);
	DOUBLE  adDiffCpy[MSR_DIM] = { 0 };
	memcpy(adDiffCpy, adDiff, sizeof(adDiffCpy));
	LAPACKE_dgetrs(LAPACK_ROW_MAJOR, 'N', MSR_DIM, 1, adSLU, MSR_DIM, Ipiv, adDiff, 1);
	DOUBLE dDist = cblas_ddot(MSR_DIM, adDiff, 1, adDiffCpy, 1);

	return dDist;
}

BOOL isSameClusterForTmpTrack(std::list<TmpTrackInfo>::iterator iter, MsrPntInfo* ptMsrPnt)
{
	/*判断点迹与暂态航迹是否满足凝聚条件*/

	if(ptMsrPnt->ucAssocFlag == 1)
		return false;

	if( fabs( ptMsrPnt->dRng - iter->qtMsrArr.at(iter->cIdxLastNonEmpty).dRng ) > g_tAlgorithmPara.dRngCentroidTH )
		return false;

	if( fabs( ptMsrPnt->dAzi - iter->qtMsrArr.at(iter->cIdxLastNonEmpty).dAzi) > g_tAlgorithmPara.dAziCentroidTH )
		return false;

	if( fabs( ptMsrPnt->dEle - iter->qtMsrArr.at(iter->cIdxLastNonEmpty).dEle) > g_tAlgorithmPara.dEleCentroidTH )
		return false;

	if( fabs( ptMsrPnt->dVr - iter->qtMsrArr.at(iter->cIdxLastNonEmpty).dVr) > g_tAlgorithmPara.dVrCentroidTH )
		return false;

	return true;
}


UINT32 GetNearestMsrPntIdxForTmpTrack(std::list<TmpTrackInfo>::iterator iter, OneAziMajorMsrInfo*ptOneAziMajorMsrPnt)
{
	/*候选航迹最近邻关联*/

	UINT32 unMsrPntIdxNearest = INF, unMsrPntIdxMaxSNR = INF;
	DOUBLE dDistMin = INF, dSNRMax = -1*INF, dDist2Gate = -1*INF;

	if(ptOneAziMajorMsrPnt->unMsrPntNum == 0)
		return unMsrPntIdxNearest;

	DOUBLE dt = ptOneAziMajorMsrPnt->dTime - iter->dLstUpdTime;

	if(dt > 0)
	{
		DOUBLE adXpre[STATE_DIM] = {0}, adPpre[STATE_DIM*STATE_DIM] = {0};
		DOUBLE adZpre[MSR_DIM] = {0}, adH[MSR_DIM * STATE_DIM] = {0}, adS[MSR_DIM * MSR_DIM] = {0};
		TowMsrPntExtend(&(iter->qtMsrArr.at(iter->cIdxLastNonEmpty)), &(iter->qtMsrArr.at(iter->cIdxLast2NonEmpty)), dt, adXpre, adPpre);
		adZpre[0] = adXpre[0]; adZpre[1] = adXpre[3]; adZpre[2] = adXpre[6];
		GetJacobiH(MSR_DIM, STATE_DIM, adH);
		GetTransformCov(adH, MSR_DIM, adPpre, STATE_DIM, ptOneAziMajorMsrPnt->atMsrPntArr[0].adRctMsrCov, adS);

		for(UINT32 i = 0; i < ptOneAziMajorMsrPnt->unMsrPntNum; i++)
		{	
			if(ptOneAziMajorMsrPnt->atMsrPntArr[i].ucAssocFlag == 1)
				continue;

			//航迹与点迹RAE粗关联，记录SNR最强的点
			//debug
			//INFO("PseudoID:{}, len:{}, PntID:{}", iter->usPseudoTrackID, iter->usPntNum, i);
			if( TmpTrackAssocMsrPntRAE(iter, ptOneAziMajorMsrPnt->atMsrPntArr+i) )
			{
				//航迹外推与点迹关联
				dDist2Gate = Msr2GateDist(ptOneAziMajorMsrPnt->atMsrPntArr+i, adZpre, adS);
				if(dDist2Gate >= 0 && dDist2Gate < g_tAlgorithmPara.dGamma && dDist2Gate < dDistMin)
				{
					dDistMin = dDist2Gate;
					unMsrPntIdxNearest = i;
				}
				else
				{
					//debug
					//INFO("[7] dist2gate:{:.4f}", dDist2Gate);
				}
			}
		}

	}
	else if(dt == 0)
	{
		// ERROR("invalid time interval:{:.4f}, measure time:{:.4f}, track time:{:.4f}", dt, ptOneAziMajorMsrPnt->dTime, iter->dLstUpdTime);
		for(UINT32 i = 0; i < ptOneAziMajorMsrPnt->unMsrPntNum; i++)
		{
			if( isSameClusterForTmpTrack(iter, ptOneAziMajorMsrPnt->atMsrPntArr+i) )
				ptOneAziMajorMsrPnt->atMsrPntArr[i].ucAssocFlag = 1;
		}
	}
	else
	{
		ERROR("invalid time interval:{:.4f}, measure time:{:.4f}, track time:{:.4f}", dt, ptOneAziMajorMsrPnt->dTime, iter->dLstUpdTime);
	}

	return unMsrPntIdxNearest;

}

UINT8 GetInitWindwLenTH(TmpTrackInitModeInfo enumInitMode)
{
	//获取暂态航迹起批窗口阈值

	if (enumInitMode == QUICK)
		return g_tAlgorithmPara.ucQuickInitWindwLenTH;
	if (enumInitMode == SLOW)
		return g_tAlgorithmPara.ucSlowInitWindwLenTH;
	if (enumInitMode == STANDARD)
		return g_tAlgorithmPara.ucStandardInitWindwLenTH;

	ERROR("invalid argument, 'ienumInitMode'{}, it must be {}, {}", enumInitMode, QUICK, SLOW);
	exit(1);
}

INT32 AddMsrPnt2TmpTrack(std::list<TmpTrackInfo>::iterator iter, MsrPntInfo* pMsrPnt)
{
	/*向候选航迹中添加量测点迹*/

	if (iter->qtMsrArr.size() >= GetInitWindwLenTH(iter->enumInitMode)) {
		BOOL bNonEmpty = iter->qtMsrArr.at(0).dRng != 0;
		iter->qtMsrArr.pop_front();
		if (bNonEmpty)
			iter->usPntNum -= 1;
		iter->cIdxLastNonEmpty  -= 1;
		iter->cIdxLast2NonEmpty -= 1;
	}

	if (pMsrPnt != nullptr) {
		iter->bAssocFlag = true;
		iter->qtMsrArr.push_back(*pMsrPnt);
		iter->cIdxLast2NonEmpty = iter->cIdxLastNonEmpty;
		iter->cIdxLastNonEmpty  = iter->qtMsrArr.size()-1;
		iter->usPntNum          += 1;
		iter->usInitWndwLen     = iter->qtMsrArr.size();
		iter->unLstUpdFrm = pMsrPnt->unCPI;
		iter->tLstUpdtDate = pMsrPnt->tDate;
		iter->dLstUpdTime  = pMsrPnt->dTime;
		iter->ucAntennaIdx = pMsrPnt->ucAntennaIdx;
		iter->enumBeamType = pMsrPnt->enumBeamType;
		iter->ucBeamCoordX = pMsrPnt->ucBeamCoordX;
		iter->ucBeamCoordY = pMsrPnt->ucBeamCoordY;
		iter->untar_type   = pMsrPnt->untar_type; 
	}
	else {
		MsrPntInfo tMsrPnt;
		bzero(&tMsrPnt, sizeof(MsrPntInfo));
		iter->qtMsrArr.push_back(tMsrPnt);
		iter->usInitWndwLen = iter->qtMsrArr.size();
	}

	return 0;
}

INT32 GetSNRMaxMsrPntIdxForTmpTrack(std::list<TmpTrackInfo>::iterator iter, OneAziMajorMsrInfo* ptOneAziMajorMsrPnt)
{
	/*航迹头关联，RAE条件关联波门*/

	UINT32 unMsrPntIdxMaxSNR = INF;
	DOUBLE dSNRMax = -1*INF;

	if(ptOneAziMajorMsrPnt->unMsrPntNum == 0)
		return unMsrPntIdxMaxSNR;

	for(UINT32 i = 0; i < ptOneAziMajorMsrPnt->unMsrPntNum; i++)
	{	
		if(ptOneAziMajorMsrPnt->atMsrPntArr[i].ucAssocFlag == 1)
			continue;

		//航迹与点迹RAE粗关联，记录SNR最强的点
		if( TmpTrackAssocMsrPntRAE(iter, ptOneAziMajorMsrPnt->atMsrPntArr+i) )
		{
			if(ptOneAziMajorMsrPnt->atMsrPntArr[i].dSNR > dSNRMax)
			{
				dSNRMax = ptOneAziMajorMsrPnt->atMsrPntArr[i].dSNR;
				unMsrPntIdxMaxSNR = i;
			}
		}
	}

	return unMsrPntIdxMaxSNR;
}

INT32 ZoomCovMat(DOUBLE afCovMat[], UINT32 unLength, DOUBLE fCoeff)
{
	/*协方差矩阵缩放*/

	for (UINT32 i = 0; i < unLength; i++)
	{
		afCovMat[i] *= fCoeff;
	}

	return 0;
}

DOUBLE Xest2Rng(DOUBLE afXest[], UINT32 unMsrDim, UINT32 unOneCoordStateDim)
{
	/*基于直角坐标状态计算目标径向距离*/

	DOUBLE fRng = cblas_dnrm2(unMsrDim, afXest, unOneCoordStateDim);
	return fRng;
}


DOUBLE Xest2Azi360(DOUBLE afXest[], UINT32 unOneCoordStateDim)
{
	/*基于直角坐标状态计算目标方位角，方位0度为y轴（阵面法向），顺时针为角度正方向，取值范围0-360度*/

	DOUBLE fAzi = 90 - atand(afXest[unOneCoordStateDim], afXest[0]);

	if(fAzi < 0)
		fAzi += 360;

	return fAzi;
}

DOUBLE Xest2Azi180(DOUBLE afXest[], UINT32 unOneCoordStateDim)
{
	/*基于直角坐标状态计算目标方位角，方位0度为y轴（阵面法向），顺时针为角度正方向，取值范围-180 ~ +180度*/

	DOUBLE fAzi = 90 - atand(afXest[unOneCoordStateDim], afXest[0]);

	if(fAzi > 180)
		fAzi -= 360;

	return fAzi;
}

DOUBLE Xest2Ele(DOUBLE afXest[], UINT32 unMsrDim, UINT32 unOneCoordStateDim)
{
	/*基于直角坐标状态计算目标俯仰角*/

	DOUBLE fRng = Xest2Rng(afXest, unMsrDim, unOneCoordStateDim);

	DOUBLE fEle = asind(afXest[(unMsrDim - 1) * unOneCoordStateDim] / fRng);
	return fEle;
}

DOUBLE Xest2Vel(DOUBLE afXest[], UINT32 unMsrDim, UINT32 unOneCoordStateDim)
{
	/*基于直角坐标状态计算目标空间速度*/

	DOUBLE fVel = cblas_dnrm2(unMsrDim, afXest + 1, unOneCoordStateDim);
	return fVel;
}

DOUBLE Xest2Acc(DOUBLE afXest[], UINT32 unMsrDim, UINT32 unOneCoordStateDim)
{
	/*基于直角坐标状态计算目标空间加速度*/

	DOUBLE fAcc = cblas_dnrm2(unMsrDim, afXest + 2, unOneCoordStateDim);
	return fAcc;
}

DOUBLE Xest2Alt(DOUBLE afXest[], UINT32 unMsrDim, UINT32 unOneCoordStateDim)
{
	/*基于直角坐标状态计算目标高度*/
	return afXest[(unMsrDim-1)*unOneCoordStateDim];
}

DOUBLE Xest2Vr(DOUBLE afXest[], UINT32 unMsrDim, UINT32 unOneCoordStateDim)
{
	/*基于直角坐标状态计算目标径向速度*/

	DOUBLE fRng = Xest2Rng(afXest, unMsrDim, unOneCoordStateDim);
	//速度向量
	DOUBLE* afVel = new DOUBLE[unMsrDim];
	for (UINT32 i = 0; i < unMsrDim; i++)
	{
		afVel[i] = afXest[i * unOneCoordStateDim + 1];
	}

	//位置向量
	DOUBLE* afPos = new DOUBLE[unMsrDim];
	for (UINT32 i = 0; i < unMsrDim; i++)
	{
		afPos[i] = afXest[i * unOneCoordStateDim];
	}

	//速度向量与位置向量内积计算径向速度
	DOUBLE fVr = -1 * cblas_ddot(unMsrDim, afVel, 1, afPos, 1) / fRng;

	delete[] afVel;
	delete[] afPos;

	return fVr;
}

DOUBLE Xest2VelAzi360(DOUBLE adXest[], UINT32 unOneCoordStateDim)
{
	/*基于直角坐标状态计算目标航向角，航向角0度为阵面所在经线正北方向，顺时针为角度正方向，取值范围0-360度*/
	/*输入ENU坐标系中的目标状态向量，此时y轴指向地理正北*/

	DOUBLE dVelAzi = 90 - atand(adXest[1+unOneCoordStateDim], adXest[1]);

	if(dVelAzi < 0)
		dVelAzi += 360;

	return dVelAzi;
}

DOUBLE Xest2VelAzi180(DOUBLE adXest[], UINT32 unOneCoordStateDim)
{
	/*基于直角坐标状态计算目标航向角，航向角0度为阵面所在经线正北方向，顺时针为角度正方向，取值范围-180~+180度*/
	/*输入ENU坐标系中的目标状态向量，此时y轴指向地理正北*/

	DOUBLE dVelAzi = 90 - atand(adXest[1+unOneCoordStateDim], adXest[1]);

	if(dVelAzi > 180 )
		dVelAzi -= 360;

	return dVelAzi;
}


INT32 GetTransMatRA2ENU(DOUBLE dYaw, DOUBLE dPitch, DOUBLE dRoll, DOUBLE adRA2ENU[])
{
	/*从阵面坐标至阵面中心ENU坐标的坐标变换矩阵*/

	DOUBLE dCy = cosd(dYaw);
	DOUBLE dSy = sind(dYaw);
	DOUBLE dCp = cosd(dPitch);
	DOUBLE dSp = sind(dPitch);
	DOUBLE dCr = cosd(dRoll);
	DOUBLE dSr = sind(dRoll);

	//ENU至RA坐标轴旋转顺序:   -3，1，2
	DOUBLE adTransMat[MSR_DIM*MSR_DIM] =
	{
			dCy*dCr+dSy*dSp*dSr,  dSy*dCp, dCy*dSr-dSy*dSp*dCr,
			-dSy*dCr+dCy*dSp*dSr, dCy*dCp, -dSy*dSr-dCy*dSp*dCr,
			-dCp*dSr,             dSp,     dCp*dCr
	};

	memcpy(adRA2ENU, adTransMat, MSR_DIM*MSR_DIM*sizeof(DOUBLE));

	return 0;
}

INT32 CovMatRotateRA2ENU(DOUBLE adTransMatRA2ENU[], DOUBLE adCovRA[], DOUBLE adCovENU[])
{
	/*将阵面坐标系中的目标协方差矩阵变换至阵面中心为原点的ENU坐标系*/

	DOUBLE adCovIN[MSR_DIM*MSR_DIM], adCovOut[MSR_DIM*MSR_DIM], adCovNoise[MSR_DIM*MSR_DIM];
	bzero(adCovIN, sizeof(DOUBLE)*MSR_DIM*MSR_DIM);
	bzero(adCovOut, sizeof(DOUBLE)*MSR_DIM*MSR_DIM);
	bzero(adCovNoise, sizeof(DOUBLE)*MSR_DIM*MSR_DIM);

	UINT8 ucCnt = 0;

	//协方差矩阵中位置所在行(0、3、6)， 速度所在行(1、4、7)，加速度所在行(2、5、8)

	for(UINT8 ucDimIdx = 0; ucDimIdx < ONE_COORD_STATE_DIM; ucDimIdx++)
	{
		//提取位置、速度、加速度对应维度的协方差
		for(UINT8 i = ucDimIdx; i < STATE_DIM; i += ONE_COORD_STATE_DIM)
		{
			for(UINT8 j = ucDimIdx; j < STATE_DIM; j += ONE_COORD_STATE_DIM)
			{
				adCovIN[ucCnt] = adCovRA[i*STATE_DIM+j];

				ucCnt++;
			}

		}

		//重置索引
		ucCnt = 0;

		//协方差矩阵变换
		GetTransformCov(adTransMatRA2ENU, MSR_DIM, adCovIN, MSR_DIM, adCovNoise, adCovOut);

		//赋值
		for(UINT8 i = ucDimIdx; i < STATE_DIM; i += ONE_COORD_STATE_DIM)
		{
			for(UINT8 j = ucDimIdx; j < STATE_DIM; j += ONE_COORD_STATE_DIM)
			{
				adCovENU[i*STATE_DIM+j] = adCovOut[ucCnt];

				ucCnt++;
			}

		}

		//重置索引
		ucCnt = 0;
	}
	return 0;
}


INT32 RA2ENU(DOUBLE dYaw, DOUBLE dPitch, DOUBLE dRoll, DOUBLE adXestRA[], DOUBLE adPestRA[], DOUBLE adXestENU[], DOUBLE adPestENU[])
{
	/*将阵面坐标系中的目标状态向量变换至阵面中心为原点的ENU坐标系*/

	DOUBLE adTransMat[MSR_DIM*MSR_DIM] = {0};
	GetTransMatRA2ENU(dYaw, dPitch, dRoll, adTransMat);

	bzero(adXestENU, sizeof(DOUBLE) * MSR_DIM * ONE_COORD_STATE_DIM);


	/*阵面坐标系中9x1的目标状态向量按行主序排列为3x3形式
	   第1列为阵面坐标系中位置坐标，第2列为阵面坐标系中速度坐标， 第3列为阵面坐标系中加速度坐标
	   将旋转矩阵作用于该3x3矩阵实现旋转*/

	cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, MSR_DIM, MSR_DIM, MSR_DIM, 1, adTransMat, MSR_DIM, adXestRA, MSR_DIM, 0, adXestENU, MSR_DIM);

	CovMatRotateRA2ENU(adTransMat, adPestRA, adPestENU);

	return 0;
}

VOID GetPredState(DOUBLE afXest[], DOUBLE dVrIN, UINT32 unMsrDim, UINT32 unOneCoordStateDim, DOUBLE fdt, PredRAEInfo& tPredRAE)
{
	/*预测目标运动状态*/

	UINT32 unStateDim = unMsrDim * unOneCoordStateDim;
	DOUBLE* afF = new DOUBLE[unStateDim * unStateDim];
	GetStateTransMat(fdt, afF);
	DOUBLE* afXpre = new DOUBLE[unStateDim];
	cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, unStateDim, 1, unStateDim, 1, afF, unStateDim, afXest, 1, 0, afXpre, 1);
	tPredRAE.dRngPre = Xest2Rng(afXpre, unMsrDim, unOneCoordStateDim);
	tPredRAE.dAziPre = Xest2Azi180(afXpre, unOneCoordStateDim);
	tPredRAE.dElePre = Xest2Ele(afXpre, unMsrDim, unOneCoordStateDim);
	tPredRAE.dVrPre  = dVrIN;
	tPredRAE.dVelPre = Xest2Vel(afXpre, unMsrDim, unOneCoordStateDim);
	tPredRAE.dAccPre = Xest2Acc(afXpre, unMsrDim, unOneCoordStateDim);
	tPredRAE.dAltPre = Xest2Alt(afXpre, unMsrDim, unOneCoordStateDim);
	delete[] afF;
	delete[] afXpre;
}

INT32 TmpTrack2TgtTrack(std::list<TmpTrackInfo>::iterator iter, TgtTrackPntInfo& tTgtTrackPnt)
{
	/*基于候选航迹新建确认航迹*/

	tTgtTrackPnt.untar_type = iter->untar_type;
	tTgtTrackPnt.unStartFrm = iter->unLstUpdFrm;
	tTgtTrackPnt.unLstUpdFrm = iter->unLstUpdFrm;
	tTgtTrackPnt.tLstUpdtDate = iter->tLstUpdtDate;
	tTgtTrackPnt.dLstUpdTime = iter->dLstUpdTime;
	tTgtTrackPnt.dModelNoise = g_tAlgorithmPara.dModelNoise;
	tTgtTrackPnt.unLen = iter->usPntNum;
	tTgtTrackPnt.qucTerminentWindow.resize(0);
	tTgtTrackPnt.usTermntWndwLen = 0;
	tTgtTrackPnt.qucConfirmWindow.resize(0);
	tTgtTrackPnt.usConfirmNum = 0;
	tTgtTrackPnt.usMissNum = 0;
	DiffInitState(&(iter->qtMsrArr.at(iter->cIdxLastNonEmpty)), &(iter->qtMsrArr.at(iter->cIdxLast2NonEmpty)), tTgtTrackPnt.adXest, tTgtTrackPnt.adPest);
	memcpy(tTgtTrackPnt.adXestLast2, tTgtTrackPnt.adXest, sizeof(DOUBLE)*STATE_DIM);
	tTgtTrackPnt.adXestLast2[0] = iter->qtMsrArr.at(iter->cIdxLast2NonEmpty).adRctMsr[0];
	tTgtTrackPnt.adXestLast2[3] = iter->qtMsrArr.at(iter->cIdxLast2NonEmpty).adRctMsr[1];
	tTgtTrackPnt.adXestLast2[6] = iter->qtMsrArr.at(iter->cIdxLast2NonEmpty).adRctMsr[2];
	tTgtTrackPnt.enumTrackUpdMode = FILTER;
	tTgtTrackPnt.enumWorkFashion  = g_tSysPara.enumWorkMode;
	tTgtTrackPnt.enumScheduleEvent = g_tSysPara.enumScheduleEvent;
	tTgtTrackPnt.bAssocFlag = true;
	tTgtTrackPnt.ucAntennaIdx = iter->ucAntennaIdx;
	tTgtTrackPnt.enumBeamType = iter->enumBeamType;
	tTgtTrackPnt.ucBeamCoordX = iter->ucBeamCoordX;
	tTgtTrackPnt.ucBeamCoordY = iter->ucBeamCoordY;
	ZoomCovMat(tTgtTrackPnt.adPest, STATE_DIM*STATE_DIM, 1e2);
	tTgtTrackPnt.tRAEest.dRngEst = iter->qtMsrArr.at(iter->cIdxLastNonEmpty).dRng;
	tTgtTrackPnt.tRAEest.dAziEst = iter->qtMsrArr.at(iter->cIdxLastNonEmpty).dAzi;
	tTgtTrackPnt.tRAEest.dEleEst = iter->qtMsrArr.at(iter->cIdxLastNonEmpty).dEle;
	tTgtTrackPnt.tRAEest.dAltEst = Xest2Alt(tTgtTrackPnt.adXest, MSR_DIM, ONE_COORD_STATE_DIM);
	tTgtTrackPnt.tRAEest.dVrEst  = iter->qtMsrArr.at(iter->cIdxLastNonEmpty).dVr;
	RA2ENU(g_tSysPara.tRadarArrayAttitude.dYaw, g_tSysPara.tRadarArrayAttitude.dPitch, g_tSysPara.tRadarArrayAttitude.dRoll, tTgtTrackPnt.adXest, tTgtTrackPnt.adPest, tTgtTrackPnt.adXestENU, tTgtTrackPnt.adPestENU);
	tTgtTrackPnt.tRAEestENU.dRngEst = Xest2Rng(tTgtTrackPnt.adXestENU, MSR_DIM, ONE_COORD_STATE_DIM);
	tTgtTrackPnt.tRAEestENU.dAziEst = Xest2Azi180(tTgtTrackPnt.adXestENU, ONE_COORD_STATE_DIM);
	tTgtTrackPnt.tRAEestENU.dEleEst = Xest2Ele(tTgtTrackPnt.adXestENU, MSR_DIM, ONE_COORD_STATE_DIM);
	tTgtTrackPnt.tRAEestENU.dAltEst = Xest2Alt(tTgtTrackPnt.adXestENU, MSR_DIM, ONE_COORD_STATE_DIM);
	tTgtTrackPnt.tRAEestENU.dVrEst  = iter->qtMsrArr.at(iter->cIdxLastNonEmpty).dVr;
	tTgtTrackPnt.dPRT = iter->qtMsrArr.at(iter->cIdxLastNonEmpty).dPRT;
	tTgtTrackPnt.dAmp = iter->qtMsrArr.at(iter->cIdxLastNonEmpty).dAmp;
	tTgtTrackPnt.dSNR = iter->qtMsrArr.at(iter->cIdxLastNonEmpty).dSNR;
	tTgtTrackPnt.dVel = Xest2Vel(tTgtTrackPnt.adXest, MSR_DIM, ONE_COORD_STATE_DIM);
	tTgtTrackPnt.dAcc = Xest2Acc(tTgtTrackPnt.adXest, MSR_DIM, ONE_COORD_STATE_DIM);
	tTgtTrackPnt.dVelAzi = Xest2VelAzi180(tTgtTrackPnt.adXestENU, ONE_COORD_STATE_DIM);
	GetPredState(tTgtTrackPnt.adXest, tTgtTrackPnt.tRAEest.dVrEst, MSR_DIM, ONE_COORD_STATE_DIM, TIME_STEP, tTgtTrackPnt.tPredRAE);
	bzero(tTgtTrackPnt.adInnov, sizeof(tTgtTrackPnt.adInnov));
	tTgtTrackPnt.dMsr2GateDist = 0;
	tTgtTrackPnt.tPntAssociatd.unAntennaIdx = iter->qtMsrArr.at(iter->cIdxLastNonEmpty).ucAntennaIdx;
	tTgtTrackPnt.tPntAssociatd.unCPI = iter->qtMsrArr.at(iter->cIdxLastNonEmpty).unCPI;
	tTgtTrackPnt.tPntAssociatd.unIdx = iter->qtMsrArr.at(iter->cIdxLastNonEmpty).unIdx;
	tTgtTrackPnt.tPntAssociatd.tMsrPnt = iter->qtMsrArr.at(iter->cIdxLastNonEmpty);
	tTgtTrackPnt.enumPredScheduleEvent = SEARCH;
	for(UINT16 i = 0;i<iter->qtMsrArr.size();i++){
		if(iter->qtMsrArr.at(i).dRng == 0){
			continue;
		}
		tTgtTrackPnt.LtMsrForInit.emplace_back(iter->qtMsrArr.at(i));
	}
	return 0;
}

UINT16 GetTrackID(std::queue<UINT16>& qusTrackID, std::vector<UINT16>& vusTrackIDUesd)
{
	/*获取新目标的批号*/

	//批号队列为空，重新加入可用的目标批号
	if(qusTrackID.empty())
	{
		sort(vusTrackIDUesd.begin(), vusTrackIDUesd.end());

		for(std::vector<UINT16>::iterator iter = vusTrackIDUesd.begin(); iter != vusTrackIDUesd.end(); iter++)
		{
			qusTrackID.push(*iter);

		}

		vusTrackIDUesd.clear();
	}

	UINT16 usTrackID = qusTrackID.front();
	qusTrackID.pop();

	return usTrackID;
}

enum TmpTrackInitModeInfo GetInitModeForTmpTrack(TmpTrackInfo* ptTmpTrack)
{
	//确定暂态航迹置信度

	if (ptTmpTrack->qtMsrArr.at(ptTmpTrack->cIdxLastNonEmpty).dRng <= g_tAlgorithmPara.dHighConfRngTH && fabs(ptTmpTrack->qtMsrArr.at(ptTmpTrack->cIdxLastNonEmpty).dVr) <= g_tAlgorithmPara.dHighConfVrTH)
		return SLOW;
	else if (ptTmpTrack->qtMsrArr.at(ptTmpTrack->cIdxLastNonEmpty).dRng >= g_tAlgorithmPara.dQuickInitRngTH)
		return QUICK;
	else
		return STANDARD;
}

INT32  GetNewTmpTrack(MsrPntInfo* ptMsrPnt, TmpTrackInfo& tTmpTrack)
{
	/*新建一条候选航迹*/

	if(ptMsrPnt == NULL)
	{
		ERROR("invalid argument, 'ptMsrPnt' == NULL");
		return -1;
	}

	tTmpTrack.qtMsrArr.resize(0);
	tTmpTrack.qtMsrArr.push_back(*ptMsrPnt);


	tTmpTrack.untar_type    = ptMsrPnt->untar_type;
	tTmpTrack.unStartFrm    = ptMsrPnt->unCPI;
	tTmpTrack.unLstUpdFrm   = ptMsrPnt->unCPI;
	tTmpTrack.tLstUpdtDate  = ptMsrPnt->tDate;
	tTmpTrack.dLstUpdTime   = ptMsrPnt->dTime;

	tTmpTrack.usPntNum           = 1;
	tTmpTrack.usInitWndwLen      = tTmpTrack.qtMsrArr.size();
	tTmpTrack.enumTmpTrackState  = REMAIN;
	tTmpTrack.bAssocFlag         = true;
	tTmpTrack.ucAntennaIdx       = ptMsrPnt->ucAntennaIdx;
	tTmpTrack.enumBeamType       = ptMsrPnt->enumBeamType;
	tTmpTrack.ucBeamCoordX       = ptMsrPnt->ucBeamCoordX;
	tTmpTrack.ucBeamCoordY       = ptMsrPnt->ucBeamCoordY;
	tTmpTrack.cIdxLastNonEmpty   = 0;
	tTmpTrack.cIdxLast2NonEmpty  = 0;
	tTmpTrack.enumInitMode       = GetInitModeForTmpTrack(&tTmpTrack);

	return 0;
}

INT8 Get1stNonEmptyMsrPntIdx(std::list<TmpTrackInfo>::iterator iter)
{
	//获取队列中第一个非空元素的索引

	for(INT8 i = 0; i < iter->qtMsrArr.size(); i++){
		if(iter->qtMsrArr.at(i).dRng != 0)
			return i;
	}

	return -1;
}

BOOL QuickCheckTmpTrack(std::list<TmpTrackInfo>::iterator iter)
{
	//起批前检查暂态航迹状态

	INT8 cIdx1stNonEmpty = Get1stNonEmptyMsrPntIdx(iter);
	if( cIdx1stNonEmpty < 0 || cIdx1stNonEmpty == iter->cIdxLastNonEmpty)
		return false;

	DOUBLE dDeltaRng = fabs(iter->qtMsrArr.at(iter->cIdxLastNonEmpty).dRng-iter->qtMsrArr.at(cIdx1stNonEmpty).dRng);
	DOUBLE dt = iter->qtMsrArr.at(iter->cIdxLastNonEmpty).dTime - iter->qtMsrArr.at(iter->cIdxLastNonEmpty).dTime;

	if(dDeltaRng < g_tAlgorithmPara.dVrMin*(dt))
		return false;

	return true;
}

DOUBLE GetStdForTmpTrack(std::list<TmpTrackInfo>::iterator iter, MsrPntMemberInfo enumMsrPntMember) {
	//计算对应成员的标准差

	// 选择目标字段的成员指针
	double MsrPntInfo::*member_ptr = nullptr;
	switch (enumMsrPntMember) {
	case RNG:
		member_ptr = &MsrPntInfo::dRng;
		break;
	case AZI:
		member_ptr = &MsrPntInfo::dAzi;
		break;
	case ELE:
		member_ptr = &MsrPntInfo::dEle;
		break;
	case VR:
		member_ptr = &MsrPntInfo::dVr;
		break;
	case SNR:
		member_ptr = &MsrPntInfo::dSNR;
		break;
	default:
		ERROR("invalid argument, 'enumMsrPntMember'{}, it can only be {},{},{},{},{}", enumMsrPntMember, RNG, AZI, ELE, VR, SNR);
		return 0.0; // 无效字段返回0.0
	}

	//计算均值
	DOUBLE dAve = 0;
	for (UINT16 i = 0; i < iter->qtMsrArr.size(); i++) {
		dAve += iter->qtMsrArr.at(i).*member_ptr;
	}
	dAve = dAve / iter->usPntNum;

	//计算平方和
	DOUBLE dSquareSum = 0;
	for (UINT16 i= 0; i < iter->qtMsrArr.size(); i++) {
		dSquareSum += pow(iter->qtMsrArr.at(i).*member_ptr-dAve, 2);
	}

	//计算标准差
	DOUBLE dStd = 0;
	if (iter->usPntNum == 1)
		dStd = 0;
	else
		dStd = sqrt(dSquareSum / (iter->usPntNum-1));

	return dStd;
}

BOOL SlowCheckTmpTrack(std::list<TmpTrackInfo>::iterator iter) {
	//暂态航迹可行性检验

	//距离微分
	if(iter->cIdxLastNonEmpty == iter->cIdxLast2NonEmpty)
		return false;

	DOUBLE dRngDiff = 0;
	if ( (dRngDiff = fabs((iter->qtMsrArr.at(iter->cIdxLastNonEmpty).dRng - iter->qtMsrArr.at(iter->cIdxLast2NonEmpty).dRng) / (iter->qtMsrArr.at(iter->cIdxLastNonEmpty).dTime - iter->qtMsrArr.at(iter->cIdxLastNonEmpty).dTime))) <= g_tAlgorithmPara.dRngDiffTH )
		return false;
//	for (UINT16 i = 0; i < iter->usPntNum-1; i++) {
//		if ( (dRngDiff = fabs((iter->atMsrArr[i+1].dRng - iter->atMsrArr[i].dRng) / (iter->atMsrArr[i+1].dTime - iter->atMsrArr[i].dTime))) <= g_tAlgorithmPara.dRngDiffTH )
//			return false;
//	}

	DOUBLE dStd = 0;
	//检查距离标准差
	if ( (dStd = GetStdForTmpTrack(iter, RNG)) <= g_tAlgorithmPara.dStdRngTH)
		return false;

	//检查速度标准差
	if ( (dStd = GetStdForTmpTrack(iter, VR)) <= g_tAlgorithmPara.dStdVrTH)
		return false;

	//检查俯仰角标准差
	if ( (dStd = GetStdForTmpTrack(iter, ELE)) <= g_tAlgorithmPara.dStdEleTH)
		return false;

	//检查方位角标准差
	if ( (dStd = GetStdForTmpTrack(iter, AZI)) <= g_tAlgorithmPara.dStdAziTH)
		return false;

	//检查信噪比标准差
	if ( (dStd = GetStdForTmpTrack(iter, SNR)) <= g_tAlgorithmPara.dStdSNRTH)
		return false;

	return true;
}

INT32 SaveTmpTrack(std::list<TmpTrackInfo>* pLTmpTrack, bool bSaveFlag, unsigned short uspseudo_track_id)
{
	/*保存暂态航迹*/

	if(!bSaveFlag)
		return 0;

	if(pLTmpTrack == NULL)
	{
		ERROR("invalid argument, 'pLTmpTrack == NULL'");
		return -2;
	}

	INT32 nTrackNum = 0;

	static FILE* pfTgtTrack = NULL;

	static char acFilePathTgtTrack[256] = "";

	static BOOL bCreateFileFlag = true;

	//获取时间
	time_t timeNowUTCSec = time(0);
	struct tm * ptLocalTime = localtime(&timeNowUTCSec);

	//创建路径，编辑文件名
	//static 类型的bCreateFileFlag确保文件仅创建一次
	if(bCreateFileFlag)
	{
		//创建父目录-目标航迹
		if( opendir("./TmpTrackdata") == NULL)
		{
			if( mkdir("./TmpTrackdata", 0777) < 0) //路径权限0777，文件所有者有rwx权限
			{
				ERROR("mkdir error");
				return -3;
			}
		}

		sprintf(acFilePathTgtTrack, "./TmpTrackdata/TmpTrack%d%02d%02d%02d%02d%02d.dat", ptLocalTime->tm_year+1900, ptLocalTime->tm_mon+1, ptLocalTime->tm_mday, ptLocalTime->tm_hour, ptLocalTime->tm_min, ptLocalTime->tm_sec);

		bCreateFileFlag = false;
	}

	//打开输入流-目标航迹
	if ((pfTgtTrack = fopen(acFilePathTgtTrack, "ab")) == NULL)
	{
		ERROR("fopen error, file path:{}", acFilePathTgtTrack);
		return -4;
	}

	if (uspseudo_track_id == 0)
	{
		// 写入各条暂态航迹
		for (std::list<TmpTrackInfo>::iterator iter = pLTmpTrack->begin(); iter != pLTmpTrack->end(); iter++)
		{
			if (iter->enumTmpTrackState != TURN_TO_TGT_TRACK)
				continue;
			nTrackNum++;
		}

		if (nTrackNum > 0)
			fwrite(&(nTrackNum), sizeof(nTrackNum), 1, pfTgtTrack);

		for (std::list<TmpTrackInfo>::iterator iter = pLTmpTrack->begin(); iter != pLTmpTrack->end(); iter++)
		{
			if (iter->enumTmpTrackState != TURN_TO_TGT_TRACK)
				continue;

			fwrite(&(iter->usPseudoTrackID), sizeof(iter->usPseudoTrackID), 1, pfTgtTrack);
			fwrite(&(iter->usToTrackID), sizeof(iter->usToTrackID), 1, pfTgtTrack);
			fwrite(&(iter->unStartFrm), sizeof(iter->unStartFrm), 1, pfTgtTrack);
			fwrite(&(iter->unLstUpdFrm), sizeof(iter->unLstUpdFrm), 1, pfTgtTrack);
			fwrite(&(iter->dLstUpdTime), sizeof(iter->dLstUpdTime), 1, pfTgtTrack);
			fwrite(&(iter->usPntNum), sizeof(iter->usPntNum), 1, pfTgtTrack);
			fwrite(&(iter->usInitWndwLen), sizeof(iter->usInitWndwLen), 1, pfTgtTrack);
			fwrite(&(iter->enumTmpTrackState), sizeof(iter->enumTmpTrackState), 1, pfTgtTrack);
			fwrite(&(iter->enumInitMode), sizeof(iter->enumInitMode), 1, pfTgtTrack);
			for (UINT16 i = 0; i < iter->qtMsrArr.size(); i++)
			{
				if (iter->qtMsrArr[i].dRng == 0)
					continue;
				fwrite(&(iter->qtMsrArr[i].dRng), sizeof(DOUBLE), 1, pfTgtTrack);
				fwrite(&(iter->qtMsrArr[i].dAzi), sizeof(DOUBLE), 1, pfTgtTrack);
				fwrite(&(iter->qtMsrArr[i].dEle), sizeof(DOUBLE), 1, pfTgtTrack);
				fwrite(&(iter->qtMsrArr[i].dAlt), sizeof(DOUBLE), 1, pfTgtTrack);
				fwrite(&(iter->qtMsrArr[i].dVr), sizeof(DOUBLE), 1, pfTgtTrack);
				fwrite(&(iter->qtMsrArr[i].dPRT), sizeof(DOUBLE), 1, pfTgtTrack);
				fwrite(&(iter->qtMsrArr[i].dAmp), sizeof(DOUBLE), 1, pfTgtTrack);
				fwrite(&(iter->qtMsrArr[i].dSNR), sizeof(DOUBLE), 1, pfTgtTrack);
				fwrite(&(iter->qtMsrArr[i].unCPI), sizeof(UINT32), 1, pfTgtTrack);
				fwrite(&(iter->qtMsrArr[i].unIdx), sizeof(UINT32), 1, pfTgtTrack);
				fwrite(&(iter->qtMsrArr[i].ucAntennaIdx), sizeof(UINT8), 1, pfTgtTrack);
				fwrite(&(iter->qtMsrArr[i].enumBeamType), sizeof(BeamTypeInfo), 1, pfTgtTrack);
				fwrite(&(iter->qtMsrArr[i].ucBeamCoordX), sizeof(UINT8), 1, pfTgtTrack);
				fwrite(&(iter->qtMsrArr[i].ucBeamCoordY), sizeof(UINT8), 1, pfTgtTrack);
				fwrite(&(iter->qtMsrArr[i].dConfidence), sizeof(DOUBLE), 1, pfTgtTrack);
				fwrite(&(iter->qtMsrArr[i].dTime), sizeof(DOUBLE), 1, pfTgtTrack);
			}
		}
	}
	else
	{
		nTrackNum = 1;
		fwrite(&(nTrackNum), sizeof(nTrackNum), 1, pfTgtTrack);
		for (std::list<TmpTrackInfo>::iterator iter = pLTmpTrack->begin(); iter != pLTmpTrack->end(); iter++)
		{
			if (iter->usPseudoTrackID != uspseudo_track_id)
				continue;

			fwrite(&(iter->usPseudoTrackID), sizeof(iter->usPseudoTrackID), 1, pfTgtTrack);
			fwrite(&(iter->usToTrackID), sizeof(iter->usToTrackID), 1, pfTgtTrack);
			fwrite(&(iter->unStartFrm), sizeof(iter->unStartFrm), 1, pfTgtTrack);
			fwrite(&(iter->unLstUpdFrm), sizeof(iter->unLstUpdFrm), 1, pfTgtTrack);
			fwrite(&(iter->dLstUpdTime), sizeof(iter->dLstUpdTime), 1, pfTgtTrack);
			fwrite(&(iter->usPntNum), sizeof(iter->usPntNum), 1, pfTgtTrack);
			fwrite(&(iter->usInitWndwLen), sizeof(iter->usInitWndwLen), 1, pfTgtTrack);
			fwrite(&(iter->enumTmpTrackState), sizeof(iter->enumTmpTrackState), 1, pfTgtTrack);
			fwrite(&(iter->enumInitMode), sizeof(iter->enumInitMode), 1, pfTgtTrack);

			for (UINT16 i = 0; i < iter->qtMsrArr.size(); i++)
			{
				if (iter->qtMsrArr[i].dRng == 0)
					continue;
				fwrite(&(iter->qtMsrArr[i].dRng), sizeof(DOUBLE), 1, pfTgtTrack);
				fwrite(&(iter->qtMsrArr[i].dAzi), sizeof(DOUBLE), 1, pfTgtTrack);
				fwrite(&(iter->qtMsrArr[i].dEle), sizeof(DOUBLE), 1, pfTgtTrack);
				fwrite(&(iter->qtMsrArr[i].dAlt), sizeof(DOUBLE), 1, pfTgtTrack);
				fwrite(&(iter->qtMsrArr[i].dVr), sizeof(DOUBLE), 1, pfTgtTrack);
				fwrite(&(iter->qtMsrArr[i].dPRT), sizeof(DOUBLE), 1, pfTgtTrack);
				fwrite(&(iter->qtMsrArr[i].dAmp), sizeof(DOUBLE), 1, pfTgtTrack);
				fwrite(&(iter->qtMsrArr[i].dSNR), sizeof(DOUBLE), 1, pfTgtTrack);
				fwrite(&(iter->qtMsrArr[i].unCPI), sizeof(UINT32), 1, pfTgtTrack);
				fwrite(&(iter->qtMsrArr[i].unIdx), sizeof(UINT32), 1, pfTgtTrack);
				fwrite(&(iter->qtMsrArr[i].ucAntennaIdx), sizeof(UINT8), 1, pfTgtTrack);
				fwrite(&(iter->qtMsrArr[i].enumBeamType), sizeof(BeamTypeInfo), 1, pfTgtTrack);
				fwrite(&(iter->qtMsrArr[i].ucBeamCoordX), sizeof(UINT8), 1, pfTgtTrack);
				fwrite(&(iter->qtMsrArr[i].ucBeamCoordY), sizeof(UINT8), 1, pfTgtTrack);
				fwrite(&(iter->qtMsrArr[i].dConfidence), sizeof(DOUBLE), 1, pfTgtTrack);
				fwrite(&(iter->qtMsrArr[i].dTime), sizeof(DOUBLE), 1, pfTgtTrack);
			}
		}
	}

	fclose(pfTgtTrack);

	return 0;
}

INT32 SaveMsrPnt2File(MsrPntInfo& tMsrPnt, FILE* pfFile)
{
    //保存量测点迹
    if(pfFile == NULL)
    {
        ERROR("invalid argument, 'pfFile == NULL'");
        return -1;
    }

    fwrite(&(tMsrPnt.unIdx),          sizeof(tMsrPnt.unIdx),          1, pfFile);
    fwrite(&(tMsrPnt.dRng),           sizeof(tMsrPnt.dRng),           1, pfFile);
    fwrite(&(tMsrPnt.dAzi),           sizeof(tMsrPnt.dAzi),           1, pfFile);
    fwrite(&(tMsrPnt.dEle),           sizeof(tMsrPnt.dEle),           1, pfFile);
    fwrite(&(tMsrPnt.dAlt),           sizeof(tMsrPnt.dAlt),           1, pfFile);
    fwrite(&(tMsrPnt.dVr),            sizeof(tMsrPnt.dVr),            1, pfFile);
    fwrite(&(tMsrPnt.dPRT),           sizeof(tMsrPnt.dPRT),           1, pfFile);
    fwrite(&(tMsrPnt.dAmp),           sizeof(tMsrPnt.dAmp),           1, pfFile);
    fwrite(&(tMsrPnt.dSNR),           sizeof(tMsrPnt.dSNR),           1, pfFile);
    fwrite(&(tMsrPnt.tMsrStdDev),     sizeof(tMsrPnt.tMsrStdDev),     1, pfFile);
    fwrite(&(tMsrPnt.tDate.usYear),   sizeof(tMsrPnt.tDate.usYear),   1, pfFile);
	fwrite(&(tMsrPnt.tDate.ucMon),    sizeof(tMsrPnt.tDate.ucMon),    1, pfFile);
	fwrite(&(tMsrPnt.tDate.ucDay),    sizeof(tMsrPnt.tDate.ucDay),    1, pfFile);
	fwrite(&(tMsrPnt.tDate.ucHour),   sizeof(tMsrPnt.tDate.ucHour),   1, pfFile);
	fwrite(&(tMsrPnt.tDate.ucMin),    sizeof(tMsrPnt.tDate.ucMin),    1, pfFile);
	fwrite(&(tMsrPnt.tDate.ucSec),    sizeof(tMsrPnt.tDate.ucSec),    1, pfFile);
	fwrite(&(tMsrPnt.tDate.unMicroSec), sizeof(tMsrPnt.tDate.unMicroSec),   1, pfFile);
    fwrite(&(tMsrPnt.dTime),          sizeof(tMsrPnt.dTime),          1, pfFile);
    fwrite(&(tMsrPnt.unCPI),          sizeof(tMsrPnt.unCPI),          1, pfFile);
    fwrite(&(tMsrPnt.dAziMajor),      sizeof(tMsrPnt.dAziMajor),      1, pfFile);
    fwrite(&(tMsrPnt.dEleMajor),      sizeof(tMsrPnt.dEleMajor),      1, pfFile);
    fwrite(&(tMsrPnt.ucAntennaIdx),   sizeof(tMsrPnt.ucAntennaIdx),   1, pfFile);
    fwrite(&(tMsrPnt.enumBeamType),   sizeof(tMsrPnt.enumBeamType),   1, pfFile);
    fwrite(&(tMsrPnt.ucBeamCoordX),   sizeof(tMsrPnt.ucBeamCoordX),   1, pfFile);
    fwrite(&(tMsrPnt.ucBeamCoordY),   sizeof(tMsrPnt.ucBeamCoordY),   1, pfFile);
    fwrite(&(tMsrPnt.unRngCell),      sizeof(tMsrPnt.unRngCell),      1, pfFile);
    fwrite(&(tMsrPnt.unDplCell),      sizeof(tMsrPnt.unDplCell),      1, pfFile);
    fwrite(tMsrPnt.adRctMsr,          sizeof(tMsrPnt.adRctMsr),       1, pfFile);
    fwrite(tMsrPnt.adRctMsrCov,       sizeof(tMsrPnt.adRctMsrCov),    1, pfFile);
    fwrite(&(tMsrPnt.ucAssocFlag),    sizeof(tMsrPnt.ucAssocFlag),    1, pfFile);
    fwrite(&(tMsrPnt.ucCentroidFlag), sizeof(tMsrPnt.ucCentroidFlag), 1, pfFile);
    fwrite(&(tMsrPnt.dConfidence),    sizeof(tMsrPnt.dConfidence),    1, pfFile);

    return 0;
}

INT32 SaveTgtTrack(std::list<TgtTrackPntInfo>* pLTgtTrack, bool bSaveFlag, unsigned short ustrack_id)
{
	/*保存目标航迹*/

	if(!bSaveFlag)
		return 0;

	if(pLTgtTrack == NULL)
	{
		ERROR("invalid argument, 'pLTgtTrack == NULL'");
		return -2;
	}

	INT32 nTrackNum = 0;

	static FILE* pfTgtTrack = NULL;

	static char acFilePathTgtTrack[256] = "";

	static BOOL bCreateFileFlag = true;

	//获取时间
	time_t timeNowUTCSec = time(0);
	struct tm * ptLocalTime = localtime(&timeNowUTCSec);

	//创建路径，编辑文件名
	//static 类型的bCreateFileFlag确保文件仅创建一次
	if(bCreateFileFlag)
	{
		//创建父目录-目标航迹
		if( opendir("./TgtTrackdata") == NULL)
		{
			if( mkdir("./TgtTrackdata", 0777) < 0) //路径权限0777，文件所有者有rwx权限
			{
				ERROR("mkdir error");
				return -3;
			}
		}

		sprintf(acFilePathTgtTrack, "./TgtTrackdata/TgtTrack%d%02d%02d%02d%02d%02d.dat", ptLocalTime->tm_year+1900, ptLocalTime->tm_mon+1, ptLocalTime->tm_mday, ptLocalTime->tm_hour, ptLocalTime->tm_min, ptLocalTime->tm_sec);

		bCreateFileFlag = false;
	}

	//打开输入流-目标航迹
	if( (pfTgtTrack = fopen(acFilePathTgtTrack, "ab")) == NULL )
	{
		ERROR("fopen error, file path:{}", acFilePathTgtTrack);
		return -4;
	}

	UINT16 usNum = 0;
	if(ustrack_id == 0)//写入所有目标航迹
	{
		//写入各条目标航迹
		nTrackNum = pLTgtTrack->size();
		if(nTrackNum > 0) fwrite(&(nTrackNum), sizeof(nTrackNum), 1, pfTgtTrack);
		for(std::list<TgtTrackPntInfo>::iterator iter = pLTgtTrack->begin(); iter != pLTgtTrack->end(); iter++)
		{
			fwrite(&(iter->usTrackID), sizeof(iter->usTrackID), 1, pfTgtTrack);
			fwrite(&(iter->usFromTmpTrackID), sizeof(iter->usFromTmpTrackID), 1, pfTgtTrack);
			fwrite(&(iter->unStartFrm), sizeof(iter->unStartFrm), 1, pfTgtTrack);
			fwrite(&(iter->unLstUpdFrm), sizeof(iter->unLstUpdFrm), 1, pfTgtTrack);
			fwrite(&(iter->dLstUpdTime), sizeof(iter->dLstUpdTime), 1, pfTgtTrack);
			fwrite(&(iter->tLstUpdtDate.usYear), sizeof(iter->tLstUpdtDate.usYear), 1, pfTgtTrack);
			fwrite(&(iter->tLstUpdtDate.ucMon), sizeof(iter->tLstUpdtDate.ucMon), 1, pfTgtTrack);
			fwrite(&(iter->tLstUpdtDate.ucDay), sizeof(iter->tLstUpdtDate.ucDay), 1, pfTgtTrack);
			fwrite(&(iter->tLstUpdtDate.ucHour), sizeof(iter->tLstUpdtDate.ucHour), 1, pfTgtTrack);
			fwrite(&(iter->tLstUpdtDate.ucMin), sizeof(iter->tLstUpdtDate.ucMin), 1, pfTgtTrack);
			fwrite(&(iter->tLstUpdtDate.ucSec), sizeof(iter->tLstUpdtDate.ucSec), 1, pfTgtTrack);
			fwrite(&(iter->tLstUpdtDate.unMicroSec), sizeof(iter->tLstUpdtDate.unMicroSec), 1, pfTgtTrack);
			fwrite(&(iter->unLen), sizeof(iter->unLen), 1, pfTgtTrack);
			fwrite(&(iter->dModelNoise), sizeof(iter->dModelNoise), 1, pfTgtTrack);
			fwrite(&(iter->usTermntWndwLen), sizeof(iter->usTermntWndwLen), 1, pfTgtTrack);
			fwrite(&(iter->usMissNum), sizeof(iter->usMissNum), 1, pfTgtTrack);
			fwrite(&(iter->enumTrackUpdMode), sizeof(iter->enumTrackUpdMode), 1, pfTgtTrack);
			fwrite(&(iter->adXest), sizeof(iter->adXest), 1, pfTgtTrack);
			fwrite(&(iter->adPest), sizeof(iter->adPest), 1, pfTgtTrack);
			fwrite(&(iter->tRAEest), sizeof(iter->tRAEest), 1, pfTgtTrack);
			fwrite(&(iter->adXestENU), sizeof(iter->adXestENU), 1, pfTgtTrack);
			fwrite(&(iter->adPestENU), sizeof(iter->adPestENU), 1, pfTgtTrack);
			fwrite(&(iter->tRAEestENU), sizeof(iter->tRAEestENU), 1, pfTgtTrack);
			fwrite(&(iter->dPRT), sizeof(iter->dPRT), 1, pfTgtTrack);
			fwrite(&(iter->dVel), sizeof(iter->dVel), 1, pfTgtTrack);
			fwrite(&(iter->dAcc), sizeof(iter->dAcc), 1, pfTgtTrack);
			fwrite(&(iter->dVelAzi), sizeof(iter->dVelAzi), 1, pfTgtTrack);
			fwrite(&(iter->dAmp), sizeof(iter->dAmp), 1, pfTgtTrack);
			fwrite(&(iter->dSNR), sizeof(iter->dSNR), 1, pfTgtTrack);
			fwrite(&(iter->tPredRAE), sizeof(iter->tPredRAE), 1, pfTgtTrack);
			fwrite(&(iter->adInnov), sizeof(iter->adInnov), 1, pfTgtTrack);
			fwrite(&(iter->adS), sizeof(iter->adS), 1, pfTgtTrack);
			fwrite(&(iter->dMsr2GateDist), sizeof(iter->dMsr2GateDist), 1, pfTgtTrack);
			fwrite(&(iter->tPntAssociatd.unAntennaIdx), sizeof(iter->tPntAssociatd.unAntennaIdx), 1, pfTgtTrack);
			fwrite(&(iter->tPntAssociatd.unCPI), sizeof(iter->tPntAssociatd.unCPI), 1, pfTgtTrack);
			fwrite(&(iter->tPntAssociatd.unIdx), sizeof(iter->tPntAssociatd.unIdx), 1, pfTgtTrack);
			SaveMsrPnt2File(iter->tPntAssociatd.tMsrPnt, pfTgtTrack);
			if(iter->unLstUpdFrm == iter->unStartFrm){
				usNum = iter->LtMsrForInit.size();
				fwrite(&(usNum), sizeof(usNum), 1, pfTgtTrack);
				for(auto iterL = iter->LtMsrForInit.begin(); iterL != iter->LtMsrForInit.end();iterL++){
					SaveMsrPnt2File(*iterL, pfTgtTrack);
				}
			}
			else{
				fwrite(&(usNum), sizeof(usNum), 1, pfTgtTrack);
			}
		}	
	}
	else{
		//写入各条目标航迹
		nTrackNum = 1;
		fwrite(&(nTrackNum), sizeof(nTrackNum), 1, pfTgtTrack);
		for(std::list<TgtTrackPntInfo>::iterator iter = pLTgtTrack->begin(); iter != pLTgtTrack->end(); iter++)
		{
			if(iter->usTrackID != ustrack_id) continue;

			fwrite(&(iter->usTrackID), sizeof(iter->usTrackID), 1, pfTgtTrack);
			fwrite(&(iter->usFromTmpTrackID), sizeof(iter->usFromTmpTrackID), 1, pfTgtTrack);
			fwrite(&(iter->unStartFrm), sizeof(iter->unStartFrm), 1, pfTgtTrack);
			fwrite(&(iter->unLstUpdFrm), sizeof(iter->unLstUpdFrm), 1, pfTgtTrack);
			fwrite(&(iter->dLstUpdTime), sizeof(iter->dLstUpdTime), 1, pfTgtTrack);
			fwrite(&(iter->tLstUpdtDate.usYear), sizeof(iter->tLstUpdtDate.usYear), 1, pfTgtTrack);
			fwrite(&(iter->tLstUpdtDate.ucMon), sizeof(iter->tLstUpdtDate.ucMon), 1, pfTgtTrack);
			fwrite(&(iter->tLstUpdtDate.ucDay), sizeof(iter->tLstUpdtDate.ucDay), 1, pfTgtTrack);
			fwrite(&(iter->tLstUpdtDate.ucHour), sizeof(iter->tLstUpdtDate.ucHour), 1, pfTgtTrack);
			fwrite(&(iter->tLstUpdtDate.ucMin), sizeof(iter->tLstUpdtDate.ucMin), 1, pfTgtTrack);
			fwrite(&(iter->tLstUpdtDate.ucSec), sizeof(iter->tLstUpdtDate.ucSec), 1, pfTgtTrack);
			fwrite(&(iter->tLstUpdtDate.unMicroSec), sizeof(iter->tLstUpdtDate.unMicroSec), 1, pfTgtTrack);
			fwrite(&(iter->unLen), sizeof(iter->unLen), 1, pfTgtTrack);
			fwrite(&(iter->dModelNoise), sizeof(iter->dModelNoise), 1, pfTgtTrack);
			fwrite(&(iter->usTermntWndwLen), sizeof(iter->usTermntWndwLen), 1, pfTgtTrack);
			fwrite(&(iter->usMissNum), sizeof(iter->usMissNum), 1, pfTgtTrack);
			fwrite(&(iter->enumTrackUpdMode), sizeof(iter->enumTrackUpdMode), 1, pfTgtTrack);
			fwrite(&(iter->adXest), sizeof(iter->adXest), 1, pfTgtTrack);
			fwrite(&(iter->adPest), sizeof(iter->adPest), 1, pfTgtTrack);
			fwrite(&(iter->tRAEest), sizeof(iter->tRAEest), 1, pfTgtTrack);
			fwrite(&(iter->adXestENU), sizeof(iter->adXestENU), 1, pfTgtTrack);
			fwrite(&(iter->adPestENU), sizeof(iter->adPestENU), 1, pfTgtTrack);
			fwrite(&(iter->tRAEestENU), sizeof(iter->tRAEestENU), 1, pfTgtTrack);
			fwrite(&(iter->dPRT), sizeof(iter->dPRT), 1, pfTgtTrack);
			fwrite(&(iter->dVel), sizeof(iter->dVel), 1, pfTgtTrack);
			fwrite(&(iter->dAcc), sizeof(iter->dAcc), 1, pfTgtTrack);
			fwrite(&(iter->dVelAzi), sizeof(iter->dVelAzi), 1, pfTgtTrack);
			fwrite(&(iter->dAmp), sizeof(iter->dAmp), 1, pfTgtTrack);
			fwrite(&(iter->dSNR), sizeof(iter->dSNR), 1, pfTgtTrack);
			fwrite(&(iter->tPredRAE), sizeof(iter->tPredRAE), 1, pfTgtTrack);
			fwrite(&(iter->adInnov), sizeof(iter->adInnov), 1, pfTgtTrack);
			fwrite(&(iter->adS), sizeof(iter->adS), 1, pfTgtTrack);
			fwrite(&(iter->dMsr2GateDist), sizeof(iter->dMsr2GateDist), 1, pfTgtTrack);
			fwrite(&(iter->tPntAssociatd.unAntennaIdx), sizeof(iter->tPntAssociatd.unAntennaIdx), 1, pfTgtTrack);
			fwrite(&(iter->tPntAssociatd.unCPI), sizeof(iter->tPntAssociatd.unCPI), 1, pfTgtTrack);
			fwrite(&(iter->tPntAssociatd.unIdx), sizeof(iter->tPntAssociatd.unIdx), 1, pfTgtTrack);
			SaveMsrPnt2File(iter->tPntAssociatd.tMsrPnt, pfTgtTrack);
			if(iter->unLstUpdFrm == iter->unStartFrm){
				usNum = iter->LtMsrForInit.size();
				fwrite(&(usNum), sizeof(usNum), 1, pfTgtTrack);
				for(auto iterL = iter->LtMsrForInit.begin(); iterL != iter->LtMsrForInit.end();iterL++){
					SaveMsrPnt2File(*iterL, pfTgtTrack);
				}
			}
			else{
				fwrite(&(usNum), sizeof(usNum), 1, pfTgtTrack);
			}
			break;
		}
	}

	/*写入目标航迹*/


	fclose(pfTgtTrack);

	return 0;
}

int get_circlular_dis(int x1, int x2) {
    int diff = std::abs(x1 - x2);
    // 比较“直接跨越的距离”和“绕过边界的距离”，取最小值
    return std::min(diff, MAX_XCOORD - diff);
}

BOOL isAssociableForTmpTrack(std::list<TmpTrackInfo>::iterator iter, OneAziMajorMsrInfo* ptOneAziMajorMsrPnt)
{
	//检查暂态航迹与特定方位的量测数据的可关联性

	if (ptOneAziMajorMsrPnt == NULL)
	{
		ERROR("invalid argument, 'ptOneAziMajorMsrPnt == NULL'");
		return false;
	}

	//暂态航迹位于左边界
	if (iter->enumBeamType == LEFT_EDGE)
	{
		if (ptOneAziMajorMsrPnt->ucAntennaIdx == iter->ucAntennaIdx && get_circlular_dis(ptOneAziMajorMsrPnt->ucBeamCoordX, iter->ucBeamCoordX) <= MAX_XCOORD_INTERVEL)
			return true;
		else if (ptOneAziMajorMsrPnt->ucAntennaIdx == GetPrevAntennaIdx(iter->ucAntennaIdx) && ptOneAziMajorMsrPnt->enumEdgeTypeInData == RIGHT_EDGE)
			return true;
		else
			return false;
	}

	//暂态航迹位于右边界
	else if (iter->enumBeamType == RIGHT_EDGE)
	{
		if (ptOneAziMajorMsrPnt->ucAntennaIdx == iter->ucAntennaIdx && get_circlular_dis(ptOneAziMajorMsrPnt->ucBeamCoordX, iter->ucBeamCoordX) <= MAX_XCOORD_INTERVEL)
			return true;
		else if (ptOneAziMajorMsrPnt->ucAntennaIdx == GetNextAntennaIdx(iter->ucAntennaIdx) && ptOneAziMajorMsrPnt->enumEdgeTypeInData == LEFT_EDGE)
			return true;
		else
			return false;
	}

	//暂态航迹位于中间
	else if (iter->enumBeamType == MIDDLE)
	{
		if (ptOneAziMajorMsrPnt->ucAntennaIdx == iter->ucAntennaIdx && get_circlular_dis(ptOneAziMajorMsrPnt->ucBeamCoordX, iter->ucBeamCoordX) <= MAX_XCOORD_INTERVEL)
			return true;
		else
			return false;
	}

	else
	{
		ERROR("invalid argument, 'iter->enumBeamType({})', it can only be -1, 0 or 1.", iter->enumBeamType);
		return false;
	}
}

UINT32 GetTrackNumTAS(std::list<TgtTrackPntInfo>* pLTgtTrack)
{
	//统计通过TAS模式维持的航迹数量

	UINT32 unTrackNumTAS = 0;

	for (auto iter = pLTgtTrack->begin(); iter != pLTgtTrack->end(); iter++) {
		if (iter->enumPredScheduleEvent == TRACK)
			unTrackNumTAS++;
	}

	return unTrackNumTAS;
}

TgtTrackNumInfo TrackCount(std::list<TgtTrackPntInfo>* pLTgtTrack)
{
	//统计各类目标航迹数量
	TgtTrackNumInfo tTgtTrackNum = {0, 0, 0};

	for (auto iter = pLTgtTrack->begin(); iter != pLTgtTrack->end(); iter++) {
		if (iter->enumPredScheduleEvent == TRACK)
			tTgtTrackNum.unNumTrack++;
		else if(iter->enumPredScheduleEvent == CONFIRM)
			tTgtTrackNum.unNumConfirm++;
		else if(iter->enumPredScheduleEvent == SEARCH)
			tTgtTrackNum.unNumSearch++;
	}

	return tTgtTrackNum;
}

INT32 TmpTrackUpdt(std::list<TmpTrackInfo>::iterator iter, OneAziMajorMsrInfo* ptOneAziMajorMsrPnt)
{
	//暂态航迹更新

	if (ptOneAziMajorMsrPnt == NULL)
	{
		ERROR("invalid argument, 'ptOneAziMajorMsrPnt == NULL'");
		return -1;
	}

	UINT32 unMsrPntIdxOpt = 0;
	if(iter->usPntNum > 1)
		unMsrPntIdxOpt = GetNearestMsrPntIdxForTmpTrack(iter, ptOneAziMajorMsrPnt);
	else if(iter->usPntNum == 1)
		unMsrPntIdxOpt = GetSNRMaxMsrPntIdxForTmpTrack(iter, ptOneAziMajorMsrPnt);
	else{
		ERROR("tmp track length is less than 1");
		return -2;
	}

	if(unMsrPntIdxOpt != INF){
		ptOneAziMajorMsrPnt->atMsrPntArr[unMsrPntIdxOpt].ucAssocFlag = 1;

		if(!iter->bAssocFlag)
			AddMsrPnt2TmpTrack(iter, ptOneAziMajorMsrPnt->atMsrPntArr+unMsrPntIdxOpt);
	}

	return 0;
}

BOOL BindTgtTrackWithTmpTrack(TmpTrackInfo& tOneTmpTrack, TgtTrackPntInfo& tOneTgtTrackPnt)
{
	//记录目标航迹与暂态航迹间对应关系

	tOneTmpTrack.usToTrackID = tOneTgtTrackPnt.usTrackID;
	tOneTgtTrackPnt.usFromTmpTrackID =tOneTmpTrack.usPseudoTrackID;

	return true;
}

INT32 TryInit(std::list<TmpTrackInfo>::iterator iter, std::list<TgtTrackPntInfo>& LTgtTrack)
{
	//检查候选航迹起批状态

	UINT8 ucInitPntNumTH = 0;
	BOOL bCheckState = true;
	switch (iter->enumInitMode) {
		case QUICK:
			ucInitPntNumTH = g_tAlgorithmPara.ucQuickInitPntNumTH;
			bCheckState    = QuickCheckTmpTrack(iter);
			break;
		case STANDARD:
			ucInitPntNumTH = g_tAlgorithmPara.ucStandardInitPntNumTH;
			bCheckState    = QuickCheckTmpTrack(iter);
			break;
		case SLOW:
			ucInitPntNumTH = g_tAlgorithmPara.ucSlowInitPntNumTH;
			//bCheckState    = SlowCheckTmpTrack(iter);
			break;
		default:
			ERROR("invalid argument, iter->enumInitMode:{}, it can only be {}, {} or {}", iter->enumInitMode, QUICK, STANDARD, SLOW);
			break;
	}

	//点迹数量检查
	if (iter->usPntNum < ucInitPntNumTH)
		return 0;

	//点迹序列一致性检查
	if (bCheckState && LTgtTrack.size() < MAX_TGT_NUM) {
		iter->enumTmpTrackState = TURN_TO_TGT_TRACK;

		TgtTrackPntInfo tOneTgtTrackPnt;
		TmpTrack2TgtTrack(iter, tOneTgtTrackPnt);
		tOneTgtTrackPnt.usTrackID = GetTrackID(g_qusTrackID, g_vusTrackIDUsed);
		BindTgtTrackWithTmpTrack(*iter, tOneTgtTrackPnt);

		LTgtTrack.emplace_back(tOneTgtTrackPnt);

		//debug
		TrackSendMsg msg;
		msg.unTriggerNum = LTgtTrack.back().usTrackID;
		msg.pTrackList = std::make_shared<std::list<TgtTrackPntInfo>>(LTgtTrack);
		g_qTrackSendMsg.push(msg);

		if (g_tSysPara.enumWorkMode==TAS && GetTrackNumTAS(&LTgtTrack) < MAX_TAS_NUM) {
			LTgtTrack.back().enumPredScheduleEvent = CONFIRM;
			g_qunSendScheduleRqstrigger.push(LTgtTrack.back().usTrackID);
		}

		SaveTmpTrack(&g_LTmpTrack, true, iter->usPseudoTrackID);
		SaveTgtTrack(&g_LTgtTrack, true, tOneTgtTrackPnt.usTrackID);
	}
	else
		iter->enumTmpTrackState = CANCEL;

	return 0;
}

INT32 MsrPnt2TmpTrack(OneAziMajorMsrInfo* ptOneAziMajorMsrPnt, std::list<TmpTrackInfo>& LTmpTrack)
{
	//量测点迹新建暂态航迹

	for(UINT32 i = 0; i < ptOneAziMajorMsrPnt->unMsrPntNum; i++)
	{
		if(ptOneAziMajorMsrPnt->atMsrPntArr[i].ucAssocFlag == 0 && fabs(ptOneAziMajorMsrPnt->atMsrPntArr[i].dVr)>g_tAlgorithmPara.dVrMin && ptOneAziMajorMsrPnt->atMsrPntArr[i].dConfidence>g_tAlgorithmPara.dConfidenceTH)
		{
			TmpTrackInfo tOneTmpTrack;

			GetNewTmpTrack(ptOneAziMajorMsrPnt->atMsrPntArr+i, tOneTmpTrack);

			tOneTmpTrack.usPseudoTrackID = GetTrackID(g_qusPseudoTrackID, g_vusPseudoTrackIDUsed);

			LTmpTrack.emplace_back(tOneTmpTrack);
		}
	}

	return 0;
}

INT32 TrackInitTWS(OneAziMajorMsrInfo* ptOneAziMajorMsrPnt, std::list<TmpTrackInfo>& LTmpTrack, std::list<TgtTrackPntInfo>& LTgtTrack)
{
	/*TWS航迹起始*/

	//已有候选航迹执行起始逻辑
	for(std::list<TmpTrackInfo>::iterator iter = LTmpTrack.begin(); iter != LTmpTrack.end(); iter++) {
		if (! isAssociableForTmpTrack(iter, ptOneAziMajorMsrPnt) )
			continue;

		if (TmpTrackUpdt(iter, ptOneAziMajorMsrPnt) < 0)
			continue;

		TryInit(iter, LTgtTrack);
	}

	//剩余点迹起始新候选航迹
	MsrPnt2TmpTrack(ptOneAziMajorMsrPnt, LTmpTrack);

	return 0;
}


INT32 ResetTermntWnd(std::list<TgtTrackPntInfo>::iterator iter, AlgorithmParaInfo tDPAlgrthmPara)
{
	/*重置航迹终结窗，航迹终结窗长度已达到航迹外推次数阈值，关联失败次数小于外推次数(n/n准则)，重置航迹终结窗*/

	if (iter->usTermntWndwLen >= tDPAlgrthmPara.ucTermntWindwLenTH && iter->usMissNum < tDPAlgrthmPara.ucTermntPntNumTH)
	{
		iter->usMissNum = 0;
		iter->usTermntWndwLen = 0;
	}

	return 0;
}

INT32 StatePred(DOUBLE adXest[], UINT32 unStateDim, DOUBLE adPest[], DOUBLE ddt, DOUBLE dModelNoise, DOUBLE adXpre[], DOUBLE adPpre[])
{
	/*目标状态预测*/

	DOUBLE* adF = new DOUBLE[unStateDim * unStateDim];
	GetStateTransMat(ddt, adF);
	cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, unStateDim, 1, unStateDim, 1, adF, unStateDim, adXest, 1, 0, adXpre, 1);
	DOUBLE* adQ = new DOUBLE[unStateDim * unStateDim];
	GetModelNoiseCov(ddt, dModelNoise, adQ);
	GetTransformCov(adF, unStateDim, adPest, unStateDim, adQ, adPpre);

	delete[] adF;
	delete[] adQ;

	return 0;
}

INT32 MsrPred(DOUBLE adXpre[], UINT32 unStateDim, UINT32 unOneCoordStateDim, DOUBLE adPpre[], DOUBLE adRctMsrCov[], UINT32 unMsrDim, DOUBLE adZpre[], DOUBLE adS[])
{
	/*量测预测*/

	for (UINT32 i = 0; i < unMsrDim; i++)
	{
		adZpre[i] = adXpre[i * unOneCoordStateDim];
	}

	DOUBLE* adH = new DOUBLE[unMsrDim * unStateDim];
	GetJacobiH(unMsrDim, unStateDim, adH);
	GetTransformCov(adH, unMsrDim, adPpre, unStateDim, adRctMsrCov, adS);
	delete[] adH;

	return 0;
}

UINT32 GetNearestMsrPntIdxForTgtTrack(std::list<TgtTrackPntInfo>::iterator iter, OneCPIMsrInfo* ptOneCircleMsrPnt)
{
	//最近邻数据关联
	UINT32 unMsrPntIdxNearest = INF;

	if(ptOneCircleMsrPnt->unMsrPntNum == 0)
		return unMsrPntIdxNearest;

	//建立波门
	DOUBLE dt = ptOneCircleMsrPnt->dTime - iter->dLstUpdTime;
	if(dt <= 0)
	{
		ERROR("invalid time interval({}), time({}) of measure point must be greater than the time({}) of target track", dt, ptOneCircleMsrPnt->atMsrPntArr[0].dTime, iter->dLstUpdTime);
		return unMsrPntIdxNearest;
	}

	DOUBLE adXpre[STATE_DIM] = {0}, adPpre[STATE_DIM*STATE_DIM] = {0};
	StatePred(iter->adXest, STATE_DIM, iter->adPest, dt, g_tAlgorithmPara.dModelNoise, adXpre, adPpre);
	DOUBLE adZpre[MSR_DIM] = {0}, adS[MSR_DIM * MSR_DIM] = {0};
	MsrPred(adXpre, STATE_DIM, ONE_COORD_STATE_DIM, adPpre, ptOneCircleMsrPnt->atMsrPntArr[0].adRctMsrCov, MSR_DIM, adZpre, adS);

	//最近邻数据关联
	DOUBLE dDist2Gate = INF, dDistMin = INF;
	for(UINT32 i = 0; i < ptOneCircleMsrPnt->unMsrPntNum; i++)
	{
		if(ptOneCircleMsrPnt->atMsrPntArr[i].ucAssocFlag == 1)
			continue;

		if( !( TgtTrackAssocMsrPntRAE(iter, ptOneCircleMsrPnt->atMsrPntArr+i) ) )
			continue;

		dDist2Gate = Msr2GateDist(ptOneCircleMsrPnt->atMsrPntArr+i, adZpre, adS);

		if(dDist2Gate >=0 && dDist2Gate < dDistMin)
		{
			dDistMin = dDist2Gate;
			iter->dMsr2GateDist = dDist2Gate;

			if(dDist2Gate <= g_tAlgorithmPara.dGamma)
				unMsrPntIdxNearest = i;
		}

	}

	return unMsrPntIdxNearest;
}

BOOL isSameClusterForTgtTrack(std::list<TgtTrackPntInfo>::iterator iter, MsrPntInfo* ptMsrPnt)
{
	/*判断点迹与目标航迹是否满足凝聚条件*/

	if(ptMsrPnt->ucAssocFlag == 1)	
		return false;

	if( fabs(ptMsrPnt->dRng - iter->tRAEest.dRngEst) > g_tAlgorithmPara.dRngCentroidTH )
		return false;

	if( fabs(ptMsrPnt->dAzi - iter->tRAEest.dAziEst) > g_tAlgorithmPara.dAziCentroidTH )
		return false;

	if( fabs(ptMsrPnt->dEle - iter->tRAEest.dEleEst) > g_tAlgorithmPara.dEleCentroidTH )
		return false;

	if( fabs(ptMsrPnt->dVr - iter->tRAEest.dVrEst) > g_tAlgorithmPara.dVrCentroidTH )
		return false;

	return true;
}

UINT32 GetNearestMsrPntIdxForTgtTrackInOneCPI(std::list<TgtTrackPntInfo>::iterator iter, OneCPIMsrInfo* ptOneCPIMsrPnt)
{
	//最近邻数据关联-一个CPI
	UINT32 unMsrPntIdxNearest = INF;

	if(ptOneCPIMsrPnt->unMsrPntNum == 0)
		return unMsrPntIdxNearest;

	DOUBLE dt = ptOneCPIMsrPnt->dTime - iter->dLstUpdTime;

	if(dt > 0)
	{
		DOUBLE adXpre[STATE_DIM] = {0}, adPpre[STATE_DIM*STATE_DIM] = {0};
		StatePred(iter->adXest, STATE_DIM, iter->adPest, dt, g_tAlgorithmPara.dModelNoise, adXpre, adPpre);
		DOUBLE adZpre[MSR_DIM] = {0}, adS[MSR_DIM * MSR_DIM] = {0};
		MsrPred(adXpre, STATE_DIM, ONE_COORD_STATE_DIM, adPpre, ptOneCPIMsrPnt->atMsrPntArr[0].adRctMsrCov, MSR_DIM, adZpre, adS);

		//最近邻数据关联
		DOUBLE dDist2Gate = INF, dDistMin = INF;
		for(UINT32 i = 0; i < ptOneCPIMsrPnt->unMsrPntNum; i++)
		{
			if(ptOneCPIMsrPnt->atMsrPntArr[i].ucAssocFlag == 1)
				continue;

			if( !( TgtTrackAssocMsrPntRAE(iter, ptOneCPIMsrPnt->atMsrPntArr+i) ) )
				continue;

			dDist2Gate = Msr2GateDist(ptOneCPIMsrPnt->atMsrPntArr+i, adZpre, adS);

			if(dDist2Gate >=0 && dDist2Gate < dDistMin)
			{
				dDistMin = dDist2Gate;
				iter->dMsr2GateDist = dDist2Gate;

				if(dDist2Gate <= g_tAlgorithmPara.dGamma)
					unMsrPntIdxNearest = i;
			}
		}
	}

	else if(dt == 0)
	{
		for(UINT32 i = 0; i < ptOneCPIMsrPnt->unMsrPntNum; i++)
		{
			if( isSameClusterForTgtTrack(iter, ptOneCPIMsrPnt->atMsrPntArr+i) )
				ptOneCPIMsrPnt->atMsrPntArr[i].ucAssocFlag = 1;
		}
	}

	else
	{
		//		ERROR("invalid time interval({:.4f}), measure time:{:.4f}, track time:{:.4f}", dt, ptOneCPIMsrPnt->dTime, iter->dLstUpdTime);
	}

	return unMsrPntIdxNearest;
}


UINT32 GetNearestMsrPntIdxForTgtTrackInOneAziMajor(std::list<TgtTrackPntInfo>::iterator iter, OneAziMajorMsrInfo* ptOneAziMajorMsrPnt)
{
	//最近邻数据关联-一个CPI
	UINT32 unMsrPntIdxNearest = INF;

	if(ptOneAziMajorMsrPnt->unMsrPntNum == 0)
		return unMsrPntIdxNearest;

	DOUBLE dt = ptOneAziMajorMsrPnt->atMsrPntArr[0].dTime - iter->dLstUpdTime;

	if(dt > 0)
	{
		DOUBLE adXpre[STATE_DIM] = {0}, adPpre[STATE_DIM*STATE_DIM] = {0};
		StatePred(iter->adXest, STATE_DIM, iter->adPest, dt, g_tAlgorithmPara.dModelNoise, adXpre, adPpre);
		DOUBLE adZpre[MSR_DIM] = {0}, adS[MSR_DIM * MSR_DIM] = {0};
		MsrPred(adXpre, STATE_DIM, ONE_COORD_STATE_DIM, adPpre, ptOneAziMajorMsrPnt->atMsrPntArr[0].adRctMsrCov, MSR_DIM, adZpre, adS);

		//debug
		BOOL bFlag = false;//(iter->tRAEest.dVrEst) > 12 && fabs(iter->tRAEest.dVrEst) < 20 && fabs(ptOneAziMajorMsrPnt->atMsrPntArr[i].dVr) > 12 && fabs(ptOneAziMajorMsrPnt->atMsrPntArr[i].dVr) < 20;
		if(bFlag)
		{
			INFO("ID:{}, R:{:.2f}, A:{:.2f}, E:{:.2f}, Vr:{:.2f}, SNR:{:.2f}, CPI:{}, Idx:{}, Time:{:.3f}",
					iter->usTrackID, iter->tRAEest.dRngEst, iter->tRAEest.dAziEst, iter->tRAEest.dEleEst, iter->tRAEest.dVrEst, iter->dSNR, iter->tPntAssociatd.unCPI, iter->tPntAssociatd.unIdx, iter->dLstUpdTime);
		}

		//最近邻数据关联
		DOUBLE dDist2Gate = INF, dDistMin = INF;
		for(UINT32 i = 0; i < ptOneAziMajorMsrPnt->unMsrPntNum; i++)
		{
			//debug
			// bFlag = iter->usTrackID == 7 && ptOneAziMajorMsrPnt->atMsrPntArr[i].unCPI == 848284;//591511;

			if(bFlag)
			{
				INFO("CPI:{}, Pnt{}, R:{:.2f}, A:{:.2f}, E:{:.2f}, Vr:{:.2f}, SNR:{:.2f}, Time:{:.3f}",
						ptOneAziMajorMsrPnt->atMsrPntArr[i].unCPI, i, ptOneAziMajorMsrPnt->atMsrPntArr[i].dRng, ptOneAziMajorMsrPnt->atMsrPntArr[i].dAzi, ptOneAziMajorMsrPnt->atMsrPntArr[i].dEle, ptOneAziMajorMsrPnt->atMsrPntArr[i].dVr, ptOneAziMajorMsrPnt->atMsrPntArr[i].dSNR, ptOneAziMajorMsrPnt->atMsrPntArr[i].dTime);
			}

			if(ptOneAziMajorMsrPnt->atMsrPntArr[i].ucAssocFlag == 1)
				continue;

			if( !( TgtTrackAssocMsrPntRAE(iter, ptOneAziMajorMsrPnt->atMsrPntArr+i) ) )
				continue;

			dDist2Gate = Msr2GateDist(ptOneAziMajorMsrPnt->atMsrPntArr+i, adZpre, adS);

			if(dDist2Gate >=0 && dDist2Gate < dDistMin)
			{
				dDistMin = dDist2Gate;
				iter->dMsr2GateDist = dDist2Gate;

				if(dDist2Gate <= g_tAlgorithmPara.dGamma)
					unMsrPntIdxNearest = i;
			}
		}
	}

	else if(dt == 0)
	{
		for(UINT32 i = 0; i < ptOneAziMajorMsrPnt->unMsrPntNum; i++)
		{
			if( isSameClusterForTgtTrack(iter, ptOneAziMajorMsrPnt->atMsrPntArr+i) )
				ptOneAziMajorMsrPnt->atMsrPntArr[i].ucAssocFlag = 1;
		}
	}

	else
	{
		//		ERROR("invalid time interval({:.4f}), measure time: {:.4f}, track time:{:.4f}", dt, ptOneCPIMsrPnt->dTime, iter->dLstUpdTime);
	}

	return unMsrPntIdxNearest;
}


INT32 CheckCovMatPositive(DOUBLE* ptradCovIN, UINT16 usMsrDim)
{
	/*检查协方差矩阵正定性*/

	for(UINT16 i = 0; i < usMsrDim; i++)
	{
		if(ptradCovIN[i*usMsrDim + i] < 0)
		{
			return -(i); //返回负数,代表第n行的对角线元素为负数
		}
	}

	return 0;
}

INT32 GetMatInverse(DOUBLE adMat[], UINT32 unRowNum, UINT32 unColNum)
{
	/*矩阵求逆（原地）*/

	INT32* anIpiv = new INT32[unRowNum];

	//LU分解
	LAPACKE_dgetrf(CblasRowMajor, unRowNum, unColNum, adMat, unColNum, anIpiv);

	//矩阵求逆
	LAPACKE_dgetri(CblasRowMajor, unRowNum, adMat, unColNum, anIpiv);

	delete[] anIpiv;

	return 0;
}


INT32 GetKalmanGain(DOUBLE adPpre[], UINT32 unStateDim, DOUBLE adH[], UINT32 unMsrDim, DOUBLE adS[], DOUBLE adK[])
{
	/*计算卡尔曼增益
		K = Ppre x H' x inv(S)*/

	DOUBLE* adTmp = new DOUBLE[unStateDim * unMsrDim];
	cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasTrans, unStateDim, unMsrDim, unStateDim, 1, adPpre, unStateDim, adH, unStateDim, 0, adTmp, unMsrDim);

	DOUBLE* adSCPY = new DOUBLE[unMsrDim * unMsrDim];
	memcpy(adSCPY, adS, sizeof(DOUBLE) * unMsrDim * unMsrDim);
	GetMatInverse(adSCPY, unMsrDim, unMsrDim);

	cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, unStateDim, unMsrDim, unMsrDim, 1, adTmp, unMsrDim, adSCPY, unMsrDim, 0, adK, unMsrDim);

	delete[] adTmp;
	delete[] adSCPY;

	return 0;
}

INT32 GetFilteringCov(DOUBLE adK[], UINT32 unStateDim, UINT32 unMsrDim, DOUBLE adH[], DOUBLE adPpre[], DOUBLE adPest[])
{
	/*计算滤波后目标状态协方差
		Pest = Ppre - K x H x Ppre*/

	DOUBLE* adTmp = new DOUBLE[unStateDim * unStateDim];
	bzero(adTmp, sizeof(DOUBLE)*unStateDim*unStateDim);
	cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, unStateDim, unStateDim, unMsrDim, 1, adK, unMsrDim, adH, unStateDim, 0, adTmp, unStateDim);

	memcpy(adPest, adPpre, sizeof(DOUBLE) * unStateDim * unStateDim);
	cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, unStateDim, unStateDim, unStateDim, -1, adTmp, unStateDim, adPpre, unStateDim, 1, adPest, unStateDim);

	delete[] adTmp;

	return 0;
}

INT32 GetInnov(DOUBLE adXest[], UINT32 unStateDim, UINT32 unOneCoordStateDim, DOUBLE adPest[], DOUBLE ddt, DOUBLE dModelNoise, MsrPntInfo* ptMsrPnt, UINT32 unMsrDim, DOUBLE adInnov[], DOUBLE adS[])
{
	//计算新息

	//状态预测
	DOUBLE* adXpre = new DOUBLE[unStateDim];
	DOUBLE* adPpre = new DOUBLE[unStateDim * unStateDim];
	StatePred(adXest, unStateDim, adPest, ddt, dModelNoise, adXpre, adPpre);

	//量测预测
	DOUBLE* adZpre = new DOUBLE[unMsrDim];
	// DOUBLE* adS = new DOUBLE[unMsrDim * unMsrDim];
	MsrPred(adXpre, unStateDim, unOneCoordStateDim, adPpre, ptMsrPnt->adRctMsrCov, unMsrDim, adZpre, adS);

	//计算新息
	memcpy(adInnov, ptMsrPnt->adRctMsr, sizeof(DOUBLE) * unMsrDim);
	cblas_daxpy(unMsrDim, -1, adZpre, 1, adInnov, 1);

	delete[] adXpre;
	delete[] adPpre;
	delete[] adZpre;
	// delete[] adS;

	return 0;
}

INT32 KalmanFilter(DOUBLE adXest[], UINT32 unStateDim, UINT32 unOneCoordStateDim, DOUBLE adPest[], DOUBLE ddt, DOUBLE dModelNoise, MsrPntInfo* ptMsrPnt, UINT32 unMsrDim)
{
	/*卡尔曼滤波器，更新目标状态和协方差(原地)*/

	//状态预测
	DOUBLE* adXpre = new DOUBLE[unStateDim];
	DOUBLE* adPpre = new DOUBLE[unStateDim * unStateDim];
	StatePred(adXest, unStateDim, adPest, ddt, dModelNoise, adXpre, adPpre);

	INT32 nCheckCode = 0;
	if( (nCheckCode = CheckCovMatPositive(adPpre, STATE_DIM)) < 0)
	{
		ERROR("invalid cov mat 'adPpre', adPpre[{}][{}] == {}", -nCheckCode, -nCheckCode, adPpre[-nCheckCode*STATE_DIM-nCheckCode]);
	}


	//量测预测
	DOUBLE* adZpre = new DOUBLE[unMsrDim];
	DOUBLE* adS = new DOUBLE[unMsrDim * unMsrDim];
	MsrPred(adXpre, unStateDim, unOneCoordStateDim, adPpre, ptMsrPnt->adRctMsrCov, unMsrDim, adZpre, adS);

	if( (nCheckCode = CheckCovMatPositive(adS, MSR_DIM)) < 0)
	{
		ERROR("invalid cov mat 'adS', adS[{}][{}] == {}", -nCheckCode, -nCheckCode, adS[-nCheckCode*MSR_DIM-nCheckCode]);
	}

	//计算新息
	DOUBLE* adInnov = new DOUBLE[unMsrDim];
	memcpy(adInnov, ptMsrPnt->adRctMsr, sizeof(DOUBLE) * unMsrDim);
	cblas_daxpy(unMsrDim, -1, adZpre, 1, adInnov, 1);

	//计算增益
	DOUBLE* adH = new DOUBLE[unMsrDim * unStateDim];
	GetJacobiH(unMsrDim, unStateDim, adH);
	DOUBLE* adK = new DOUBLE[unStateDim * unMsrDim];
	GetKalmanGain(adPpre, unStateDim, adH, unMsrDim, adS, adK);

	//状态滤波
	memcpy(adXest, adXpre, sizeof(DOUBLE) * unStateDim);
	cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, unStateDim, 1, unMsrDim, 1, adK, unMsrDim, adInnov, 1, 1, adXest, 1);

	//计算滤波协方差
	GetFilteringCov(adK, unStateDim, unMsrDim, adH, adPpre, adPest);

	if( (nCheckCode = CheckCovMatPositive(adPest, STATE_DIM)) < 0)
	{
		ERROR("invalid cov mat 'adPest', adPest[{}][{}] == {}", -nCheckCode, -nCheckCode, adPest[-nCheckCode*STATE_DIM-nCheckCode]);
	}

	delete[] adXpre;
	delete[] adPpre;
	delete[] adInnov;
	delete[] adZpre;
	delete[] adS;
	delete[] adK;
	delete[] adH;

	return 0;
}



INT32 CpyRAE2PredRAE(RAEestInfo* ptRAEest, PredRAEInfo& tPredRAE)
{
	//拷贝估计的RAE至预测RAE

	if(ptRAEest == NULL)
	{
		ERROR("invalid argument, 'ptRAEest == NULL'");
		return -1;
	}

	tPredRAE.dRngPre = ptRAEest->dRngEst;
	tPredRAE.dAziPre = ptRAEest->dAziEst;
	tPredRAE.dElePre = ptRAEest->dEleEst;
	tPredRAE.dVrPre  = ptRAEest->dVrEst;

	return 0;
}

INT32 TgtTrackUpdt(std::list<TgtTrackPntInfo>::iterator iter, OneAziMajorMsrInfo* ptOneAziMajorMsrPnt, UINT32 unMsrPntIdxNearest, TgtTrackUpdtModeInfo TgtTrackUpdMode)
{
	/*更新目标航迹--滤波、预测*/

	DOUBLE ddt = 0;
	//	DOUBLE adXpre[STATE_DIM] = { 0 }, adPpre[STATE_DIM*STATE_DIM] = { 0 };
	MsrPntInfo* ptMsrPnt = NULL;

	switch (TgtTrackUpdMode)
	{
	case FILTER:
		ddt = ptOneAziMajorMsrPnt->atMsrPntArr[unMsrPntIdxNearest].dTime - iter->dLstUpdTime;
		if(ddt < 0)
		{
			ERROR("invalid time interval({}), time({}) of measure point must be greater than the time({}) of target track", ddt, ptOneAziMajorMsrPnt->atMsrPntArr[unMsrPntIdxNearest].dTime, iter->dLstUpdTime);
			return -1;
		}

		ptMsrPnt = ptOneAziMajorMsrPnt->atMsrPntArr+unMsrPntIdxNearest;
		iter->unLstUpdFrm = ptOneAziMajorMsrPnt->atMsrPntArr[unMsrPntIdxNearest].unCPI;
		iter->tLstUpdtDate    = ptOneAziMajorMsrPnt->atMsrPntArr[unMsrPntIdxNearest].tDate;
		iter->unLen += 1;
		iter->dLstUpdTime = ptOneAziMajorMsrPnt->atMsrPntArr[unMsrPntIdxNearest].dTime;
		iter->dModelNoise = g_tAlgorithmPara.dModelNoise;
		iter->enumTrackUpdMode = FILTER;
		iter->enumScheduleEvent = g_tSysPara.enumScheduleEvent;
		iter->enumWorkFashion = g_tSysPara.enumWorkMode;
		memcpy(iter->adXestLast2, iter->adXest, sizeof(DOUBLE)*STATE_DIM);
		GetInnov(iter->adXest, STATE_DIM, ONE_COORD_STATE_DIM, iter->adPest, ddt, g_tAlgorithmPara.dModelNoise, ptMsrPnt, MSR_DIM, iter->adInnov, iter->adS);
		KalmanFilter(iter->adXest, STATE_DIM, ONE_COORD_STATE_DIM, iter->adPest, ddt, g_tAlgorithmPara.dModelNoise, ptMsrPnt, MSR_DIM);
		iter->tRAEest.dRngEst = Xest2Rng(iter->adXest, MSR_DIM, ONE_COORD_STATE_DIM);
		iter->tRAEest.dAziEst = Xest2Azi180(iter->adXest, ONE_COORD_STATE_DIM);
		iter->tRAEest.dEleEst = Xest2Ele(iter->adXest, MSR_DIM, ONE_COORD_STATE_DIM);
		iter->tRAEest.dAltEst = Xest2Alt(iter->adXest, MSR_DIM, ONE_COORD_STATE_DIM);
		//		iter->tRAEest.dVrEst      = Xest2Vr(iter->adXest, MSR_DIM, ONE_COORD_STATE_DIM);
		iter->tRAEest.dVrEst  = ptOneAziMajorMsrPnt->atMsrPntArr[unMsrPntIdxNearest].dVr;
		RA2ENU(g_tSysPara.tRadarArrayAttitude.dYaw, g_tSysPara.tRadarArrayAttitude.dPitch, g_tSysPara.tRadarArrayAttitude.dRoll, iter->adXest, iter->adPest, iter->adXestENU,iter->adPestENU);
		iter->tRAEestENU.dRngEst = Xest2Rng(iter->adXestENU, MSR_DIM, ONE_COORD_STATE_DIM);
		iter->tRAEestENU.dAziEst = Xest2Azi180(iter->adXestENU, ONE_COORD_STATE_DIM);
		iter->tRAEestENU.dEleEst = Xest2Ele(iter->adXestENU, MSR_DIM, ONE_COORD_STATE_DIM);
		iter->tRAEestENU.dAltEst = Xest2Alt(iter->adXestENU, MSR_DIM, ONE_COORD_STATE_DIM);
		//		iter->tRAEestENU.dVrEst = Xest2Vr(iter->adXestENU, MSR_DIM, ONE_COORD_STATE_DIM);
		iter->tRAEestENU.dVrEst = ptOneAziMajorMsrPnt->atMsrPntArr[unMsrPntIdxNearest].dVr;
		iter->dPRT = ptMsrPnt->dPRT;
		iter->dVel = Xest2Vel(iter->adXest, MSR_DIM, ONE_COORD_STATE_DIM);
		iter->dAcc = Xest2Acc(iter->adXest, MSR_DIM, ONE_COORD_STATE_DIM);
		iter->dVelAzi = Xest2VelAzi180(iter->adXestENU, ONE_COORD_STATE_DIM);
		iter->dAmp = ptMsrPnt->dAmp;
		iter->dSNR = ptMsrPnt->dSNR;
		GetPredState(iter->adXest, iter->tRAEest.dVrEst, MSR_DIM, ONE_COORD_STATE_DIM, TIME_STEP, iter->tPredRAE);
		iter->tPntAssociatd.unAntennaIdx = ptMsrPnt->ucAntennaIdx;
		iter->tPntAssociatd.unCPI = ptMsrPnt->unCPI;
		iter->tPntAssociatd.unIdx = ptMsrPnt->unIdx;
		iter->tPntAssociatd.tMsrPnt = *ptMsrPnt;
		// iter->untar_type = ptMsrPnt->untar_type;
		iter->ucAntennaIdx = ptMsrPnt->ucAntennaIdx;
		iter->enumBeamType = ptMsrPnt->enumBeamType;
		iter->ucBeamCoordX = ptMsrPnt->ucBeamCoordX;
		iter->ucBeamCoordY = ptMsrPnt->ucBeamCoordY;
		break;

	case PRED:
		//预测
		ddt = ptOneAziMajorMsrPnt->dTime - iter->dLstUpdTime;
		if(ddt < 0)
		{
//			ERROR("invalid time interval({:.3f}), time({:.3f}) of measure point must be greater than the time({:.3f}) of target track", ddt, ptOneAziMajorMsrPnt->dTime, iter->dLstUpdTime);
			return -1;
		}
		iter->dLstUpdTime = ptOneAziMajorMsrPnt->dTime;
		iter->dModelNoise = g_tAlgorithmPara.dModelNoise;
		iter->tLstUpdtDate = ptOneAziMajorMsrPnt->tDate;
		iter->enumTrackUpdMode = PRED;
		iter->enumScheduleEvent = g_tSysPara.enumScheduleEvent;
		iter->enumWorkFashion = g_tSysPara.enumWorkMode;
		memcpy(iter->adXestLast2, iter->adXest, sizeof(DOUBLE)*STATE_DIM);
		bzero(iter->adInnov, sizeof(iter->adInnov));
		DOUBLE adXpre[STATE_DIM], adPpre[STATE_DIM*STATE_DIM];
		StatePred(iter->adXest, STATE_DIM, iter->adPest, ddt, g_tAlgorithmPara.dModelNoise, adXpre, adPpre);
		memcpy(iter->adXest, adXpre, sizeof(DOUBLE) * STATE_DIM);
		memcpy(iter->adPest, adPpre, sizeof(DOUBLE) * STATE_DIM * STATE_DIM);
		iter->tRAEest.dRngEst = Xest2Rng(iter->adXest, MSR_DIM, ONE_COORD_STATE_DIM);
		iter->tRAEest.dAziEst = Xest2Azi180(iter->adXest, ONE_COORD_STATE_DIM);
		iter->tRAEest.dEleEst = Xest2Ele(iter->adXest, MSR_DIM, ONE_COORD_STATE_DIM);
		iter->tRAEest.dAltEst = Xest2Alt(iter->adXest, MSR_DIM, ONE_COORD_STATE_DIM);
		iter->tRAEest.dVrEst  = Xest2Vr(iter->adXest, MSR_DIM, ONE_COORD_STATE_DIM);
		RA2ENU(g_tSysPara.tRadarArrayAttitude.dYaw, g_tSysPara.tRadarArrayAttitude.dPitch, g_tSysPara.tRadarArrayAttitude.dRoll, iter->adXest, iter->adPest, iter->adXestENU,iter->adPestENU);
		iter->tRAEestENU.dRngEst = Xest2Rng(iter->adXestENU, MSR_DIM, ONE_COORD_STATE_DIM);
		iter->tRAEestENU.dAziEst = Xest2Azi180(iter->adXestENU, ONE_COORD_STATE_DIM);
		iter->tRAEestENU.dEleEst = Xest2Ele(iter->adXestENU, MSR_DIM, ONE_COORD_STATE_DIM);
		iter->tRAEestENU.dAltEst = Xest2Alt(iter->adXestENU, MSR_DIM, ONE_COORD_STATE_DIM);
		iter->tRAEestENU.dVrEst  = Xest2Vr(iter->adXestENU, MSR_DIM, ONE_COORD_STATE_DIM);
		iter->dVel = Xest2Vel(iter->adXest, MSR_DIM, ONE_COORD_STATE_DIM);
		iter->dAcc = Xest2Acc(iter->adXest, MSR_DIM, ONE_COORD_STATE_DIM);
		iter->dVelAzi = Xest2VelAzi180(iter->adXestENU, ONE_COORD_STATE_DIM);
		GetPredState(iter->adXest, iter->tRAEest.dVrEst, MSR_DIM, ONE_COORD_STATE_DIM, TIME_STEP, iter->tPredRAE);
		break;

	default:
		ERROR("Invalid input argument 'TgtTrackUpdMode'{}, this argument must be 'FILTER'{} or 'PRED'{}", TgtTrackUpdMode, FILTER, PRED);
		break;
	}

	return 0;
}

INT32 TgtTrackUpdtOneCPI(std::list<TgtTrackPntInfo>::iterator iter, OneCPIMsrInfo* ptOneCPIMsrPnt, UINT32 unMsrPntIdxNearest, TgtTrackUpdtModeInfo TgtTrackUpdMode)
{
	/*更新目标航迹--滤波、预测*/

	DOUBLE ddt = 0;
	//	DOUBLE adXpre[STATE_DIM] = { 0 }, adPpre[STATE_DIM*STATE_DIM] = { 0 };
	MsrPntInfo* ptMsrPnt = NULL;

	switch (TgtTrackUpdMode)
	{
	case FILTER:
		ddt = ptOneCPIMsrPnt->atMsrPntArr[unMsrPntIdxNearest].dTime - iter->dLstUpdTime;
		if(ddt <= 0)
		{
			ERROR("invalid time interval({}), time({}) of measure point must be greater than the time({}) of target track", ddt, ptOneCPIMsrPnt->atMsrPntArr[unMsrPntIdxNearest].dTime, iter->dLstUpdTime);
			return -1;
		}

		ptMsrPnt = ptOneCPIMsrPnt->atMsrPntArr+unMsrPntIdxNearest;
		iter->unLstUpdFrm = ptOneCPIMsrPnt->atMsrPntArr[unMsrPntIdxNearest].unCPI;
		iter->tLstUpdtDate = ptOneCPIMsrPnt->atMsrPntArr[unMsrPntIdxNearest].tDate;
		iter->unLen += 1;
		//if (iter->usTermntWndwLen > 0) iter->usTermntWndwLen += 1;
		iter->dLstUpdTime = ptOneCPIMsrPnt->atMsrPntArr[unMsrPntIdxNearest].dTime;
		iter->dModelNoise = g_tAlgorithmPara.dModelNoise;
		iter->enumTrackUpdMode = FILTER;
		iter->enumScheduleEvent = g_tSysPara.enumScheduleEvent;
		iter->enumWorkFashion = g_tSysPara.enumWorkMode;
		memcpy(iter->adXestLast2, iter->adXest, sizeof(DOUBLE)*STATE_DIM);
		GetInnov(iter->adXest, STATE_DIM, ONE_COORD_STATE_DIM, iter->adPest, ddt, g_tAlgorithmPara.dModelNoise, ptMsrPnt, MSR_DIM, iter->adInnov, iter->adS);
		KalmanFilter(iter->adXest, STATE_DIM, ONE_COORD_STATE_DIM, iter->adPest, ddt, g_tAlgorithmPara.dModelNoise, ptMsrPnt, MSR_DIM);
		iter->tRAEest.dRngEst = Xest2Rng(iter->adXest, MSR_DIM, ONE_COORD_STATE_DIM);
		iter->tRAEest.dAziEst = Xest2Azi180(iter->adXest, ONE_COORD_STATE_DIM);
		iter->tRAEest.dEleEst = Xest2Ele(iter->adXest, MSR_DIM, ONE_COORD_STATE_DIM);
		iter->tRAEest.dAltEst = Xest2Alt(iter->adXest, MSR_DIM, ONE_COORD_STATE_DIM);
		//		iter->tRAEest.dVrEst      = Xest2Vr(iter->adXest, MSR_DIM, ONE_COORD_STATE_DIM);
		iter->tRAEest.dVrEst  = ptOneCPIMsrPnt->atMsrPntArr[unMsrPntIdxNearest].dVr;
		RA2ENU(g_tSysPara.tRadarArrayAttitude.dYaw, g_tSysPara.tRadarArrayAttitude.dPitch, g_tSysPara.tRadarArrayAttitude.dRoll, iter->adXest, iter->adPest, iter->adXestENU,iter->adPestENU);
		iter->tRAEestENU.dRngEst = Xest2Rng(iter->adXestENU, MSR_DIM, ONE_COORD_STATE_DIM);
		iter->tRAEestENU.dAziEst = Xest2Azi180(iter->adXestENU, ONE_COORD_STATE_DIM);
		iter->tRAEestENU.dEleEst = Xest2Ele(iter->adXestENU, MSR_DIM, ONE_COORD_STATE_DIM);
		iter->tRAEestENU.dAltEst = Xest2Alt(iter->adXestENU, MSR_DIM, ONE_COORD_STATE_DIM);
		//		iter->tRAEestENU.dVrEst = Xest2Vr(iter->adXestENU, MSR_DIM, ONE_COORD_STATE_DIM);
		iter->tRAEestENU.dVrEst = ptOneCPIMsrPnt->atMsrPntArr[unMsrPntIdxNearest].dVr;
		iter->dPRT = ptMsrPnt->dPRT;
		iter->dVel = Xest2Vel(iter->adXest, MSR_DIM, ONE_COORD_STATE_DIM);
		iter->dAcc = Xest2Acc(iter->adXest, MSR_DIM, ONE_COORD_STATE_DIM);
		iter->dVelAzi = Xest2VelAzi180(iter->adXestENU, ONE_COORD_STATE_DIM);
		iter->dAmp = ptMsrPnt->dAmp;
		iter->dSNR = ptMsrPnt->dSNR;
		GetPredState(iter->adXest, iter->tRAEest.dVrEst, MSR_DIM, ONE_COORD_STATE_DIM, TIME_STEP, iter->tPredRAE);
		iter->tPntAssociatd.unAntennaIdx = ptMsrPnt->ucAntennaIdx;
		iter->tPntAssociatd.unCPI = ptMsrPnt->unCPI;
		iter->tPntAssociatd.unIdx = ptMsrPnt->unIdx;
		iter->tPntAssociatd.tMsrPnt = *ptMsrPnt;
		iter->untar_type  = ptMsrPnt->untar_type;
		iter->ucAntennaIdx = ptMsrPnt->ucAntennaIdx;
		iter->enumBeamType = ptMsrPnt->enumBeamType;
		iter->ucBeamCoordX = ptMsrPnt->ucBeamCoordX;
		iter->ucBeamCoordY = ptMsrPnt->ucBeamCoordY;
		break;

	case PRED:
		//预测
		ddt = ptOneCPIMsrPnt->dTime - iter->dLstUpdTime;
		if(ddt <= 0)
		{
			ERROR("invalid time interval({}), time({}) of measure point must be greater than the time({}) of target track", ddt, ptOneCPIMsrPnt->dTime, iter->dLstUpdTime);
			return -1;
		}
		iter->dLstUpdTime = ptOneCPIMsrPnt->dTime;
		iter->dModelNoise = g_tAlgorithmPara.dModelNoise;
		iter->unLstUpdFrm = ptOneCPIMsrPnt->unCPI;
		iter->tLstUpdtDate = ptOneCPIMsrPnt->tDate;
		iter->enumTrackUpdMode = PRED;
		iter->enumScheduleEvent = g_tSysPara.enumScheduleEvent;
		iter->enumWorkFashion = g_tSysPara.enumWorkMode;
		memcpy(iter->adXestLast2, iter->adXest, sizeof(DOUBLE)*STATE_DIM);
		bzero(iter->adInnov, sizeof(iter->adInnov));
		DOUBLE adXpre[STATE_DIM], adPpre[STATE_DIM*STATE_DIM];
		StatePred(iter->adXest, STATE_DIM, iter->adPest, ddt, g_tAlgorithmPara.dModelNoise, adXpre, adPpre);
		memcpy(iter->adXest, adXpre, sizeof(DOUBLE) * STATE_DIM);
		memcpy(iter->adPest, adPpre, sizeof(DOUBLE) * STATE_DIM * STATE_DIM);
		iter->tRAEest.dRngEst = Xest2Rng(iter->adXest, MSR_DIM, ONE_COORD_STATE_DIM);
		iter->tRAEest.dAziEst = Xest2Azi180(iter->adXest, ONE_COORD_STATE_DIM);
		iter->tRAEest.dEleEst = Xest2Ele(iter->adXest, MSR_DIM, ONE_COORD_STATE_DIM);
		iter->tRAEest.dAltEst = Xest2Alt(iter->adXest, MSR_DIM, ONE_COORD_STATE_DIM);
		iter->tRAEest.dVrEst  = Xest2Vr(iter->adXest, MSR_DIM, ONE_COORD_STATE_DIM);
		RA2ENU(g_tSysPara.tRadarArrayAttitude.dYaw, g_tSysPara.tRadarArrayAttitude.dPitch, g_tSysPara.tRadarArrayAttitude.dRoll, iter->adXest, iter->adPest, iter->adXestENU,iter->adPestENU);
		iter->tRAEestENU.dRngEst = Xest2Rng(iter->adXestENU, MSR_DIM, ONE_COORD_STATE_DIM);
		iter->tRAEestENU.dAziEst = Xest2Azi180(iter->adXestENU, ONE_COORD_STATE_DIM);
		iter->tRAEestENU.dEleEst = Xest2Ele(iter->adXestENU, MSR_DIM, ONE_COORD_STATE_DIM);
		iter->tRAEestENU.dAltEst = Xest2Alt(iter->adXestENU, MSR_DIM, ONE_COORD_STATE_DIM);
		//iter->tRAEestENU.dVrEst  = Xest2Vr(iter->adXestENU, MSR_DIM, ONE_COORD_STATE_DIM);
		iter->dVel = Xest2Vel(iter->adXest, MSR_DIM, ONE_COORD_STATE_DIM);
		iter->dAcc = Xest2Acc(iter->adXest, MSR_DIM, ONE_COORD_STATE_DIM);
		iter->dVelAzi = Xest2VelAzi180(iter->adXestENU, ONE_COORD_STATE_DIM);
		GetPredState(iter->adXest, iter->tRAEest.dVrEst, MSR_DIM, ONE_COORD_STATE_DIM, TIME_STEP, iter->tPredRAE);
		break;

	default:
		ERROR("Invalid input argument 'TgtTrackUpdMode'{}, this argument must be 'FILTER'{} or 'PRED'{}", TgtTrackUpdMode, FILTER, PRED);
		break;
	}

	return 0;
}


INT32 RemoveOneCoordX(std::map<BeamInfo, SPReportDataInBuffInfo*, CompareBeam>& mDataMap, std::map<BeamInfo, SPReportDataInBuffInfo*, CompareBeam>& mLeftEdgeMap, std::map<BeamInfo, SPReportDataInBuffInfo*, CompareBeam>& mRightEdgeMap, std::list<BeamInfo>& tLBeamCoordXInMap)
{
	//移除数据map、左边界map、右边界map和list中当前方位横坐标的数据

	if(mDataMap.empty())
	{
		ERROR("invalid argument, 'mDataMap' is empty");
		return -1;
	}

	if(tLBeamCoordXInMap.empty())
	{
		ERROR("invalid argument, 'tLBeamCoordXInMap' is empty");
		return -2;
	}

	//搜索数据map中第一包符合波位横坐标的数据
	BeamInfo tBeam = tLBeamCoordXInMap.front();
	std::map<BeamInfo, SPReportDataInBuffInfo*, CompareBeam>::iterator iter = GetIter1stofBeamCoordXInMap(&mDataMap, tBeam);
	if(iter == mDataMap.end())
	{
		ERROR("failed to remove data from map");
		return -3;
	}

	//删除数据map和左右边界中符合当前波位横坐标的数据
	while(iter != mDataMap.end() && iter->first.ucBeamCoordX == tBeam.ucBeamCoordX)
	{
		if (iter->first.enumBeamType == LEFT_EDGE)
		{
			auto iterEdge = mLeftEdgeMap.find(iter->first);
			if (iterEdge == mLeftEdgeMap.end()){
				ERROR("can not find data with CorrdX:({}), CoordY:({}), BeamType:({}) in left edge map", iter->first.ucBeamCoordX, iter->first.ucBeamCoordY, iter->first.enumBeamType);
			}
			else{
				mLeftEdgeMap.erase(iterEdge);
			}
		}

		else if (iter->first.enumBeamType == RIGHT_EDGE)
		{
			auto iterEdge = mRightEdgeMap.find(iter->first);
			if (iterEdge == mRightEdgeMap.end()){
				ERROR("can not find data with CorrdX:({}), CoordY:({}), BeamType:({}) in right edge map", iter->first.ucBeamCoordX, iter->first.ucBeamCoordY, iter->first.enumBeamType);
			}
			else{
				mRightEdgeMap.erase(iterEdge);
			}
		}

		g_qptSPRprtBuffFree.push(iter->second);
		iter = mDataMap.erase(iter);
	}

	tLBeamCoordXInMap.pop_front();

	return 0;
}

BOOL isAssociableForTgtTrack(std::list<TgtTrackPntInfo>::iterator iter, OneAziMajorMsrInfo* ptOneAziMajorMsrPnt)
{
	//检查确认航迹与特定方位的量测数据的可关联性

	if (ptOneAziMajorMsrPnt == NULL)
	{
		ERROR("invalid argument, 'ptOneAziMajorMsrPnt == NULL'");
		return false;
	}

	//航迹位于左边界
	if (iter->enumBeamType == LEFT_EDGE)
	{
		if (ptOneAziMajorMsrPnt->ucAntennaIdx == iter->ucAntennaIdx && get_circlular_dis(ptOneAziMajorMsrPnt->ucBeamCoordX, iter->ucBeamCoordX) <= MAX_XCOORD_INTERVEL)
			return true;
		else if (ptOneAziMajorMsrPnt->ucAntennaIdx == GetPrevAntennaIdx(iter->ucAntennaIdx) && ptOneAziMajorMsrPnt->enumEdgeTypeInData == RIGHT_EDGE)
			return true;
		else
			return false;
	}

	//航迹位于右边界
	else if (iter->enumBeamType == RIGHT_EDGE)
	{
		if (ptOneAziMajorMsrPnt->ucAntennaIdx == iter->ucAntennaIdx && get_circlular_dis(ptOneAziMajorMsrPnt->ucBeamCoordX, iter->ucBeamCoordX) <= MAX_XCOORD_INTERVEL)
			return true;
		else if (ptOneAziMajorMsrPnt->ucAntennaIdx == GetNextAntennaIdx(iter->ucAntennaIdx) && ptOneAziMajorMsrPnt->enumEdgeTypeInData == LEFT_EDGE)
			return true;
		else
			return false;
	}

	//航迹位于中间
	else if (iter->enumBeamType == MIDDLE)
	{
		if (ptOneAziMajorMsrPnt->ucAntennaIdx == iter->ucAntennaIdx && get_circlular_dis(ptOneAziMajorMsrPnt->ucBeamCoordX, iter->ucBeamCoordX) <= MAX_XCOORD_INTERVEL)
			return true;
		else
			return false;
	}

	else
	{
		ERROR("invalid argument, 'iter->enumBeamType({})', it can only be -1, 0 or 1.", iter->enumBeamType);
		return false;
	}
}

INT32 TrackMaintainTWS(OneAziMajorMsrInfo* ptOneAziMajorMsrPnt, std::list<TgtTrackPntInfo>& LTgtTrack)
{
	/*TWS航迹维持*/

	UINT32 unMsrPntIdxNearest = INF;

	for(std::list<TgtTrackPntInfo>::iterator iter = LTgtTrack.begin(); iter != LTgtTrack.end(); iter++)
	{
		if ( ! isAssociableForTgtTrack(iter, ptOneAziMajorMsrPnt) )
			continue;

		unMsrPntIdxNearest = GetNearestMsrPntIdxForTgtTrackInOneAziMajor(iter, ptOneAziMajorMsrPnt);

		if(unMsrPntIdxNearest != INF){
			ptOneAziMajorMsrPnt->atMsrPntArr[unMsrPntIdxNearest].ucAssocFlag = 1;

			iter->bAssocFlag = true;

			TgtTrackUpdt(iter, ptOneAziMajorMsrPnt, unMsrPntIdxNearest, FILTER);

			TrackSendMsg msg;
			msg.unTriggerNum = iter->usTrackID;
			msg.pTrackList = std::make_shared<std::list<TgtTrackPntInfo>>(g_LTgtTrack);
			g_qTrackSendMsg.push(msg);
		}
		else if(iter->bAssocFlag == false && ptOneAziMajorMsrPnt->ucBeamCoordX-iter->ucBeamCoordX == MAX_XCOORD_INTERVEL){
			TgtTrackUpdt(iter, ptOneAziMajorMsrPnt, unMsrPntIdxNearest, PRED);
		}

		SaveTgtTrack(&LTgtTrack, true, iter->usTrackID);

		if (iter->enumPredScheduleEvent==SEARCH && g_tSysPara.enumWorkMode == TAS && GetTrackNumTAS(&LTgtTrack) < MAX_TAS_NUM) {
			iter->enumPredScheduleEvent = TRACK;
			g_qunSendScheduleRqstrigger.push(iter->usTrackID);
		}
	}

	return 0;
}

INT32 TrackTWS()
{
	/*基于TWS的目标跟踪*/

	ConvertMsrPntOneAziMajor(&g_tSysPara, g_tOneAziMajorMsrPnt);

	TrackMaintainTWS(&g_tOneAziMajorMsrPnt, g_LTgtTrack);

	TrackInitTWS(&g_tOneAziMajorMsrPnt, g_LTmpTrack, g_LTgtTrack);

	return 0;
}

INT32 FindTgtTrack(std::list<TgtTrackPntInfo>* pLTgtTrack, UINT16 usTrackID)
{
	//搜索特定批号的目标航迹，成功返回航迹索引，失败返回-1

	INT32 nRes = -1, nCnt = 0;

	for(std::list<TgtTrackPntInfo>::iterator iter = pLTgtTrack->begin(); iter != pLTgtTrack->end(); iter++)
	{
		if(iter->usTrackID == usTrackID)
		{
			nRes = nCnt;
			break;
		}

		nCnt++;	
	}

	return nRes;
}


INT32 TrackTAS()
{
	//搜索特定批号的目标在List中的索引
	INT32 nTgtIdx = FindTgtTrack(&g_LTgtTrack, g_tSysPara.usTgtID);
	if(nTgtIdx == -1)
	{
		// ERROR("no such target track with ID:{}", g_tSysPara.usTgtID);
		return -1;
	}

	//获得特定批号航迹的迭代器
	std::list<TgtTrackPntInfo>::iterator iter = g_LTgtTrack.begin();
	std::advance(iter, nTgtIdx);

	//时间异常检查
	DOUBLE ddt = g_tOneCPIMsrPnt.dTime - iter->dLstUpdTime;
	if( ddt <= 0)
	{
		ERROR("invalid time interval({:.4f}), time({:.4f}) of measure point must be greater than the time({:.4f}) of target track", ddt, g_tOneCPIMsrPnt.dTime, iter->dLstUpdTime);
		return -2;
	}

	//量测转换
	ConvertMsrPntOneCPI(&g_tSysPara, g_tOneCPIMsrPnt);

	//数据关联
	UINT32 unMsrPntIdxNearest = INF;
	unMsrPntIdxNearest = GetNearestMsrPntIdxForTgtTrackInOneCPI(iter, &g_tOneCPIMsrPnt);

	if(unMsrPntIdxNearest != INF)
	{
		g_tOneCPIMsrPnt.atMsrPntArr[unMsrPntIdxNearest].ucAssocFlag = 1;

		iter->bAssocFlag = true;

		TgtTrackUpdtOneCPI(iter, &g_tOneCPIMsrPnt, unMsrPntIdxNearest, FILTER);
	}
	else{
		iter->bAssocFlag = false;

		TgtTrackUpdtOneCPI(iter, &g_tOneCPIMsrPnt, unMsrPntIdxNearest, PRED);
	}

	iter->enumWorkFashion = TAS;

	return 0;
}

INT32 RecycleTrackID(std::vector<UINT16>& vusTrackID, UINT16 usDiscardTrackID)
{
	/*回收航迹批号*/

	vusTrackID.emplace_back(usDiscardTrackID);

	return 0;
}

INT32 TgtTrackClear(std::list<TgtTrackPntInfo>& LTgtTrack, UINT8 ucAntennaIdx)
{
	/*航迹清除--自动终结航迹、显控清除航迹*/

	//检查状态为终结航迹，删除
	for(std::list<TgtTrackPntInfo>::iterator iter = LTgtTrack.begin(); iter != LTgtTrack.end();)
	{
		if(iter->ucAntennaIdx == ucAntennaIdx && iter->enumTrackUpdMode == TERMINATE)
		{
			RecycleTrackID(g_vusTrackIDUsed, iter->usTrackID);//回收航迹批号

			iter = LTgtTrack.erase(iter);
		}
		else
		{
			iter++;
		}
	}

	return 0;
} 

INT32 TgtTrackClearOne(std::list<TgtTrackPntInfo>& LTgtTrack, UINT32 unTriggerNum)
{
	/*航迹清除--自动终结航迹、显控清除航迹*/

	INT32 nTgtIdx = FindTgtTrack(&LTgtTrack, unTriggerNum);
	std::list<TgtTrackPntInfo>::iterator iter = LTgtTrack.begin();

	if(nTgtIdx >= 0 && iter != g_LTgtTrack.end()){
		std::advance(iter, nTgtIdx);
		if(iter->enumTrackUpdMode == TERMINATE)
			iter = LTgtTrack.erase(iter);
	}
	else{
		ERROR("no such target track with ID:{}", unTriggerNum);
		return -1;
	}

	return 0;
}

INT32 TmpTrackClear(std::list<TmpTrackInfo>& LTmpTrack, UINT8 ucAntennaIdx)
{
	/*候选航迹清除--已经起批、撤销*/

	for(std::list<TmpTrackInfo>::iterator iter = LTmpTrack.begin(); iter != LTmpTrack.end();)
	{
		if( (iter->ucAntennaIdx == ucAntennaIdx) && (iter->enumTmpTrackState == TURN_TO_TGT_TRACK || iter->enumTmpTrackState == CANCEL) )
		{
			RecycleTrackID(g_vusPseudoTrackIDUsed, iter->usPseudoTrackID);

			iter = LTmpTrack.erase(iter);
		}
		else
		{
			iter++;
		}
	}

	return 0;
} 

INT32 TgtTrackTermWindowUpdt(UINT8 ucAntennaIdx)
{
	/*目标航迹终结窗口更新*/

	for(std::list<TgtTrackPntInfo>::iterator iter = g_LTgtTrack.begin(); iter != g_LTgtTrack.end();iter++)
	{
		if( iter->ucAntennaIdx != ucAntennaIdx)
			continue;			

		//终结窗口更新
		UINT8 ucState = iter->bAssocFlag ? 0 : 1;
		if (iter->qucTerminentWindow.size() >= g_tAlgorithmPara.ucTermntWindwLenTH) {
			iter->usMissNum -= iter->qucTerminentWindow.front();
			iter->qucTerminentWindow.pop_front();
		}

		iter->qucTerminentWindow.push_back(ucState);
		iter->usMissNum += ucState;
		iter->usTermntWndwLen = iter->qucTerminentWindow.size();

		//航迹维持状态更新
		if(iter->usMissNum >= g_tAlgorithmPara.ucTermntPntNumTH)
			iter->enumTrackUpdMode = TERMINATE;
		else{
			if(iter->bAssocFlag)
				iter->enumTrackUpdMode = FILTER;
			else
				iter->enumTrackUpdMode = PRED;
		}
	}

	return 0;
}

INT32 TgtTrackConfirmWindowUpdtOne(UINT32 unTriggerNum)
{
	/*目标航迹确认窗口更新*/

	INT32 nTgtIdx = FindTgtTrack(&g_LTgtTrack, unTriggerNum);
	std::list<TgtTrackPntInfo>::iterator iter = g_LTgtTrack.begin();
	if(nTgtIdx >= 0 && iter != g_LTgtTrack.end()){
		std::advance(iter, nTgtIdx);
	}
	else{
		ERROR("no such target track with ID:{}", unTriggerNum);
		return -1;
	}

	//确认窗口更新
	UINT8 ucState = iter->bAssocFlag ? 1 : 0;
	iter->qucConfirmWindow.push_back(ucState);
	iter->usConfirmNum += ucState;
	iter->usConfirmWndwLen = iter->qucConfirmWindow.size();

	//航迹维持状态更新
	if(iter->usConfirmNum >= g_tAlgorithmPara.ucConfirmPntNumTH){
		iter->enumPredScheduleEvent = TRACK;
	}	
	else{
		if(iter->usConfirmWndwLen >= g_tAlgorithmPara.ucConfirmWindwLenTH)
			iter->enumTrackUpdMode = TERMINATE;
	}

	return 0;
}

INT32 TgtTrackTermWindowUpdtOne(UINT32 unTriggerNum)
{
	/*目标航迹终结窗口更新*/

	INT32 nTgtIdx = FindTgtTrack(&g_LTgtTrack, unTriggerNum);
	std::list<TgtTrackPntInfo>::iterator iter = g_LTgtTrack.begin();
	if(nTgtIdx >= 0 && iter != g_LTgtTrack.end()){
		std::advance(iter, nTgtIdx);
	}
	else{
		ERROR("no such target track with ID:{}", unTriggerNum);
		return -1;
	}

	//终结窗口更新
	UINT8 ucState = iter->bAssocFlag ? 0 : 1;
	if (iter->qucTerminentWindow.size() >= g_tAlgorithmPara.ucTermntWindwLenTH) {
		iter->usMissNum -= iter->qucTerminentWindow.front();
		iter->qucTerminentWindow.pop_front();
	}

	iter->qucTerminentWindow.push_back(ucState);
	iter->usMissNum += ucState;
	iter->usTermntWndwLen = iter->qucTerminentWindow.size();

	//航迹维持状态更新
	if(iter->usMissNum >= g_tAlgorithmPara.ucTermntPntNumTH)
		iter->enumTrackUpdMode = TERMINATE;
	else{
		if(iter->bAssocFlag)
			iter->enumTrackUpdMode = FILTER;
		else
			iter->enumTrackUpdMode = PRED;
	}

	return 0;
}

INT32 TmpTrackInitWindowUpdt(UINT8 ucAntennaIdx)
{
	/*暂态航迹起批窗口更新*/

	UINT8 ucInitWindwLenTH = 0;

	for(std::list<TmpTrackInfo>::iterator iter = g_LTmpTrack.begin(); iter != g_LTmpTrack.end(); iter++)
	{
		if(iter->ucAntennaIdx == ucAntennaIdx && !iter->bAssocFlag)
			AddMsrPnt2TmpTrack(iter, nullptr);

		if (iter->qtMsrArr.size()-1-iter->cIdxLastNonEmpty >= g_tAlgorithmPara.ucCancleInitPntNumTH)
			iter->enumTmpTrackState = CANCEL;
	}

	return 0;
}

INT32 RecoverAssocFlag(UINT8 ucAntennaIdx)
{
	/*重置所有目标航迹和暂态航迹的关联标记*/

	for(std::list<TgtTrackPntInfo>::iterator iter = g_LTgtTrack.begin(); iter != g_LTgtTrack.end();iter++)
	{
		if(iter->ucAntennaIdx == ucAntennaIdx)
			iter->bAssocFlag = false;
	}


	for(std::list<TmpTrackInfo>::iterator iter = g_LTmpTrack.begin();iter != g_LTmpTrack.end(); iter++)
	{
		if(iter->ucAntennaIdx == ucAntennaIdx)
			iter->bAssocFlag = false;
	}

	return 0;
}

INT32 RecordAziMajorInMap(SPReportDataInBuffInfo* ptSPReportData, std::list<BeamInfo>& tLBeamCoordXInMap)
{
	//记录map中缓存数据的方位波束指向

	BeamInfo tBeam = {ptSPReportData->enumBeamType, ptSPReportData->ucBeamCoordX, ptSPReportData->ucBeamCoordY, ptSPReportData->dPRT, ptSPReportData->dTime};

	for(auto iter = tLBeamCoordXInMap.begin(); iter != tLBeamCoordXInMap.end(); iter++)
	{
		if(tBeam.ucBeamCoordX == iter->ucBeamCoordX)
			return -1;
	}

	tLBeamCoordXInMap.push_back(tBeam);

	return 0;
}

INT32 MergeMsrPacket(SPReportDataInBuffInfo* ptSPReportDataDst, SPReportDataInBuffInfo* ptSPReportDataSrc)
{
	//copy msr pnts in src to dst

	if(ptSPReportDataSrc == NULL)
	{
		ERROR("invalid argument, 'ptSPReportDataSrc == NULL'");
		return -1;
	}

	if(ptSPReportDataDst == NULL)
	{
		ERROR("invalid argument, 'ptSPReportDataDst == NULL'");
		return -2;
	}

	UINT32 unPntNum = 0;
	if( ptSPReportDataDst->unMsrNum + ptSPReportDataSrc->unMsrNum <= MAX_MSR_NUM )
	{
		unPntNum = ptSPReportDataSrc->unMsrNum;
	}
	else
	{
		WARN("num of the msr pnt merged exceed the maximum value({}), {} msr pnt will be discarded",
				MAX_MSR_NUM, ptSPReportDataDst->unMsrNum + ptSPReportDataSrc->unMsrNum - MAX_MSR_NUM);
		unPntNum = MAX_MSR_NUM - ptSPReportDataDst->unMsrNum;
	}

	memcpy(ptSPReportDataDst->atMsrPnrArr+ptSPReportDataDst->unMsrNum, ptSPReportDataSrc->atMsrPnrArr, sizeof(MSrPntInBuffInfo)*unPntNum);

	ptSPReportDataDst->unMsrNum = ptSPReportDataDst->unMsrNum + unPntNum;

	return 0;
}

INT8 GetAntennaIdx(SPReportDataInBuffInfo* ptSPRprtData)
{
	//获取天线编号

	if (ptSPRprtData == NULL)
	{
		ERROR("invalid argument, 'ptSPRprtData == NULL'");
		return -1;
	}

	if (ptSPRprtData->ucAntennaIdx >= MAX_ANTENNA_NUM)
	{
		ERROR("invalid argument, 'ptSPRprtData->ucAntennaIdx({})', it must be less than {}", ptSPRprtData->ucAntennaIdx, MAX_ANTENNA_NUM);
		return -2;
	}

	return ptSPRprtData->ucAntennaIdx;
}

INT32 InsertMsrPacket2Map(SPReportDataInBuffInfo* ptSPReportData, std::map<BeamInfo, SPReportDataInBuffInfo*, CompareBeam>& mDataMap, std::map<BeamInfo, SPReportDataInBuffInfo*, CompareBeam>& mLeftEdgeMap, std::map<BeamInfo, SPReportDataInBuffInfo*, CompareBeam>& mRightEdgeMap)
{
	/*插入量测数据包至map*/

	if(ptSPReportData == NULL)
	{
		ERROR("invalid argument, 'ptSPReportData == NULL'");
		return -1;
	}

	BeamInfo tBeam = {ptSPReportData->enumBeamType, ptSPReportData->ucBeamCoordX, ptSPReportData->ucBeamCoordY, ptSPReportData->dPRT, ptSPReportData->dTime};

	auto iter = mDataMap.begin();
	BOOL bSuccess = false;
	auto Rtn = std::make_pair(iter, bSuccess);

	//插入数据至数据map中
	Rtn = mDataMap.insert(std::make_pair(tBeam, ptSPReportData));

	//插入成功，拷贝数据至左、右边界map中
	if(Rtn.second)
	{
		if (tBeam.enumBeamType == LEFT_EDGE) {
			Rtn = mLeftEdgeMap.insert(std::make_pair(tBeam, ptSPReportData));
		}
		else if (tBeam.enumBeamType == RIGHT_EDGE) {
			Rtn = mRightEdgeMap.insert(std::make_pair(tBeam, ptSPReportData));
		}

	}

	//插入失败或合并，不拷贝
	else
	{
		if(Rtn.first->first.dPRT != tBeam.dPRT)
		{
			MergeMsrPacket(Rtn.first->second, ptSPReportData);
			g_qptSPRprtBuffFree.push(ptSPReportData);
		}
		else
		{
			WARN("msr packet with CoordX:{}, CoordY:{}, PRT:{:.1f}us already exists, ID in pakcket:({}, {}), ID in map:({},{})", 
				tBeam.ucBeamCoordX, tBeam.ucBeamCoordY, tBeam.dPRT*1e6, ptSPReportData->uncycle_id, ptSPReportData->usbeam_id, Rtn.first->second->uncycle_id, Rtn.first->second->usbeam_id);
			g_qptSPRprtBuffFree.push(ptSPReportData);
			return -2;
		}
	}

	return 0;
}

int UpdtCenter(MSrPntInBuffInfo* ptMsrPntInBuff, std::list<MSrPntInBuffInfo>::iterator iter)
{
	//更新凝聚中心

	float fSNRm = pow(10, 0.1 * (ptMsrPntInBuff->fSNR)), fSNRc = pow(10, 0.1 * (iter->fSNR));
	float fwm  = fSNRm / (fSNRm + fSNRc), fwc = fSNRc / (fSNRm + fSNRc);

	iter->fVr  = fwm * ptMsrPntInBuff->fVr  + fwc * iter->fVr;
	iter->fAzi = fwm * ptMsrPntInBuff->fAzi + fwc * iter->fAzi;
	iter->fEle = fwm * ptMsrPntInBuff->fEle + fwc * iter->fEle;
	iter->fRng = fwm * ptMsrPntInBuff->fRng + fwc * iter->fRng;

	return 0;
}

INT32 CentroidMsrPntIn1Beam(SPReportDataInBuffInfo* ptSPReporBeam1)
{
	//凝聚两个波束点迹，标记SNR低的点迹，保留高SNR点迹

	if(ptSPReporBeam1 == NULL)
	{
		ERROR("invalid argument, 'ptSPReporBeam1 == NULL'");
		return -1;
	}

	std::list<MSrPntInBuffInfo> pLCenter;
	bool bAssoc = false;
	for(UINT16 i = 0; i < ptSPReporBeam1->unMsrNum; i++)
	{
		bAssoc = false;
		for(auto iter = pLCenter.begin();iter != pLCenter.end();iter++)
		{
			if (fabs(ptSPReporBeam1->atMsrPnrArr[i].fVr - iter->fVr) > ptSPReporBeam1->tDPAlgrthmPara.dVrCentroidTH)
				continue;

			if (fabs(ptSPReporBeam1->atMsrPnrArr[i].fAzi - iter->fAzi) > ptSPReporBeam1->tDPAlgrthmPara.dAziCentroidTH)
				continue;

			if (fabs(ptSPReporBeam1->atMsrPnrArr[i].fEle - iter->fEle) > ptSPReporBeam1->tDPAlgrthmPara.dEleCentroidTH)
				continue;

			if (fabs(ptSPReporBeam1->atMsrPnrArr[i].fRng - iter->fRng) > ptSPReporBeam1->tDPAlgrthmPara.dRngCentroidTH)
				continue;

			//相关，更新中心，标记参与凝聚的点迹
			UpdtCenter(ptSPReporBeam1->atMsrPnrArr+i, iter);
			if(ptSPReporBeam1->atMsrPnrArr[i].fSNR > iter->fSNR)
			{
				ptSPReporBeam1->atMsrPnrArr[iter->unIdx].ucCentroidFlag = 1;
				iter->unIdx = i;
			}
			else
			{
				ptSPReporBeam1->atMsrPnrArr[i].ucCentroidFlag = 1;
			}

			bAssoc = true;
			break;
		}

		if(!bAssoc)
		{
			//与任何中心均不相关，生成新中心
			pLCenter.emplace_back(ptSPReporBeam1->atMsrPnrArr[i]);
		}
	}

	return 0;
}

INT32 CentroidMsrPntIn2Beam(SPReportDataInBuffInfo* ptSPReporBeam1, SPReportDataInBuffInfo* ptSPReporBeam2)
{
	//凝聚两个波束点迹，标记SNR低的点迹，保留高SNR点迹

	if(ptSPReporBeam1 == NULL)
	{
		ERROR("invalid argument, 'ptSPReporBeam1 == NULL'");
		return -1;
	}

	if(ptSPReporBeam2 == NULL)
	{
		ERROR("invalid argument, 'ptSPReporBeam2 == NULL'");
		return -2;
	}

	for(UINT16 i = 0; i < ptSPReporBeam1->unMsrNum; i++)
	{
		if(ptSPReporBeam1->atMsrPnrArr[i].ucCentroidFlag == 1)
			continue;

		for(UINT16 j = 0; j < ptSPReporBeam2->unMsrNum; j++)
		{
			if(ptSPReporBeam2->atMsrPnrArr[j].ucCentroidFlag == 1)
				continue;

			if ( fabs(ptSPReporBeam1->atMsrPnrArr[i].fVr - ptSPReporBeam2->atMsrPnrArr[j].fVr) > ptSPReporBeam2->tDPAlgrthmPara.dVrCentroidTH)
				continue;

			if ( fabs(ptSPReporBeam1->atMsrPnrArr[i].fAzi - ptSPReporBeam2->atMsrPnrArr[j].fAzi) > ptSPReporBeam2->tDPAlgrthmPara.dAziCentroidTH)
				continue;

			if ( fabs(ptSPReporBeam1->atMsrPnrArr[i].fEle - ptSPReporBeam2->atMsrPnrArr[j].fEle) > ptSPReporBeam2->tDPAlgrthmPara.dEleCentroidTH)
				continue;

			if ( fabs(ptSPReporBeam1->atMsrPnrArr[i].fRng - ptSPReporBeam2->atMsrPnrArr[j].fRng) > ptSPReporBeam2->tDPAlgrthmPara.dRngCentroidTH)
				continue;

			if( ptSPReporBeam2->atMsrPnrArr[j].fSNR <= ptSPReporBeam1->atMsrPnrArr[i].fSNR )
				ptSPReporBeam2->atMsrPnrArr[j].ucCentroidFlag = 1;
			else
			{
				ptSPReporBeam1->atMsrPnrArr[i].ucCentroidFlag = 1;
				break;
			}
		}
	}

	return 0;
}

INT32 CentroidAllEleInOneAzi(std::map<BeamInfo, SPReportDataInBuffInfo*, CompareBeam>& mDataMap, BeamInfo tBeam)
{
	//凝聚指定方位的所有俯仰波束

	if(mDataMap.empty())
	{
		ERROR("invalid argument, 'mDataMap' is empty");
		return -1;
	}

	UINT8 ucBeamCoordX = tBeam.ucBeamCoordX;

	auto prevIter = mDataMap.begin();
	auto Last = std::prev(mDataMap.end());

	BOOL bFlag = false;

	for(;prevIter != Last; prevIter++)
	{
		if(prevIter->first.ucBeamCoordX != ucBeamCoordX) continue;

		bFlag = true;

		auto nextIter = std::next(prevIter);

		if(nextIter->first.ucBeamCoordX != ucBeamCoordX) break;

		CentroidMsrPntIn2Beam(prevIter->second, nextIter->second);
	}

	if(!bFlag)
	{
		//WARN("no such AziMajor({:.2f}) in map, failed to centroid msr pnt", dAziMajor);
		return -2;
	}

	return 0;
}

INT32 CentroidAdjacentAzi(std::map<BeamInfo, SPReportDataInBuffInfo*, CompareBeam>& mDataMap, BeamInfo tBeam1, BeamInfo tBeam2)
{
	//凝聚相邻方位波束的数据

	if(mDataMap.empty())
	{
		ERROR("invalid argument, 'mDataMap' is empty");
		return -1;
	}

	BOOL bFlag1 = false, bFlag2 = false;

	auto iter = mDataMap.begin(), iter1Bgn = mDataMap.begin(), iter2Bgn = mDataMap.begin();

	//搜索方位1和方位2的波束在map中首次出现的位置
	for(; iter != mDataMap.end() && (!bFlag1 || !bFlag2); iter++)
	{
		if(iter->first.ucBeamCoordX == tBeam1.ucBeamCoordX && !bFlag1)
		{
			iter1Bgn = iter;
			bFlag1 = true;
		}

		if(iter->first.ucBeamCoordX == tBeam2.ucBeamCoordX && !bFlag2)
		{
			iter2Bgn = iter;
			bFlag2 = true;
		}
	}

	if(!bFlag1)
	{
		WARN("no such CoordX({}) in map, failed to centroid msr pnt", tBeam1.ucBeamCoordX);
		return -2;
	}

	if(!bFlag2)
	{
		WARN("no such CoordX({}) in map, failed to centroid msr pnt", tBeam2.ucBeamCoordX);
		return -3;
	}

	//凝聚方位和俯仰都相邻的波束
	for(auto iter1 = iter1Bgn; iter1 != mDataMap.end() && iter1->first.ucBeamCoordX == tBeam1.ucBeamCoordX; iter1++)
	{
		for(auto iter2 = iter2Bgn; iter2 != mDataMap.end() && iter2->first.ucBeamCoordX == tBeam2.ucBeamCoordX; iter2++)
		{
			if( fabs(iter1->first.ucBeamCoordY - iter2->first.ucBeamCoordY) > 1) continue;

			CentroidMsrPntIn2Beam(iter1->second, iter2->second);
		}
	}

	return 0;
}

INT32 MarkSearchDataNearTracking(SPReportDataInBuffInfo* ptSPReporBeamTracking, SPReportDataInBuffInfo* ptSPReporBeamSearch)
{
	//标记与跟踪数据相邻的搜索点迹

	if(ptSPReporBeamTracking == NULL)
	{
		ERROR("invalid argument, 'ptSPReporBeamTracking == NULL'");
		return -1;
	}

	if(ptSPReporBeamSearch == NULL)
	{
		ERROR("invalid argument, 'ptSPReporBeamSearch == NULL'");
		return -2;
	}

	for(UINT16 i = 0; i < ptSPReporBeamTracking->unMsrNum; i++)
	{
		for(UINT16 j = 0; j < ptSPReporBeamSearch->unMsrNum; j++)
		{
			if(ptSPReporBeamSearch->atMsrPnrArr[j].ucCentroidFlag == 1)
				continue;

			if ( fabs(ptSPReporBeamTracking->atMsrPnrArr[i].fVr - ptSPReporBeamSearch->atMsrPnrArr[j].fVr) > ptSPReporBeamSearch->tDPAlgrthmPara.dVrCentroidTH)
				continue;

			if ( fabs(ptSPReporBeamTracking->atMsrPnrArr[i].fAzi - ptSPReporBeamSearch->atMsrPnrArr[j].fAzi) > ptSPReporBeamSearch->tDPAlgrthmPara.dAziCentroidTH)
				continue;

			if ( fabs(ptSPReporBeamTracking->atMsrPnrArr[i].fEle - ptSPReporBeamSearch->atMsrPnrArr[j].fEle) > ptSPReporBeamSearch->tDPAlgrthmPara.dEleCentroidTH)
				continue;

			if ( fabs(ptSPReporBeamTracking->atMsrPnrArr[i].fRng - ptSPReporBeamSearch->atMsrPnrArr[j].fRng) > ptSPReporBeamSearch->tDPAlgrthmPara.dVrMax*TIME_STEP)
				continue;

			ptSPReporBeamSearch->atMsrPnrArr[j].ucCentroidFlag = 1;
		}
	}

	return 0;
}

INT32 CentroidTrackingAndSearch(SPReportDataInBuffInfo* ptSPRprtData, std::map<BeamInfo, SPReportDataInBuffInfo*, CompareBeam>* pmDataMap,  std::list<BeamInfo>* ptLBeamCoordXInMap)
{
	//标记搜索map中与跟踪波束点迹相邻的点

	if (ptSPRprtData == NULL) {
		ERROR("invalid argument, 'ptSPRprtData == NULL'");
		return -1;
	}

	if (pmDataMap == NULL) {
		ERROR("invalid argument, 'pmDataMap == NULL'");
		return -1;
	}

	if (pmDataMap->empty()) {
		ERROR("invalid argument, 'mDataMap' is empty");
		return -3;
	}

	if (ptLBeamCoordXInMap == NULL) {
		ERROR("invalid argument, 'ptLBeamCoordXInMap == NULL'");
		return -4;
	}

	auto iterMap = pmDataMap->begin();
	BeamInfo tBeam;
	bzero(&tBeam, sizeof(BeamInfo));

	for (auto iterList = ptLBeamCoordXInMap->begin(); iterList != ptLBeamCoordXInMap->end(); iterList++) {
		if (get_circlular_dis(ptSPRprtData->ucBeamCoordX, iterList->ucBeamCoordX) > MAX_COORD_INTERVEL_CENTROID)
			continue;

		tBeam.ucBeamCoordX = iterList->ucBeamCoordX;
		iterMap = GetIter1stofBeamCoordXInMap(pmDataMap, tBeam);

		for (;iterMap != pmDataMap->end() && iterMap->first.ucBeamCoordX == iterList->ucBeamCoordX; iterMap++) {
			if (abs(iterMap->first.ucBeamCoordY - ptSPRprtData->ucBeamCoordY) > MAX_COORD_INTERVEL_CENTROID)
				continue;

			MarkSearchDataNearTracking(ptSPRprtData, iterMap->second);
		}
	}

	return 0;
}

INT32 CentroidAdjacentAntenna(std::map<BeamInfo, SPReportDataInBuffInfo*, CompareBeam>* pmDataMap, BeamInfo tBeamCoordX1, std::map<BeamInfo, SPReportDataInBuffInfo*, CompareBeam>* pmLeftEdgeMap, std::map<BeamInfo, SPReportDataInBuffInfo*, CompareBeam>* pmRightEdgeMap)
{
	//凝聚相邻天线中的点迹
	if (pmDataMap == NULL)
	{
		ERROR("invalid argument, 'pmDataMap == NULL'");
		return -1;
	}

	if ( pmLeftEdgeMap == NULL)
	{
		ERROR("invalid argument, 'pmLeftEdgeMap == NULL'");
		return -2;
	}

	if ( pmRightEdgeMap == NULL)
	{
		ERROR("invalid argument, 'pmRightEdgeMap == NULL'");
		return -3;
	}

	//搜索当前方位标记为边界的波位
	auto iterMap = GetIter1stofBeamCoordXInMap(pmDataMap, tBeamCoordX1);
	for (;iterMap != pmDataMap->end() && iterMap->first.ucBeamCoordX == tBeamCoordX1.ucBeamCoordX; iterMap++)
	{
		//左边界
		if (iterMap->first.enumBeamType == LEFT_EDGE)
		{
			for (auto iterRightMap = pmRightEdgeMap->begin(); iterRightMap != pmRightEdgeMap->end(); iterRightMap++)
			{
				if(abs(iterMap->first.ucBeamCoordY - iterRightMap->first.ucBeamCoordY) <= 1)
					CentroidMsrPntIn2Beam(iterMap->second, iterRightMap->second);
			}
		}

		//右边界
		else if (iterMap->first.enumBeamType == RIGHT_EDGE)
		{
			for (auto iterLeftMap = pmLeftEdgeMap->begin(); iterLeftMap != pmLeftEdgeMap->end(); iterLeftMap++)
			{
				if(abs(iterMap->first.ucBeamCoordY - iterLeftMap->first.ucBeamCoordY) <= 1)
					CentroidMsrPntIn2Beam(iterMap->second, iterLeftMap->second);
			}
		}
	}

	return 0;
}

UINT8 GetPrevAntennaIdx(UINT8 ucAntennaIdx)
{
	//获取前一个天线索引

	if (ucAntennaIdx >= MAX_ANTENNA_NUM)
	{
		ERROR("invalid argument, 'ucAntennaIdx'({}), it must be less than {}", ucAntennaIdx, MAX_ANTENNA_NUM);
		exit(1);
	}

	return (ucAntennaIdx-1+MAX_ANTENNA_NUM)%MAX_ANTENNA_NUM;
}

UINT8 GetNextAntennaIdx(UINT8 ucAntennaIdx)
{
	//获取后一个天线索引

	if (ucAntennaIdx >= MAX_ANTENNA_NUM)
	{
		ERROR("invalid argument, 'ucAntennaIdx'({}), it must be less than {}", ucAntennaIdx, MAX_ANTENNA_NUM);
		exit(1);
	}

	return (ucAntennaIdx+1)%MAX_ANTENNA_NUM;
}

BOOL IsOneCircleFinished(BeamInfo tBeamPrev, BeamInfo tBeamNext)
{
	//

	if(tBeamNext.dTime >= tBeamPrev.dTime && (tBeamNext.ucBeamCoordX-tBeamPrev.ucBeamCoordX) < -0.5*COORD_X_IN_MAP_TH)
		return true;
	else
		return false;
}



