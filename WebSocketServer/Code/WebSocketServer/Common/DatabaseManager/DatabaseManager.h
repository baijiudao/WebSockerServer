//------------------------------------------------------------------------------
// 创建日期：2017-09-11 13:52:46
// 文件描述：所有操作数据库的接口都在这个里面
//------------------------------------------------------------------------------
#ifndef _DATABASE_MANAGER_H_
#define _DATABASE_MANAGER_H_

#include "AlarmAssistantTable.h"
#include "ChangeShiftsTable.h"
#include "DatabaseReportForms.h"
#include "BMSInfoManager.h"

// 数据库状态码
typedef enum class _eDBStatus : uint32_t
{
  OK = 0,     // 0  正常
  UNCONNECT,  // 1  不能连接
  LAST
}EDBStatus;

// 数据库状态信息
typedef struct _DBStatusInfo
{
  std::string strDescribe;  // 数据库描述（或者数据库名字）
  EDBStatus   eStatus;      // 数据库状态
}KDBStatusInfo;

class CDatabaseManager
{
private:
  std::recursive_mutex  m_mutex;  // 用来确保数据库不会重复连接
  std::shared_ptr<COTLConnectPool> m_pDBConnectPool;
  std::shared_ptr<CAlarmAssistantTable> m_pAlarmAssistantTable;
  std::shared_ptr<CChangeShiftsTable>   m_pChangeShiftsTable;
  std::shared_ptr<DatabaseReportForms>  m_pDatabaseRepportForms;
  std::shared_ptr<BMSInfoManager> m_pBMSInfoManager;
  shared_ptr<std::thread> m_pThread;

public:
  CDatabaseManager();
  ~CDatabaseManager();
  bool  Connect(const std::string &strUID, const std::string &strPWD, const std::string &strDSN);
  bool  CheckDBStatus(std::list<std::shared_ptr<KDBStatusInfo>> &lstInfo);
  bool  AddAlarmAssistant(const std::list<std::shared_ptr<KTBAlarmAssistant>> &lstInfo, std::list<uint32_t> &lstID);
  bool  DeleteAlarmAssistant(const std::list<uint32_t> &lstID);
  bool  ModifyAlarmAssistant(const std::list<std::shared_ptr<KTBAlarmAssistant>> &lstInfo);
  bool  SearchAlarmAssistant(uint32_t ulGlobalID, std::list<std::shared_ptr<KTBAlarmAssistant>> &lstInfo);
  bool  SearchAlarmAssistant_Num(const std::shared_ptr<KTBAlarmAssistantSearchCondition_Num> ptCondition, std::shared_ptr<KTBAlarmAssistantSearchResult_Num> ptResult);
  bool  SearchPagingAlarmAssistant(const std::shared_ptr<KTBAlarmAssistantSearchCondition> ptCondition, std::shared_ptr<KTBAlarmAssistantSearchResult> ptResult);
  bool  AddChangeShifts(const std::list<std::shared_ptr<KTBChgShifts>> &lstInfo, std::list<uint32_t> &lstID);

  EErrCode SearchSupplierForTagID(const uint32_t &lstID, std::shared_ptr<SupplierInfoAndID> &ptResult);
  EErrCode SearchDeviceInfForTagID(const uint32_t &lstID, std::shared_ptr<DeviceInfoAndID> &ptResult);
  EErrCode SearchRepairInfForTagID(const uint32_t &lstID, std::shared_ptr<RepairInfoAndID> &ptResult);
  EErrCode SearchRunInfForTagID(const uint32_t &lstID, std::shared_ptr<DeviceRunInfoAndID> &ptResult);

  EErrCode AddSupplier(const std::shared_ptr<SupplierInfo> &ptResult);
  EErrCode DeleteSupplier(const string supplierName);
  EErrCode ModifySuplier(const std::shared_ptr<SupplierInfo> &ptResult);
  EErrCode SearchSuplier(const string supplierName, list<std::shared_ptr<SupplierInfo>> &ptResult);

  EErrCode AddMeterType(const std::shared_ptr<MeterType> &ptResult);
  EErrCode DeleteMeterType(const string meterTypeName);
  EErrCode ModifyMeterType(const std::shared_ptr<MeterType> &ptResult);
  EErrCode SearchMeterType(const string meterTypeName, list<std::shared_ptr<MeterType>> &ptResult);

  EErrCode AddEnergyCate(const std::shared_ptr<EnergyCate> &ptResult);
  EErrCode DeleteEnergyCate(const string name);
  EErrCode ModifyEnergyCate(const std::shared_ptr<EnergyCate> &ptResult);
  EErrCode SearchEnergyCate(const string name, list<std::shared_ptr<EnergyCate>> &ptResult);

