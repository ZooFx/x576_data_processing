#pragma once
#include "StructDef.h"

extern VOID* DPRcvMsrPnt(VOID*);
extern INT32 ParseMsgDPRcvSPMsrPnt(UINT8* pRcvBuffer, SPReportDataInBuffInfo* ptSPRprtDataBuff, UINT32 unRcvCnt);
