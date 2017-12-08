//------------------------------------------------------------------------------
// 创建日期：2017-06-12 15:07:17
// 文件描述：该文件用来解包/打包数据包
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "NetPackage.h"
#include <WinNls.h>

// -- CNetPackage --

//------------------------------------------------------------------------------
CNetPackage::CNetPackage()
{
}

//------------------------------------------------------------------------------
CNetPackage::~CNetPackage()
{
}

//------------------------------------------------------------------------------
// 解析出请求命令类型
//------------------------------------------------------------------------------
ECommand CNetPackage::GetCommand(const std::string &strRecv)
{
  if (strRecv.empty()) return ECommand::CMD_LAST;

  Json::Reader  jReader;
  Json::Value   jRoot;

  // 解析
  if (!jReader.parse(strRecv, jRoot)) return ECommand::CMD_LAST;
  // 获取请求命令类型
  if (!jRoot[JSON_KEY_COMMAND].isUInt()) return ECommand::CMD_LAST;
  return static_cast<ECommand>(jRoot[JSON_KEY_COMMAND].asUInt());
}

//------------------------------------------------------------------------------
// 构建默认的发送包
//------------------------------------------------------------------------------
bool CNetPackage::BuildDefault(const std::string &strSID, ECommand eCmd,
  EErrCode eEC, std::string &strSend)
{
  // 不检查数据有效性

  Json::FastWriter  jWriter;
  Json::Value       jRoot;

  // 添加最基本的信息
  jRoot[JSON_KEY_COMMAND] = static_cast<uint32_t>(eCmd);
  jRoot[JSON_KEY_SESSION_ID] = strSID;
  jRoot[JSON_KEY_STATUS_CODE] = static_cast<uint32_t>(eEC);

  // 转换为字符串
  strSend = jWriter.write(jRoot);
  return true;
}

//------------------------------------------------------------------------------
bool CNetPackage::ParseLogin(const std::string &strRecv,
  std::string &strUserName, std::string &strPassword)
{
  return CLoginPackage::Inst()->ParseLogin(strRecv, strUserName, strPassword);
}

//------------------------------------------------------------------------------
bool CNetPackage::ParseLogout(const std::string &strRecv, std::string &strSID)
{
  return CLoginPackage::Inst()->ParseLogOut(strRecv, strSID);
}

//------------------------------------------------------------------------------
bool CNetPackage::ParseReadTag(const std::string &strRecv,
  std::string &strSID, std::list<shared_ptr<KTagInfo>> &lstInfo)
{
  return CTagPackage::Inst()->ParseRead(strRecv, strSID, lstInfo);
}

//------------------------------------------------------------------------------
bool CNetPackage::ParseWriteTag(const std::string &strRecv,
  std::string &strSID, std::list<shared_ptr<KTagInfo>> &lstInfo)
{
  return CTagPackage::Inst()->ParseWrite(strRecv, strSID, lstInfo);
}

//------------------------------------------------------------------------------
bool CNetPackage::ParseWriteEvent(const std::string &strRecv,
  std::string &strSID, std::list<shared_ptr<KEventInfo>> &lstInfo)
{
  return CEventPackage::Inst()->ParseWrite(strRecv, strSID, lstInfo);
}

//------------------------------------------------------------------------------
bool CNetPackage::ParseReadHistoryEvent(const std::string &strRecv,
  std::string &strSID, uint64_t &ullStartTime, uint64_t &ullEndTime)
{
  return CEventPackage::Inst()->ParseReadHistory(strRecv, strSID, ullStartTime, ullEndTime);
}

//------------------------------------------------------------------------------
bool CNetPackage::ParseReadHistoryAlarm(const std::string &strRecv,
  std::string &strSID, uint64_t &ullStartTime, uint64_t &ullEndTime)
{
  return CAlarmPackage::Inst()->ParseReadHistory(strRecv, strSID, ullStartTime, ullEndTime);
}

//------------------------------------------------------------------------------
bool CNetPackage::ParseAdd_AlarmAssistant(const std::string &strRecv,
  std::string &strSID, std::list<shared_ptr<KTBAlarmAssistant>> &lstInfo)
{
  return CAlarmAssistantPackage::Inst()->ParseAdd(strRecv, strSID, lstInfo);
}

//------------------------------------------------------------------------------
bool CNetPackage::ParseDelete_AlarmAssistant(const std::string &strRecv,
  std::string &strSID, std::list<uint32_t> &lstID)
{
  return CAlarmAssistantPackage::Inst()->ParseDelete(strRecv, strSID, lstID);
}

