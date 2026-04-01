#pragma once
#include "MacroDef.h"
#include "DataTypeDef.h"
#include "EnumDef.h"
#include "SpdLogger.h"
#include <deque>
#include <list>

//姿态角
typedef struct
{
	DOUBLE dYaw;    //偏航角,yaw
	DOUBLE dPitch;  //俯仰角, pitch
	DOUBLE dRoll;   //横滚角,roll
} AttitudeInfo;

//经纬高
typedef struct
{
	DOUBLE dLat;    
	DOUBLE dLon;  
	DOUBLE dAlt;   
} LLAInfo;

//日期类型
typedef struct
{
	UINT16 usYear;
	UINT8 ucMon;
	UINT8 ucDay;
	UINT8 ucHour;
	UINT8 ucMin;
	UINT8 ucSec;
	UINT32 unMicroSec;
}DateInfo;


//数据处理算法参数
typedef struct
{
	UINT8 ucQuickInitWindwLenTH;
	UINT8 ucQuickInitPntNumTH;
	UINT8 ucStandardInitWindwLenTH;
	UINT8 ucStandardInitPntNumTH;
    UINT8 ucSlowInitWindwLenTH;
    UINT8 ucSlowInitPntNumTH;
	UINT8 ucConfirmWindwLenTH;
	UINT8 ucConfirmPntNumTH;
	UINT8 ucTermntWindwLenTH;
	UINT8 ucTermntPntNumTH;
	UINT8 ucCancleInitPntNumTH;
	DOUBLE dModelNoise;
	DOUBLE dVrMin; //径向速度最小值
	DOUBLE dVrMax; //径向速度最大值
	DOUBLE dQuickInitAziAssocTH; //方位关联阈值
	DOUBLE dEleAssocTH; //俯仰关联阈值
	DOUBLE dAltAssocTH; //高度关联阈值
	DOUBLE dQuickInitVrAssocTH; //多普勒速度关联阈值
	DOUBLE dGamma;
	DOUBLE dRngCentroidTH;  //距离凝聚波门
	DOUBLE dAziCentroidTH;  //方位凝聚波门
	DOUBLE dEleCentroidTH;  //俯仰凝聚波门
	DOUBLE dVrCentroidTH;   //多普勒速度凝聚波门
	DOUBLE dDefAngleForTmpTrackTH; //偏转角阈值
	DOUBLE dDefAngleForTgtTrackTH; //偏转角阈值
	DOUBLE dHighConfVrTH;   //高置信度速度阈值
	DOUBLE dHighConfRngTH;  //高置信度距离阈值
	DOUBLE dQuickInitRngTH;  //快速起批距离阈值
	DOUBLE dSlowInitAziAssocTH; //方位关联阈值-低置信度航迹
	DOUBLE dSlowInitVrAssocTH; //多普勒速度关联阈值-低置信度航迹
	DOUBLE dRngDiffTH; //距离微分阈值-低置信度航迹
	DOUBLE dStdRngTH;  //距离标准差阈值-低置信度航迹
	DOUBLE dStdAziTH;  //方位角标准差阈值-低置信度航迹
	DOUBLE dStdEleTH;  //俯仰角标准差阈值-低置信度航迹
	DOUBLE dStdVrTH;  //多普勒速度标准差阈值-低置信度航迹
	DOUBLE dStdSNRTH;  //信噪比标准差阈值-低置信度航迹
	DOUBLE dConfidenceTH; //量测源自目标的置信度阈值
	DOUBLE adSNRAccumTH[SNR_ACCUM_ARR_LEN];  //SNR积累阈值-低置信度航迹
} AlgorithmParaInfo;

