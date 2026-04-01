#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "global.h"
#include "CommunicateProtocol.h"
#include "SpdLogger.h"
#include "dirent.h"
#include "DPFunc.h"

UINT16 GetTrackNum(std::list<TgtTrackPntInfo>* pLTgtTrack);
UINT16 GetTrackNumAssocFailed(std::list<TgtTrackPntInfo>* pLTgtTrack); 


VOID* DPSendTrack(VOID*)
{
	//新建套接字
	INT32 fd = socket(AF_INET, SOCK_DGRAM, 0);
	if( fd < 0)
		ERROR("Socket fd build failed");

	//配置数据处理套接字地址结构
	struct sockaddr_in tDPAddr;
	bzero(&tDPAddr, sizeof(tDPAddr));
	BuildSockAddr(ANY_IP, DP_SEND_PORT_2DISP, tDPAddr);

	//套接字与地址结构绑定
	if( bind(fd, (struct sockaddr*)&tDPAddr, sizeof(tDPAddr)) < 0)
	{
		ERROR("bind failed");
		exit(1);
	}

	//配置显控套接字地址结构
	struct sockaddr_in tDispAddr;
	bzero(&tDispAddr, sizeof(tDispAddr));
	UINT32 unSendAddrLen = sizeof(tDispAddr);
	BuildSockAddr(DISP_IP, DISP_RCV_PORT_FOR_DP, tDispAddr);	 

	//航迹存储缓冲区
	OneTrackPointENUInfo* ptTrackBuff = new OneTrackPointENUInfo [MAX_TGT_NUM];
	bzero(ptTrackBuff, sizeof(OneTrackPointENUInfo)*MAX_TGT_NUM);

	//发送缓冲区
	INT8* cSendBuff = new INT8 [BUFFER_SIZE], *ptr = NULL;

	//一帧目标航迹协议
	OneFrmTrackPointENUMsg tOneFrmTrackPointENUMsg;
	bzero(&tOneFrmTrackPointENUMsg, sizeof(OneFrmTrackPointENUMsg));
	BuildMsgPrefix(HEAD_FLAG, 0, 0, DP_ID, DISP_ID, TRACK_FOR_DISP_MSG_ID, tOneFrmTrackPointENUMsg.tMsgPrefix);
	tOneFrmTrackPointENUMsg.tMsgSuffix.unTailFlag = TAIL_FLAG;

	UINT32 unSendCnt = 0;
	UINT16 usTgtNum = 0, usMsgLen = 0, usTgtTASNum = 0;
	INT32 nSendLen = 0, nTgtIdx = 0;
	std::list<TgtTrackPntInfo>::iterator iter;

	TrackSendMsg msg;

	while(true)
	{
		msg = g_qTrackSendMsg.pop();

		if(msg.unTriggerNum == 0) // TWS 整周扫描结束，发送批量航迹（通常为未关联成功的推迹）
		{
			usTgtNum = 0;
			if(g_tSysPara.enumWorkMode == TAS)
				usTgtTASNum = GetTrackNumTAS(msg.pTrackList.get());

			for(iter = msg.pTrackList->begin(); iter != msg.pTrackList->end(); iter++)
			{
				if(iter->bAssocFlag) continue; // 原逻辑：跳过本周已关联成功的航迹
			
				// --- 核心发送逻辑判定 ---
				bool bShouldSend = false;
				if (g_tSysPara.enumWorkMode == TWS) {
					bShouldSend = true; // TWS模式：发送所有航迹
				} else if (g_tSysPara.enumWorkMode == TAS) {
					if (iter->enumPredScheduleEvent == TRACK || (usTgtTASNum >= MAX_TAS_NUM && iter->enumPredScheduleEvent == SEARCH)) {
						bShouldSend = true;
					}
				}

				if (!bShouldSend) continue;

				// 赋值逻辑
				ptTrackBuff[usTgtNum].usTrackID = iter->usTrackID;
				ptTrackBuff[usTgtNum].usCPI = iter->unLstUpdFrm;
				ptTrackBuff[usTgtNum].unUTCSec = static_cast<UINT32>(iter->dLstUpdTime);
				ptTrackBuff[usTgtNum].unMicroSec = iter->tLstUpdtDate.unMicroSec;
				ptTrackBuff[usTgtNum].enumTrackUpdtMode = iter->enumTrackUpdMode;
				ptTrackBuff[usTgtNum].fAmp = iter->dAmp;
				ptTrackBuff[usTgtNum].fSNR = iter->dSNR;
				ptTrackBuff[usTgtNum].fRng = iter->tRAEest.dRngEst;
				if(iter->tRAEest.dAziEst<0)
					ptTrackBuff[usTgtNum].fAzi = iter->tRAEest.dAziEst+360;
				else
					ptTrackBuff[usTgtNum].fAzi = iter->tRAEest.dAziEst;
				ptTrackBuff[usTgtNum].fEle = iter->tRAEest.dEleEst;
				ptTrackBuff[usTgtNum].fAlt = iter->tRAEest.dAltEst;
				ptTrackBuff[usTgtNum].fVr  = iter->dVel;
				ptTrackBuff[usTgtNum].fVel = iter->dVel;
				ptTrackBuff[usTgtNum].fAcc = iter->dAcc;
				ptTrackBuff[usTgtNum].untar_type = iter->untar_type;
				usTgtNum++;
			}

			if(usTgtNum == 0) continue; // 如果没有需要发送的航迹，直接跳过

			tOneFrmTrackPointENUMsg.tMsgPrefix.unComunicatCnt = unSendCnt;
			tOneFrmTrackPointENUMsg.tMsgPrefix.usMsgLen = MSG_PREFIX_LEN + sizeof(UINT16) + sizeof(OneTrackPointENUInfo)*usTgtNum;
			tOneFrmTrackPointENUMsg.usTgtNum = usTgtNum;
			tOneFrmTrackPointENUMsg.ptOneFrmTrackPntENU = ptTrackBuff;

			ptr = cSendBuff;
			memcpy(ptr, &(tOneFrmTrackPointENUMsg.tMsgPrefix), sizeof(MsgPrefixInfo));
			ptr += sizeof(MsgPrefixInfo);
			memcpy(ptr, &(tOneFrmTrackPointENUMsg.usTgtNum), sizeof(UINT16));
			ptr += sizeof(UINT16);
			memcpy(ptr, tOneFrmTrackPointENUMsg.ptOneFrmTrackPntENU, sizeof(OneTrackPointENUInfo)*usTgtNum);
			tOneFrmTrackPointENUMsg.tMsgSuffix.ucCheckCode = GetXORCode(cSendBuff, tOneFrmTrackPointENUMsg.tMsgPrefix.usMsgLen);
			ptr += sizeof(OneTrackPointENUInfo)*usTgtNum;
			memcpy(ptr, &(tOneFrmTrackPointENUMsg.tMsgSuffix), sizeof(MsgSuffixInfo));

			usMsgLen = tOneFrmTrackPointENUMsg.tMsgPrefix.usMsgLen + sizeof(MsgSuffixInfo);
		}
		else if(msg.unTriggerNum > 0) // 实时点迹更新，发送单目标
		{
			nTgtIdx = FindTgtTrack(msg.pTrackList.get(), msg.unTriggerNum);
			if(nTgtIdx >= 0)
			{
				iter = msg.pTrackList->begin();
				std::advance(iter, nTgtIdx);
			}
			else
			{
				ERROR("no such target track with ID:{}", msg.unTriggerNum);
				continue;
			}

			if (g_tSysPara.enumWorkMode == TAS)
				usTgtTASNum = GetTrackNumTAS(msg.pTrackList.get());

			// --- 核心发送逻辑判定 ---
			bool bShouldSend = false;
			if (g_tSysPara.enumWorkMode == TWS) {
				bShouldSend = true; // TWS模式：全发
			} else if (g_tSysPara.enumWorkMode == TAS) {
				if (iter->enumPredScheduleEvent == TRACK || (usTgtTASNum >= MAX_TAS_NUM && iter->enumPredScheduleEvent == SEARCH)) {
					bShouldSend = true;
				}
			}

			if(bShouldSend)
			{
				usTgtNum = 1;

				tOneFrmTrackPointENUMsg.tMsgPrefix.unComunicatCnt = unSendCnt;
				tOneFrmTrackPointENUMsg.tMsgPrefix.usMsgLen = MSG_PREFIX_LEN + sizeof(UINT16) + sizeof(OneTrackPointENUInfo)*usTgtNum;

				ptTrackBuff[0].usTrackID = iter->usTrackID;
				ptTrackBuff[0].usCPI = iter->unLstUpdFrm;
				ptTrackBuff[0].unUTCSec = static_cast<UINT32>(iter->dLstUpdTime);
				ptTrackBuff[0].unMicroSec = iter->tLstUpdtDate.unMicroSec;
				ptTrackBuff[0].enumTrackUpdtMode = iter->enumTrackUpdMode;
				ptTrackBuff[0].fAmp = iter->dAmp;
				ptTrackBuff[0].fSNR = iter->dSNR;
				ptTrackBuff[0].fRng = iter->tRAEest.dRngEst;
				if(iter->tRAEest.dAziEst<0)
					ptTrackBuff[0].fAzi = iter->tRAEest.dAziEst+360;
				else
					ptTrackBuff[0].fAzi = iter->tRAEest.dAziEst;
				ptTrackBuff[0].fEle = iter->tRAEest.dEleEst;
				ptTrackBuff[0].fAlt = iter->tRAEest.dAltEst;
				ptTrackBuff[0].fVr = iter->dVel;
				ptTrackBuff[0].fVel = iter->dVel;
				ptTrackBuff[0].fAcc = iter->dAcc;
				ptTrackBuff[0].untar_type = iter->untar_type;

				tOneFrmTrackPointENUMsg.usTgtNum = usTgtNum;
				tOneFrmTrackPointENUMsg.ptOneFrmTrackPntENU = ptTrackBuff;

				ptr = cSendBuff;
				memcpy(ptr, &(tOneFrmTrackPointENUMsg.tMsgPrefix), sizeof(MsgPrefixInfo));
				ptr += sizeof(MsgPrefixInfo);
				memcpy(ptr, &(tOneFrmTrackPointENUMsg.usTgtNum), sizeof(UINT16));
				ptr += sizeof(UINT16);
				memcpy(ptr, tOneFrmTrackPointENUMsg.ptOneFrmTrackPntENU, sizeof(OneTrackPointENUInfo)*usTgtNum);
				tOneFrmTrackPointENUMsg.tMsgSuffix.ucCheckCode = GetXORCode(cSendBuff, tOneFrmTrackPointENUMsg.tMsgPrefix.usMsgLen);
				ptr += sizeof(OneTrackPointENUInfo)*usTgtNum;
				memcpy(ptr, &(tOneFrmTrackPointENUMsg.tMsgSuffix), sizeof(MsgSuffixInfo));

				usMsgLen = tOneFrmTrackPointENUMsg.tMsgPrefix.usMsgLen + sizeof(MsgSuffixInfo);				
			}
			else
			{
				continue; // 过滤掉不满足条件的单目标发送触发
			}
		}
		else
		{
			ERROR("invalid trigger num({})", msg.unTriggerNum);
			continue;
		}

		nSendLen = sendto(fd, cSendBuff, usMsgLen, 0, (struct sockaddr*)&tDispAddr, unSendAddrLen);

		if(nSendLen <= 0)
		{
			ERROR("track-ENU send failed. msg len:{}, send len:{}", usMsgLen, nSendLen);
		}

		unSendCnt++;
	}
	
	delete [] ptTrackBuff;
	delete [] cSendBuff;
}

UINT16 GetTrackNum(std::list<TgtTrackPntInfo>* pLTgtTrack)
{
	/*获取目标数量*/

	UINT16 usTgtNum = pLTgtTrack->size();

	if(usTgtNum > MAX_TGT_NUM)
	{
		usTgtNum = MAX_TGT_NUM;
		WARN("the number of targets is greater than the maximum capacity, tracks of {} targets will be discarded", usTgtNum-MAX_TGT_NUM);
	}

	return usTgtNum;
}

UINT16 GetTrackNumAssocFailed(std::list<TgtTrackPntInfo>* pLTgtTrack)
{
	/*获取未关联点迹的目标数量*/

	UINT16 usTgtNum = 0;

	for(std::list<TgtTrackPntInfo>::iterator iter = pLTgtTrack->begin(); iter != pLTgtTrack->end();iter++)
	{
		if(!iter->bAssocFlag) usTgtNum++;
	}

	if(usTgtNum > MAX_TGT_NUM)
	{
		usTgtNum = MAX_TGT_NUM;
		WARN("the number of targets is greater than the maximum capacity, tracks of {} targets will be discarded", usTgtNum-MAX_TGT_NUM);
	}

	return usTgtNum;
}