//------------------------------------------------------------------------------
bool CNetPackage::ParseModify_AlarmAssistant(const std::string &strRecv,
  std::string &strSID, std::list<std::shared_ptr<KTBAlarmAssistant>> &lstInfo)
{
  return CAlarmAssistantPackage::Inst()->ParseModify(strRecv, strSID, lstInfo);
}

//------------------------------------------------------------------------------
bool CNetPackage::ParseSearch_AlarmAssistant(const std::string &strRecv,
  std::string &strSID, uint32_t &ulGlobalID)
{
  return CAlarmAssistantPackage::Inst()->ParseSearch(strRecv, strSID, ulGlobalID);
}

//------------------------------------------------------------------------------
bool CNetPackage::ParseSearch_Num_AlarmAssistant(const std::string &strRecv,
  std::string &strSID, std::shared_ptr<KTBAlarmAssistantSearchCondition_Num> ptCondition)
{
  return CAlarmAssistantPackage::Inst()->ParseSearch_Num(strRecv, strSID, ptCondition);
}

//------------------------------------------------------------------------------
bool CNetPackage::ParseSearchPaging_AlarmAssistant(const std::string &strRecv,
  std::string &strSID, std::shared_ptr<KTBAlarmAssistantSearchCondition> ptCondition)
{
  return CAlarmAssistantPackage::Inst()->ParseSearchPaging(strRecv, strSID, ptCondition);
}

//------------------------------------------------------------------------------
bool CNetPackage::ParseAdd_ChangeShifts(const std::string &strRecv,
  std::string &strSID, std::list<shared_ptr<KTBChgShifts>> &lstInfo)
{
  return CChgShiftsPackage::Inst()->ParseAdd(strRecv, strSID, lstInfo);
}

//------------------------------------------------------------------------------
bool CNetPackage::ParseCheckDBStatus(const std::string &strRecv, std::string &strSID)
{
  return CCheckStatusPackage::Inst()->ParseDB(strRecv, strSID);
}

//------------------------------------------------------------------------------
bool CNetPackage::ParseCheckOPCStatus(const std::string &strRecv, std::string &strSID)
{
  return CCheckStatusPackage::Inst()->ParseOPC(strRecv, strSID);
}

//------------------------------------------------------------------------------
bool CNetPackage::ParseSearchID(const std::string &strRecv, std::string &strSID, uint32_t &lstInfo)
{
    return CBMSInfoPackage::Inst()->ParseSearchID(strRecv, strSID, lstInfo);
}

bool CNetPackage::ParseAddOrModSupplier(const std::string &strRecv, std::string &strSID, shared_ptr<SupplierInfo> &lstInfo)
{
    return CBMSInfoPackage::Inst()->ParseAddOrModSupplier(strRecv, strSID, lstInfo);
}

bool CNetPackage::ParseDelOrSelSupplier(const std::string &strRecv, std::string &strSID, string &lstInfo)
{
    return CBMSInfoPackage::Inst()->ParseDelOrSelSupplier(strRecv, strSID, lstInfo);
}

bool CNetPackage::ParseAddOrModMeterType(const std::string &strRecv, std::string &strSID, shared_ptr<MeterType>&lstInfo)
{
    return CBMSInfoPackage::Inst()->ParseAddOrModMeterType(strRecv, strSID, lstInfo);
}

bool CNetPackage::ParseDelOrSelMeterType(const std::string &strRecv, std::string &strSID, string &lstInfo)
{
    return CBMSInfoPackage::Inst()->ParseDelOrSelMeterType(strRecv, strSID, lstInfo);
}

bool CNetPackage::ParseAddOrModEnergyCate(const std::string &strRecv, std::string &strSID, shared_ptr<EnergyCate>&lstInfo)
{
    return CBMSInfoPackage::Inst()->ParseAddOrModEnergyCate(strRecv, strSID, lstInfo);
}

bool CNetPackage::ParseDelOrSelEnergyCate(const std::string &strRecv, std::string &strSID, string &lstInfo)
{
    return CBMSInfoPackage::Inst()->ParseDelOrSelEnergyCate(strRecv, strSID, lstInfo);
}

bool CNetPackage::ParseAddOrModDevFaultCate(const std::string &strRecv, std::string &strSID, shared_ptr<DevFaultCate>&lstInfo)
{
    return CBMSInfoPackage::Inst()->ParseAddOrModDevFaultCate(strRecv, strSID, lstInfo);
}

