//------------------------------------------------------------------------------
// 创建日期：2017-10-11 14:00:45
// 文件描述：用来读写数据库表信息
//------------------------------------------------------------------------------
#ifndef _BMS_INFO_MANAGER_H_
#define _BMS_INFO_MANAGER_H_

#include "TableBase.h"

// 设备厂商信息
typedef struct _SupplierInfo
{
    string sSupplierCode;   //供应商代码
    string sSupplierName;   //供应商名称
    string sConPerson;      //联系人
    string sConDepartment;  //联系部门
    string sConTelephone;   //联系电话
    string sFax;            //传真
    string sAddress;        //地址
    string sZipCode;        //邮政编码
    string sE_Mail;         //电子邮箱
    string sURL;            //网址
    string sRemarks;        //备注
    _SupplierInfo()
    {
        sSupplierCode = "";
        sSupplierName = "";
        sConPerson = "";
        sConDepartment = "";
        sConTelephone = "";
        sFax = "";
        sAddress = "";
        sZipCode = "";
        sE_Mail = "";
        sURL = "";
        sRemarks = "";
    }
}SupplierInfo;

typedef struct _SupplierInfoAndID
{
    uint32_t ulGID;
    SupplierInfo supplierInfo;
}SupplierInfoAndID;

// 设备档案信息
typedef struct _DeviceInfo
{
    string DeviceNum;               //设备编号
    string DeviceName;              //设备名称
    string InstallLocation;         //安装位置
    string UseDate;                 //使用日期
    string UsageDepart;             //使用部门
    string DeviceStatus;            //设备状况
    string ProductionDate;          //生产日期
    string DeviceSupplier;          //设备供应商
    double AlarmUpLimit;            //报警上限
    double AlarmDownLimit;          //报警下限
    double RunCtrlUpLimit;          //运行控制上限
    double RunCtrlDownLimit;        //运行控制下限
    _DeviceInfo()
    {
        DeviceNum = "";
        DeviceName = "";
        InstallLocation = "";
        UseDate = "";
        UsageDepart = "";
        DeviceStatus = "";
        ProductionDate = "";
        DeviceSupplier = "";
        AlarmUpLimit = 0;
        AlarmDownLimit = 0;
        RunCtrlUpLimit = 0;
        RunCtrlDownLimit = 0;
    }
}DeviceInfo;

typedef struct _DeviceInfoAndID
{
    uint32_t ulGID;
    DeviceInfo deviceInfo;
}DeviceInfoAndID;

// 设备维修记录
typedef struct _RepairInfo
{
    int recordingID;             //记录id
    string DeviceName;              //设备名称
    string RepairContent;           //维修内容
    string RepairTime;              //维修时间
    string FaultReason;             //故障原因
    string RepairResults;           //维修结果
    string RepairManager;           //维修经办人
    string RepairUnit;              //维修单位
    string Remarks;                 //备注
    _RepairInfo()
    {
        recordingID = 0;
        DeviceName = "";
        RepairContent = "";
        RepairTime = "";
        FaultReason = "";
        RepairResults = "";
        RepairManager = "";
        RepairUnit = "";
        Remarks = "";
    }
}RepairInfo;

typedef struct _RepairInfoAndID
{
    uint32_t ulGID;
    RepairInfo repairInfo;
}RepairInfoAndID;

// 设备运行信息
typedef struct _DeviceRunInfo
{
    int recordingID;             //记录id
    string DeviceName;              //设备名称
    string StartTime;               //开始时间
    string EndTime;                 //结束时间
    string Operation;               //运行情况
    string Recorder;                //记录员
    string Remarks;                 //备注
    _DeviceRunInfo()
    {
        recordingID = 0;
        DeviceName = "";
        StartTime = "";
        EndTime = "";
        Operation = "";
        Recorder = "";
        Remarks = "";
    }
}DeviceRunInfo;

typedef struct _DeviceRunInfoAndID
{
    uint32_t ulGID;
    DeviceRunInfo deviceRunInfo;
}DeviceRunInfoAndID;