//雷达系统参数
typedef struct
{
	DOUBLE dB;			                       /*波形带宽*/
	DOUBLE dAzi3dB;                            /*方位维波束宽度*/
	DOUBLE dEle3dB;	                           /*俯仰维波束宽度*/
	UINT16 usCPINumOneCircle;	               /*搜索一圈CPI数量*/
	DOUBLE dFc;			                       /*载频*/
	DOUBLE dLambda;                            /*波长*/
	DOUBLE dPRT;                               /*当前脉冲重复周期*/
	UINT16 usPulseNum;                         /*积累脉冲数*/
	enum WorkFashionInfo enumWorkMode;         /*工作模式*/
	enum ScheduleEventInfo enumScheduleEvent;  /*调度事件*/
	UINT16 usTgtID;                            /*TAS的目标航迹批号*/
	AttitudeInfo tRadarArrayAttitude;          /*雷达阵面姿态*/
	UINT8 ucAntennaIdx;                        /*天线编号，取值0-3*/
	enum BeamTypeInfo enumBeamType;                            //波位类型,-1:左边界，0:中间，1:右边界
	UINT8 ucBeamCoordX;                  //波位X坐标
	UINT8 ucBeamCoordY;                  //波位Y坐标
} SysParaInfo;

//量测点迹保存格式
typedef struct
{
	UINT32 unIdx;       /*点迹在信号处理发出的udp报文中序号*/
	FLOAT32 fRng;      /*径向距离*/
	UINT32 unRngCell;   /*距离单元*/
	FLOAT32 fAzi;       /*方位角*/
	FLOAT32 fAziMajor;  /*方位主瓣指向*/
	FLOAT32 fEle;       /*俯仰角*/
	FLOAT32 fAlt;       /*高度*/
	FLOAT32 fEleMajor;  /*俯仰主瓣指向*/
	FLOAT32 fVr;        /*径向速度*/
	UINT32 unDplCell;   /*多普勒单元*/
	FLOAT32 fAmp;       /*信号幅度*/
	FLOAT32 fSNR;       /*信噪比*/
	DateInfo tDate;
	DOUBLE dTime;      /*点迹捕获时间*/
	DOUBLE dPRT;
	UINT32 unCPI;       /*点迹捕获帧号*/
	FLOAT32 fConfidence; //点迹源自目标的置信度
	UINT32 untar_type; //目标识别结果，0：其他，1：无人机
	UINT8 ucCentroidFlag; //凝聚标记
} MSrPntInBuffInfo;

//信处发送系统参数、目标类型、点迹信息
typedef struct
{
	DOUBLE dB;				         /*波形带宽*/
	DOUBLE dFc;			             /*载频*/
	DOUBLE dPRT;                     /*脉冲重复周期*/
	UINT32 unPulseNum;               /*积累脉冲数*/
	enum WorkFashionInfo enumWorkMode; //0：常规机扫模式（TWS）1：常规相扫模式（TAS）
	enum ScheduleEventInfo enumScheduleEvent;  /*调度事件类型*/
	DOUBLE dAziMajor;                /*波束方位指向*/
	DOUBLE dEleMajor;                /*波束俯仰指向*/
	UINT16 usTgtID;                  /*TAS模式下探测的目标批号*/
	DateInfo tDate;
	DOUBLE dTime;			         /*点迹录取时间*/
	UINT32 unCPI;				     /*点迹录取帧号*/
	AlgorithmParaInfo tDPAlgrthmPara; /*数据处理算法参数*/
	UINT8 ucRadarID; //雷达ID
	AttitudeInfo tAttitude;             /*阵面姿态*/
	LLAInfo tLLA; //雷达LLA
	UINT16 usCPINumOneCircle;          /*扫描一圈CPI数量*/
	UINT8 ucAntennaIdx;                 /*天线编号，取值0-3*/
	enum BeamTypeInfo enumBeamType;     //波位类型,-1:左边界，0:中间，1:右边界
	unsigned int uncycle_id;            //搜索圈序号
	unsigned short usbeam_id;           //波位序号
	UINT8 ucBeamCoordX;                  //波位X坐标
	UINT8 ucBeamCoordY;                  //波位Y坐标
	UINT32 unMsrNum;			               /*量测点迹数量*/
	MSrPntInBuffInfo atMsrPnrArr[MAX_MSR_NUM]; /*量测点迹数组*/
} SPReportDataInBuffInfo;

