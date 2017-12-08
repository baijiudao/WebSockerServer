//------------------------------------------------------------------------------
// 创建日期：2017-10-12 13:09:39
// 文件描述：信息管理相关解包打包
//------------------------------------------------------------------------------
#ifndef _BMS_INFO_PACKAGE_H_
#define _BMS_INFO_PACKAGE_H_

#include "PackageBase.h"

//------------------------------------------------------------------------------
// 创建日期：2017-10-12 13:10:03 
// 类 描 述：用来解析/打包信息管理的相关操作的包
//------------------------------------------------------------------------------
class CBMSInfoPackage : public CPackageBase, public ISingleton<CBMSInfoPackage>
{
public:
    ~CBMSInfoPackage();
    CBMSInfoPackage();
    bool ParseSearchID(const std::string &strRecv, std::string &strSID, uint32_t &lstInfo);
    bool BuildSearchSupplierForID(const std::string &strSID, EErrCode eEC, const std::shared_ptr<SupplierInfoAndID> &lstInfo, std::string &strSend);
    bool BuildSearchDeviceInfForID(const std::string &strSID, EErrCode eEC, const std::shared_ptr<DeviceInfoAndID> &lstInfo, std::string &strSend);
    bool BuildSearchRepairInfForID(const std::string &strSID, EErrCode eEC, const std::shared_ptr<RepairInfoAndID> &lstInfo, std::string &strSend);
    bool BuildSearchRunInfForID(const std::string &strSID, EErrCode eEC, const std::shared_ptr<DeviceRunInfoAndID> &lstInfo, std::string &strSend);

    //信息管理供应商
    bool ParseAddOrModSupplier(const std::string &strRecv, std::string &strSID, shared_ptr<SupplierInfo> &lstInfo);
    bool ParseDelOrSelSupplier(const std::string &strRecv, std::string &strSID, string &lstInfo);
    bool BuildOnlyHead(std::string &strSID, EErrCode eEC, ECommand eCmd, std::string &strSend);
    bool BuildAddDeviceRecordInfo(std::string &strSID, EErrCode eEC, ECommand eCmd, const uint32_t &recordingID, std::string &strSend);
    bool BuildSearchSupplier(std::string &strSID, EErrCode eEC, const list<std::shared_ptr<SupplierInfo>> &lstInfo, std::string &strSend);
    //信息管理计量表类型
    bool ParseAddOrModMeterType(const std::string &strRecv, std::string &strSID, shared_ptr<MeterType> &lstInfo);
    bool ParseDelOrSelMeterType(const std::string &strRecv, std::string &strSID, string &lstInfo);
    bool BuildSearchMeterType(std::string &strSID, EErrCode eEC, const list<std::shared_ptr<MeterType>> &lstInfo, std::string &strSend);
    //信息管理耗能类别
    bool ParseAddOrModEnergyCate(const std::string &strRecv, std::string &strSID, shared_ptr<EnergyCate> &lstInfo);
    bool ParseDelOrSelEnergyCate(const std::string &strRecv, std::string &strSID, string &lstInfo);
    bool BuildSearchEnergyCate(std::string &strSID, EErrCode eEC, const list<std::shared_ptr<EnergyCate>> &lstInfo, std::string &strSend);
    //信息管理设备故障类别
    bool ParseAddOrModDevFaultCate(const std::string &strRecv, std::string &strSID, shared_ptr<DevFaultCate> &lstInfo);
    bool ParseDelOrSelDevFaultCate(const std::string &strRecv, std::string &strSID, string &lstInfo);
    bool BuildSearchDevFaultCate(std::string &strSID, EErrCode eEC, const list<std::shared_ptr<DevFaultCate>> &lstInfo, std::string &strSend);
    //信息管理设备档案信息
    bool ParseAddOrModDeviceInfo(const std::string &strRecv, std::string &strSID, shared_ptr<DeviceInfo> &lstInfo);
    bool ParseDelOrSelDeviceInfo(const std::string &strRecv, std::string &strSID, string &lstInfo);
    bool BuildSearchDeviceInfo(std::string &strSID, EErrCode eEC, const list<std::shared_ptr<DeviceInfo>> &lstInfo, std::string &strSend);
    //信息管理计量表档案信息
    bool ParseAddOrModMeterInfo(const std::string &strRecv, std::string &strSID, shared_ptr<MeterInfo> &lstInfo);
    bool ParseDelOrSelMeterInfo(const std::string &strRecv, std::string &strSID, string &lstInfo);
    bool BuildSearchMeterInfo(std::string &strSID, EErrCode eEC, const list<std::shared_ptr<MeterInfo>> &lstInfo, std::string &strSend);
    //信息管理设备参数
    bool ParseAddDeviceParameter(const std::string &strRecv, std::string &strSID, const shared_ptr<DeviceParameter>&lstInfo);
    bool ParseDelDeviceParameter(const std::string &strRecv, std::string &strSID, shared_ptr<DeviceParameter>&lstInfo);
    bool ParseSearchDeviceParameter(const std::string &strRecv, std::string &strSID, string &lstInfo);
    bool BuildSearchDeviceParameter(std::string &strSID, EErrCode eEC, const std::shared_ptr<DeviceParameter> &lstInfo, std::string &strSend);
    //信息管理计量表参数
    bool ParseAddMeterParameter(const std::string &strRecv, std::string &strSID, const shared_ptr<MeterParameter>&lstInfo);
    bool ParseDelMeterParameter(const std::string &strRecv, std::string &strSID, shared_ptr<MeterParameter>&lstInfo);
    bool ParseSearchMeterParameter(const std::string &strRecv, std::string &strSID, string &lstInfo);
    bool BuildSearchMeterParameter(std::string &strSID, EErrCode eEC, const std::shared_ptr<MeterParameter> &lstInfo, std::string &strSend);
    //信息管理设备维修记录
    bool ParseAddOrModDeviceRepair(const std::string &strRecv, std::string &strSID, shared_ptr<RepairInfo> &lstInfo);
    bool ParseDelOrSelDeviceRepair(const std::string &strRecv, std::string &strSID, int &lstInfo);
    bool ParseSearchDeviceRepair(const std::string &strRecv, std::string &strSID, string &lstInfo);
    bool BuildSearchDeviceRepair(std::string &strSID, EErrCode eEC, ECommand eCmd, const list<shared_ptr<RepairInfo>> &lstInfo, std::string &strSend);
    //信息管理设备运行记录
    bool ParseAddOrModDeviceRun(const std::string &strRecv, std::string &strSID, shared_ptr<DeviceRunInfo> &lstInfo);
    bool ParseDelOrSelDeviceRun(const std::string &strRecv, std::string &strSID, int &lstInfo);
    bool ParseSearchDeviceRun(const std::string &strRecv, std::string &strSID, string &lstInfo);
    bool BuildSearchDeviceRun(std::string &strSID, EErrCode eEC, ECommand eCmd, const list<shared_ptr<DeviceRunInfo>> &lstInfo, std::string &strSend);
    //信息管理点表信息
    bool ParseSearchSubSysVariableInfo(const std::string &strRecv, std::string &strSID);
    bool ParseSearchDeviceVariableInfo(const std::string &strRecv, std::string &strSID, std::string &lstInfo);
    bool ParseSearchTagVariableInfo(const std::string &strRecv, std::string &strSID, string &lstInfo);
    bool BuildSearchSubSysVariableInfo(std::string &strSID, EErrCode eEC, const shared_ptr<SubSysInfo> &lstInfo, std::string &strSend);
    bool BuildSearchDeviceVariableInfo(std::string &strSID, EErrCode eEC, const shared_ptr<DeviceVarInfo> &lstInfo, std::string &strSend);
    bool BuildSearchTagVariableInfo(std::string &strSID, EErrCode eEC, const shared_ptr<TagVarInfo> &lstInfo, std::string &strSend, ECommand command);
  
