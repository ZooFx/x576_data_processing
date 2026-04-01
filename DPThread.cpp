#include <cmath>

#include "global.h"
#include "SpdLogger.h"
#include "StructDef.h"
#include "DPFunc.h"
#include "dirent.h"
#include <string.h>
#include <sys/time.h>

INT32 CheckMannualTrackQueue();

VOID* DataProcess(VOID*)
{
	/*数据处理*/

	SPReportDataInBuffInfo tSPRprtData, *ptSPRprtData = NULL;
	bzero(&tSPRprtData, sizeof(SPReportDataInBuffInfo));

	struct timeval tBgn, tEnd;
	DOUBLE dTimeCost = 0;

	INT32 nRtn = 0, nAntennaIdx = 0;

	UINT32 aunCircleNum[MAX_ANTENNA_NUM];
	bzero(aunCircleNum, sizeof(aunCircleNum));

	BeamInfo tBeam;
	bzero(&tBeam, sizeof(BeamInfo));
	auto iterBeamCoordX1 = g_aLBeamCoordXInMap[0].begin(), iterBeamCoordX2 =  g_aLBeamCoordXInMap[0].begin();
	auto iterMap = g_aMMsrPacketMap[0].begin();

	TrackSendMsg msg;
	TgtTrackNumInfo tTgtTrackCount;

	BOOL bis_cycle_finished = false;

	while(true)
	{
		//弹出缓存的点迹数据
		ptSPRprtData = g_qptSPRprtData.pop();
		//  	debug
		//		if(ptSPRprtData->unMsrNum >= 0)
		//		{
					// INFO("CPI:{}, Antenna:{}, Type:{}, X:{}, Y:{}, PRT:{:.1f}us, Azi:{:.2f}, Ele:{:.2f}, PntNum:{}, Time:{:.3f}",
					// 		ptSPRprtData->unCPI, ptSPRprtData->ucAntennaIdx, (INT32)(ptSPRprtData->enumBeamType), ptSPRprtData->ucBeamCoordX, ptSPRprtData->ucBeamCoordY, ptSPRprtData->dPRT*1e6, ptSPRprtData->dAziMajor, ptSPRprtData->dEleMajor, ptSPRprtData->unMsrNum, ptSPRprtData->dTime);
		//		}
		gettimeofday(&tBgn, NULL);

		//获取数据包中天线编号
		if ( (nAntennaIdx = GetAntennaIdx(ptSPRprtData)) < 0)
			continue;

		switch(ptSPRprtData->enumScheduleEvent)
		{
		case SEARCH:
		{
			//波束内凝聚
			CentroidMsrPntIn1Beam(ptSPRprtData);

			//数据包插入Map
			InsertMsrPacket2Map(ptSPRprtData, g_aMMsrPacketMap[nAntennaIdx], g_aMLeftEdgeMap[nAntennaIdx], g_aMRightEdgeMap[nAntennaIdx]);

			//记录波位坐标
			RecordAziMajorInMap(ptSPRprtData, g_aLBeamCoordXInMap[nAntennaIdx]);

			if(g_aLBeamCoordXInMap[nAntennaIdx].size() < COORD_X_IN_MAP_TH)
				continue;
				
			//缓存完成，对链表记录的第一个方位指向的所有俯仰波束凝聚
			iterBeamCoordX1 = g_aLBeamCoordXInMap[nAntennaIdx].begin();
			iterBeamCoordX2 = std::next(g_aLBeamCoordXInMap[nAntennaIdx].begin());
			CentroidAllEleInOneAzi(g_aMMsrPacketMap[nAntennaIdx], *iterBeamCoordX1);
			CentroidAllEleInOneAzi(g_aMMsrPacketMap[nAntennaIdx], *iterBeamCoordX2);
			CentroidAdjacentAzi(g_aMMsrPacketMap[nAntennaIdx], *iterBeamCoordX1, *iterBeamCoordX2);

			//边界波位与相邻天线的边界波位凝聚
			CentroidAdjacentAntenna(g_aMMsrPacketMap+nAntennaIdx, *iterBeamCoordX1, g_aMLeftEdgeMap+GetNextAntennaIdx(nAntennaIdx), g_aMRightEdgeMap+GetPrevAntennaIdx(nAntennaIdx));

			if ( ( iterMap = GetIter1stofBeamCoordXInMap(g_aMMsrPacketMap+nAntennaIdx, *iterBeamCoordX1) ) == g_aMMsrPacketMap[nAntennaIdx].end() )
				continue;

			//系统参数更新
			SysParaUpdt(iterMap->second, g_tSysPara);

			//算法参数更新
			AlgorithmParaUpdt(iterMap->second, g_tAlgorithmPara);

			//拷贝当前方位凝聚完成的点迹
			CpyMsrPntOneCoordX(g_aMMsrPacketMap+nAntennaIdx, *iterBeamCoordX1, g_tOneAziMajorMsrPnt);

			TrackTWS();

			bis_cycle_finished = IsOneCircleFinished(*iterBeamCoordX1, *iterBeamCoordX2);

			//释放内存，删除Map、List中当前方位数据
			RemoveOneCoordX(g_aMMsrPacketMap[nAntennaIdx], g_aMLeftEdgeMap[nAntennaIdx], g_aMRightEdgeMap[nAntennaIdx], g_aLBeamCoordXInMap[nAntennaIdx]);

			//debug
			// INFO("Antenna:{}, Circlr:{}, CoordX:{} finished", nAntennaIdx, aunCircleNum[nAntennaIdx], iterBeamCoordX1->ucBeamCoordX);
			// for (auto iter = g_LTgtTrack.begin(); iter != g_LTgtTrack.end(); iter++)
			// {
			// 	// if(iter->tRAEest.dVrEst >= 13 && iter->tRAEest.dVrEst <= 15)
			// 	{
			// 		INFO("ID:{}, R:{:.2f}, A:{:.2f}, E:{:.2f}, Vr:{:.2f}, SNR:{:.2f}, State:{}, MissNum:{}, TerminateWindLen:{}",
			// 			 iter->usTrackID, iter->tRAEest.dRngEst, iter->tRAEest.dAziEst, iter->tRAEest.dEleEst, iter->tRAEest.dVrEst, iter->dSNR, iter->enumTrackUpdMode, iter->usMissNum, iter->usTermntWndwLen);
			// 	}
			// }

			//更新航迹维持和起始窗口
			if(bis_cycle_finished)
			{			
				TgtTrackTermWindowUpdt(nAntennaIdx);
				TmpTrackInitWindowUpdt(nAntennaIdx);

				//debug
				gettimeofday(&tEnd, NULL);
				tTgtTrackCount = TrackCount(&g_LTgtTrack);
				INFO("Antenna:{} Circle {} finished, TgtNum:{}, TRACKnum:{}, CONFIRMnum:{}, SEARCHnum:{}, TimeCost:{:.3f}ms", 
					nAntennaIdx, aunCircleNum[nAntennaIdx], g_LTgtTrack.size(), tTgtTrackCount.unNumTrack,tTgtTrackCount.unNumConfirm,tTgtTrackCount.unNumSearch,GetTimeCost(tBgn, tEnd)*1e3);
				// for(auto iter = g_LTgtTrack.begin(); iter != g_LTgtTrack.end(); iter++)
				// {
				// 	if(iter->usTrackID == 1)
				// 	{
				// 		INFO("ID:{}, R:{:.2f}, A:{:.2f}, E:{:.2f}, Vr:{:.2f}, SNR:{:.2f}, State:{}, MissNum:{}, TerminateWindLen:{}",
				// 				iter->usTrackID, iter->tRAEest.dRngEst, iter->tRAEest.dAziEst, iter->tRAEest.dEleEst, iter->tRAEest.dVrEst, iter->dSNR, iter->enumTrackUpdMode, iter->usMissNum, iter->usTermntWndwLen);
					
				// 		//debug
				// 		if(iter->usMissNum >= 5)
				// 			INFO("debug");
				// 	}
				// }

				//debug
				// for (auto iter = g_LTmpTrack.begin(); iter != g_LTmpTrack.end(); iter++)
				// {
				// 	if (iter->qtMsrArr.at(iter->cIdxLastNonEmpty).dVr < -10 && iter->qtMsrArr.at(iter->cIdxLastNonEmpty).dVr > -15 && iter->qtMsrArr.at(iter->cIdxLastNonEmpty).dRng < 1e3
				// && iter->qtMsrArr.at(iter->cIdxLastNonEmpty).dAzi > 10 && iter->qtMsrArr.at(iter->cIdxLastNonEmpty).dAzi<20)
				// 	{
				// 		INFO("ID:{}, Ant:{}, CPI:{}, time:{:.3f}, Win:{}, len:{}, AF:{}, R:{:.2f}, A:{:.2f}, E:{:.2f}, V:{:.2f}, S:{:.2f}, X:{}, Y:{}",
				// 			 iter->usPseudoTrackID, iter->ucAntennaIdx, iter->unLstUpdFrm, iter->qtMsrArr.at(iter->cIdxLastNonEmpty).dTime, iter->qtMsrArr.size(), iter->usPntNum, iter->bAssocFlag,
				// 			 iter->qtMsrArr.at(iter->cIdxLastNonEmpty).dRng, iter->qtMsrArr.at(iter->cIdxLastNonEmpty).dAzi, iter->qtMsrArr.at(iter->cIdxLastNonEmpty).dEle, iter->qtMsrArr.at(iter->cIdxLastNonEmpty).dVr,
				// 			 iter->qtMsrArr.at(iter->cIdxLastNonEmpty).dConfidence, iter->qtMsrArr.at(iter->cIdxLastNonEmpty).ucBeamCoordX, iter->qtMsrArr.at(iter->cIdxLastNonEmpty).ucBeamCoordY);
				// 	}
				// }

				msg.unTriggerNum = 0;
				msg.pTrackList = std::make_shared<std::list<TgtTrackPntInfo>>(g_LTgtTrack);
				g_qTrackSendMsg.push(msg);

				SaveTgtTrack(&g_LTgtTrack, true, 0);

				RecoverAssocFlag(nAntennaIdx);

				aunCircleNum[nAntennaIdx]++;
			}

			//航迹清除
			TgtTrackClear(g_LTgtTrack, nAntennaIdx);
			TmpTrackClear(g_LTmpTrack, nAntennaIdx);
			break;
		}

		case CONFIRM:
		{
			memcpy(&tSPRprtData, ptSPRprtData, sizeof(SPReportDataInBuffInfo));
			g_qptSPRprtBuffFree.push(ptSPRprtData);

			//波束内凝聚
			CentroidMsrPntIn1Beam(&tSPRprtData);

			//系统参数更新
			SysParaUpdt(&tSPRprtData, g_tSysPara);

			//算法参数更新
			AlgorithmParaUpdt(&tSPRprtData, g_tAlgorithmPara);

			//标记map中与跟踪点迹相邻的点
			CentroidTrackingAndSearch(&tSPRprtData, g_aMMsrPacketMap+nAntennaIdx, g_aLBeamCoordXInMap+nAntennaIdx);

			//点迹拷贝
			CpyMsrPnt(&tSPRprtData, g_tOneCPIMsrPnt);

			//TAS处理
			nRtn = TrackTAS();
			if(nRtn == 0)
			{
				TgtTrackConfirmWindowUpdtOne(g_tSysPara.usTgtID);

				//debug
				// for(auto iter = g_LTgtTrack.begin(); iter != g_LTgtTrack.end(); iter++)
				// {
				// 	if(iter->usTrackID == g_tSysPara.usTgtID)
				// 	{
				// 		INFO("TAS track update, ID:{}, rng:{:.2f}, azi:{:.2f}, ele:{:.2f}, SNR:{:.2f}, AssocFlag:{}, UpdtMode:{}, conf_num:{}, conf_wind:{}",
				// 				iter->usTrackID, iter->tRAEest.dRngEst, iter->tRAEest.dAziEst, iter->tRAEest.dEleEst, iter->dSNR, iter->bAssocFlag, UINT8(iter->enumTrackUpdMode), iter->usConfirmNum, iter->qucConfirmWindow.size());
				// 		break;
				// 	}
				// }

				msg.unTriggerNum = g_tSysPara.usTgtID;
				msg.pTrackList = std::make_shared<std::list<TgtTrackPntInfo>>(g_LTgtTrack);
				g_qTrackSendMsg.push(msg);

				g_qunSendScheduleRqstrigger.push(g_tSysPara.usTgtID);

				SaveTgtTrack(&g_LTgtTrack, true, g_tSysPara.usTgtID);

				//航迹清除
				TgtTrackClearOne(g_LTgtTrack, g_tSysPara.usTgtID);
			}			
			break;	
		}
			

		case TRACK:
		{
			memcpy(&tSPRprtData, ptSPRprtData, sizeof(SPReportDataInBuffInfo));
			g_qptSPRprtBuffFree.push(ptSPRprtData);

			//波束内凝聚
			CentroidMsrPntIn1Beam(&tSPRprtData);

			//系统参数更新
			SysParaUpdt(&tSPRprtData, g_tSysPara);

			//算法参数更新
			AlgorithmParaUpdt(&tSPRprtData, g_tAlgorithmPara);

			//标记map中与跟踪点迹相邻的点
			CentroidTrackingAndSearch(&tSPRprtData, g_aMMsrPacketMap+nAntennaIdx, g_aLBeamCoordXInMap+nAntennaIdx);

			//点迹拷贝
			CpyMsrPnt(&tSPRprtData, g_tOneCPIMsrPnt);

			//TAS处理
			nRtn = TrackTAS();
			if(nRtn == 0)
			{
				TgtTrackTermWindowUpdtOne(g_tSysPara.usTgtID);

				//debug
				// for(auto iter = g_LTgtTrack.begin(); iter != g_LTgtTrack.end(); iter++)
				// {
					// if(iter->usTrackID == g_tSysPara.usTgtID)
					// if(iter->usTrackID == g_tSysPara.usTgtID && iter->usTrackID == 1)
					// {
					// 	INFO("TAS track update, ID:{}, rng:{:.2f}, azi:{:.2f}, ele:{:.2f}, SNR:{:.2f}, AssocFlag:{}, UpdtMode:{}, missnum:{}, termwindlen:{}",
					// 			iter->usTrackID, iter->tRAEest.dRngEst, iter->tRAEest.dAziEst, iter->tRAEest.dEleEst, iter->dSNR, iter->bAssocFlag, UINT8(iter->enumTrackUpdMode), iter->usMissNum, iter->qucTerminentWindow.size());
					// 	break;
					// }
				// }

				msg.unTriggerNum = g_tSysPara.usTgtID;
				msg.pTrackList = std::make_shared<std::list<TgtTrackPntInfo>>(g_LTgtTrack);
				g_qTrackSendMsg.push(msg);

				g_qunSendScheduleRqstrigger.push(g_tSysPara.usTgtID);

				SaveTgtTrack(&g_LTgtTrack, true, g_tSysPara.usTgtID);

				//航迹清除
				TgtTrackClearOne(g_LTgtTrack, g_tSysPara.usTgtID);
			}
			break;
		}

		default:
			ERROR("invalid schedule event({}), it can only be 0 for search or 1 for track", ptSPRprtData->enumScheduleEvent);
			break;
		}
	}
}

INT32 CheckMannualTrackQueue()
{
	//检查手动跟踪批号队列

	UINT16 usMannualTrackID = 0;

	pthread_mutex_lock(&mutexIDForTrackMannual);

	while(!g_qucIDForTrackMannual.empty())
	{
		usMannualTrackID = g_qucIDForTrackMannual.front();
		g_qunSendScheduleRqstrigger.push(usMannualTrackID);
		g_qucIDForTrackMannual.pop();
	}

	pthread_mutex_unlock(&mutexIDForTrackMannual);

	return 0;
}