bool CNetPackage::ParseDelOrSelDevFaultCate(const std::string &strRecv, std::string &strSID, string &lstInfo)
{
    return CBMSInfoPackage::Inst()->ParseDelOrSelDevFaultCate(strRecv, strSID, lstInfo);
}

bool CNetPackage::ParseAddOrModDeviceInfo(const std::string &strRecv, std::string &strSID, shared_ptr<DeviceInfo>&lstInfo)
{
    return CBMSInfoPackage::Inst()->ParseAddOrModDeviceInfo(strRecv, strSID, lstInfo);
}

bool CNetPackage::ParseDelOrSelDeviceInfo(const std::string &strRecv, std::string &strSID, string &lstInfo)
{
    return CBMSInfoPackage::Inst()->ParseDelOrSelDeviceInfo(strRecv, strSID, lstInfo);
}

bool CNetPackage::ParseAddOrModMeterInfo(const std::string &strRecv, std::string &strSID, shared_ptr<MeterInfo>&lstInfo)
{
    return CBMSInfoPackage::Inst()->ParseAddOrModMeterInfo(strRecv, strSID, lstInfo);
}

bool CNetPackage::ParseDelOrSelMeterInfo(const std::string &strRecv, std::string &strSID, string &lstInfo)
{
    return CBMSInfoPackage::Inst()->ParseDelOrSelMeterInfo(strRecv, strSID, lstInfo);
}

bool CNetPackage::ParseAddDeviceParameter(const std::string &strRecv, std::string &strSID, shared_ptr<DeviceParameter>&lstInfo)
{
    return CBMSInfoPackage::Inst()->ParseAddDeviceParameter(strRecv, strSID, lstInfo);
}

bool CNetPackage::ParseDelDeviceParameter(const std::string &strRecv, std::string &strSID, shared_ptr<DeviceParameter>&lstInfo)
{
    return CBMSInfoPackage::Inst()->ParseDelDeviceParameter(strRecv, strSID, lstInfo);
}

bool CNetPackage::ParseSearchDeviceParameter(const std::string &strRecv, std::string &strSID, string &lstInfo)
{
    return CBMSInfoPackage::Inst()->ParseSearchDeviceParameter(strRecv, strSID, lstInfo);
}

bool CNetPackage::ParseAddMeterParameter(const std::string &strRecv, std::string &strSID, shared_ptr<MeterParameter>&lstInfo)
{
    return CBMSInfoPackage::Inst()->ParseAddMeterParameter(strRecv, strSID, lstInfo);
}

bool CNetPackage::ParseDelMeterParameter(const std::string &strRecv, std::string &strSID, shared_ptr<MeterParameter>&lstInfo)
{
    return CBMSInfoPackage::Inst()->ParseDelMeterParameter(strRecv, strSID, lstInfo);
}

bool CNetPackage::ParseSearchMeterParameter(const std::string &strRecv, std::string &strSID, string &lstInfo)
{
    return CBMSInfoPackage::Inst()->ParseSearchMeterParameter(strRecv, strSID, lstInfo);
}

bool CNetPackage::ParseAddOrModDeviceRepair(const std::string &strRecv, std::string &strSID, shared_ptr<RepairInfo>&lstInfo)
{
    return CBMSInfoPackage::Inst()->ParseAddOrModDeviceRepair(strRecv, strSID, lstInfo);
}

bool CNetPackage::ParseDelOrSelDeviceRepair(const std::string &strRecv, std::string &strSID, int &lstInfo)
{
    return CBMSInfoPackage::Inst()->ParseDelOrSelDeviceRepair(strRecv, strSID, lstInfo);
}

bool CNetPackage::ParseSearchDeviceRepair(const std::string &strRecv, std::string &strSID, string &lstInfo)
{
    return CBMSInfoPackage::Inst()->ParseSearchDeviceRepair(strRecv, strSID, lstInfo);
}

bool CNetPackage::ParseAddOrModDeviceRun(const std::string &strRecv, std::string &strSID, shared_ptr<DeviceRunInfo>&lstInfo)
{
    return CBMSInfoPackage::Inst()->ParseAddOrModDeviceRun(strRecv, strSID, lstInfo);
}

bool CNetPackage::ParseDelOrSelDeviceRun(const std::string &strRecv, std::string &strSID, int &lstInfo)
{
    return CBMSInfoPackage::Inst()->ParseDelOrSelDeviceRun(strRecv, strSID, lstInfo);
}

