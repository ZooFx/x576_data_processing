#include "StructDef.h"
#include <map>
#include <list>

extern DOUBLE GetTimeCost(const struct timeval& tBgn, const struct timeval tEnd);
extern INT32 AlgorithmParaUpdt(SPReportDataInBuffInfo* ptSPRprtData, AlgorithmParaInfo& tAlgorithmPara);
extern INT32 SysParaUpdt(SPReportDataInBuffInfo* ptSPRprtData, SysParaInfo& tSysPara);
extern INT32 CpyMsrPnt(SPReportDataInBuffInfo* ptSPRprtData, OneCPIMsrInfo& tOneCPIMsrPnt);
extern INT32 CentroidMsrPnt(OneCPIMsrInfo& tOneFrmMsr, DOUBLE dSNRRef, DOUBLE dB, DOUBLE dAzi3dB, DOUBLE dEle3dB, DOUBLE dFc, DOUBLE dPRT, UINT32 unPulseNum, enum MsrStdEstFashionInfo enumMsrStdEstFashion, AlgorithmParaInfo tAlgrthmPara);
extern INT32 TrackTWS();
extern INT32 TrackTAS(std::list<TgtTrackPntInfo>& LTgtTrack, unsigned short ustrack_id);
extern INT32 TgtTrackClear(std::list<TgtTrackPntInfo>& LTgtTrack, UINT8);
extern INT32 TgtTrackClearOne(std::list<TgtTrackPntInfo>& LTgtTrack, UINT32 unTriggerNum);
extern INT32 TmpTrackClear(std::list<TmpTrackInfo>& vTmpTrack, UINT8);
extern INT32 FindTgtTrack(std::list<TgtTrackPntInfo>* pLTgtTrack, UINT16 usTrackID);
extern INT32 TgtTrackTermWindowUpdt(UINT8);
extern INT32 TmpTrackInitWindowUpdt(UINT8);
extern INT32 TgtTrackConfirmWindowUpdtOne(std::list<TgtTrackPntInfo>& LTgtTrack, UINT32 unTriggerNum);
extern INT32 TgtTrackTermWindowUpdtOne(std::list<TgtTrackPntInfo>& LTgtTrack, UINT32 unTriggerNum);
extern INT32 RecoverAssocFlag(UINT8);
extern INT32 SaveTgtTrack(std::list<TgtTrackPntInfo>* pLTgtTrack, bool bSaveFlag, unsigned short ustrack_id);
extern INT32 SaveTmpTrack(std::list<TmpTrackInfo>* pLTmpTrack, bool bSaveFlag, unsigned short ustrack_id);
extern INT32 InsertMsrPacket2Map(SPReportDataInBuffInfo* ptSPReportData, std::map<BeamInfo, SPReportDataInBuffInfo*, CompareBeam>& mDataMap, std::map<BeamInfo, SPReportDataInBuffInfo*, CompareBeam>& mLeftEdgeMap, std::map<BeamInfo, SPReportDataInBuffInfo*, CompareBeam>& mRightEdgeMap);
extern INT32 CentroidMsrPntIn1Beam(SPReportDataInBuffInfo* ptSPReporBeam1);
extern INT32 CentroidAllEleInOneAzi(std::map<BeamInfo, SPReportDataInBuffInfo*, CompareBeam>& mDataMap, BeamInfo tBeam);
extern INT32 CentroidAdjacentAzi(std::map<BeamInfo, SPReportDataInBuffInfo*, CompareBeam>& mDataMap, BeamInfo tBeam1, BeamInfo tBeam2);
extern INT32 CpyMsrPntOneCoordX(std::map<BeamInfo, SPReportDataInBuffInfo*, CompareBeam>* pmDataMap, BeamInfo tBeam, OneAziMajorMsrInfo& tOneAziMajorMsrPnt);
extern std::map<BeamInfo, SPReportDataInBuffInfo*, CompareBeam>::iterator GetIter1stofBeamCoordXInMap(std::map<BeamInfo, SPReportDataInBuffInfo*, CompareBeam>* pmDataMap, BeamInfo tBeam);
extern INT32 RemoveOneCoordX(std::map<BeamInfo, SPReportDataInBuffInfo*, CompareBeam>& mDataMap, std::map<BeamInfo, SPReportDataInBuffInfo*, CompareBeam>& mLeftEdgeMap, std::map<BeamInfo, SPReportDataInBuffInfo*, CompareBeam>& mRightEdgeMap, std::list<BeamInfo>& tLBeamCoordXInMap);
extern BOOL IsOneCircleFinished(BeamInfo tBeamPrev, BeamInfo tBeamNext);
extern INT8 GetAntennaIdx(SPReportDataInBuffInfo* ptSPRprtData);
extern INT32 RecordAziMajorInMap(SPReportDataInBuffInfo* ptSPReportData, std::list<BeamInfo>& tLBeamCoordXInMap);
extern INT32 CentroidTrackingAndSearch(SPReportDataInBuffInfo* ptSPRprtData, std::map<BeamInfo, SPReportDataInBuffInfo*, CompareBeam>* pmDataMap,  std::list<BeamInfo>* ptLBeamCoordXInMap);
extern INT32 CentroidAdjacentAntenna(std::map<BeamInfo, SPReportDataInBuffInfo*, CompareBeam>* pmDataMap, BeamInfo tBeamCoordX1, std::map<BeamInfo, SPReportDataInBuffInfo*, CompareBeam>* pmLeftEdgeMap, std::map<BeamInfo, SPReportDataInBuffInfo*, CompareBeam>* pmRightEdgeMap);
extern UINT8 GetNextAntennaIdx(UINT8 ucAntennaIdx);
extern UINT8 GetPrevAntennaIdx(UINT8 ucAntennaIdx);
extern UINT32 GetTrackNumTAS(std::list<TgtTrackPntInfo>* pLTgtTrack);
extern TgtTrackNumInfo TrackCount(std::list<TgtTrackPntInfo>* pLTgtTrack);
