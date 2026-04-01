#ifndef COMMUNICATEPROTOCOL_H_
#define COMMUNICATEPROTOCOL_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "DataTypeDef.h"
#include "EnumDef.h"

//分机IP
#define DP_IP                 "192.168.64.3"
#define DISP_IP               "192.168.64.4"
#define SP_IP                 "192.168.64.3"
#define SCHEDULE_IP           "192.168.64.3"
#define ANY_IP                "0.0.0.0"


//通信端口
#define SP_SEND_PORT_2DP          6005          
#define DP_RCV_PORT_FOR_SP        8005 
#define DP_SEND_PORT_2DISP        6006           
#define DISP_RCV_PORT_FOR_DP      8006 
#define DP_SEND_PORT_2SCHEDULE    6007
#define SCHEDULE_RCV_PORT_FOR_DP  8007
#define DISP_SEND_PORT_2DP        6008          
#define DP_RCV_PORT_FOR_DISP      8008     

//分机ID
#define SCHEDULE_ID   0xBB01
#define SP_ID         0xBB02
#define DP_ID         0xBB03
#define DISP_ID       0xBB04

//协议头尾标志
#define HEAD_FLAG 0xFA55FA55
#define TAIL_FLAG 0x55FA55FA

//消息编号
#define MSR_PNT_MSG_ID           0xDD01 //检测点迹
#define TRACK_FOR_DISP_MSG_ID    0xEE01 //目标航迹
#define SCHEDULE_REQUEST_MSG_ID  0xFF01 //跟踪调度请求
#define TRACK_MANNUAL_MSG_ID     0xDF01 //手动跟踪

//消息前缀长度
#define MSG_PREFIX_LEN 16

#pragma pack(push)
#pragma pack(1)

//报文前缀
typedef struct
{
	UINT32 unHeadFlag; //头标志
	UINT16 usSrcID; //源地址,分机编号
	UINT16 usDstID; //目的地址,分机编号
	UINT32 unComunicatCnt; //通信次数
	UINT16 usMsgLen; //报文总长度,包含头尾+数据总长度
	UINT16 usMsgID; //消息编号
}MsgPrefixInfo;

//报文后缀
typedef struct
{
	UINT8 ucCheckCode; //校验码
	UINT32 unTailFlag; //尾标志
}MsgSuffixInfo;


//控制表
typedef struct
{
	MsgPrefixInfo tPrefix;
	UINT32 unCPI;
	UINT16 usYear;
	UINT8 ucMon;
	UINT8 ucDay;
	UINT8 ucHour;
	UINT8 ucMin;
	UINT8 ucSec;
	UINT32 unMicroSec;
	UINT8 aucRsv0[1];
	LONG lLat_9; //单位：1e-9度
	LONG lLon_9; //单位：1e-9度
	INT32 nAlt_3;//单位：1e-3米
	UINT32 unYaw_3; //阵面偏航,量化刻度1e-3度
	INT32 nPitch_3;  //阵面俯仰,量化刻度1e-3度
	INT16 sRoll_2; //阵面横滚，量化刻度1e-2度
	UINT8 aucRsv1[2];
	UINT8 ucWorkMode; //0：常规机扫模式（TWS）1：常规相扫模式（TAS）
	UINT8 ucScheduleType; //0：确认  1：跟踪  2：搜索  3：空闲
	UINT8 ucFcIdx; //9.2GHz~10GHz每隔20MHz一个频点,41个频点
	UINT8 ucWaveID; //0：点频  1：上调频LFM  2：下调频LFM
	UINT8 ucBandWidthID; //0：6MHz  1：15MHz
	UINT8 ucSampleRateID; //0：8MHz  1：20MHz
	INT16 sAziMajor_2; //0.01°量化
	INT16 sEleMajor_2; //0.01°量化
	UINT8 aucRsv2[32];
	UINT16 usPulseNum; //相参积累脉冲数
	UINT16 usPRT_1; //量化单位:0.1us 生效脉冲重复周期之和：T=T1+T2+T3
	UINT8 aucRsv3[56];
	UINT8 ucTRRcv; //0：关 1：开
	UINT8 ucTRTrans; //0：关 1：开
	UINT16 usAziBgnScan_2; //[0 360]量化单位：0.01°机扫模式使用
	UINT16 usAziEndScan_2; //[0 360]量化单位：0.01°机扫模式使用
	UINT8 aucRsv4[237];
	UINT8 ucQuickInitWindwLenTH;   //快起批窗口长度
	UINT8 ucQuickInitPntNumTH;     //快起批点数
	UINT8 ucTermntWindwLenTH; //终结窗口长度
	UINT8 ucTermntPntNumTH;   //终结点数
	UINT16 usModelNoise_2;    //模型噪声强度,量化刻度1e-2
	UINT16 usVrMin_1;      //最小径向速度,量化刻度1e-1
	UINT16 usVrMax_1;      //最大径向速度,量化刻度1e-1
	UINT16 usAziAssocTH_1; //方位角关联阈值,量化刻度1e-1
	UINT16 usEleAssocTH_1;      //俯仰角关联阈值,量化刻度1e-1
	UINT16 usVrAssocTH_1;       //多普勒速度关联阈值,量化刻度1e-1
	UINT16 usGamma_1;           //统计距离阈值,量化刻度1e-1
	UINT8 ucRngCentroidTH;  //距离凝聚波门
	UINT8 ucAziCentroidTH_1;  //方位角凝聚波门,量化刻度1e-1
	UINT8 ucEleCentroidTH_1;  //俯仰角凝聚波门,量化刻度1e-1
	UINT8 ucVrCentroidTH_1;   //多普勒速度凝聚波门,量化刻度1e-1
	INT8 cBeamType; //波位类型,-1:左边界，0:中间，1:右边界
	UINT8 ucBeamCoordX; //波位横坐标
	UINT8 ucBeamCoordY; //波位纵坐标
	UINT16 usBeamID; //波位序号
	UINT32 unCycleID; //扫描圈计数
	UINT8 aucRsv5[3];
	UINT16 usTASID; //TAS目标批号
	UINT16 usTASRngPred; //TAS预测距离
	INT16 sTASVrPred_1; //TAS预测径向速度 单位：0.1m/s
	INT16 sTASAccrPred_1; //TAS预测径向加速度 单位：0.1m/s^2
	INT16 sTASAziPred_2; //TAS预测方位角 单位：0.01°
	INT16 sTASElePred_2; //TAS预测俯仰角 单位：0.01°
	INT16 sTASAltPred; //TAS预测高度
	INT16 sVelPRed_1; //TAS预测速度 单位：0.1m/s
	UINT8 aucRsv6[48];
	MsgSuffixInfo tMsgSuffix;
}CtrlListMsg;

