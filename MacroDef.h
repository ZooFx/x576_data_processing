#pragma once

#define PI 3.14159265358979323846
#define CVEL 299792458.0 //光速
#define INF 99999999
#define RNG_CELL 10.0 //距离单元
#define AZI_3dB 4.6 //方位维接收波束宽度
#define ELE_3dB 4.6 //俯仰维接收波束宽度
#define MSR_DIM 3 //量测维度
#define ONE_COORD_STATE_DIM 3      /*单个坐标轴目标状态维度*/
#define RNG_STD RNG_CELL/5 //测距精度
#define AZI_STD AZI_3dB/5 //方位角精度
#define ELE_STD ELE_3dB/5 //俯仰角精度
#define VR_STD 2
#define RATIO_OF_VR_STD_TO_DOPPLER_UNIT 1.0/5 //测速精度相对于单通道多普勒速度的占比
#define ANGLE_TH 90 //关联阶段航向偏转角阈值
#define MODEL_NOISE 0.01
#define TIME_STEP 0.5 //数据率2Hz
#define SNR_REF 15 //参考SNR
#define COORD_X_IN_MAP_TH 6 //搜索数据缓存方位角数量阈值
#define MAX_XCOORD_INTERVEL 10 //可关联的最大波位方位坐标间距
#define MAX_COORD_INTERVEL_CENTROID 10 //可凝聚的最大波位坐标间距
#define BUFFER_NUM 2 //接收缓冲区数量           
#define BUFFER_SIZE 64*1024 //64Ki,接收缓冲区大小
#define RADAR_INDEX_OFFSET 528 //信处上报点迹数据，移动到雷达编号的偏置
#define MSR_PNT_NUM_OFFSET 529 //信处上报点迹数据，移动到点迹数量的偏置
#define MAX_BEAM_NUM 450 //最大扫描范围（方位±45， 俯仰±45）所需波位数
#define MAX_MSR_NUM 1024 //检测点数量最大值
#define MAX_PAK_NUM 1024 //信处数据包缓存上限
#define MAX_MSR_NUM_ONE_AZIMAJOR 4096 //一个方位指向点迹上限
#define STATE_DIM 9 //状态向量维度
#define	MAX_PNT_NUM_IN_TMP_TRACK 16 //暂态航迹点迹数量上限
#define MAX_TGT_NUM 255
#define MAX_TMP_TRACK_NUM 65535
#define ONE_COORD_STATE_DIM 3
#define MAX_ANTENNA_NUM 4 //天线数量上限
#define SNR_ACCUM_ARR_LEN 16 //信噪比积累数组长度
#define MAX_TAS_NUM 10
#define MAX_FC_ID 80//频点索引最大值
#define MAX_XCOORD 112//X坐标最大值