bool CNetPackage::ParseSearchDeviceRun(const std::string &strRecv, std::string &strSID, string &lstInfo)
{
    return CBMSInfoPackage::Inst()->ParseSearchDeviceRun(strRecv, strSID, lstInfo);
}

bool CNetPackage::ParseSearchSubSysVariableInfo(const std::string &strRecv, std::string &strSID)
{
    return CBMSInfoPackage::Inst()->ParseSearchSubSysVariableInfo(strRecv, strSID);
}

bool CNetPackage::ParseSearchDeviceVariableInfo(const std::string &strRecv, std::string &strSID, std::string &lstInfo)
{
    return CBMSInfoPackage::Inst()->ParseSearchDeviceVariableInfo(strRecv, strSID, lstInfo);
}

bool CNetPackage::ParseSearchTagVariableInfo(const std::string &strRecv, std::string &strSID, string &lstInfo)
{
    return CBMSInfoPackage::Inst()->ParseSearchTagVariableInfo(strRecv, strSID, lstInfo);
}

bool CNetPackage::ParseSearchOverViewInfo(const std::string &strRecv, std::string &strSID, string &lstInfo)
{
	return CBMSInfoPackage::Inst()->ParseSearchOverViewInfo(strRecv, strSID, lstInfo);
}

bool CNetPackage::ParseSearchMeterValueInfo(const std::string &strRecv, std::string &strSID, shared_ptr<ValueRequestInfo> &lstInfo)
{
    return CBMSInfoPackage::Inst()->ParseSearchMeterValueInfo(strRecv, strSID, lstInfo);
}

bool CNetPackage::ParseSearchDeviceValueInfo(const std::string &strRecv, std::string &strSID, shared_ptr<ValueRequestInfo> &lstInfo)
{
    return CBMSInfoPackage::Inst()->ParseSearchDeviceValueInfo(strRecv, strSID, lstInfo);
}

bool CNetPackage::ParseSearchVariableValueInfo(const std::string &strRecv, std::string &strSID, shared_ptr<ValueRequestInfo> &lstInfo)
{
    return CBMSInfoPackage::Inst()->ParseSearchVariableValueInfo(strRecv, strSID, lstInfo);
}

bool CNetPackage::ParseSearchHistoryEventExInfo(const std::string &strRecv, std::string &strSID, shared_ptr<HistoryEventRequestInfo> &lstInfo)
{
    return CBMSInfoPackage::Inst()->ParseSearchHistoryEventExInfo(strRecv, strSID, lstInfo);
}

bool CNetPackage::ParseSearchHistoryAlarmExInfo(const std::string &strRecv, std::string &strSID, shared_ptr<HistoryAlarmRequestInfo> &lstInfo)
{
    return CBMSInfoPackage::Inst()->ParseSearchHistoryAlarmExInfo(strRecv, strSID, lstInfo);
}

//------------------------------------------------------------------------------
bool CNetPackage::ParseCameraScreenshot(const std::string &strRecv,
  std::string &strSID, std::shared_ptr<KCamScreenshotInfo> ptInfo)
{
  return CCameraControlPackage::Inst()->ParseScreenshot(strRecv, strSID, ptInfo);
}

//------------------------------------------------------------------------------
bool CNetPackage::ParseSearchCameraConfig(const std::string &strRecv, std::string &strSID)
{
  return CCameraControlPackage::Inst()->ParseSearchConfig(strRecv, strSID);
}

//------------------------------------------------------------------------------
bool CNetPackage::ParseReadLinkageWatchInfo(const std::string &strRecv, std::string &strSID)
{
  return CLinkagePackage::Inst()->ParseReadWatchInfo(strRecv, strSID);
}

//------------------------------------------------------------------------------
bool CNetPackage::ParseLinkageManualConfirmation(const std::string &strRecv,
  std::string &strSID, uint32_t &ulItemID)
{
  return CLinkagePackage::Inst()->ParseManualConfirmation(strRecv, strSID, ulItemID);
}

//------------------------------------------------------------------------------
bool CNetPackage::ParseLinkageTerminateTask(const std::string &strRecv,
  std::string &strSID, uint32_t &ulItemID)
{
  return CLinkagePackage::Inst()->ParseTerminateTask(strRecv, strSID, ulItemID);
}

//------------------------------------------------------------------------------
bool CNetPackage::ParseReadTimetableDispatchWatchInfo(const std::string &strRecv,
  std::string &strSID)
{
  return CTimetableDispatchPackage::Inst()->ParseReadWatchInfo(strRecv, strSID);
}

