#include "global.h"
#include "SpdLogger.h"
#include <string.h>

INT32 InitSPRprtBuffFreePtrQ()
{
	//初始化信处数据缓冲队列

	for (UINT32 i = 0; i < MAX_PAK_NUM; i++)
	{
		bzero(g_atSPRprtBuff+i, sizeof(SPReportDataInBuffInfo));
		g_qptSPRprtBuffFree.push(g_atSPRprtBuff + i);
	}

	return 0;
}

INT32 InitAlgorithmPara()
{
	//初始化算法参数
	bzero(&g_tAlgorithmPara, sizeof(AlgorithmParaInfo));

	//TWS模式
	bzero(&g_tAlgorithmParaTWS, sizeof(AlgorithmParaInfo));
	g_tAlgorithmParaTWS.dAziCentroidTH = AZI_3dB / 10.0 * 3;
	g_tAlgorithmParaTWS.dEleCentroidTH = ELE_3dB / 10.0 * 3;
	g_tAlgorithmParaTWS.dRngCentroidTH = RNG_CELL * 2;
	g_tAlgorithmParaTWS.dVrCentroidTH = 5;
	g_tAlgorithmParaTWS.ucTermntPntNumTH = 5;
	g_tAlgorithmParaTWS.ucTermntWindwLenTH = 5;
	g_tAlgorithmParaTWS.ucConfirmPntNumTH = 3;
	g_tAlgorithmParaTWS.ucConfirmWindwLenTH = 3;
	g_tAlgorithmParaTWS.ucCancleInitPntNumTH = 3;
	g_tAlgorithmParaTWS.dModelNoise = 1;
	g_tAlgorithmParaTWS.dVrMin = 0.1;
	g_tAlgorithmParaTWS.dVrMax = 120;
	g_tAlgorithmParaTWS.dEleAssocTH = ELE_3dB / 10.0 * 3;
	g_tAlgorithmParaTWS.dAltAssocTH = 10;
	g_tAlgorithmParaTWS.dGamma = 6;
	g_tAlgorithmParaTWS.dDefAngleForTmpTrackTH = 60;
	g_tAlgorithmParaTWS.dDefAngleForTgtTrackTH = 180;
	g_tAlgorithmParaTWS.dConfidenceTH = 0.5;

	g_tAlgorithmParaTWS.ucStandardInitPntNumTH = 4;
	g_tAlgorithmParaTWS.ucStandardInitWindwLenTH = 5;

	g_tAlgorithmParaTWS.dQuickInitRngTH = 5000;
	g_tAlgorithmParaTWS.ucQuickInitPntNumTH = 2;
	g_tAlgorithmParaTWS.ucQuickInitWindwLenTH = 3;
	g_tAlgorithmParaTWS.dQuickInitAziAssocTH = AZI_3dB / 10.0 * 8;
	g_tAlgorithmParaTWS.dQuickInitVrAssocTH = 15;

	g_tAlgorithmParaTWS.dSlowInitVrTH = 5;	   // 慢起批速度阈值
	g_tAlgorithmParaTWS.dSlowInitRngTH = 1000; // 慢起批距离阈值
	g_tAlgorithmParaTWS.ucSlowInitPntNumTH = 6;
	g_tAlgorithmParaTWS.ucSlowInitWindwLenTH = 8;
	g_tAlgorithmParaTWS.dSlowInitAziAssocTH = 1.6; // 方位关联阈值-低置信度航迹
	g_tAlgorithmParaTWS.dSlowInitVrAssocTH = 7;	   // 多普勒速度关联阈值-低置信度航迹

	//TWS模式
	bzero(&g_tAlgorithmParaTAS, sizeof(AlgorithmParaInfo));
	g_tAlgorithmParaTAS.dAziCentroidTH = AZI_3dB / 10.0 * 3;
	g_tAlgorithmParaTAS.dEleCentroidTH = ELE_3dB / 10.0 * 3;
	g_tAlgorithmParaTAS.dRngCentroidTH = RNG_CELL * 2;
	g_tAlgorithmParaTAS.dVrCentroidTH = 5;
	g_tAlgorithmParaTAS.ucTermntPntNumTH = 5;
	g_tAlgorithmParaTAS.ucTermntWindwLenTH = 5;
	g_tAlgorithmParaTAS.ucConfirmPntNumTH = 3;
	g_tAlgorithmParaTAS.ucConfirmWindwLenTH = 3;
	g_tAlgorithmParaTAS.ucCancleInitPntNumTH = 3;
	g_tAlgorithmParaTAS.dModelNoise = 1;
	g_tAlgorithmParaTAS.dVrMin = 0.1;
	g_tAlgorithmParaTAS.dVrMax = 120;
	g_tAlgorithmParaTAS.dEleAssocTH = ELE_3dB / 10.0 * 3;
	g_tAlgorithmParaTAS.dAltAssocTH = 10;
	g_tAlgorithmParaTAS.dGamma = 6;
	g_tAlgorithmParaTAS.dDefAngleForTmpTrackTH = 60;
	g_tAlgorithmParaTAS.dDefAngleForTgtTrackTH = 180;
	g_tAlgorithmParaTAS.dConfidenceTH = 0.5;

	g_tAlgorithmParaTAS.ucStandardInitPntNumTH = 4;
	g_tAlgorithmParaTAS.ucStandardInitWindwLenTH = 5;

	g_tAlgorithmParaTAS.dQuickInitRngTH = 5000;
	g_tAlgorithmParaTAS.ucQuickInitPntNumTH = 2;
	g_tAlgorithmParaTAS.ucQuickInitWindwLenTH = 3;
	g_tAlgorithmParaTAS.dQuickInitAziAssocTH = AZI_3dB / 10.0 * 8;
	g_tAlgorithmParaTAS.dQuickInitVrAssocTH = 15;

	g_tAlgorithmParaTAS.dSlowInitVrTH = 5;	   // 慢起批速度阈值
	g_tAlgorithmParaTAS.dSlowInitRngTH = 1000; // 慢起批距离阈值
	g_tAlgorithmParaTAS.ucSlowInitPntNumTH = 6;
	g_tAlgorithmParaTAS.ucSlowInitWindwLenTH = 8;
	g_tAlgorithmParaTAS.dSlowInitAziAssocTH = 1.6; // 方位关联阈值-低置信度航迹
	g_tAlgorithmParaTAS.dSlowInitVrAssocTH  = 7;	   // 多普勒速度关联阈值-低置信度航迹
	
	return 0;
}