//信处上报单个点迹信息
typedef struct
{
	FLOAT32 fDist;
	FLOAT32 fVr;
	FLOAT32 fAzi;
	FLOAT32 fEle;
	FLOAT32 fAlt;
	FLOAT32 fAmp;
	FLOAT32 fSNR_CFAR; //CFAR信噪比
	FLOAT32 fSNR_STATISTIC; //统计信噪比
	FLOAT32 fAziMajor;
	FLOAT32 fEleMajor;
	UINT32 unRngCell;
	UINT32 unDplCell;
	FLOAT32 fConfidence; //点迹源自目标的置信度
	UINT32 untar_type;//目标识别结果, 0：其他，1：无人机
}SPSendMsrPntInfo;

//阵面ENU坐标系中单个航迹点信息
typedef struct
{
	UINT16 usTrackID;
	UINT16 usCPI;
	UINT32 unUTCSec; //UTC秒数，从1970年1月1日0时0分0秒至点迹录取时刻的相对时间
	UINT32 unMicroSec; //微秒计数
	enum TgtTrackUpdtModeInfo enumTrackUpdtMode; //航迹更新方式
	FLOAT32 fAmp; //幅度
	FLOAT32 fSNR; //信噪比
	FLOAT32 fRng;
	FLOAT32 fAzi;
	FLOAT32 fEle;
	FLOAT32 fAlt;//高度，加阵面仰角得到相对水平面的仰角
	FLOAT32 fVr;//径向速度
	FLOAT32 fVel;//空间速度
	FLOAT32 fAcc;//加速度
	UINT32  untar_type;//目标识别结果, 0：其他，1：无人机
	FLOAT32 afRsv[2];
}OneTrackPointENUInfo;

typedef struct
{
	MsgPrefixInfo tMsgPrefix;
	UINT16  usTgtNum;
	OneTrackPointENUInfo* ptOneFrmTrackPntENU;
	MsgSuffixInfo tMsgSuffix;	
}OneFrmTrackPointENUMsg;

//跟踪调度请求
typedef struct
{
	UINT16 usScheduleRequestIndex; //请求流水号
	UINT8 ucScheduleType; //调度请求类型
	UINT16 usYear;
	UINT8 ucMon;
	UINT8 ucDay;
	UINT8 ucHour;
	UINT8 ucMin;
	UINT8 ucSec;
	UINT32 unMicroSec;
	UINT8 ucRsv;
	UINT16 usTrackID;
	UINT16 usRngPred; //目标预测距离
	INT16 sVrPred_1; //目标预测径向速度, 量化单位0.1m/s
	INT16 sArPred_1; //目标预测径向加速度,量化单位0.1m/s2
	INT16 sAziPred_2; //目标预测方位角,量化单位：0.01°
	INT16 sElePred_2; //目标预测俯仰角,量化单位：0.01°
	INT16 sAltPred; //目标预测高度
	INT16 sVelPred_1; //目标预测航向速度, 量化单位0.1m/s
	UINT8 aucRsv[4];
}ScheduleRequestInfo;

typedef struct
{
	MsgPrefixInfo tMsgPrefix;
	ScheduleRequestInfo tScheduleRqst;
	MsgSuffixInfo tMsgSuffix;
}ScheduleRqstMsg;

//手动跟踪指令
typedef struct
{
	UINT8 ucTrackID;
}TrackMannualInfo;

#pragma pack(pop)

extern INT32 BuildSockAddr(const INT8* pcIP, UINT16 usPort, struct sockaddr_in& tSockAddr);
extern INT32 CheckMsg(VOID* ptrBuff, UINT32 unMsgLen);
extern INT32 BuildMsgPrefix(UINT32 unHeadFlag, UINT16 usDataLen, UINT32 unComCnt, UINT16 usSrcID, UINT16 usDestID, UINT16 usMsgID, MsgPrefixInfo& tMsgPrefix);
extern UINT8 GetXORCode(VOID* ptrData, UINT32 unLength);

#endif