// 计量表类型信息
typedef struct _MeterType
{
    string strMeterType;       //计量表类型
    string TypeDes;        //类型描述
    _MeterType()
    {
        strMeterType = "";
        TypeDes = "";
    }
}MeterType;

//耗能类别信息
typedef struct _EnergyCate
{
    string strEnergyCate;       //耗能类别
    string TypeDes;        //类型描述
    _EnergyCate()
    {
        strEnergyCate = "";
        TypeDes = "";
    }
}EnergyCate;

//设备故障类别信息
typedef struct _DevFaultCate
{
    string FaultCate;          //故障类别
    string CategoryDes;        //类别说明
    string FaultFeature;       //故障特征
    _DevFaultCate()
    {
        FaultCate = "";
        CategoryDes = "";
        FaultFeature = "";
    }
}DevFaultCate;

// 设备档案信息
typedef struct _MeterInfo
{
    string MeterNumber;               //计量表编号
    string strMeterType;              //计量表类型
    string strEnergyCate;             //耗能类别
    string MeterModel;                //表型号
    string UseUnits;                  //使用单位
    double RateStandard;              //费率标准
    string StandardUnit;              //标准计量单位
    string InstallDate;               //安装日期
    string InstallLocation;           //安装位置
    string ProductionFactory;         //生成厂家
    short MeterMagnification;         //表倍率
    double TranMeterBase;             //转表基数
    double MeterInitValue;            //表的初始读数
    double TranMeterValue;            //转表读数
    string Remarks;                   //备注
    string OldMeterNumber;            //旧表编号
    uint32_t tagID;                   //能源参数id

    _MeterInfo()
    {
        MeterNumber = "";
        strMeterType = "";
        strEnergyCate = "";
        MeterModel = "";
        UseUnits = "";
        RateStandard = 0;
        StandardUnit = "";
        InstallDate = "";
        InstallLocation = "";
        ProductionFactory = "";
        MeterMagnification = 0;
        TranMeterBase = 0;
        MeterInitValue = 0;
        TranMeterValue = 0;
        Remarks = "";
        tagID = 0;
        OldMeterNumber = "";
    }
}MeterInfo;

// 设备参数
typedef struct _DeviceParameter
{
    string DeviceName;               //设备名称
    map<uint32_t,string> lstGID;     //参数id列表
    _DeviceParameter()
    {
        DeviceName = "";
    }
}DeviceParameter;

// 计量表参数
typedef struct _MeterParameter
{
    string MeterNum;                //计量表编号
    uint32_t ulGID;                 //参数id列表
    _MeterParameter()
    {
        MeterNum = "";
        ulGID = 0;
    }
}MeterParameter;

//点表子系统
typedef struct _SubSysInfo
{
    map<string, string> lstSubSys;     //子系统列表（id，name）
}SubSysInfo;

//点表设备
typedef struct _DeviceVarInfo
{
    string SysID;
    map<string, string> lstDevice;     //设备列表（id，name）
    _DeviceVarInfo()
    {
        SysID = "";
    }
}DeviceVarInfo;

//点表变量
typedef struct _TagVarInfo
{
    string DevID;
    map<string, string> lstTag;     //变量列表（id，name）
    _TagVarInfo()
    {
        DevID = "";
    }
}TagVarInfo;

//分页查询中公共变量
typedef struct _PublicRequestInfo
{
    uint64_t ullStartTime;
    uint64_t ullEndTime;
    int pageSize;
    int pageNum;
    _PublicRequestInfo()
    {
        ullStartTime = 0;
        ullEndTime = 0;
        pageSize = 0;
        pageNum = 0;
    }
}PublicRequestInfo;

//计量表/设备/变量请求结构体
typedef struct _ValueRequestInfo
{
    PublicRequestInfo publicInfo;
    string strName;
    _ValueRequestInfo()
    {
        strName = "";
    }
}ValueRequestInfo;

//变量值
typedef struct _ValueInfo
{
    uint32_t ulId;
    string tagName;
    string tagValue;
    string collectionTime;
    _ValueInfo()
    {
        ulId = 0;
        tagName = "";
        tagValue = "";
        collectionTime = "";
    }
}ValueInfo;