//测量精度
typedef struct
{
	DOUBLE dSgmR;	 /*测距精度*/
	DOUBLE dSgmAzi; /*方位角测角精度*/
	DOUBLE dSgmEle; /*俯仰角测角精度*/
	DOUBLE dSgmVr;	 /*测速精度*/
}MsrStdDevInfo;

//数据处理使用的点迹类型
typedef struct
{
	UINT32 unIdx;                /*点迹在信号处理发出的udp报文中序号*/
	DOUBLE dRng;			     /*径向距离*/
	DOUBLE dAzi;				 /*方位角*/
	DOUBLE dEle;				 /*俯仰角*/
	DOUBLE dAlt;                 /*高度*/
	DOUBLE dVr;				     /*径向速度*/
	DOUBLE dPRT;                 /*点迹录取时脉冲重复周期*/
	DOUBLE dAmp;				 /*信号幅度*/
	DOUBLE dSNR;				 /*信噪比*/
	MsrStdDevInfo tMsrStdDev;	 /*点迹测量精度*/
	DateInfo tDate;
	DOUBLE dTime;			     /*点迹捕获时间*/
	UINT32 unCPI;				 /*点迹捕获帧号*/
	DOUBLE dAziMajor;            /*接收波束方位指向*/
	DOUBLE dEleMajor;            /*接收波束俯仰指向*/
	UINT8 ucAntennaIdx;			 /*点迹所在天线编号*/
	enum BeamTypeInfo enumBeamType;		         /*点迹所在波位类型*/
	UINT8 ucBeamCoordX;		     /*点迹所在波位X坐标*/
	UINT8 ucBeamCoordY;		     /*点迹所在波位Y坐标*/
	UINT32 unRngCell;            /*距离单元*/
	UINT32 unDplCell;            /*距离单元*/
	DOUBLE adRctMsr[MSR_DIM];    /*点迹直角坐标表示的位置*/
	DOUBLE adRctMsrCov[MSR_DIM * MSR_DIM]; /*直角坐标位置向量的协方差矩阵*/
	UINT8 ucAssocFlag;			 /*点迹关联标志，关联(1), 未关联(0)*/
	UINT8 ucCentroidFlag;        /*点迹凝聚标志*/
	DOUBLE dConfidence;
	UINT32 untar_type; //目标识别结果, 0：其他，1：无人机
} MsrPntInfo;

//一帧点迹
typedef struct
{
	UINT32 unCircleCnt;
	UINT32 unCPI;
	DateInfo tDate;
	DOUBLE dTime;
	DOUBLE dPRT;
	UINT32 unPulseNum;
	UINT32 unMsrPntNum;
	MsrPntInfo atMsrPntArr[MAX_MSR_NUM];
} OneCPIMsrInfo;

//一个方位指向点迹
typedef struct
{
	UINT8 ucAntennaIdx;
	UINT8 ucBeamCoordX;
	enum BeamTypeInfo enumEdgeTypeInData; //当前方位数据中包含的边界类型
	DOUBLE dTime;
	DateInfo tDate;
	UINT32 unMsrPntNum;
	MsrPntInfo atMsrPntArr[MAX_MSR_NUM_ONE_AZIMAJOR];
} OneAziMajorMsrInfo;

//一圈点迹
typedef struct
{
	UINT32 unMsrPntNum;
	DOUBLE dTime;
	MsrPntInfo atMsrPntArr[MAX_BEAM_NUM*MAX_MSR_NUM];
}OneCircleMsrInfo;

