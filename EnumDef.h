#pragma once
#include "DataTypeDef.h"
#include <type_traits>

//调度事件类型
enum ScheduleEventInfo : UINT8
{
	CONFIRM = 0,
	TRACK,
	SEARCH,
	IDLE
};

//工作方式
enum WorkFashionInfo : UINT8
{
	TWS = 0,
	TAS
};

//测量精度计算方式枚举
enum MsrStdEstFashionInfo : UINT8
{
	STATIC = 0, //静态测量精度
	DYNAMIC     //动态测量精度
};


//航迹更新方式
enum TgtTrackUpdtModeInfo : UINT8
{
	FILTER = 0,
	PRED,
	TERMINATE
};

//候选航迹状态
enum TmpTrackStateInfo : UINT8
{
	TURN_TO_TGT_TRACK = 0, //起批
	REMAIN, //保留
	CANCEL //撤销
};

//候选航迹起批模式
enum TmpTrackInitModeInfo : UINT8
{
    QUICK = 0, //快起批
    STANDARD, //标准起批
    SLOW //慢起批
};

//量测成员枚举
enum MsrPntMemberInfo : UINT8 {
	RNG = 0,
	AZI,
	ELE,
	VR,
	SNR
};

//点迹凝聚方式
enum CentroidFashionInfo : UINT8
{
	MAX_AMP = 0,  //最强信噪比
	AVERAGE      //加权平均
};

//波位类型
enum BeamTypeInfo : INT8
{
	LEFT_EDGE = -1,
	MIDDLE = 0,
	RIGHT_EDGE = 1
};

template <typename E>
typename std::enable_if<std::is_enum<E>::value, int>::type
format_as(E e) noexcept
{
    return static_cast<int>(e);
}