//计量表变量结构体
typedef struct _MeterValueInfo
{
    string meterNumber;
    string meterType;
    ValueInfo valueInfo;
    _MeterValueInfo()
    {
        meterNumber = "";
        meterType = "";
    }
}MeterValueInfo;

//计量表变量读数
typedef struct _ResponseMeterValueInfo
{
    int pageSize;
    int pageNum;
    int pageTotal;
    list<MeterValueInfo> lstValueInfo;
    _ResponseMeterValueInfo()
    {
        pageSize = 0;
        pageNum = 0;
        pageTotal = 0;
    }
}ResponseMeterValueInfo;

//设备与变量读数
typedef struct _DeviceValueInfo
{
    int pageSize;
    int pageNum;
    int pageTotal;
    list<ValueInfo> lstValueInfo;
    _DeviceValueInfo()
    {
        pageSize = 0;
        pageNum = 0;
        pageTotal = 0;
    }
}DeviceValueInfo;

//历史事件请求结构体
typedef struct _HistoryEventRequestInfo
{
    PublicRequestInfo publicInfo;
    string strUserName;
    EEventType enventType;
    
    _HistoryEventRequestInfo()
    {
        strUserName = "";
        enventType = 0;
    }
}HistoryEventRequestInfo;

//历史告警请求结构体
typedef struct _HistoryAlarmRequestInfo
{
    PublicRequestInfo publicInfo;
    string strDeviceName;
    uint32_t    ulPriority;
    string strArea;
    _HistoryAlarmRequestInfo()
    {
        strDeviceName = "";
        ulPriority = 0;
        strArea = "";
    }
}HistoryAlarmRequestInfo;

typedef struct _HistoryEventInfo
{
    int pageSize;
    int pageNum;
    int pageTotal;
    list<KEventInfo> lstValueInfo;
    _HistoryEventInfo()
    {
        pageSize = 0;
        pageNum = 0;
        pageTotal = 0;
    }
}HistoryEventInfo;

// Alarm
typedef struct _AlarmInfoEx
{
    uint32_t    ulGID;      // 全局变量
    uint64_t    ullTimeMs;  // 时间（毫秒）
    uint32_t    ulID;       // 告警id
    uint32_t    ulCategory;
    uint32_t    ulType;
    uint32_t    ulPriority;
    string      strSource;
    string      strValue;
    string      strMessage;
    string      strLimit;
    bool        bAck;

    _AlarmInfoEx()
    {
        ulGID = 0;
        ullTimeMs = 0;
        ulID = 0;
        ulCategory = 0;
        ulType = 0;
        ulPriority = 0;
        strSource = "";
        strValue = "";
        strMessage = "";
        strLimit = "";
        bAck = false;
    }
}KAlarmInfoEx;

typedef struct _HistoryAlarmInfo
{
    int pageSize;
    int pageNum;
    int pageTotal;
    list<KAlarmInfoEx> lstValueInfo;
    _HistoryAlarmInfo()
    {
        pageSize = 0;
        pageNum = 0;
        pageTotal = 0;
    }
}HistoryAlarmInfo;

typedef struct _CommonData
{
	string subsystemId;
	int deviceCount;
	int tagCount;
	//map<shared_ptr<DeviceVarInfo>, int> commonData;
}CommonData;

typedef struct _OverViewInfo
{
	list<shared_ptr<CommonData>> overData;
}OverViewInfo;

typedef struct _Subsystem_Alarm
{
	map<string, int> alarmInfo;
}Subsystem_Alarm;

class BMSInfoManager : public CTableBase
{
public:
    BMSInfoManager();
    ~BMSInfoManager();
    EErrCode SearchSupplierForTagID(const otl_connect &dbconnect, const uint32_t &lstID, std::shared_ptr<SupplierInfoAndID> &ptResult);
    EErrCode SearchDeviceInfForTagID(const otl_connect &dbconnect, const uint32_t &lstID, std::shared_ptr<DeviceInfoAndID> &ptResult);
    EErrCode SearchRepairInfForTagID(const otl_connect &dbconnect, const uint32_t &lstID, std::shared_ptr<RepairInfoAndID> &ptResult);
    EErrCode SearchRunInfForTagID(const otl_connect &dbconnect, const uint32_t &lstID, std::shared_ptr<DeviceRunInfoAndID> &ptResult);