//------------------------------------------------------------------------------
bool CNetPackage::ParseTimetableDispatchTerminateTask(const std::string &strRecv,
  std::string &strSID, uint32_t &ulItemID)
{
  return CTimetableDispatchPackage::Inst()->ParseTerminateTask(strRecv, strSID, ulItemID);
}

//------------------------------------------------------------------------------
bool CNetPackage::BuildLogin(const std::string &strSID,
  EErrCode eEC, EUserType eType, std::string &strSend)
{
  return CLoginPackage::Inst()->BuildLogin(strSID, eEC, eType, strSend);
}

//------------------------------------------------------------------------------
bool CNetPackage::BuildLogout(EErrCode eEC, std::string &strSend)
{
  return CLoginPackage::Inst()->BuildLogOut(eEC, strSend);
}

//------------------------------------------------------------------------------
bool CNetPackage::BuildReadTag(const std::string &strSID,
  EErrCode eEC, const std::list<shared_ptr<KTagInfo>> &lstInfo, std::string &strSend)
{
  return CTagPackage::Inst()->BuildRead(strSID, eEC, lstInfo, strSend);
}

//------------------------------------------------------------------------------
bool CNetPackage::BuildWriteTag(const std::string &strSID,
  EErrCode eEC, const std::list<shared_ptr<KTagInfo>> &lstInfo, std::string &strSend)
{
  return CTagPackage::Inst()->BuildWrite(strSID, eEC, lstInfo, strSend);
}

//------------------------------------------------------------------------------
bool CNetPackage::BuildPushTag(const std::string &strSID,
  EErrCode eEC, const std::list<shared_ptr<KTagInfo>> &lstInfo, std::string &strSend)
{
  return CTagPackage::Inst()->BuildPush(strSID, eEC, lstInfo, strSend);
}

//------------------------------------------------------------------------------
bool CNetPackage::BuildWriteEvent(const std::string &strSID,
  EErrCode eEC, std::string &strSend)
{
  return CEventPackage::Inst()->BuildWrite(strSID, eEC, strSend);
}

//------------------------------------------------------------------------------
bool CNetPackage::BuildReadHistoryEvent(const std::string &strSID,
  EErrCode eEC, const std::list<shared_ptr<KEventInfo>> &lstInfo, std::string &strSend)
{
  return CEventPackage::Inst()->BuildReadHistory(strSID, eEC, lstInfo, strSend);
}

//------------------------------------------------------------------------------
bool CNetPackage::BuildPushEvent(const std::string &strSID,
  EErrCode eEC, const std::list<shared_ptr<KEventInfo>> &lstInfo, std::string &strSend)
{
  return CEventPackage::Inst()->BuildPush(strSID, eEC, lstInfo, strSend);
}

//------------------------------------------------------------------------------
bool CNetPackage::BuildReadHistoryAlarm(const std::string &strSID,
  EErrCode eEC, const std::list<shared_ptr<KAlarmInfo>> &lstInfo, std::string &strSend)
{
  return CAlarmPackage::Inst()->BuildReadHistory(strSID, eEC, lstInfo, strSend);
}

//------------------------------------------------------------------------------
bool CNetPackage::BuildPushAlarm(const std::string &strSID, EErrCode eEC,
  const std::list<shared_ptr<KAlarmInfo>> &lstInfo, std::string &strSend)
{
  return CAlarmPackage::Inst()->BuildPush(strSID, eEC, lstInfo, strSend);
}

//------------------------------------------------------------------------------
bool CNetPackage::BuildAdd_AlarmAssistant(const std::string &strSID,
  EErrCode eEC, const std::list<uint32_t> &lstID, std::string &strSend)
{
  return CAlarmAssistantPackage::Inst()->BuildAdd(strSID, eEC, lstID, strSend);
}

//------------------------------------------------------------------------------
bool CNetPackage::BuildDelete_AlarmAssistant(const std::string &strSID,
  EErrCode eEC, const std::list<uint32_t> &lstID, std::string &strSend)
{
  return CAlarmAssistantPackage::Inst()->BuildDelete(strSID, eEC, lstID, strSend);
}

//------------------------------------------------------------------------------
bool CNetPackage::BuildModify_AlarmAssistant(const std::string &strSID,
  EErrCode eEC, const std::list<uint32_t> &lstID, std::string &strSend)
{
  return CAlarmAssistantPackage::Inst()->BuildModify(strSID, eEC, lstID, strSend);
}