	bool ParseSearchOverViewInfo(const std::string &strRecv, std::string &strSID, string &lstInfo);
	//总览
	bool BuildSearchOverViewInfo(std::string &strSID, EErrCode eEC, const shared_ptr<OverViewInfo> &lstInfo, std::string &strSend);
	bool BuildSearchSystemAlarmCountInfo(std::string &strSID, EErrCode eEC, const shared_ptr<Subsystem_Alarm> &lstInfo, std::string &strSend);
	//信息管理计量表、设备、变量查询
    bool ParseSearchMeterValueInfo(const std::string &strRecv, std::string &strSID, shared_ptr<ValueRequestInfo> &lstInfo);
    bool ParseSearchDeviceValueInfo(const std::string &strRecv, std::string &strSID, shared_ptr<ValueRequestInfo> &lstInfo);
    bool ParseSearchVariableValueInfo(const std::string &strRecv, std::string &strSID, shared_ptr<ValueRequestInfo> &lstInfo);
    bool BuildSearchMeterValueInfo(std::string &strSID, EErrCode eEC, const shared_ptr<ResponseMeterValueInfo> &lstInfo, std::string &strSend);
    bool BuildSearchDeviceValueInfo(std::string &strSID, EErrCode eEC, const shared_ptr<DeviceValueInfo> &lstInfo, std::string &strSend);
    bool BuildSearchVariableValueInfo(std::string &strSID, EErrCode eEC, const shared_ptr<DeviceValueInfo> &lstInfo, std::string &strSend);
    //信息管理历史事件/告警
    bool ParseSearchHistoryEventExInfo(const std::string &strRecv, std::string &strSID, shared_ptr<HistoryEventRequestInfo> &lstInfo);
    bool ParseSearchHistoryAlarmExInfo(const std::string &strRecv, std::string &strSID, shared_ptr<HistoryAlarmRequestInfo> &lstInfo);
    bool BuildSearchHistoryEventExInfo(std::string &strSID, EErrCode eEC, const shared_ptr<HistoryEventInfo> &lstInfo, std::string &strSend);
    bool BuildSearchHistoryAlarmExInfo(std::string &strSID, EErrCode eEC, const shared_ptr<HistoryAlarmInfo> &lstInfo, std::string &strSend);
};

#endif // !_BMS_INFO_PACKAGE_H_