//雷达坐标系中目标RAE状态估计
typedef struct
{
	DOUBLE dRngEst; /*距离*/
	DOUBLE dAziEst; /*方位*/
	DOUBLE dEleEst; /*俯仰*/
	DOUBLE dAltEst; /*高度*/
	DOUBLE dVrEst;   /*多普勒速度*/
}RAEestInfo;

//数处预测信息
typedef struct
{
	DOUBLE dRngPre;
	DOUBLE dAziPre;
	DOUBLE dElePre;
	DOUBLE dVrPre;
	DOUBLE dVelPre;
	DOUBLE dAccPre;
	DOUBLE dAltPre;
} PredRAEInfo;

//数据关联信息
typedef struct
{
	UINT32 unAntennaIdx;
	UINT32 unCPI;
	UINT32 unIdx;
	MsrPntInfo tMsrPnt;
}PntAssociatedInfo;

//确认航迹信息
typedef struct
{
	UINT16 usTrackID;			             /*航迹批号*/
	UINT32 untar_type;                       //目标识别结果, 0：其他，1：无人机
	UINT16 usFromTmpTrackID;                 /*与目标航迹对应的暂态航迹号*/
	UINT32 unStartFrm;			             /*起批帧号*/
	UINT32 unLstUpdFrm;			             /*最后一次更新帧号*/
	DateInfo tLstUpdtDate;					 /*最后一次更新日期*/
	DOUBLE dLstUpdTime;		                 /*最后一次更新时间*/
	UINT32 unLen;			                 /*航迹维持帧数*/
	DOUBLE dModelNoise;                      /*过程噪声强度*/
	std::deque<UINT8> qucConfirmWindow;      /*目标确认窗口*/
	UINT16 usConfirmWndwLen;			     /*目标确认窗长度*/
	UINT16 usConfirmNum;					 /*目标确认次数*/
	std::deque<UINT8> qucTerminentWindow;    /*数据关联窗口*/
	UINT16 usTermntWndwLen;			         /*航迹终结窗长度*/
	UINT16 usMissNum;					     /*关联失败次数*/
	TgtTrackUpdtModeInfo enumTrackUpdMode;   /*目标航迹更新方式,0--滤波，1--预测*/
	WorkFashionInfo enumWorkFashion;         /*航迹维持方式*/
	ScheduleEventInfo enumScheduleEvent;     /*资源调度状态*/
	BOOL bAssocFlag;                         /*数据关联标记*/
	UINT8 ucAntennaIdx;                      /*目标所在天线编号*/
	enum BeamTypeInfo enumBeamType;          /*目标所在波位类型*/
	UINT8 ucBeamCoordX;                      /*目标所在波位X坐标*/
	UINT8 ucBeamCoordY;                      /*目标所在波位Y坐标*/
	DOUBLE adXest[STATE_DIM];			     /*目标状态向量-阵面天线坐标系*/
	DOUBLE adXestLast2[STATE_DIM];			 /*目标倒数第2帧状态向量-阵面天线坐标系*/
	DOUBLE adPest[STATE_DIM * STATE_DIM];    /*状态向量协方差矩阵*/
	RAEestInfo tRAEest;                      /*目标RAE、Alt和Vr估计--阵面天线坐标系*/
	DOUBLE adXestENU[STATE_DIM];             /*目标状态向量--阵面中心ENU坐标系*/
	DOUBLE adPestENU[STATE_DIM*STATE_DIM];   /*状态向量协方差矩阵--阵面中心ENU坐标*/
	RAEestInfo tRAEestENU;                   /*目标RAE、Alt和Vr估计--阵面中心ENU坐标系*/
	DOUBLE dPRT;                             /*航迹更新时的脉冲PRT*/
	DOUBLE dVel;						     /*目标空间速度大小*/
	DOUBLE dAcc;						     /*目标空间加速度大小*/
	DOUBLE dVelAzi;                          /*目标航向*/
	DOUBLE dAmp;						     /*目标回波幅度*/
	DOUBLE dSNR;						     /*目标回波信噪比*/
	PredRAEInfo tPredRAE;			         /*针对下一帧的预测波门*/
	ScheduleEventInfo enumPredScheduleEvent; /*针对下一帧的调度类型*/
	DOUBLE adInnov[MSR_DIM];                 /*新息*/
	DOUBLE adS[MSR_DIM*MSR_DIM];             /*新息协方差矩阵*/
	DOUBLE dMsr2GateDist;                    /*量测点至预测波门距离*/
	PntAssociatedInfo tPntAssociatd;         /*当前帧关联点迹*/
	std::list<MsrPntInfo> LtMsrForInit;      /*航迹起始阶段的点迹*/
} TgtTrackPntInfo;