//------------------------------------------------------------------------------
bool CNetPackage::BuildSearch_AlarmAssistant(const std::string &strSID,
  EErrCode eEC, const std::list<std::shared_ptr<KTBAlarmAssistant>> &lstInfo,
  std::string &strSend)
{
  return CAlarmAssistantPackage::Inst()->BuildSearch(strSID, eEC, lstInfo, strSend);
}

//------------------------------------------------------------------------------
bool CNetPackage::BuildSearch_Num_AlarmAssistant(const std::string &strSID,
  EErrCode eEC, const std::shared_ptr<KTBAlarmAssistantSearchResult_Num> ptResult,
  std::string &strSend)
{
  return CAlarmAssistantPackage::Inst()->BuildSearch_Num(strSID, eEC, ptResult, strSend);
}

//------------------------------------------------------------------------------
bool CNetPackage::BuildSearchPaging_AlarmAssistant(const std::string &strSID,
  EErrCode eEC, const std::shared_ptr<KTBAlarmAssistantSearchResult> ptResult,
  std::string &strSend)
{
  return CAlarmAssistantPackage::Inst()->BuildSearchPaging(strSID, eEC, ptResult, strSend);
}

//------------------------------------------------------------------------------
bool CNetPackage::BuildAdd_ChangeShifts(const std::string &strSID,
  EErrCode eEC, const std::list<uint32_t> &lstID, std::string &strSend)
{
  return CChgShiftsPackage::Inst()->BuildAdd(strSID, eEC, lstID, strSend);
}

//------------------------------------------------------------------------------
bool CNetPackage::BuildCheckDBStatus(const std::string &strSID, EErrCode eEC,
  const std::list<std::shared_ptr<KDBStatusInfo>> &lstInfo, std::string &strSend)
{
  return CCheckStatusPackage::Inst()->BuildDB(strSID, eEC, lstInfo, strSend);
}

//------------------------------------------------------------------------------
bool CNetPackage::BuildCheckOPCStatus(const std::string &strSID, EErrCode eEC,
  const std::list<std::shared_ptr<KOPCStatusInfo>> &lstInfo, std::string &strSend)
{
  return CCheckStatusPackage::Inst()->BuildOPC(strSID, eEC, lstInfo, strSend);
}

//------------------------------------------------------------------------------
bool CNetPackage::BuildSearchSupplierForTagID(std::string &strSID, EErrCode eEC, const std::shared_ptr<SupplierInfoAndID> &lstInfo, std::string &strSend)
{
    return CBMSInfoPackage::Inst()->BuildSearchSupplierForID(strSID, eEC, lstInfo, strSend);
}

//------------------------------------------------------------------------------
bool CNetPackage::BuildSearchDeviceInfForTagID(std::string &strSID, EErrCode eEC, const std::shared_ptr<DeviceInfoAndID> &lstInfo, std::string &strSend)
{
    return CBMSInfoPackage::Inst()->BuildSearchDeviceInfForID(strSID, eEC, lstInfo, strSend);
}

//------------------------------------------------------------------------------
bool CNetPackage::BuildSearchRepairInfForTagID(std::string &strSID, EErrCode eEC, const std::shared_ptr<RepairInfoAndID> &lstInfo, std::string &strSend)
{
    return CBMSInfoPackage::Inst()->BuildSearchRepairInfForID(strSID, eEC, lstInfo, strSend);
}

//------------------------------------------------------------------------------
bool CNetPackage::BuildSearchRunInfForTagID(std::string &strSID, EErrCode eEC, const std::shared_ptr<DeviceRunInfoAndID> &lstInfo, std::string &strSend)
{
    return CBMSInfoPackage::Inst()->BuildSearchRunInfForID(strSID, eEC, lstInfo, strSend);
}

bool CNetPackage::BuildOnlyHead(std::string &strSID, EErrCode eEC, ECommand eCmd, std::string &strSend)
{
    return CBMSInfoPackage::Inst()->BuildOnlyHead(strSID, eEC, eCmd, strSend);
}

bool CNetPackage::BuildAddDeviceRecordInfo(std::string &strSID, EErrCode eEC, ECommand eCmd, const uint32_t &ulRecordingID, std::string &strSend)
{
    return CBMSInfoPackage::Inst()->BuildAddDeviceRecordInfo(strSID, eEC, eCmd, ulRecordingID, strSend);
}