    EErrCode AddSuplier(const otl_connect &dbconnect, const std::shared_ptr<SupplierInfo> &ptResult);
    EErrCode DeleteSupplier(const otl_connect &dbconnect, const string supplierName);
    EErrCode ModifySuplier(const otl_connect &dbconnect, const std::shared_ptr<SupplierInfo> &ptResult);
    EErrCode SearchSuplier(const otl_connect &dbconnect, const string supplierName, list<std::shared_ptr<SupplierInfo>> &ptResult);

    EErrCode AddMeterType(const otl_connect &dbconnect, const std::shared_ptr<MeterType> &ptResult);
    EErrCode DeleteMeterType(const otl_connect &dbconnect, const string meterTypeName);
    EErrCode ModifyMeterType(const otl_connect &dbconnect, const std::shared_ptr<MeterType> &ptResult);
    EErrCode SearchMeterType(const otl_connect &dbconnect, const string meterTypeName, list<std::shared_ptr<MeterType>> &ptResult);

    EErrCode AddEnergyCate(const otl_connect &dbconnect, const std::shared_ptr<EnergyCate> &ptResult);
    EErrCode DeleteEnergyCate(const otl_connect &dbconnect, const string name);
    EErrCode ModifyEnergyCate(const otl_connect &dbconnect, const std::shared_ptr<EnergyCate> &ptResult);
    EErrCode SearchEnergyCate(const otl_connect &dbconnect, const string name, list<std::shared_ptr<EnergyCate>> &ptResult);

    EErrCode AddDevFaultCate(const otl_connect &dbconnect, const std::shared_ptr<DevFaultCate> &ptResult);
    EErrCode DeleteDevFaultCate(const otl_connect &dbconnect, const string name);
    EErrCode ModifyDevFaultCate(const otl_connect &dbconnect, const std::shared_ptr<DevFaultCate> &ptResult);
    EErrCode SearchDevFaultCate(const otl_connect &dbconnect, const string name, list<std::shared_ptr<DevFaultCate>> &ptResult); 

    EErrCode AddDeviceInfo(const otl_connect &dbconnect, const std::shared_ptr<DeviceInfo> &ptResult);
    EErrCode DeleteDeviceInfo(const otl_connect &dbconnect, const string name);
    EErrCode ModifyDeviceInfo(const otl_connect &dbconnect, const std::shared_ptr<DeviceInfo> &ptResult);
    EErrCode SearchDeviceInfo(const otl_connect &dbconnect, const string name, list<std::shared_ptr<DeviceInfo>> &ptResult);

    EErrCode AddMeterInfo(const otl_connect &dbconnect, const std::shared_ptr<MeterInfo> &ptResult);
    EErrCode DeleteMeterInfo(const otl_connect &dbconnect, const string name);
    EErrCode ModifyMeterInfo(const otl_connect &dbconnect, const std::shared_ptr<MeterInfo> &ptResult);
    EErrCode SearchMeterInfo(const otl_connect &dbconnect, const string name, list<std::shared_ptr<MeterInfo>> &ptResult);

    EErrCode AddDeviceParameter(const otl_connect &dbconnect, const std::shared_ptr<DeviceParameter> &ptResult);
    EErrCode DeleteDeviceParameter(const otl_connect &dbconnect, const std::shared_ptr<DeviceParameter> &ptResult);
    EErrCode SearchDeviceParameter(const otl_connect &dbconnect, const string name, std::shared_ptr<DeviceParameter> &ptResult);

    EErrCode AddMeterParameter(const otl_connect &dbconnect, const std::shared_ptr<MeterParameter> &ptResult);
    EErrCode DeleteMeterParameter(const otl_connect &dbconnect, const std::shared_ptr<MeterParameter> &ptResult);
    EErrCode SearchMeterParameter(const otl_connect &dbconnect, const string name, std::shared_ptr<MeterParameter> &ptResult);
    
