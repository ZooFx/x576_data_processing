#pragma once

#include "StructDef.h"
#include "SafeQueue.h"
#include "MacroDef.h"
#include <list>
#include <mutex>
#include <map>
#include <atomic>

extern SPReportDataInBuffInfo g_atSPRprtBuff[MAX_PAK_NUM];
extern safe_queue<SPReportDataInBuffInfo*> g_qptSPRprtBuffFree; //信处上报数据空闲缓冲区
extern safe_queue<SPReportDataInBuffInfo*> g_qptSPRprtData; //信处上报数据

extern	AlgorithmParaInfo g_tAlgorithmPara;
extern SysParaInfo g_tSysPara;
extern OneCPIMsrInfo g_tOneCPIMsrPnt;
extern OneAziMajorMsrInfo g_tOneAziMajorMsrPnt;
extern OneCircleMsrInfo g_tOneCircleMsrPnt;

extern std::list<TgtTrackPntInfo> g_LTgtTrack;
extern std::list<TgtTrackPntInfo> g_LTgtTrackCpy;
extern std::list<TmpTrackInfo> g_LTmpTrack;

extern std::queue<UINT16> g_qusTrackID; //未使用过的航迹批号
extern std::vector<UINT16> g_vusTrackIDUsed; //使用过的航迹批号  

extern std::queue<UINT16> g_qusPseudoTrackID; //未使用过的伪航迹批号
extern std::vector<UINT16> g_vusPseudoTrackIDUsed; //使用过的伪航迹批号

extern safe_queue<UINT32> g_qunSendTrackTrigger; //航迹发送触发队列

extern safe_queue<UINT32> g_qunSendScheduleRqstrigger; //调度请求发送触发队列

extern std::queue<UINT8> g_qucIDForTrackMannual; //手动跟踪批号
extern pthread_mutex_t mutexIDForTrackMannual;

extern std::map<BeamInfo, SPReportDataInBuffInfo*, CompareBeam> g_aMMsrPacketMap[MAX_ANTENNA_NUM];
extern std::list<BeamInfo> g_aLBeamCoordXInMap[MAX_ANTENNA_NUM];
extern std::map<BeamInfo, SPReportDataInBuffInfo*, CompareBeam> g_aMLeftEdgeMap[MAX_ANTENNA_NUM];
extern std::map<BeamInfo, SPReportDataInBuffInfo*, CompareBeam> g_aMRightEdgeMap[MAX_ANTENNA_NUM];

extern safe_queue<TrackSendMsg> g_qTrackSendMsg;

extern std::atomic<bool> g_bIsRunning;