//暂态航迹信息
typedef struct
{
	UINT16 usPseudoTrackID;                   /*伪航迹批号*/
	UINT32 untar_type;                        //目标识别结果, 0：其他，1：无人机
	UINT16 usToTrackID;                       /*起批后转至对应的目标航迹*/
	UINT32 unStartFrm;					      /*首次捕获帧号*/
	UINT32 unLstUpdFrm;					      /*最后一次更新帧号*/
	DateInfo tLstUpdtDate;					  /*最后一次更新日期*/	
	DOUBLE dLstUpdTime;			              /*最后一次更新时间*/
	UINT16 usPntNum;						  /*关联成功的点迹数量*/
	UINT16 usInitWndwLen;				      /*起批窗长度*/
	TmpTrackStateInfo enumTmpTrackState;      /*候选航迹状态*/
	TmpTrackInitModeInfo enumInitMode;        //起批方式
	BOOL bAssocFlag;				          /*数据关联标记*/
	UINT8 ucAntennaIdx;                       /*目标所在天线编号*/
	enum BeamTypeInfo enumBeamType;		      /*点迹所在波位类型*/
	UINT8 ucBeamCoordX;                       /*目标所在波位X坐标*/
	UINT8 ucBeamCoordY;                       /*目标所在波位Y坐标*/
	INT8 cIdxLastNonEmpty;                    /*最后1个非空量测索引*/
	INT8 cIdxLast2NonEmpty;				      /*倒数第2个非空量测索引*/
	std::deque<MsrPntInfo> qtMsrArr;          /*候选航迹关联到的全部量测点迹*/
	DOUBLE adXest[STATE_DIM];			      /*目标状态向量-阵面天线坐标系*/
	DOUBLE adPest[STATE_DIM * STATE_DIM];     /*状态向量协方差矩阵*/
} TmpTrackInfo;

//缓存数据Map Key类型
typedef struct 
{
	enum BeamTypeInfo enumBeamType;     //波位类型,-1:左边界，0:中间，1:右边界
	UINT8 ucBeamCoordX; //波位横坐标
	UINT8 ucBeamCoordY; //波位纵坐标
	DOUBLE dPRT;
	DOUBLE dTime;
}BeamInfo;

//map函数对象
struct CompareBeam
{
	BOOL operator() (const BeamInfo& tBeam1, const BeamInfo& tBeam2) const
	{
		if(tBeam1.ucBeamCoordX < tBeam2.ucBeamCoordX)
			return true;
		if(tBeam1.ucBeamCoordX > tBeam2.ucBeamCoordX)
			return false;

		if(tBeam1.ucBeamCoordY < tBeam2.ucBeamCoordY)
			return true;
		if(tBeam1.ucBeamCoordY > tBeam2.ucBeamCoordY)
			return false;

		return false;
	}
};

struct TrackSendMsg
{
	UINT32 unTriggerNum;
	std::shared_ptr<std::list<TgtTrackPntInfo>> pTrackList;
};

struct TgtTrackNumInfo
{
	UINT32 unNumSearch;
	UINT32 unNumConfirm;
	UINT32 unNumTrack;
};