    EErrCode AddDeviceRepair(const otl_connect &dbconnect, const std::shared_ptr<RepairInfo> &ptResult, uint32_t & ulRecordingID);
    EErrCode DeleteDeviceRepair(const otl_connect &dbconnect, const int name);
    EErrCode ModifyDeviceRepair(const otl_connect &dbconnect, const std::shared_ptr<RepairInfo> &ptResult);
    EErrCode SearchDeviceRepair(const otl_connect &dbconnect, const int name, list<shared_ptr<RepairInfo>> &ptResult);
    EErrCode SearchDeviceRepair(const otl_connect &dbconnect, const string name, list<shared_ptr<RepairInfo>> &ptResult);

    EErrCode AddDeviceRun(const otl_connect &dbconnect, const std::shared_ptr<DeviceRunInfo> &ptResult, uint32_t & ulRecordingID);
    EErrCode DeleteDeviceRun(const otl_connect &dbconnect, const int name);
    EErrCode ModifyDeviceRun(const otl_connect &dbconnect, const std::shared_ptr<DeviceRunInfo> &ptResult);
    EErrCode SearchDeviceRun(const otl_connect &dbconnect, const int name, list<shared_ptr<DeviceRunInfo>> &ptResult);
    EErrCode SearchDeviceRun(const otl_connect &dbconnect, const string name, list<shared_ptr<DeviceRunInfo>> &ptResult);

    EErrCode SearchSubSysVariableInfo(const otl_connect &dbconnect, shared_ptr<SubSysInfo> &ptResult);
    EErrCode SearchDeviceVariableInfo(const otl_connect &dbconnect, const string name, shared_ptr<DeviceVarInfo> &ptResult);
    EErrCode SearchTagVariableInfo(const otl_connect &dbconnect, const string name, shared_ptr<TagVarInfo> &ptResult);
 
	//总览
	EErrCode SearchOverViewInfo(const otl_connect &dbconnect, shared_ptr<OverViewInfo> &ptResult);

	EErrCode SearchTagVarWithoutPar(const otl_connect &dbconnect, const string name, shared_ptr<TagVarInfo> &ptResult);
    
    EErrCode SearchMeterValueInfo(const otl_connect &dbconnect, const shared_ptr<ValueRequestInfo>  ptRequestInfo, shared_ptr<ResponseMeterValueInfo> &ptResult);
    EErrCode SearchDeviceValueInfo(const otl_connect &dbconnect, const shared_ptr<ValueRequestInfo>  ptRequestInfo, shared_ptr<DeviceValueInfo> &ptResult);
    EErrCode SearchVariableValueInfo(const otl_connect &dbconnect, const shared_ptr<ValueRequestInfo>  ptRequestInfo, shared_ptr<DeviceValueInfo> &ptResult);

    EErrCode SearchHistoryEventExInfo(const otl_connect &dbconnect, const shared_ptr<HistoryEventRequestInfo>  ptRequestInfo, shared_ptr<HistoryEventInfo> &ptResult);
    EErrCode SearchHistoryAlarmExInfo(const otl_connect &dbconnect, const shared_ptr<HistoryAlarmRequestInfo>  ptRequestInfo, shared_ptr<HistoryAlarmInfo> &ptResult);
	EErrCode SearchSystemAlarmCountInfo(const otl_connect &dbconnect, shared_ptr<Subsystem_Alarm> &ptResult);
private:
    bool SearchDeviceFromID(const otl_connect &dbconnect, const uint32_t &lstID, string &devResult);
    bool NameExists(const otl_connect &dbconnect, const string tableName, const string keyName, const string &keyValue);
    bool IdExists(const otl_connect &dbconnect, const string tableName, const string keyIdName, const int &keyIdValue = 0);
    otl_datetime TransformTimeString(const string &strTime);
    string TransformTimeOtlTime(const otl_datetime &otlTime);
    string TransformTimeUnint64(uint64_t ulltime);

    otl_datetime m_otlTime;
};

#endif // !_BMS_INFO_MANAGER_H_