bool CNetPackage::BuildSearchSupplier(std::string &strSID, EErrCode eEC, const list<std::shared_ptr<SupplierInfo>> &lstInfo, std::string &strSend)
{
    return CBMSInfoPackage::Inst()->BuildSearchSupplier(strSID, eEC, lstInfo, strSend);
}

bool CNetPackage::BuildSearchMeterType(std::string &strSID, EErrCode eEC, const list<std::shared_ptr<MeterType>> &lstInfo, std::string &strSend)
{
    return CBMSInfoPackage::Inst()->BuildSearchMeterType(strSID, eEC, lstInfo, strSend);
}

bool CNetPackage::BuildSearchEnergyCate(std::string &strSID, EErrCode eEC, const list<std::shared_ptr<EnergyCate>> &lstInfo, std::string &strSend)
{
    return CBMSInfoPackage::Inst()->BuildSearchEnergyCate(strSID, eEC, lstInfo, strSend);
}

bool CNetPackage::BuildSearchDevFaultCate(std::string &strSID, EErrCode eEC, const list<std::shared_ptr<DevFaultCate>> &lstInfo, std::string &strSend)
{
    return CBMSInfoPackage::Inst()->BuildSearchDevFaultCate(strSID, eEC, lstInfo, strSend);
}

bool CNetPackage::BuildSearchDeviceInfo(std::string &strSID, EErrCode eEC, const list<std::shared_ptr<DeviceInfo>> &lstInfo, std::string &strSend)
{
    return CBMSInfoPackage::Inst()->BuildSearchDeviceInfo(strSID, eEC, lstInfo, strSend);
}

bool CNetPackage::BuildSearchMeterInfo(std::string &strSID, EErrCode eEC, const list<std::shared_ptr<MeterInfo>> &lstInfo, std::string &strSend)
{
    return CBMSInfoPackage::Inst()->BuildSearchMeterInfo(strSID, eEC, lstInfo, strSend);
}

bool CNetPackage::BuildSearchDeviceParameter(std::string &strSID, EErrCode eEC, const std::shared_ptr<DeviceParameter> &lstInfo, std::string &strSend)
{
    return CBMSInfoPackage::Inst()->BuildSearchDeviceParameter(strSID, eEC, lstInfo, strSend);
}

bool CNetPackage::BuildSearchMeterParameter(std::string &strSID, EErrCode eEC, const std::shared_ptr<MeterParameter> &lstInfo, std::string &strSend)
{
    return CBMSInfoPackage::Inst()->BuildSearchMeterParameter(strSID, eEC, lstInfo, strSend);
}

bool CNetPackage::BuildSearchDeviceRepair(std::string &strSID, EErrCode eEC, ECommand eCmd, const list<shared_ptr<RepairInfo>> &lstInfo, std::string &strSend)
{
    return CBMSInfoPackage::Inst()->BuildSearchDeviceRepair(strSID, eEC, eCmd, lstInfo, strSend);
}

bool CNetPackage::BuildSearchDeviceRun(std::string &strSID, EErrCode eEC, ECommand eCmd, const list<shared_ptr<DeviceRunInfo>> &lstInfo, std::string &strSend)
{
    return CBMSInfoPackage::Inst()->BuildSearchDeviceRun(strSID, eEC, eCmd, lstInfo, strSend);
}

bool CNetPackage::BuildSearchSubSysVariableInfo(std::string &strSID, EErrCode eEC, const shared_ptr<SubSysInfo> &lstInfo, std::string &strSend)
{
    return CBMSInfoPackage::Inst()->BuildSearchSubSysVariableInfo(strSID, eEC, lstInfo, strSend);
}

bool CNetPackage::BuildSearchDeviceVariableInfo(std::string &strSID, EErrCode eEC, const shared_ptr<DeviceVarInfo> &lstInfo, std::string &strSend)
{
    return CBMSInfoPackage::Inst()->BuildSearchDeviceVariableInfo(strSID, eEC, lstInfo, strSend);
}

bool CNetPackage::BuildSearchTagVariableInfo(std::string &strSID, EErrCode eEC, const shared_ptr<TagVarInfo> &lstInfo, std::string &strSend, ECommand command)
{
    return CBMSInfoPackage::Inst()->BuildSearchTagVariableInfo(strSID, eEC, lstInfo, strSend, command);
}

//总览
bool CNetPackage::BuildSearchOverViewInfo(std::string &strSID, EErrCode eEC, const shared_ptr<OverViewInfo> &lstInfo, std::string &strSend)
{
	return CBMSInfoPackage::Inst()->BuildSearchOverViewInfo(strSID, eEC, lstInfo, strSend);
}