  EErrCode AddDevFaultCate(const std::shared_ptr<DevFaultCate> &ptResult);
  EErrCode DeleteDevFaultCate(const string name);
  EErrCode ModifyDevFaultCate(const std::shared_ptr<DevFaultCate> &ptResult);
  EErrCode SearchDevFaultCate(const string name, list<std::shared_ptr<DevFaultCate>> &ptResult);

  EErrCode AddDeviceInfo(const std::shared_ptr<DeviceInfo> &ptResult);
  EErrCode DeleteDeviceInfo(const string name);
  EErrCode ModifyDeviceInfo(const std::shared_ptr<DeviceInfo> &ptResult);
  EErrCode SearchDeviceInfo(const string name, list<std::shared_ptr<DeviceInfo>> &ptResult);

  EErrCode AddMeterInfo(const std::shared_ptr<MeterInfo> &ptResult);
  EErrCode DeleteMeterInfo(const string name);
  EErrCode ModifyMeterInfo(const std::shared_ptr<MeterInfo> &ptResult);
  EErrCode SearchMeterInfo(const string name, list<std::shared_ptr<MeterInfo>> &ptResult);

  EErrCode AddDeviceParameter(const std::shared_ptr<DeviceParameter> &ptResult);
  EErrCode DeleteDeviceParameter(const std::shared_ptr<DeviceParameter> &ptResult);
  EErrCode SearchDeviceParameter(const string name, std::shared_ptr<DeviceParameter> &ptResult);
  
  EErrCode AddMeterParameter(const std::shared_ptr<MeterParameter> &ptResult);
  EErrCode DeleteMeterParameter(const std::shared_ptr<MeterParameter> &ptResult);
  EErrCode SearchMeterParameter(const string name, std::shared_ptr<MeterParameter> &ptResult);

  EErrCode AddDeviceRepair(const std::shared_ptr<RepairInfo> &ptResult, uint32_t & ulRecordingID);
  EErrCode DeleteDeviceRepair(const int name);
  EErrCode ModifyDeviceRepair(const std::shared_ptr<RepairInfo> &ptResult);
  EErrCode SearchDeviceRepair(const int name, list<shared_ptr<RepairInfo>> &ptResult);
  EErrCode SearchDeviceRepair(const string name, list<shared_ptr<RepairInfo>> &ptResult);

  EErrCode AddDeviceRun(const std::shared_ptr<DeviceRunInfo> &ptResult, uint32_t & ulRecordingID);
  EErrCode DeleteDeviceRun(const int name);
  EErrCode ModifyDeviceRun(const std::shared_ptr<DeviceRunInfo> &ptResult);
  EErrCode SearchDeviceRun(const int name, list<shared_ptr<DeviceRunInfo>> &ptResult);
  EErrCode SearchDeviceRun(const string name, list<shared_ptr<DeviceRunInfo>> &ptResult);

  EErrCode SearchSubSysVariableInfo(shared_ptr<SubSysInfo> &ptResult);
  EErrCode SearchDeviceVariableInfo(const string name, shared_ptr<DeviceVarInfo> &ptResult);
  EErrCode SearchTagVariableInfo(const string name, shared_ptr<TagVarInfo> &ptResult);
  EErrCode SearchTagVarWithoutPar(const string name, shared_ptr<TagVarInfo> &ptResult);

  EErrCode SearchMeterValueInfo(const shared_ptr<ValueRequestInfo>  ptRequestInfo, shared_ptr<ResponseMeterValueInfo> &ptResult);
  EErrCode SearchDeviceValueInfo(const shared_ptr<ValueRequestInfo>  ptRequestInfo, shared_ptr<DeviceValueInfo> &ptResult);
  EErrCode SearchVariableValueInfo(const shared_ptr<ValueRequestInfo>  ptRequestInfo, shared_ptr<DeviceValueInfo> &ptResult);

  EErrCode SearchHistoryEventExInfo(const shared_ptr<HistoryEventRequestInfo>  ptRequestInfo, shared_ptr<HistoryEventInfo> &ptResult);
  EErrCode SearchHistoryAlarmExInfo(const shared_ptr<HistoryAlarmRequestInfo>  ptRequestInfo, shared_ptr<HistoryAlarmInfo> &ptResult);
  
  //总览
  EErrCode SearchOverViewInfo(shared_ptr<OverViewInfo> &ptRequest);

  EErrCode SearchAlarmCountInfo(shared_ptr<Subsystem_Alarm> &ptResult);
protected:
    void OnThreadPrc();
};

#endif // !_DATABASE_MANAGER_H_
