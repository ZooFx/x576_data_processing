#include "global.h"
#include "Init.h"
#include "DPRcvMsrPnt.h"
#include "SpdLogger.h"
#include "DPThread.h"
#include "DPSendTrack.h"
#include "LoadMsrPnt.h"
#include "DPRcvDisp.h"
#include "DPSendScheduleRqst.h"

// 设置CPU亲核性
void threadAffinity(UINT32 coreID, UINT32 uwCoreNum, pthread_t tid)
{
	cpu_set_t mask;
	CPU_ZERO(&mask);
	for(UINT32 uwId = 0; uwId < uwCoreNum; uwId++)
	{
		CPU_SET(coreID + uwId, &mask);
	}
	pthread_setaffinity_np(tid, sizeof(cpu_set_t), &mask);
}

INT32 main(INT32 nArgc, INT8* Argv[])
{
	LoggerInit();

	//初始化
	Init();

	//点迹接收
	pthread_t tid_DPRcvMsrPnt;
	pthread_create(&tid_DPRcvMsrPnt, NULL, DPRcvMsrPnt, NULL);
	threadAffinity(55, 1, tid_DPRcvMsrPnt);

	//数据处理
	pthread_t tid_DataProcess;
	pthread_create(&tid_DataProcess, NULL, DataProcess, NULL);
	threadAffinity(56, 1, tid_DataProcess);

	//航迹发送
	pthread_t tid_TrackSend;
	pthread_create(&tid_TrackSend, NULL, DPSendTrack, NULL);
	threadAffinity(57, 1, tid_TrackSend);

	//调度请求发送
	pthread_t tid_ScheduleRqstSend;
	pthread_create(&tid_ScheduleRqstSend, NULL, DPSendScheduleRqst, NULL);
	threadAffinity(58, 1, tid_ScheduleRqstSend);

	//测试线程-加载量测点迹
	// pthread_t tid_LoadMsrPnt;
	// pthread_create(&tid_LoadMsrPnt, NULL, LoadMsrPnt, NULL);
	// pthread_join(tid_LoadMsrPnt, NULL);

	pthread_join(tid_DataProcess, NULL);
	pthread_join(tid_DPRcvMsrPnt, NULL);
	pthread_join(tid_TrackSend, NULL);
	pthread_join(tid_ScheduleRqstSend, NULL);

	LoggerDrop();

	return 0;
}