bool CNetPackage::BuildSearchSystemAlarmCountInfo(std::string &strSID, EErrCode eEC, const shared_ptr<Subsystem_Alarm> &lstInfo, std::string &strSend)
{
	return CBMSInfoPackage::Inst()->BuildSearchSystemAlarmCountInfo(strSID, eEC, lstInfo, strSend);
}


bool CNetPackage::BuildSearchMeterValueInfo(std::string &strSID, EErrCode eEC, const shared_ptr<ResponseMeterValueInfo> &lstInfo, std::string &strSend)
{
    return CBMSInfoPackage::Inst()->BuildSearchMeterValueInfo(strSID, eEC, lstInfo, strSend);
}

bool CNetPackage::BuildSearchDeviceValueInfo(std::string &strSID, EErrCode eEC, const shared_ptr<DeviceValueInfo> &lstInfo, std::string &strSend)
{
    return CBMSInfoPackage::Inst()->BuildSearchDeviceValueInfo(strSID, eEC, lstInfo, strSend);
}

bool CNetPackage::BuildSearchVariableValueInfo(std::string &strSID, EErrCode eEC, const shared_ptr<DeviceValueInfo> &lstInfo, std::string &strSend)
{
    return CBMSInfoPackage::Inst()->BuildSearchVariableValueInfo(strSID, eEC, lstInfo, strSend);
}

bool CNetPackage::BuildSearchHistoryEventExInfo(std::string &strSID, EErrCode eEC, const shared_ptr<HistoryEventInfo> &lstInfo, std::string &strSend)
{
    return CBMSInfoPackage::Inst()->BuildSearchHistoryEventExInfo(strSID, eEC, lstInfo, strSend);
}

bool CNetPackage::BuildSearchHistoryAlarmExInfo(std::string &strSID, EErrCode eEC, const shared_ptr<HistoryAlarmInfo> &lstInfo, std::string &strSend)
{
    return CBMSInfoPackage::Inst()->BuildSearchHistoryAlarmExInfo(strSID, eEC, lstInfo, strSend);
}

//------------------------------------------------------------------------------
bool CNetPackage::BuildCameraScreenshot(const std::string &strSID,
  EErrCode eEC, std::string &strSend)
{
  return CCameraControlPackage::Inst()->BuildScreenshot(strSID, eEC, strSend);
}

//------------------------------------------------------------------------------
bool CNetPackage::BuildSearchCameraConfig(const std::string &strSID, EErrCode eEC,
  const std::list<std::shared_ptr<KCamConf>> &lstCamConf, std::string &strSend)
{
  return CCameraControlPackage::Inst()->BuildSearchConfig(strSID, eEC, lstCamConf, strSend);
}

//------------------------------------------------------------------------------
bool CNetPackage::BuildReadLinkageWatchInfo(const std::string &strSID, EErrCode eEC,
  const std::list<shared_ptr<KLinkageWatchInfo>> &lstInfo, std::string &strSend)
{
  return CLinkagePackage::Inst()->BuildReadWatchInfo(strSID, eEC, lstInfo, strSend);
}

//------------------------------------------------------------------------------
bool CNetPackage::BuildLinkageManualConfirmation(const std::string &strSID,
  EErrCode eEC, std::string &strSend)
{
  return CLinkagePackage::Inst()->BuildManualConfirmation(strSID, eEC, strSend);
}

//------------------------------------------------------------------------------
bool CNetPackage::BuildLinkageTerminateTask(const std::string &strSID,
  EErrCode eEC, std::string &strSend)
{
  return CLinkagePackage::Inst()->BuildTerminateTask(strSID, eEC, strSend);
}

//------------------------------------------------------------------------------
bool CNetPackage::BuildReadTimetableDispatchInfo(const std::string &strSID,
  EErrCode eEC, const std::list<shared_ptr<KTimetableDispatchWatchInfo>> &lstInfo,
  std::string &strSend)
{
  return CTimetableDispatchPackage::Inst()->BuildReadWatchInfo(strSID, eEC, lstInfo, strSend);
}

//------------------------------------------------------------------------------
bool CNetPackage::BuildTimetableDispatchTerminateTask(const std::string &strSID,
  EErrCode eEC, std::string &strSend)
{
  return CTimetableDispatchPackage::Inst()->BuildTerminateTask(strSID, eEC, strSend);
}
