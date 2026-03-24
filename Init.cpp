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
	g_tAlgorithmPara.ucQuickInitPntNumTH = 3;
	g_tAlgorithmPara.ucQuickInitWindwLenTH = 4;
	g_tAlgorithmPara.ucTermntPntNumTH = 3;
	g_tAlgorithmPara.ucTermntWindwLenTH = 3;
	g_tAlgorithmPara.dModelNoise = 1e-2;
	g_tAlgorithmPara.dVrMin = 1;
	g_tAlgorithmPara.dVrMax = 30;
	g_tAlgorithmPara.dQuickInitAziAssocTH = 1.5;
	g_tAlgorithmPara.dEleAssocTH = 2;
	g_tAlgorithmPara.dQuickInitVrAssocTH = 10;
	g_tAlgorithmPara.dGamma = 16;
	g_tAlgorithmPara.dRngCentroidTH = 15; //20MHz采样率，2个采样单元长度
	g_tAlgorithmPara.dAziCentroidTH = 0.37; //1/10方位波束宽度
	g_tAlgorithmPara.dEleCentroidTH = 0.72; //1/10俯仰波束宽度
	g_tAlgorithmPara.dVrCentroidTH = 6; //PRT=25ms,128个脉冲,一个多普勒通道约3m/s，2个通道
	
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
