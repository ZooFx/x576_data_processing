#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iterator>
#include "global.h"
#include "CommunicateProtocol.h"
#include "SpdLogger.h"
#include "dirent.h"
#include "DPFunc.h"


VOID* DPSendScheduleRqst(VOID*)
{
	//新建套接字
	INT32 fd = socket(AF_INET, SOCK_DGRAM, 0);
	if( fd < 0)
		ERROR("Socket fd build failed");

	//配置数据处理套接字地址结构
	struct sockaddr_in tDPAddr;
	bzero(&tDPAddr, sizeof(tDPAddr));
	BuildSockAddr(ANY_IP, DP_SEND_PORT_2SCHEDULE, tDPAddr);

	//套接字与地址结构绑定
	if( bind(fd, (struct sockaddr*)&tDPAddr, sizeof(tDPAddr)) < 0)
	{
		ERROR("bind failed");
		exit(1);
	}

	//配置调度软件套接字地址结构
	struct sockaddr_in tScheduleAddr;
	bzero(&tScheduleAddr, sizeof(tScheduleAddr));
	UINT32 unSendAddrLen = sizeof(tScheduleAddr);
	BuildSockAddr(SCHEDULE_IP, SCHEDULE_RCV_PORT_FOR_DP, tScheduleAddr);	

	//发送缓冲区
	INT8* cSendBuff = new INT8 [BUFFER_SIZE], *ptr = NULL;

	INT32 nTgtIdx = 0, nSendLen = 0;
	UINT32 unTgtTrackID = 0, unSendCnt = 0;
	UINT16 usRqstCnt = 0, usMsgLen = 0;
	std::list<TgtTrackPntInfo>::iterator iter;

	ScheduleRqstMsg tScheduleRqstMsg;
	bzero(&tScheduleRqstMsg, sizeof(ScheduleRqstMsg));
	BuildMsgPrefix(HEAD_FLAG, 0, 0, DP_ID, SCHEDULE_ID, SCHEDULE_REQUEST_MSG_ID, tScheduleRqstMsg.tMsgPrefix);
	tScheduleRqstMsg.tMsgSuffix.unTailFlag = TAIL_FLAG;

	while(true)
	{
		unTgtTrackID = g_qunSendScheduleRqstrigger.pop();

		nTgtIdx = FindTgtTrack(&g_LTgtTrack, unTgtTrackID);

		if(nTgtIdx >= 0)
		{
			iter = g_LTgtTrack.begin();
			std::advance(iter, nTgtIdx);

			if(iter->enumTrackUpdMode == TERMINATE)
				continue;
		}
		else{
			ERROR("no such target track with ID:{}", unTgtTrackID);
			continue;
		}

		tScheduleRqstMsg.tMsgPrefix.unComunicatCnt = unSendCnt;
		tScheduleRqstMsg.tMsgPrefix.usMsgLen = MSG_PREFIX_LEN + sizeof(ScheduleRequestInfo);

		tScheduleRqstMsg.tScheduleRqst.usScheduleRequestIndex = usRqstCnt;
		tScheduleRqstMsg.tScheduleRqst.ucScheduleType = iter->enumPredScheduleEvent;
		tScheduleRqstMsg.tScheduleRqst.usYear = iter->tLstUpdtDate.usYear;
		tScheduleRqstMsg.tScheduleRqst.ucMon = iter->tLstUpdtDate.ucMon;
		tScheduleRqstMsg.tScheduleRqst.ucDay = iter->tLstUpdtDate.ucDay;
		tScheduleRqstMsg.tScheduleRqst.ucHour = iter->tLstUpdtDate.ucHour;
		tScheduleRqstMsg.tScheduleRqst.ucMin = iter->tLstUpdtDate.ucMin;
		tScheduleRqstMsg.tScheduleRqst.ucSec = iter->tLstUpdtDate.ucSec;
		tScheduleRqstMsg.tScheduleRqst.unMicroSec = iter->tLstUpdtDate.unMicroSec;
		tScheduleRqstMsg.tScheduleRqst.usTrackID = iter->usTrackID;
		tScheduleRqstMsg.tScheduleRqst.usRngPred = UINT16(round(iter->tPredRAE.dRngPre));
		tScheduleRqstMsg.tScheduleRqst.sVrPred_1 = INT16(round(iter->tPredRAE.dVrPre * 1e1));
		tScheduleRqstMsg.tScheduleRqst.sArPred_1 = INT16(round(iter->tPredRAE.dAccPre * 1e1));
		tScheduleRqstMsg.tScheduleRqst.sAziPred_2 = INT16(round(iter->tPredRAE.dAziPre * 1e2));
		tScheduleRqstMsg.tScheduleRqst.sElePred_2 = INT16(round(iter->tPredRAE.dElePre * 1e2));
		tScheduleRqstMsg.tScheduleRqst.sAltPred = INT16(round(iter->tPredRAE.dAltPre));
		tScheduleRqstMsg.tScheduleRqst.sVelPred_1 = INT16(round(iter->tPredRAE.dVelPre * 1e1));

		ptr = cSendBuff;
		memcpy(ptr, &(tScheduleRqstMsg.tMsgPrefix), sizeof(MsgPrefixInfo));
		ptr += sizeof(MsgPrefixInfo);
		memcpy(ptr, &(tScheduleRqstMsg.tScheduleRqst), sizeof(ScheduleRequestInfo));
		tScheduleRqstMsg.tMsgSuffix.ucCheckCode = GetXORCode(cSendBuff, tScheduleRqstMsg.tMsgPrefix.usMsgLen);
		ptr += sizeof(ScheduleRequestInfo);
		memcpy(ptr, &(tScheduleRqstMsg.tMsgSuffix), sizeof(MsgSuffixInfo));

		usMsgLen = tScheduleRqstMsg.tMsgPrefix.usMsgLen + sizeof(MsgSuffixInfo);

		nSendLen = sendto(fd, cSendBuff, usMsgLen, 0, (struct sockaddr *)&tScheduleAddr, unSendAddrLen);

		if (nSendLen <= 0)
		{
			ERROR("schedule reqsut send failed. msg len:{}, send len:{}", usMsgLen, nSendLen);

			// debug
			//				INFO("Schedule requse, ID:{}, Rng:{:.2f}, Azi:{:.2f}, Ele:{:.2f}", iter->usTrackID, iter->tPredRAE.dRngPre, iter->tPredRAE.dAziPre, iter->tPredRAE.dElePre);
		}
		else
		{
			// INFO("send schedule, ID:{}, sched_type:{}, rng:{}, azi:{}, ele:{}, hour:{}, min:{}, sec:{:.3f}",
			// 	 tScheduleRqstMsg.tScheduleRqst.usTrackID, tScheduleRqstMsg.tScheduleRqst.ucScheduleType, tScheduleRqstMsg.tScheduleRqst.usRngPred, (tScheduleRqstMsg.tScheduleRqst.sAziPred_2), (tScheduleRqstMsg.tScheduleRqst.sElePred_2),
			// 	 tScheduleRqstMsg.tScheduleRqst.ucHour, tScheduleRqstMsg.tScheduleRqst.ucMin, (tScheduleRqstMsg.tScheduleRqst.ucSec + tScheduleRqstMsg.tScheduleRqst.unMicroSec * 1e-6));
		}

		unSendCnt++;
		usRqstCnt++;
	}
	
	delete [] cSendBuff;
}
