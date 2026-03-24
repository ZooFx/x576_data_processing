#include "DataTypeDef.h"
#include <cmath>
#include "MacroDef.h"

DOUBLE sind(DOUBLE dAngleDeg)
{
	DOUBLE dAngleRad = dAngleDeg / 180.0*PI;
	DOUBLE dRes = sin(dAngleRad);
	return dRes;
}

DOUBLE cosd(DOUBLE dAngleDeg)
{
	DOUBLE dAngleRad = dAngleDeg / 180.0*PI;
	DOUBLE dRes = cos(dAngleRad);
	return dRes;
}

DOUBLE tand(DOUBLE dAngleDeg)
{
	DOUBLE dAngleRad = dAngleDeg / 180.0*PI;
	DOUBLE dRes = tan(dAngleRad);
	return dRes;
}

DOUBLE asind(DOUBLE dSinValue)
{
	DOUBLE dAngleRad = asin(dSinValue);
	DOUBLE dRes = dAngleRad / PI * 180.0;
	return dRes;
}

DOUBLE acosd(DOUBLE dCosValue)
{
	DOUBLE dAngleRad = acos(dCosValue);
	DOUBLE dRes = dAngleRad / PI * 180.0;
	return dRes;
}


DOUBLE atand(DOUBLE dy, DOUBLE dx)
{
	//坐标系为标准右手笛卡尔坐标系，角度正方向：x轴正向为起点，逆时针旋转为正，顺时针旋转为负

	//返回值为输入向量与x轴正向夹角，值域  [-180°,180°]

	if (dx == 0 && dy > 0)
		return 90.0;

	if (dx == 0 && dy < 0)
		return -90.0;

	if (dx == 0 && dy == 0)
		return 0.0;

	DOUBLE dratio = dy / dx;
	DOUBLE dAngleDeg = atan(dratio) / PI * 180.0;
	if (dx > 0)
		return dAngleDeg;
	else if (dy > 0)
		return dAngleDeg + 180.0;
	else
		return dAngleDeg - 180.0;
}

INT32 min(INT32 nArg1, INT32 nArg2)
{
	INT32 nRtn = (nArg1 <= nArg2) ? nArg1 : nArg2;

	return nRtn;
}