INT32 InitSysPara()
{
	//初始化系统参数

	bzero(&g_tSysPara, sizeof(SysParaInfo));
	return 0;
}

INT32 InitOneCPIMsrPnt()
{
	//初始化一帧点迹

	bzero(&g_tOneCPIMsrPnt, sizeof(OneCPIMsrInfo));
	return 0;
}



INT32 InitOneCircleMsrPnt()
{
	//初始化一圈量测点迹

	bzero(&g_tOneCircleMsrPnt, sizeof(OneCircleMsrInfo));
	return 0;
}

INT32 InitTrackIDQueue()
{
	//初始化航迹批号队列

	for(UINT32 i = 1; i <= 65535; i++)
	{
		g_qusTrackID.push(i);
	}

	return 0;
}

INT32 init_pending_conf_track_id()
{
	//初始化待确认航迹批号队列

	for(UINT32 i = 10001; i <= 65535; i++)
	{
		g_quspending_conf_track_id.push(i);
	}

	return 0;
}

INT32 InitPseudoTrackIDQueue()
{
	//初始化伪航迹批号队列

	for(UINT32 i = 1; i <= MAX_TMP_TRACK_NUM; i++)
	{
		g_qusPseudoTrackID.push(i);
	}

	return 0;
}


INT32 Init()
{	
	//初始化信处上报数据缓冲队列
	InitSPRprtBuffFreePtrQ();

	//初始化系统参数
	InitSysPara();

	//初始化算法参数
	InitAlgorithmPara();

	//初始化一帧点迹
	InitOneCPIMsrPnt();

	//初始化一圈量测点迹
	InitOneCircleMsrPnt();

	//初始化航迹批号队列
	InitTrackIDQueue();

	//初始化伪航迹批号队列
	InitPseudoTrackIDQueue();

	INFO("init finished, running.");

	return 0;
}
