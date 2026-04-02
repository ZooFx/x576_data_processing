#include "StructDef.h"
#include "SafeQueue.h"
#include "MacroDef.h"
#include <list>
#include <queue>
#include <mutex>
#include <map>
#include <atomic>


SPReportDataInBuffInfo g_atSPRprtBuff[MAX_PAK_NUM]; //信处上报数据存储缓冲区
safe_queue<SPReportDataInBuffInfo*> g_qptSPRprtBuffFree; //信处上报数据空闲缓冲区
safe_queue<SPReportDataInBuffInfo*> g_qptSPRprtData; //信处上报数据存储队列

AlgorithmParaInfo g_tAlgorithmPara, g_tAlgorithmParaTWS, g_tAlgorithmParaTAS;
SysParaInfo g_tSysPara;
OneCPIMsrInfo g_tOneCPIMsrPnt;
OneAziMajorMsrInfo g_tOneAziMajorMsrPnt;
OneCircleMsrInfo g_tOneCircleMsrPnt;

std::list<TgtTrackPntInfo> g_LTgtTrack;
std::list<TgtTrackPntInfo> g_LTgtTrackCpy;
std::list<TgtTrackPntInfo> g_Lpending_conf_track;
std::list<TmpTrackInfo> g_LTmpTrack;

std::queue<UINT16> g_qusTrackID; //未使用过的航迹批号
std::vector<UINT16> g_vusTrackIDUsed; //使用过的航迹批号

std::queue<UINT16> g_quspending_conf_track_id; //未使用过的航迹批号
std::vector<UINT16> g_vuspending_conf_track_id_used; //使用过的航迹批号

std::queue<UINT16> g_qusPseudoTrackID; //未使用过的伪航迹批号
std::vector<UINT16> g_vusPseudoTrackIDUsed; //使用过的伪航迹批号

safe_queue<UINT32> g_qunSendTrackTrigger; //航迹发送触发队列

safe_queue<UINT32> g_qunSendScheduleRqstrigger; //调度请求发送触发队列

std::queue<UINT8> g_qucIDForTrackMannual; //手动跟踪批号
pthread_mutex_t mutexIDForTrackMannual;

std::map<BeamInfo, SPReportDataInBuffInfo*, CompareBeam> g_aMMsrPacketMap[MAX_ANTENNA_NUM];
std::list<BeamInfo> g_aLBeamCoordXInMap[MAX_ANTENNA_NUM];
std::map<BeamInfo, SPReportDataInBuffInfo*, CompareBeam> g_aMLeftEdgeMap[MAX_ANTENNA_NUM];
std::map<BeamInfo, SPReportDataInBuffInfo*, CompareBeam> g_aMRightEdgeMap[MAX_ANTENNA_NUM];

safe_queue<TrackSendMsg> g_qTrackSendMsg;

std::atomic<bool> g_bIsRunning{true};
