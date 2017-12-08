//------------------------------------------------------------------------------
// 创建日期：2017-09-06 13:09:39
// 文件描述：告警辅助相关包的编解码
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "BMSInfoPackage.h"

using namespace std;

//------------------------------------------------------------------------------
CBMSInfoPackage::CBMSInfoPackage()
{
}

//------------------------------------------------------------------------------
CBMSInfoPackage::~CBMSInfoPackage()
{
}

bool CBMSInfoPackage::ParseSearchID(const std::string &strRecv, std::string &strSID, uint32_t &lstInfo)
{
    if (strRecv.empty()) return false;
    //strRecv = R"({ "Command":104, "ID" : {"DeviceID":1, "SubsystemID" : 1, "TagID" : 1}, "Session_value" : "6CDFAE0C" })";
    Json::ArrayIndex  jIndex = 0;
    Json::Reader  jReader;
    Json::Value   jRoot;

    // 解析数据
    if (!jReader.parse(strRecv, jRoot)) return false;
    // 解析包头
    if (!OnParseHeader(jRoot, strSID)) return false;
    // 获取一个ID项

    uint32_t  tmp = 0;

    // 获取ID数据
    if (!OnParseGlobalID(jRoot[JSON_KEY_ID], lstInfo)) return false;
    return true;
}

//------------------------------------------------------------------------------
bool CBMSInfoPackage::BuildSearchSupplierForID(const std::string &strSID, EErrCode eEC, const std::shared_ptr<SupplierInfoAndID> &lstInfo, std::string &strSend)
{
    if (strSID.empty() || !lstInfo) return false;

    Json::FastWriter  jWriter;
    Json::Value       jRoot;
    Json::Value       jArrData;
    Json::Value       jValue;

    // 构建包头
    if (!OnBuildHeader(strSID, ECommand::CMD_SEARCH_SUPPLIERINF_FORTAGID, eEC, jRoot)) return false;

    // 构建ID组
    if (!OnBuildGlobalID(lstInfo->ulGID, jValue[JSON_KEY_ID])) return false;

    // 判断sSupplierName是否有效,添加查询结果
    if (!lstInfo->supplierInfo.sSupplierName.empty())
    {
        jValue[JSON_KEY_SUPPLIERCODE] = lstInfo->supplierInfo.sSupplierCode;
        jValue[JSON_KEY_SUPPLIERNAME] = lstInfo->supplierInfo.sSupplierName;
        jValue[JSON_KEY_CONPERSON] = lstInfo->supplierInfo.sConPerson;
        jValue[JSON_KEY_CONDEPARTMENT] = lstInfo->supplierInfo.sConDepartment;
        jValue[JSON_KEY_CONTELEPHONE] = lstInfo->supplierInfo.sConTelephone;
        jValue[JSON_KEY_FAX] = lstInfo->supplierInfo.sFax;
        jValue[JSON_KEY_ADDRESS] = lstInfo->supplierInfo.sAddress;
        jValue[JSON_KEY_ZIPCODE] = lstInfo->supplierInfo.sZipCode;
        jValue[JSON_KEY_E_MAIL] = lstInfo->supplierInfo.sE_Mail;
        jValue[JSON_KEY_URL] = lstInfo->supplierInfo.sURL;
        jValue[JSON_KEY_REMARKS] = lstInfo->supplierInfo.sRemarks;
    }
    jArrData.append(jValue);

    // 添加Data组
    jRoot[JSON_KEY_DATA] = jArrData;

    // 转换为字符串
    strSend = jWriter.write(jRoot);
    return true;
}

//------------------------------------------------------------------------------
bool CBMSInfoPackage::BuildSearchDeviceInfForID(const std::string &strSID, EErrCode eEC, const std::shared_ptr<DeviceInfoAndID> &lstInfo, std::string &strSend)
{
    if (strSID.empty() || !lstInfo) return false;

    Json::FastWriter  jWriter;
    Json::Value       jRoot;
    Json::Value       jArrData;
    Json::Value       jValue;

    // 构建包头
    if (!OnBuildHeader(strSID, ECommand::CMD_SEARCH_DEVICEINF_FORTAGID, eEC, jRoot)) return false;

    // 构建ID组
    if (!OnBuildGlobalID(lstInfo->ulGID, jValue[JSON_KEY_ID])) return false;

    // 判断sSupplierName是否有效,添加查询结果
    if (!lstInfo->deviceInfo.DeviceName.empty())
    {
        jValue[JSON_KEY_DEVICENUM] = lstInfo->deviceInfo.DeviceNum;
        jValue[JSON_KEY_DEVICENAME] = lstInfo->deviceInfo.DeviceName;
        jValue[JSON_KEY_INSTALLLOCATION] = lstInfo->deviceInfo.InstallLocation;
        jValue[JSON_KEY_USEDATE] = lstInfo->deviceInfo.UseDate;
        jValue[JSON_KEY_USAGEDEPART] = lstInfo->deviceInfo.UsageDepart;
        jValue[JSON_KEY_DEVICESTATUS] = lstInfo->deviceInfo.DeviceStatus;
        jValue[JSON_KEY_PRODUCTIONDATE] = lstInfo->deviceInfo.ProductionDate;
        jValue[JSON_KEY_DEVICESUPPLIER] = lstInfo->deviceInfo.DeviceSupplier;
        jValue[JSON_KEY_ALARMUPLIMIT] = lstInfo->deviceInfo.AlarmUpLimit;
        jValue[JSON_KEY_ALARMDOWNLIMIT] = lstInfo->deviceInfo.AlarmDownLimit;
        jValue[JSON_KEY_RUNCTRLUPLIMIT] = lstInfo->deviceInfo.RunCtrlUpLimit;
        jValue[JSON_KEY_RUNCTRLDOWNLIMIT] = lstInfo->deviceInfo.RunCtrlDownLimit;
        
    }
    jArrData.append(jValue);

    // 添加Data组
    jRoot[JSON_KEY_DATA] = jArrData;

    // 转换为字符串
    strSend = jWriter.write(jRoot);
    return true;
}

//------------------------------------------------------------------------------
bool CBMSInfoPackage::BuildSearchRepairInfForID(const std::string &strSID, EErrCode eEC, const std::shared_ptr<RepairInfoAndID> &lstInfo, std::string &strSend)
{
    if (strSID.empty() || !lstInfo) return false;

    Json::FastWriter  jWriter;
    Json::Value       jRoot;
    Json::Value       jArrData;
    Json::Value       jValue;

    // 构建包头
    if (!OnBuildHeader(strSID, ECommand::CMD_SEARCH_REPAIRINF_FORTAGID, eEC, jRoot)) return false;

    // 构建ID组
    if (!OnBuildGlobalID(lstInfo->ulGID, jValue[JSON_KEY_ID])) return false;

    //if (!lstInfo->repairInfo.recordingID.empty())
    {
        jValue[JSON_KEY_RECORDINGID] = lstInfo->repairInfo.recordingID;
        jValue[JSON_KEY_DEVICENAME] = lstInfo->repairInfo.DeviceName;
        jValue[JSON_KEY_REPAIRCONTENT] = lstInfo->repairInfo.RepairContent;
        jValue[JSON_KEY_REPAIRTIME] = lstInfo->repairInfo.RepairTime;
        jValue[JSON_KEY_FAULTREASON] = lstInfo->repairInfo.FaultReason;
        jValue[JSON_KEY_REPAIRRESULTS] = lstInfo->repairInfo.RepairResults;
        jValue[JSON_KEY_PEPAIRMANAGER] = lstInfo->repairInfo.RepairManager;
        jValue[JSON_KEY_REPAIRUNIT] = lstInfo->repairInfo.RepairUnit;
        jValue[JSON_KEY_REMARKS] = lstInfo->repairInfo.Remarks;

    }
    jArrData.append(jValue);

    // 添加Data组
    jRoot[JSON_KEY_DATA] = jArrData;

    // 转换为字符串
    strSend = jWriter.write(jRoot);
    return true;
}

//------------------------------------------------------------------------------
bool CBMSInfoPackage::BuildSearchRunInfForID(const std::string &strSID, EErrCode eEC, const std::shared_ptr<DeviceRunInfoAndID> &lstInfo, std::string &strSend)
{
    if (strSID.empty() || !lstInfo) return false;

    Json::FastWriter  jWriter;
    Json::Value       jRoot;
    Json::Value       jArrData;
    Json::Value       jValue;

    // 构建包头
    if (!OnBuildHeader(strSID, ECommand::CMD_SEARCH_RUNINF_FORTAGID, eEC, jRoot)) return false;

    // 构建ID组
    if (!OnBuildGlobalID(lstInfo->ulGID, jValue[JSON_KEY_ID])) return false;

    // 判断sSupplierName是否有效,添加查询结果
    //if (!lstInfo->deviceRunInfo.recordingID.empty())
    {
        jValue[JSON_KEY_RECORDINGID] = lstInfo->deviceRunInfo.recordingID;
        jValue[JSON_KEY_DEVICENAME] = lstInfo->deviceRunInfo.DeviceName;
        jValue[JSON_KEY_STARTTIME_] = lstInfo->deviceRunInfo.StartTime;
        jValue[JSON_KEY_ENDTIME_] = lstInfo->deviceRunInfo.EndTime;
        jValue[JSON_KEY_OPERATION] = lstInfo->deviceRunInfo.Operation;
        jValue[JSON_KEY_RECORDER] = lstInfo->deviceRunInfo.Recorder;
        jValue[JSON_KEY_PEPAIRMANAGER] = lstInfo->deviceRunInfo.Remarks;

    }
    jArrData.append(jValue);

    // 添加Data组
    jRoot[JSON_KEY_DATA] = jArrData;

    // 转换为字符串
    strSend = jWriter.write(jRoot);
    return true;
}
//------------------------------------------------------------------------------
//信息管理供应商
bool CBMSInfoPackage::ParseAddOrModSupplier(const std::string &strRecv, std::string &strSID, shared_ptr<SupplierInfo> &lstInfo)
{
    if (strRecv.empty()) return false;

    Json::ArrayIndex  jIndex = 0;
    Json::Reader  jReader;
    Json::Value   jRoot;
    Json::Value   jArrData;

    // 解析数据
    if (!jReader.parse(strRecv, jRoot)) return false;
    // 解析包头
    if (!OnParseHeader(jRoot, strSID)) return false;

    // 获取Data组
    jArrData = jRoot[JSON_KEY_DATA];
    if (!jArrData.isArray() || !jArrData.isValidIndex(0)) return false;

    if (!jArrData[0][JSON_KEY_SUPPLIERCODE].isString()) return false;
    lstInfo->sSupplierCode = jArrData[0][JSON_KEY_SUPPLIERCODE].asString();
    if (!jArrData[0][JSON_KEY_SUPPLIERNAME].isString()) return false;
    lstInfo->sSupplierName = jArrData[0][JSON_KEY_SUPPLIERNAME].asString();
    if (!jArrData[0][JSON_KEY_CONPERSON].isString()) return false;
    lstInfo->sConPerson = jArrData[0][JSON_KEY_CONPERSON].asString();
    if (!jArrData[0][JSON_KEY_CONDEPARTMENT].isString()) return false;
    lstInfo->sConDepartment = jArrData[0][JSON_KEY_CONDEPARTMENT].asString();
    if (!jArrData[0][JSON_KEY_CONTELEPHONE].isString()) return false;
    lstInfo->sConTelephone = jArrData[0][JSON_KEY_CONTELEPHONE].asString();
    if (!jArrData[0][JSON_KEY_FAX].isString()) return false;
    lstInfo->sFax = jArrData[0][JSON_KEY_FAX].asString();
    if (!jArrData[0][JSON_KEY_ADDRESS].isString()) return false;
    lstInfo->sAddress = jArrData[0][JSON_KEY_ADDRESS].asString();
    if (!jArrData[0][JSON_KEY_ZIPCODE].isString()) return false;
    lstInfo->sZipCode = jArrData[0][JSON_KEY_ZIPCODE].asString();
    if (!jArrData[0][JSON_KEY_E_MAIL].isString()) return false;
    lstInfo->sE_Mail = jArrData[0][JSON_KEY_E_MAIL].asString();
    if (!jArrData[0][JSON_KEY_URL].isString()) return false;
    lstInfo->sURL = jArrData[0][JSON_KEY_URL].asString();
    if (!jArrData[0][JSON_KEY_REMARKS].isString()) return false;
    lstInfo->sRemarks = jArrData[0][JSON_KEY_REMARKS].asString();

    return true;
}

bool CBMSInfoPackage::ParseDelOrSelSupplier(const std::string &strRecv, std::string &strSID, string &lstInfo)
{
    if (strRecv.empty()) return false;

    Json::ArrayIndex  jIndex = 0;
    Json::Reader  jReader;
    Json::Value   jRoot;
    Json::Value   jArrData;

    // 解析数据
    if (!jReader.parse(strRecv, jRoot)) return false;
    // 解析包头
    if (!OnParseHeader(jRoot, strSID)) return false;

    // 获取Data组
    jArrData = jRoot[JSON_KEY_DATA];
    if (!jArrData.isArray() || !jArrData.isValidIndex(0)) return false;

    if (!jArrData[0][JSON_KEY_SUPPLIERNAME].isString()) return false;
    lstInfo = jArrData[0][JSON_KEY_SUPPLIERNAME].asString();
    return true;
}

bool CBMSInfoPackage::ParseAddOrModMeterType(const std::string &strRecv, std::string &strSID, shared_ptr<MeterType> &lstInfo)
{
    if (strRecv.empty()) return false;

    Json::ArrayIndex  jIndex = 0;
    Json::Reader  jReader;
    Json::Value   jRoot;
    Json::Value   jArrData;

    // 解析数据
    if (!jReader.parse(strRecv, jRoot)) return false;
    // 解析包头
    if (!OnParseHeader(jRoot, strSID)) return false;

    // 获取Data组
    jArrData = jRoot[JSON_KEY_DATA];
    if (!jArrData.isArray() || !jArrData.isValidIndex(0)) return false;

    if (!jArrData[0][JSON_KEY_METERTYPE].isString()) return false;
    lstInfo->strMeterType = jArrData[0][JSON_KEY_METERTYPE].asString();
    if (!jArrData[0][JSON_KEY_TYPEDES].isString()) return false;
    lstInfo->TypeDes = jArrData[0][JSON_KEY_TYPEDES].asString();

    return true;
}

bool CBMSInfoPackage::ParseDelOrSelMeterType(const std::string &strRecv, std::string &strSID, string &lstInfo)
{
    if (strRecv.empty()) return false;

    Json::ArrayIndex  jIndex = 0;
    Json::Reader  jReader;
    Json::Value   jRoot;
    Json::Value   jArrData;

    // 解析数据
    if (!jReader.parse(strRecv, jRoot)) return false;
    // 解析包头
    if (!OnParseHeader(jRoot, strSID)) return false;

    // 获取Data组
    jArrData = jRoot[JSON_KEY_DATA];
    if (!jArrData.isArray() || !jArrData.isValidIndex(0)) return false;

    if (!jArrData[0][JSON_KEY_METERTYPE].isString()) return false;
    lstInfo = jArrData[0][JSON_KEY_METERTYPE].asString();
    return true;
}

bool CBMSInfoPackage::ParseAddOrModEnergyCate(const std::string &strRecv, std::string &strSID, shared_ptr<EnergyCate> &lstInfo)
{
    if (strRecv.empty()) return false;

    Json::ArrayIndex  jIndex = 0;
    Json::Reader  jReader;
    Json::Value   jRoot;
    Json::Value   jArrData;

    // 解析数据
    if (!jReader.parse(strRecv, jRoot)) return false;
    // 解析包头
    if (!OnParseHeader(jRoot, strSID)) return false;

    // 获取Data组
    jArrData = jRoot[JSON_KEY_DATA];
    if (!jArrData.isArray() || !jArrData.isValidIndex(0)) return false;

    if (!jArrData[0][JSON_KEY_ENERGYCATE].isString()) return false;
    lstInfo->strEnergyCate = jArrData[0][JSON_KEY_ENERGYCATE].asString();
    if (!jArrData[0][JSON_KEY_TYPEDES].isString()) return false;
    lstInfo->TypeDes = jArrData[0][JSON_KEY_TYPEDES].asString();

    return true;
}

bool CBMSInfoPackage::ParseDelOrSelEnergyCate(const std::string &strRecv, std::string &strSID, string &lstInfo)
{
    if (strRecv.empty()) return false;

    Json::ArrayIndex  jIndex = 0;
    Json::Reader  jReader;
    Json::Value   jRoot;
    Json::Value   jArrData;

    // 解析数据
    if (!jReader.parse(strRecv, jRoot)) return false;
    // 解析包头
    if (!OnParseHeader(jRoot, strSID)) return false;

    // 获取Data组
    jArrData = jRoot[JSON_KEY_DATA];
    if (!jArrData.isArray() || !jArrData.isValidIndex(0)) return false;

    if (!jArrData[0][JSON_KEY_ENERGYCATE].isString()) return false;
    lstInfo = jArrData[0][JSON_KEY_ENERGYCATE].asString();
    return true;
}

bool CBMSInfoPackage::ParseAddOrModDevFaultCate(const std::string &strRecv, std::string &strSID, shared_ptr<DevFaultCate> &lstInfo)
{
    if (strRecv.empty()) return false;

    Json::ArrayIndex  jIndex = 0;
    Json::Reader  jReader;
    Json::Value   jRoot;
    Json::Value   jArrData;

    // 解析数据
    if (!jReader.parse(strRecv, jRoot)) return false;
    // 解析包头
    if (!OnParseHeader(jRoot, strSID)) return false;

    // 获取Data组
    jArrData = jRoot[JSON_KEY_DATA];
    if (!jArrData.isArray() || !jArrData.isValidIndex(0)) return false;
    if (!jArrData[0][JSON_KEY_FAULTCATE].isString()) return false;
    lstInfo->FaultCate = jArrData[0][JSON_KEY_FAULTCATE].asString();
    if (!jArrData[0][JSON_KEY_CATEGORYDES].isString()) return false;
    lstInfo->CategoryDes = jArrData[0][JSON_KEY_CATEGORYDES].asString();
    if (!jArrData[0][JSON_KEY_FAULTFEATURE].isString()) return false;
    lstInfo->FaultFeature = jArrData[0][JSON_KEY_FAULTFEATURE].asString();

    return true;
}

bool CBMSInfoPackage::ParseDelOrSelDevFaultCate(const std::string &strRecv, std::string &strSID, string &lstInfo)
{
    if (strRecv.empty()) return false;

    Json::ArrayIndex  jIndex = 0;
    Json::Reader  jReader;
    Json::Value   jRoot;
    Json::Value   jArrData;

    // 解析数据
    if (!jReader.parse(strRecv, jRoot)) return false;
    // 解析包头
    if (!OnParseHeader(jRoot, strSID)) return false;

    // 获取Data组
    jArrData = jRoot[JSON_KEY_DATA];
    if (!jArrData.isArray() || !jArrData.isValidIndex(0)) return false;

    if (!jArrData[0][JSON_KEY_FAULTCATE].isString()) return false;
    lstInfo = jArrData[0][JSON_KEY_FAULTCATE].asString();
    return true;
}

bool CBMSInfoPackage::ParseAddOrModDeviceInfo(const std::string &strRecv, std::string &strSID, shared_ptr<DeviceInfo> &lstInfo)
{
    if (strRecv.empty()) return false;

    Json::ArrayIndex  jIndex = 0;
    Json::Reader  jReader;
    Json::Value   jRoot;
    Json::Value   jArrData;

    // 解析数据
    if (!jReader.parse(strRecv, jRoot)) return false;
    // 解析包头
    if (!OnParseHeader(jRoot, strSID)) return false;

    // 获取Data组
    jArrData = jRoot[JSON_KEY_DATA];
    if (!jArrData.isArray() || !jArrData.isValidIndex(0)) return false;
    if (!jArrData[0][JSON_KEY_DEVICENUM].isString()) return false;
    lstInfo->DeviceNum = jArrData[0][JSON_KEY_DEVICENUM].asString();
    if (!jArrData[0][JSON_KEY_DEVICENAME].isString()) return false;
    lstInfo->DeviceName = jArrData[0][JSON_KEY_DEVICENAME].asString();
    if (!jArrData[0][JSON_KEY_INSTALLLOCATION].isString()) return false;
    lstInfo->InstallLocation = jArrData[0][JSON_KEY_INSTALLLOCATION].asString();
    if (!jArrData[0][JSON_KEY_USEDATE].isString()) return false;
    lstInfo->UseDate = jArrData[0][JSON_KEY_USEDATE].asString();
    if (!jArrData[0][JSON_KEY_USAGEDEPART].isString()) return false;
    lstInfo->UsageDepart = jArrData[0][JSON_KEY_USAGEDEPART].asString();
    if (!jArrData[0][JSON_KEY_DEVICESTATUS].isString()) return false;
    lstInfo->DeviceStatus = jArrData[0][JSON_KEY_DEVICESTATUS].asString();
    if (!jArrData[0][JSON_KEY_PRODUCTIONDATE].isString()) return false;
    lstInfo->ProductionDate = jArrData[0][JSON_KEY_PRODUCTIONDATE].asString();
    if (!jArrData[0][JSON_KEY_DEVICESUPPLIER].isString()) return false;
    lstInfo->DeviceSupplier = jArrData[0][JSON_KEY_DEVICESUPPLIER].asString();
    if (jArrData[0][JSON_KEY_ALARMUPLIMIT].isNull())
    {
        lstInfo->AlarmUpLimit = 0;
    }
    else
    {
        if (!jArrData[0][JSON_KEY_ALARMUPLIMIT].isDouble()) return false;
        lstInfo->AlarmUpLimit = jArrData[0][JSON_KEY_ALARMUPLIMIT].asDouble();
    }
    if (jArrData[0][JSON_KEY_ALARMUPLIMIT].isNull())
    {
        lstInfo->AlarmUpLimit = 0;
    }
    else
    {
        if (!jArrData[0][JSON_KEY_ALARMDOWNLIMIT].isDouble()) return false;
        lstInfo->AlarmDownLimit = jArrData[0][JSON_KEY_ALARMDOWNLIMIT].asDouble();
    }
    if (jArrData[0][JSON_KEY_ALARMUPLIMIT].isNull())
    {
        lstInfo->AlarmUpLimit = 0;
    }
    else
    {
        if (!jArrData[0][JSON_KEY_RUNCTRLUPLIMIT].isDouble()) return false;
        lstInfo->RunCtrlUpLimit = jArrData[0][JSON_KEY_RUNCTRLUPLIMIT].asDouble();
    }
    if (jArrData[0][JSON_KEY_ALARMUPLIMIT].isNull())
    {
        lstInfo->AlarmUpLimit = 0;
    }
    else
    {
        if (!jArrData[0][JSON_KEY_RUNCTRLDOWNLIMIT].isDouble()) return false;
        lstInfo->RunCtrlDownLimit = jArrData[0][JSON_KEY_RUNCTRLDOWNLIMIT].asDouble();
    }
    
    return true;
}

bool CBMSInfoPackage::ParseDelOrSelDeviceInfo(const std::string &strRecv, std::string &strSID, string &lstInfo)
{
    if (strRecv.empty()) return false;

    Json::ArrayIndex  jIndex = 0;
    Json::Reader  jReader;
    Json::Value   jRoot;
    Json::Value   jArrData;

    // 解析数据
    if (!jReader.parse(strRecv, jRoot)) return false;
    // 解析包头
    if (!OnParseHeader(jRoot, strSID)) return false;

    // 获取Data组
    jArrData = jRoot[JSON_KEY_DATA];
    if (!jArrData.isArray() || !jArrData.isValidIndex(0)) return false;

    if (!jArrData[0][JSON_KEY_DEVICENAME].isString()) return false;
    lstInfo = jArrData[0][JSON_KEY_DEVICENAME].asString();
    return true;
}


bool CBMSInfoPackage::ParseAddOrModMeterInfo(const std::string &strRecv, std::string &strSID, shared_ptr<MeterInfo> &lstInfo)
{
    if (strRecv.empty()) return false;

    Json::ArrayIndex  jIndex = 0;
    Json::Reader  jReader;
    Json::Value   jRoot;
    Json::Value   jArrData;

    // 解析数据
    if (!jReader.parse(strRecv, jRoot)) return false;
    // 解析包头
    if (!OnParseHeader(jRoot, strSID)) return false;

    // 获取Data组
    jArrData = jRoot[JSON_KEY_DATA];
    if (!jArrData.isArray() || !jArrData.isValidIndex(0)) return false;
    if (!jArrData[0][JSON_KEY_METERNUM].isString()) return false;
    lstInfo->MeterNumber = jArrData[0][JSON_KEY_METERNUM].asString();
    if (!jArrData[0][JSON_KEY_METERTYPE].isString()) return false;
    lstInfo->strMeterType = jArrData[0][JSON_KEY_METERTYPE].asString();
    if (!jArrData[0][JSON_KEY_ENERGYCATE].isString()) return false;
    lstInfo->strEnergyCate = jArrData[0][JSON_KEY_ENERGYCATE].asString();
    if (!jArrData[0][JSON_KEY_METERMODEL].isString()) return false;
    lstInfo->MeterModel = jArrData[0][JSON_KEY_METERMODEL].asString();
    if (!jArrData[0][JSON_KEY_USEUNITS].isString()) return false;
    lstInfo->UseUnits = jArrData[0][JSON_KEY_USEUNITS].asString();
    if (!jArrData[0][JSON_KEY_RATESTANDARD].isDouble()) return false;
    lstInfo->RateStandard = jArrData[0][JSON_KEY_RATESTANDARD].asDouble();
    if (!jArrData[0][JSON_KEY_STANDARDUNIT].isString()) return false;
    lstInfo->StandardUnit = jArrData[0][JSON_KEY_STANDARDUNIT].asString();
    if (!jArrData[0][JSON_KEY_INSTALLDATE].isString()) return false;
    lstInfo->InstallDate = jArrData[0][JSON_KEY_INSTALLDATE].asString();
    if (!jArrData[0][JSON_KEY_INSTALLLOCATION].isString()) return false;
    lstInfo->InstallLocation = jArrData[0][JSON_KEY_INSTALLLOCATION].asString();
    if (!jArrData[0][JSON_KEY_PRODUCTIONFACTORY].isString()) return false;
    lstInfo->ProductionFactory = jArrData[0][JSON_KEY_PRODUCTIONFACTORY].asString();
    if (!jArrData[0][JSON_KEY_METERMAGNIFICATION].isDouble()) return false;
    lstInfo->MeterMagnification = (short)(jArrData[0][JSON_KEY_METERMAGNIFICATION].asDouble());
    if (!jArrData[0][JSON_KEY_TRANMETERBASE].isDouble()) return false;
    lstInfo->TranMeterBase = jArrData[0][JSON_KEY_TRANMETERBASE].asDouble();
    if (!jArrData[0][JSON_KEY_METERINITVALUE].isDouble()) return false;
    lstInfo->MeterInitValue = jArrData[0][JSON_KEY_METERINITVALUE].asDouble();
    if (!jArrData[0][JSON_KEY_TRANMETERVALUE].isDouble()) return false;
    lstInfo->TranMeterValue = jArrData[0][JSON_KEY_TRANMETERVALUE].asDouble();
    if (!jArrData[0][JSON_KEY_REMARKS].isString()) return false;
    lstInfo->Remarks = jArrData[0][JSON_KEY_REMARKS].asString();
    //if (!jArrData[0][JSON_KEY_OLDMETERNUMBER].isString()) return false;
    //lstInfo->OldMeterNumber = jArrData[0][JSON_KEY_OLDMETERNUMBER].asString();

    return true;
}

bool CBMSInfoPackage::ParseDelOrSelMeterInfo(const std::string &strRecv, std::string &strSID, string &lstInfo)
{
    if (strRecv.empty()) return false;

    Json::ArrayIndex  jIndex = 0;
    Json::Reader  jReader;
    Json::Value   jRoot;
    Json::Value   jArrData;

    // 解析数据
    if (!jReader.parse(strRecv, jRoot)) return false;
    // 解析包头
    if (!OnParseHeader(jRoot, strSID)) return false;

    // 获取Data组
    jArrData = jRoot[JSON_KEY_DATA];
    if (!jArrData.isArray() || !jArrData.isValidIndex(0)) return false;

    if (!jArrData[0][JSON_KEY_METERNUM].isString()) return false;
    lstInfo = jArrData[0][JSON_KEY_METERNUM].asString();
    return true;
}

bool CBMSInfoPackage::ParseAddDeviceParameter(const std::string &strRecv, std::string &strSID, const shared_ptr<DeviceParameter>&lstInfo)
{
    if (strRecv.empty()) return false;

    Json::ArrayIndex  jIndex = 0;
    Json::Reader  jReader;
    Json::Value   jRoot;
    Json::Value   jArrData;

    // 解析数据
    if (!jReader.parse(strRecv, jRoot)) return false;
    // 解析包头
    if (!OnParseHeader(jRoot, strSID)) return false;

    if (!jRoot[JSON_KEY_DEVICE_NAME].isString()) return false;
    lstInfo->DeviceName = jRoot[JSON_KEY_DEVICE_NAME].asString();
    // 获取Data组
    jArrData = jRoot[JSON_KEY_DATA];
    if (!jArrData.isArray()) return false;

    jIndex = jArrData.size();
    // 获取每个的内容
    for (Json::ArrayIndex i = 0; i < jIndex; i++)
    {
        if (!jArrData.isValidIndex(i))
        {
            break;
        }
        uint32_t uID;
        string tempName;
        if (!OnParseGlobalID(jArrData[i][JSON_KEY_ID], uID)) return false;
        if (!jArrData[i][JSON_KEY_TAG_NAME].isString()) return false;
        tempName = jArrData[i][JSON_KEY_TAG_NAME].asString();
        lstInfo->lstGID.insert(pair<uint32_t, string>(uID, tempName));
    }

    return true;
}

bool CBMSInfoPackage::ParseDelDeviceParameter(const std::string &strRecv, std::string &strSID, shared_ptr<DeviceParameter>&lstInfo)
{
    if (strRecv.empty()) return false;

    Json::ArrayIndex  jIndex = 0;
    Json::Reader  jReader;
    Json::Value   jRoot;
    Json::Value   jArrData;

    // 解析数据
    if (!jReader.parse(strRecv, jRoot)) return false;
    // 解析包头
    if (!OnParseHeader(jRoot, strSID)) return false;

    if (!jRoot[JSON_KEY_DEVICE_NAME].isString()) return false;
    lstInfo->DeviceName = jRoot[JSON_KEY_DEVICE_NAME].asString();
    // 获取Data组
    jArrData = jRoot[JSON_KEY_DATA];
    if (!jArrData.isArray()) return false;

    jIndex = jArrData.size();
    // 获取每个的内容
    for (Json::ArrayIndex i = 0; i < jIndex; i++)
    {
        if (!jArrData.isValidIndex(i))
        {
            break;
        }
        uint32_t uID;
        if (!OnParseGlobalID(jArrData[i][JSON_KEY_ID], uID)) return false;
        lstInfo->lstGID.insert(pair<uint32_t, string>(uID, ""));
    }
    return true;
}

bool CBMSInfoPackage::ParseSearchDeviceParameter(const std::string &strRecv, std::string &strSID, string &lstInfo)
{
    if (strRecv.empty()) return false;

    Json::ArrayIndex  jIndex = 0;
    Json::Reader  jReader;
    Json::Value   jRoot;
    Json::Value   jArrData;

    // 解析数据
    if (!jReader.parse(strRecv, jRoot)) return false;
    // 解析包头
    if (!OnParseHeader(jRoot, strSID)) return false;

    // 获取Data组
    jArrData = jRoot[JSON_KEY_DATA];
    if (!jArrData.isArray()) return false;

    if (!jArrData.isValidIndex(0))return false;
    if (!jArrData[0][JSON_KEY_DEVICE_NAME].isString()) return false;
    lstInfo = jArrData[0][JSON_KEY_DEVICE_NAME].asString();
    return true;
}

bool CBMSInfoPackage::ParseAddMeterParameter(const std::string &strRecv, std::string &strSID, const shared_ptr<MeterParameter>&lstInfo)
{
    if (strRecv.empty()) return false;

    Json::Reader  jReader;
    Json::Value   jRoot;

    // 解析数据
    if (!jReader.parse(strRecv, jRoot)) return false;
    // 解析包头
    if (!OnParseHeader(jRoot, strSID)) return false;

    if (!jRoot[JSON_KEY_METERNUM].isString()) return false;
    lstInfo->MeterNum = jRoot[JSON_KEY_METERNUM].asString();
    if (!OnParseGlobalID(jRoot[JSON_KEY_ID], lstInfo->ulGID)) return false;
    
    return true;
}

bool CBMSInfoPackage::ParseDelMeterParameter(const std::string &strRecv, std::string &strSID, shared_ptr<MeterParameter>&lstInfo)
{
    if (strRecv.empty()) return false;

    Json::Reader  jReader;
    Json::Value   jRoot;

    // 解析数据
    if (!jReader.parse(strRecv, jRoot)) return false;
    // 解析包头
    if (!OnParseHeader(jRoot, strSID)) return false;

    if (!jRoot[JSON_KEY_METERNUM].isString()) return false;
    lstInfo->MeterNum = jRoot[JSON_KEY_METERNUM].asString();
    if (!OnParseGlobalID(jRoot[JSON_KEY_ID], lstInfo->ulGID)) return false;

    return true;
}

bool CBMSInfoPackage::ParseSearchMeterParameter(const std::string &strRecv, std::string &strSID, string &lstInfo)
{
    if (strRecv.empty()) return false;

    Json::Reader  jReader;
    Json::Value   jRoot;
    Json::Value   jArrData;

    // 解析数据
    if (!jReader.parse(strRecv, jRoot)) return false;
    // 解析包头
    if (!OnParseHeader(jRoot, strSID)) return false;

    // 获取Data组
    jArrData = jRoot[JSON_KEY_DATA];
    if (!jArrData.isArray()) return false;

    if (!jArrData.isValidIndex(0))return false;
    if (jArrData[0][JSON_KEY_METERNUM].isString()) return false;
    lstInfo = jRoot[JSON_KEY_METERNUM].asString();
    return true;
}

bool CBMSInfoPackage::ParseAddOrModDeviceRepair(const std::string &strRecv, std::string &strSID, shared_ptr<RepairInfo> &lstInfo)
{
    if (strRecv.empty()) return false;

    Json::ArrayIndex  jIndex = 0;
    Json::Reader  jReader;
    Json::Value   jRoot;
    Json::Value   jArrData;

    // 解析数据
    if (!jReader.parse(strRecv, jRoot)) return false;
    // 解析包头
    if (!OnParseHeader(jRoot, strSID)) return false;

    // 获取Data组
    jArrData = jRoot[JSON_KEY_DATA];
    if (!jArrData.isArray() || !jArrData.isValidIndex(0)) return false;

    if (!jArrData[0][JSON_KEY_RECORDINGID].empty())
    {
        if (!jArrData[0][JSON_KEY_RECORDINGID].isInt()) return false;
        lstInfo->recordingID = jArrData[0][JSON_KEY_RECORDINGID].asInt();
    }
    if (!jArrData[0][JSON_KEY_DEVICENAME].isString()) return false;
    lstInfo->DeviceName = jArrData[0][JSON_KEY_DEVICENAME].asString();
    if (!jArrData[0][JSON_KEY_REPAIRCONTENT].isString()) return false;
    lstInfo->RepairContent = jArrData[0][JSON_KEY_REPAIRCONTENT].asString();
    if (!jArrData[0][JSON_KEY_REPAIRTIME].isString()) return false;
    lstInfo->RepairTime = jArrData[0][JSON_KEY_REPAIRTIME].asString();
    if (!jArrData[0][JSON_KEY_FAULTREASON].isString()) return false;
    lstInfo->FaultReason = jArrData[0][JSON_KEY_FAULTREASON].asString();
    if (!jArrData[0][JSON_KEY_REPAIRRESULTS].isString()) return false;
    lstInfo->RepairResults = jArrData[0][JSON_KEY_REPAIRRESULTS].asString();
    if (!jArrData[0][JSON_KEY_PEPAIRMANAGER].isString()) return false;
    lstInfo->RepairManager = jArrData[0][JSON_KEY_PEPAIRMANAGER].asString();
    if (!jArrData[0][JSON_KEY_REPAIRUNIT].isString()) return false;
    lstInfo->RepairUnit = jArrData[0][JSON_KEY_REPAIRUNIT].asString();
    if (!jArrData[0][JSON_KEY_REMARKS].isString()) return false;
    lstInfo->Remarks = jArrData[0][JSON_KEY_REMARKS].asString();

    return true;
}

bool CBMSInfoPackage::ParseDelOrSelDeviceRepair(const std::string &strRecv, std::string &strSID, int &lstInfo)
{
    if (strRecv.empty()) return false;

    Json::ArrayIndex  jIndex = 0;
    Json::Reader  jReader;
    Json::Value   jRoot;
    Json::Value   jArrData;

    // 解析数据
    if (!jReader.parse(strRecv, jRoot)) return false;
    // 解析包头
    if (!OnParseHeader(jRoot, strSID)) return false;

    // 获取Data组
    jArrData = jRoot[JSON_KEY_DATA];
    if (!jArrData.isArray() || !jArrData.isValidIndex(0)) return false;
    if (jArrData[0][JSON_KEY_RECORDINGID].isNull())
    {
        lstInfo = -1;
    }
    else
    {
        if (!jArrData[0][JSON_KEY_RECORDINGID].isInt()) return false;
        lstInfo = jArrData[0][JSON_KEY_RECORDINGID].asInt();
    }
    return true;
}

bool CBMSInfoPackage::ParseSearchDeviceRepair(const std::string &strRecv, std::string &strSID, string &lstInfo)
{
    if (strRecv.empty()) return false;

    Json::ArrayIndex  jIndex = 0;
    Json::Reader  jReader;
    Json::Value   jRoot;
    Json::Value   jArrData;

    // 解析数据
    if (!jReader.parse(strRecv, jRoot)) return false;
    // 解析包头
    if (!OnParseHeader(jRoot, strSID)) return false;

    // 获取Data组
    jArrData = jRoot[JSON_KEY_DATA];
    if (!jArrData.isArray() || !jArrData.isValidIndex(0)) return false;

    if (!jArrData[0][JSON_KEY_DEVICENAME].isString()) return false;
    lstInfo = jArrData[0][JSON_KEY_DEVICENAME].asString();
    return true;
}

bool CBMSInfoPackage::ParseAddOrModDeviceRun(const std::string &strRecv, std::string &strSID, shared_ptr<DeviceRunInfo> &lstInfo)
{
    if (strRecv.empty()) return false;

    Json::ArrayIndex  jIndex = 0;
    Json::Reader  jReader;
    Json::Value   jRoot;
    Json::Value   jArrData;

    // 解析数据
    if (!jReader.parse(strRecv, jRoot)) return false;
    // 解析包头
    if (!OnParseHeader(jRoot, strSID)) return false;

    // 获取Data组
    jArrData = jRoot[JSON_KEY_DATA];
    if (!jArrData.isArray() || !jArrData.isValidIndex(0)) return false;

    if (!jArrData[0][JSON_KEY_RECORDINGID].empty())
    {
        if (!jArrData[0][JSON_KEY_RECORDINGID].isInt()) return false;
        lstInfo->recordingID = jArrData[0][JSON_KEY_RECORDINGID].asInt();
    }
    if (!jArrData[0][JSON_KEY_DEVICENAME].isString()) return false;
    lstInfo->DeviceName = jArrData[0][JSON_KEY_DEVICENAME].asString();
    if (!jArrData[0][JSON_KEY_STARTTIME_].isString()) return false;
    lstInfo->StartTime = jArrData[0][JSON_KEY_STARTTIME_].asString();
    if (!jArrData[0][JSON_KEY_ENDTIME_].isString()) return false;
    lstInfo->EndTime = jArrData[0][JSON_KEY_ENDTIME_].asString();
    if (!jArrData[0][JSON_KEY_OPERATION].isString()) return false;
    lstInfo->Operation = jArrData[0][JSON_KEY_OPERATION].asString();
    if (!jArrData[0][JSON_KEY_RECORDER].isString()) return false;
    lstInfo->Recorder = jArrData[0][JSON_KEY_RECORDER].asString();
    if (!jArrData[0][JSON_KEY_REMARKS].isString()) return false;
    lstInfo->Remarks = jArrData[0][JSON_KEY_REMARKS].asString();

    return true;
}

bool CBMSInfoPackage::ParseDelOrSelDeviceRun(const std::string &strRecv, std::string &strSID, int &lstInfo)
{
    if (strRecv.empty()) return false;

    Json::ArrayIndex  jIndex = 0;
    Json::Reader  jReader;
    Json::Value   jRoot;
    Json::Value   jArrData;

    // 解析数据
    if (!jReader.parse(strRecv, jRoot)) return false;
    // 解析包头
    if (!OnParseHeader(jRoot, strSID)) return false;

    // 获取Data组
    jArrData = jRoot[JSON_KEY_DATA];
    if (!jArrData.isArray() || !jArrData.isValidIndex(0)) return false;

    if (jArrData[0][JSON_KEY_RECORDINGID].isNull())
    {
        lstInfo = -1;
    }
    else
    {
        if (!jArrData[0][JSON_KEY_RECORDINGID].isInt()) return false;
        lstInfo = jArrData[0][JSON_KEY_RECORDINGID].asInt();
    }
    return true;
}

bool CBMSInfoPackage::ParseSearchDeviceRun(const std::string &strRecv, std::string &strSID, string &lstInfo)
{
    if (strRecv.empty()) return false;

    Json::ArrayIndex  jIndex = 0;
    Json::Reader  jReader;
    Json::Value   jRoot;
    Json::Value   jArrData;

    // 解析数据
    if (!jReader.parse(strRecv, jRoot)) return false;
    // 解析包头
    if (!OnParseHeader(jRoot, strSID)) return false;

    // 获取Data组
    jArrData = jRoot[JSON_KEY_DATA];
    if (!jArrData.isArray() || !jArrData.isValidIndex(0)) return false;

    if (!jArrData[0][JSON_KEY_DEVICENAME].isString()) return false;
    lstInfo = jArrData[0][JSON_KEY_DEVICENAME].asString();
    return true;
}

bool CBMSInfoPackage::ParseSearchSubSysVariableInfo(const std::string &strRecv, std::string &strSID)
{
    if (strRecv.empty()) return false;

    Json::ArrayIndex  jIndex = 0;
    Json::Reader  jReader;
    Json::Value   jRoot;

    // 解析数据
    if (!jReader.parse(strRecv, jRoot)) return false;
    // 解析包头
    if (!OnParseHeader(jRoot, strSID)) return false;

    return true;
}

bool CBMSInfoPackage::ParseSearchDeviceVariableInfo(const std::string &strRecv, std::string &strSID, std::string &lstInfo)
{
    if (strRecv.empty()) return false;

    Json::ArrayIndex  jIndex = 0;
    Json::Reader  jReader;
    Json::Value   jRoot;
    Json::Value   jArrData;

    // 解析数据
    if (!jReader.parse(strRecv, jRoot)) return false;
    // 解析包头
    if (!OnParseHeader(jRoot, strSID)) return false;

    // 获取Data组
    jArrData = jRoot[JSON_KEY_DATA];
    if (!jArrData.isArray() || !jArrData.isValidIndex(0)) return false;

    if (!jArrData[0][JSON_KEY_SUBSYSTEM_ID].isString()) return false;
    lstInfo = jArrData[0][JSON_KEY_SUBSYSTEM_ID].asString();
    return true;
}

bool CBMSInfoPackage::ParseSearchTagVariableInfo(const std::string &strRecv, std::string &strSID, string &lstInfo)
{
    if (strRecv.empty()) return false;

    Json::ArrayIndex  jIndex = 0;
    Json::Reader  jReader;
    Json::Value   jRoot;
    Json::Value   jArrData;

    // 解析数据
    if (!jReader.parse(strRecv, jRoot)) return false;
    // 解析包头
    if (!OnParseHeader(jRoot, strSID)) return false;

    // 获取Data组
    jArrData = jRoot[JSON_KEY_DATA];
    if (!jArrData.isArray() || !jArrData.isValidIndex(0)) return false;

    if (!jArrData[0][JSON_KEY_DEVICE_ID].isString()) return false;
    lstInfo = jArrData[0][JSON_KEY_DEVICE_ID].asString();
    return true;
}


bool CBMSInfoPackage::ParseSearchOverViewInfo(const std::string &strRecv, std::string &strSID, string &lstInfo)
{
	if (strRecv.empty()) return false;

	Json::ArrayIndex  jIndex = 0;
	Json::Reader  jReader;
	Json::Value   jRoot;
	Json::Value   jArrData;

	// 解析数据
	if (!jReader.parse(strRecv, jRoot)) return false;
	// 解析包头
	if (!OnParseHeader(jRoot, strSID)) return false;
	return true;
}

bool CBMSInfoPackage::ParseSearchMeterValueInfo(const std::string &strRecv, std::string &strSID, shared_ptr<ValueRequestInfo> &lstInfo)
{
    if (strRecv.empty()) return false;

    Json::ArrayIndex  jIndex = 0;
    Json::Reader  jReader;
    Json::Value   jRoot;
    Json::Value   jArrData;

    // 解析数据
    if (!jReader.parse(strRecv, jRoot)) return false;
    // 解析包头
    if (!OnParseHeader(jRoot, strSID)) return false;
    // 解析时间
    if (!OnParseDateTime(jRoot[JSON_KEY_STARTTIME], lstInfo->publicInfo.ullStartTime)) return false;
    if (!OnParseDateTime(jRoot[JSON_KEY_ENDTIME], lstInfo->publicInfo.ullEndTime)) return false;
    
    if (!jRoot[JSON_KEY_PAGE_SIZE].isInt()) return false;
    lstInfo->publicInfo.pageSize = jRoot[JSON_KEY_PAGE_SIZE].asInt();
    if (!jRoot[JSON_KEY_PAGE_NUM].isInt()) return false;
    lstInfo->publicInfo.pageNum = jRoot[JSON_KEY_PAGE_NUM].asInt();

    // 获取Data组
    jArrData = jRoot[JSON_KEY_DATA];
    if (!jArrData.isArray() || !jArrData.isValidIndex(0)) return false;

    if (!jArrData[0][JSON_KEY_METERNUM].isString()) return false;
    lstInfo->strName = jArrData[0][JSON_KEY_METERNUM].asString();
    return true;
}

bool CBMSInfoPackage::ParseSearchDeviceValueInfo(const std::string &strRecv, std::string &strSID, shared_ptr<ValueRequestInfo> &lstInfo)
{
    if (strRecv.empty()) return false;

    Json::ArrayIndex  jIndex = 0;
    Json::Reader  jReader;
    Json::Value   jRoot;
    Json::Value   jArrData;

    // 解析数据
    if (!jReader.parse(strRecv, jRoot)) return false;
    // 解析包头
    if (!OnParseHeader(jRoot, strSID)) return false;
    // 解析时间
    if (!OnParseDateTime(jRoot[JSON_KEY_STARTTIME], lstInfo->publicInfo.ullStartTime)) return false;
    if (!OnParseDateTime(jRoot[JSON_KEY_ENDTIME], lstInfo->publicInfo.ullEndTime)) return false;

    if (!jRoot[JSON_KEY_PAGE_SIZE].isInt()) return false;
    lstInfo->publicInfo.pageSize = jRoot[JSON_KEY_PAGE_SIZE].asInt();
    if (!jRoot[JSON_KEY_PAGE_NUM].isInt()) return false;
    lstInfo->publicInfo.pageNum = jRoot[JSON_KEY_PAGE_NUM].asInt();

    // 获取Data组
    jArrData = jRoot[JSON_KEY_DATA];
    if (!jArrData.isArray() || !jArrData.isValidIndex(0)) return false;

    if (!jArrData[0][JSON_KEY_DEVICENAME].isString()) return false;
    lstInfo->strName = jArrData[0][JSON_KEY_DEVICENAME].asString();
    return true;
}

bool CBMSInfoPackage::ParseSearchVariableValueInfo(const std::string &strRecv, std::string &strSID, shared_ptr<ValueRequestInfo> &lstInfo)
{
    if (strRecv.empty()) return false;

    Json::ArrayIndex  jIndex = 0;
    Json::Reader  jReader;
    Json::Value   jRoot;

    // 解析数据
    if (!jReader.parse(strRecv, jRoot)) return false;
    // 解析包头
    if (!OnParseHeader(jRoot, strSID)) return false;
    // 解析时间
    if (!OnParseDateTime(jRoot[JSON_KEY_STARTTIME], lstInfo->publicInfo.ullStartTime)) return false;
    if (!OnParseDateTime(jRoot[JSON_KEY_ENDTIME], lstInfo->publicInfo.ullEndTime)) return false;

    if (!jRoot[JSON_KEY_PAGE_SIZE].isInt()) return false;
    lstInfo->publicInfo.pageSize = jRoot[JSON_KEY_PAGE_SIZE].asInt();
    if (!jRoot[JSON_KEY_PAGE_NUM].isInt()) return false;
    lstInfo->publicInfo.pageNum = jRoot[JSON_KEY_PAGE_NUM].asInt();
    return true;
}

bool CBMSInfoPackage::ParseSearchHistoryEventExInfo(const std::string &strRecv, std::string &strSID, shared_ptr<HistoryEventRequestInfo> &lstInfo)
{
    if (strRecv.empty()) return false;

    Json::ArrayIndex  jIndex = 0;
    Json::Reader  jReader;
    Json::Value   jRoot;

    // 解析数据
    if (!jReader.parse(strRecv, jRoot)) return false;
    // 解析包头
    if (!OnParseHeader(jRoot, strSID)) return false;
    // 解析时间
    if (!OnParseDateTime(jRoot[JSON_KEY_EVENT][JSON_KEY_STARTTIME], lstInfo->publicInfo.ullStartTime)) return false;
    if (!OnParseDateTime(jRoot[JSON_KEY_EVENT][JSON_KEY_ENDTIME], lstInfo->publicInfo.ullEndTime)) return false;

    if (!jRoot[JSON_KEY_PAGE_SIZE].isInt()) return false;
    lstInfo->publicInfo.pageSize = jRoot[JSON_KEY_PAGE_SIZE].asInt();
    if (!jRoot[JSON_KEY_PAGE_NUM].isInt()) return false;
    lstInfo->publicInfo.pageNum = jRoot[JSON_KEY_PAGE_NUM].asInt();

    if (!jRoot[JSON_KEY_TYPE].isInt()) return false;
    lstInfo->enventType = jRoot[JSON_KEY_TYPE].asInt();
    if (!jRoot[JSON_KEY_USER_NAME].isString()) return false;
    lstInfo->strUserName = jRoot[JSON_KEY_USER_NAME].asString();

    return true;
}

bool CBMSInfoPackage::ParseSearchHistoryAlarmExInfo(const std::string &strRecv, std::string &strSID, shared_ptr<HistoryAlarmRequestInfo> &lstInfo)
{
    if (strRecv.empty()) return false;

    Json::ArrayIndex  jIndex = 0;
    Json::Reader  jReader;
    Json::Value   jRoot;

    // 解析数据
    if (!jReader.parse(strRecv, jRoot)) return false;
    // 解析包头
    if (!OnParseHeader(jRoot, strSID)) return false;
    // 解析时间
    if (!OnParseDateTime(jRoot[JSON_KEY_ALARM][JSON_KEY_STARTTIME], lstInfo->publicInfo.ullStartTime)) return false;
    if (!OnParseDateTime(jRoot[JSON_KEY_ALARM][JSON_KEY_ENDTIME], lstInfo->publicInfo.ullEndTime)) return false;

    if (!jRoot[JSON_KEY_PAGE_SIZE].isInt()) return false;
    lstInfo->publicInfo.pageSize = jRoot[JSON_KEY_PAGE_SIZE].asInt();
    if (!jRoot[JSON_KEY_PAGE_NUM].isInt()) return false;
    lstInfo->publicInfo.pageNum = jRoot[JSON_KEY_PAGE_NUM].asInt();

    if (!jRoot[JSON_KEY_PRIORITY].isInt()) return false;
    lstInfo->ulPriority = jRoot[JSON_KEY_PRIORITY].asInt();
    if (!jRoot[JSON_KEY_AREA].isString()) return false;
    lstInfo->strArea = jRoot[JSON_KEY_AREA].asString();
    if (!jRoot[JSON_KEY_DEVICE_NAME].isString()) return false;
    lstInfo->strDeviceName = jRoot[JSON_KEY_DEVICE_NAME].asString();
    return true;
}

bool CBMSInfoPackage::BuildOnlyHead(std::string &strSID, EErrCode eEC, ECommand eCmd, std::string &strSend)
{
    if (strSID.empty()) return false;

    Json::FastWriter  jWriter;
    Json::Value       jRoot;

    // 构建包头
    if (!OnBuildHeader(strSID, eCmd, eEC, jRoot)) return false;
    strSend = jWriter.write(jRoot);
    return true;
}
bool CBMSInfoPackage::BuildAddDeviceRecordInfo(std::string &strSID, EErrCode eEC, ECommand eCmd, const uint32_t &recordingID, std::string &strSend)
{
    if (strSID.empty()) return false;

    Json::FastWriter  jWriter;
    Json::Value       jRoot;

    // 构建包头
    if (!OnBuildHeader(strSID, eCmd, eEC, jRoot)) return false;
    jRoot[JSON_KEY_RECORDINGID] = recordingID;
    strSend = jWriter.write(jRoot);
    return true;
}
bool CBMSInfoPackage::BuildSearchSupplier(std::string &strSID, EErrCode eEC, const list<std::shared_ptr<SupplierInfo>> &lstInfo, std::string &strSend)
{
    if (strSID.empty()) return false;

    Json::FastWriter  jWriter;
    Json::Value       jRoot;
    Json::Value       jArrData;
    Json::Value       jValue;

    // 构建包头
    if (!OnBuildHeader(strSID, ECommand::CMD_SEARCH_SUPPLIER, eEC, jRoot)) return false;

    // 添加查询结果
    for (auto &item : lstInfo)
    {
        jValue.clear();

        jValue[JSON_KEY_SUPPLIERCODE] = item->sSupplierCode;
        jValue[JSON_KEY_SUPPLIERNAME] = item->sSupplierName;
        jValue[JSON_KEY_CONPERSON] = item->sConPerson;
        jValue[JSON_KEY_CONDEPARTMENT] = item->sConDepartment;
        jValue[JSON_KEY_CONTELEPHONE] = item->sConTelephone;
        jValue[JSON_KEY_FAX] = item->sFax;
        jValue[JSON_KEY_ADDRESS] = item->sAddress;
        jValue[JSON_KEY_ZIPCODE] = item->sZipCode;
        jValue[JSON_KEY_E_MAIL] = item->sE_Mail;
        jValue[JSON_KEY_URL] = item->sURL;
        jValue[JSON_KEY_REMARKS] = item->sRemarks;

        jArrData.append(jValue);
    }

    // 添加Data组
    jRoot[JSON_KEY_DATA] = jArrData;

    // 转换为字符串
    strSend = jWriter.write(jRoot);
    return true;
}

bool CBMSInfoPackage::BuildSearchMeterType(std::string &strSID, EErrCode eEC, const list<std::shared_ptr<MeterType>> &lstInfo, std::string &strSend)
{
    if (strSID.empty()) return false;

    Json::FastWriter  jWriter;
    Json::Value       jRoot;
    Json::Value       jArrData;
    Json::Value       jValue;

    // 构建包头
    if (!OnBuildHeader(strSID, ECommand::CMD_SEARCH_METERTYPE, eEC, jRoot)) return false;

    // 添加查询结果
    for (auto &item : lstInfo)
    {
        jValue.clear();

        jValue[JSON_KEY_METERTYPE] = item->strMeterType;
        jValue[JSON_KEY_TYPEDES] = item->TypeDes;

        jArrData.append(jValue);
    }

    // 添加Data组
    jRoot[JSON_KEY_DATA] = jArrData;

    // 转换为字符串
    strSend = jWriter.write(jRoot);
    return true;
}

bool CBMSInfoPackage::BuildSearchEnergyCate(std::string &strSID, EErrCode eEC, const list<std::shared_ptr<EnergyCate>> &lstInfo, std::string &strSend)
{
    if (strSID.empty()) return false;

    Json::FastWriter  jWriter;
    Json::Value       jRoot;
    Json::Value       jArrData;
    Json::Value       jValue;

    // 构建包头
    if (!OnBuildHeader(strSID, ECommand::CMD_SEARCH_ENERGYCATE, eEC, jRoot)) return false;

    // 添加查询结果
    for (auto &item : lstInfo)
    {
        jValue.clear();

        jValue[JSON_KEY_ENERGYCATE] = item->strEnergyCate;
        jValue[JSON_KEY_TYPEDES] = item->TypeDes;

        jArrData.append(jValue);
    }

    // 添加Data组
    jRoot[JSON_KEY_DATA] = jArrData;

    // 转换为字符串
    strSend = jWriter.write(jRoot);
    return true;
}

bool CBMSInfoPackage::BuildSearchDevFaultCate(std::string &strSID, EErrCode eEC, const list<std::shared_ptr<DevFaultCate>> &lstInfo, std::string &strSend)
{
    if (strSID.empty()) return false;

    Json::FastWriter  jWriter;
    Json::Value       jRoot;
    Json::Value       jArrData;
    Json::Value       jValue;

    // 构建包头
    if (!OnBuildHeader(strSID, ECommand::CMD_SEARCH_FAULTCATE, eEC, jRoot)) return false;

    // 添加查询结果
    for (auto &item : lstInfo)
    {
        jValue.clear();

        jValue[JSON_KEY_FAULTCATE] = item->FaultCate;
        jValue[JSON_KEY_CATEGORYDES] = item->CategoryDes;
        jValue[JSON_KEY_FAULTFEATURE] = item->FaultFeature;
        jArrData.append(jValue);
    }

    // 添加Data组
    jRoot[JSON_KEY_DATA] = jArrData;

    // 转换为字符串
    strSend = jWriter.write(jRoot);
    return true;
}

bool CBMSInfoPackage::BuildSearchDeviceInfo(std::string &strSID, EErrCode eEC, const list<std::shared_ptr<DeviceInfo>> &lstInfo, std::string &strSend)
{
    if (strSID.empty()) return false;

    Json::FastWriter  jWriter;
    Json::Value       jRoot;
    Json::Value       jArrData;
    Json::Value       jValue;

    // 构建包头
    if (!OnBuildHeader(strSID, ECommand::CMD_SEARCH_DEVICEINF, eEC, jRoot)) return false;

    // 添加查询结果
    for (auto &item : lstInfo)
    {
        jValue.clear();
        // 判断sSupplierName是否有效,添加查询结果
        if (!item->DeviceName.empty())
        {
            jValue[JSON_KEY_DEVICENUM] = item->DeviceNum;
            jValue[JSON_KEY_DEVICENAME] = item->DeviceName;
            jValue[JSON_KEY_INSTALLLOCATION] = item->InstallLocation;
            jValue[JSON_KEY_USEDATE] = item->UseDate;
            jValue[JSON_KEY_USAGEDEPART] = item->UsageDepart;
            jValue[JSON_KEY_DEVICESTATUS] = item->DeviceStatus;
            jValue[JSON_KEY_PRODUCTIONDATE] = item->ProductionDate;
            jValue[JSON_KEY_DEVICESUPPLIER] = item->DeviceSupplier;
            jValue[JSON_KEY_ALARMUPLIMIT] = item->AlarmUpLimit;
            jValue[JSON_KEY_ALARMDOWNLIMIT] = item->AlarmDownLimit;
            jValue[JSON_KEY_RUNCTRLUPLIMIT] = item->RunCtrlUpLimit;
            jValue[JSON_KEY_RUNCTRLDOWNLIMIT] = item->RunCtrlDownLimit;
            jArrData.append(jValue);
        }
    }
    // 添加Data组
    jRoot[JSON_KEY_DATA] = jArrData;

    // 转换为字符串
    strSend = jWriter.write(jRoot);
    return true;
}

bool CBMSInfoPackage::BuildSearchMeterInfo(std::string &strSID, EErrCode eEC, const list<std::shared_ptr<MeterInfo>> &lstInfo, std::string &strSend)
{
    if (strSID.empty()) return false;

    Json::FastWriter  jWriter;
    Json::Value       jRoot;
    Json::Value       jArrData;
    Json::Value       jValue;

    // 构建包头
    if (!OnBuildHeader(strSID, ECommand::CMD_SEARCH_METERINF, eEC, jRoot)) return false;

    // 添加查询结果
    for (auto &item : lstInfo)
    {
        jValue.clear();
        // 判断MeterNumber是否有效,添加查询结果
        if (!item->MeterNumber.empty())
        {
            jValue[JSON_KEY_METERNUM] = item->MeterNumber;
            jValue[JSON_KEY_METERTYPE] = item->strMeterType;
            jValue[JSON_KEY_ENERGYCATE] = item->strEnergyCate;
            jValue[JSON_KEY_METERMODEL] = item->MeterModel;
            jValue[JSON_KEY_USEUNITS] = item->UseUnits;
            jValue[JSON_KEY_RATESTANDARD] = item->RateStandard;
            jValue[JSON_KEY_STANDARDUNIT] = item->StandardUnit;
            jValue[JSON_KEY_INSTALLDATE] = item->InstallDate;
            jValue[JSON_KEY_INSTALLLOCATION] = item->InstallLocation;
            jValue[JSON_KEY_PRODUCTIONFACTORY] = item->ProductionFactory;
            jValue[JSON_KEY_METERMAGNIFICATION] = item->MeterMagnification;
            jValue[JSON_KEY_TRANMETERBASE] = item->TranMeterBase;
            jValue[JSON_KEY_METERINITVALUE] = item->MeterInitValue;
            jValue[JSON_KEY_TRANMETERVALUE] = item->TranMeterValue;
            jValue[JSON_KEY_REMARKS] = item->Remarks;
            if (0 != item->tagID)
            {
                OnBuildGlobalID(item->tagID, jValue[JSON_KEY_ID]);
            }
            
            //jValue[JSON_KEY_TAG_NAME] = item->tagName;
            //jValue[JSON_KEY_OLDMETERNUMBER] = item->OldMeterNumber;
            jArrData.append(jValue);
        }
    }
    // 添加Data组
    jRoot[JSON_KEY_DATA] = jArrData;

    // 转换为字符串
    strSend = jWriter.write(jRoot);
    return true;
}

bool CBMSInfoPackage::BuildSearchDeviceParameter(std::string &strSID, EErrCode eEC, const std::shared_ptr<DeviceParameter> &lstInfo, std::string &strSend)
{
    if (strSID.empty()) return false;

    Json::FastWriter  jWriter;
    Json::Value       jRoot;
    Json::Value       jArrData;
    Json::Value       jValue;

    // 构建包头
    if (!OnBuildHeader(strSID, ECommand::CMD_SEARCH_DEVICEPARAMETER, eEC, jRoot)) return false;

    jRoot[JSON_KEY_DEVICE_NAME] = lstInfo->DeviceName;
    // 添加查询结果
    for (auto &item : lstInfo->lstGID)
    {
        jValue.clear();

        // 添加ID值
        if (!OnBuildGlobalID(item.first , jValue[JSON_KEY_ID])) return false;
        jValue[JSON_KEY_TAG_NAME] = item.second;
        jArrData.append(jValue);
    }
    // 添加Data组
    jRoot[JSON_KEY_DATA] = jArrData;

    // 转换为字符串
    strSend = jWriter.write(jRoot);
    return true;
}

bool CBMSInfoPackage::BuildSearchMeterParameter(std::string &strSID, EErrCode eEC, const std::shared_ptr<MeterParameter> &lstInfo, std::string &strSend)
{
    if (strSID.empty()) return false;

    Json::FastWriter  jWriter;
    Json::Value       jRoot;
    Json::Value       jArrData;
    Json::Value       jValue;

    // 构建包头
    if (!OnBuildHeader(strSID, ECommand::CMD_SEARCH_METERPARAMETER, eEC, jRoot)) return false;

    jRoot[JSON_KEY_METERNUM] = lstInfo->MeterNum;
    
    // 添加ID值
    if (0 != lstInfo->ulGID)
    {
        if (!OnBuildGlobalID(lstInfo->ulGID, jRoot[JSON_KEY_ID])) return false;
    }

    strSend = jWriter.write(jRoot);
    return true;
}

bool CBMSInfoPackage::BuildSearchDeviceRepair(std::string &strSID, EErrCode eEC, ECommand eCmd, const list<shared_ptr<RepairInfo>> &lstInfo, std::string &strSend)
{
    if (strSID.empty()) return false;

    Json::FastWriter  jWriter;
    Json::Value       jRoot;
    Json::Value       jArrData;
    Json::Value       jValue;

    // 构建包头
    if (!OnBuildHeader(strSID, eCmd, eEC, jRoot)) return false;

    // 添加查询结果
    for (auto &item : lstInfo)
    {
        jValue.clear();
        
        if (!item->DeviceName.empty() && 0 != item->recordingID)
        {
            jValue[JSON_KEY_RECORDINGID] = item->recordingID;
            jValue[JSON_KEY_DEVICENAME] = item->DeviceName;
            jValue[JSON_KEY_REPAIRCONTENT] = item->RepairContent;
            jValue[JSON_KEY_REPAIRTIME] = item->RepairTime;
            jValue[JSON_KEY_FAULTREASON] = item->FaultReason;
            jValue[JSON_KEY_REPAIRRESULTS] = item->RepairResults;
            jValue[JSON_KEY_PEPAIRMANAGER] = item->RepairManager;
            jValue[JSON_KEY_REPAIRUNIT] = item->RepairUnit;
            jValue[JSON_KEY_REMARKS] = item->Remarks;
            jArrData.append(jValue);
        }
    }
    
    // 添加Data组
    jRoot[JSON_KEY_DATA] = jArrData;

    // 转换为字符串
    strSend = jWriter.write(jRoot);
    return true;
}


bool CBMSInfoPackage::BuildSearchDeviceRun(std::string &strSID, EErrCode eEC, ECommand eCmd, const list<shared_ptr<DeviceRunInfo>> &lstInfo, std::string &strSend)
{
    if (strSID.empty()) return false;

    Json::FastWriter  jWriter;
    Json::Value       jRoot;
    Json::Value       jArrData;
    Json::Value       jValue;

    // 构建包头
    if (!OnBuildHeader(strSID, eCmd, eEC, jRoot)) return false;

    // 添加查询结果
    for (auto &item : lstInfo)
    {
        jValue.clear();

        if (!item->DeviceName.empty() && 0 != item->recordingID)
        {
            jValue[JSON_KEY_RECORDINGID] = item->recordingID;
            jValue[JSON_KEY_DEVICENAME] = item->DeviceName;
            jValue[JSON_KEY_STARTTIME_] = item->StartTime;
            jValue[JSON_KEY_ENDTIME_] = item->EndTime;
            jValue[JSON_KEY_OPERATION] = item->Operation;
            jValue[JSON_KEY_RECORDER] = item->Recorder;
            jValue[JSON_KEY_REMARKS] = item->Remarks;
            jArrData.append(jValue);
        }
    }
    // 添加Data组
    jRoot[JSON_KEY_DATA] = jArrData;
    // 转换为字符串
    strSend = jWriter.write(jRoot);
    return true;
}


bool CBMSInfoPackage::BuildSearchSubSysVariableInfo(std::string &strSID, EErrCode eEC, const shared_ptr<SubSysInfo> &lstInfo, std::string &strSend)
{
    if (strSID.empty()) return false;

    Json::FastWriter  jWriter;
    Json::Value       jRoot;
    Json::Value       jArrData;
    Json::Value       jValue;

    // 构建包头
    if (!OnBuildHeader(strSID, ECommand::CMD_SEARCH_SYSTEM, eEC, jRoot)) return false;

    // 添加查询结果
    for (auto &item : lstInfo->lstSubSys)
    {
        jValue.clear();

        if (!item.first.empty() && !item.second.empty())
        {
            jValue[JSON_KEY_SUBSYSTEM_ID] = item.first;
            jValue[JSON_KEY_SUBSTYTEM_NAME] = item.second;
            jArrData.append(jValue);
        }
    }
    // 添加Data组
    jRoot[JSON_KEY_DATA] = jArrData;
    // 转换为字符串
    strSend = jWriter.write(jRoot);
    return true;
}

bool CBMSInfoPackage::BuildSearchDeviceVariableInfo(std::string &strSID, EErrCode eEC, const shared_ptr<DeviceVarInfo> &lstInfo, std::string &strSend)
{
    if (strSID.empty()) return false;

    Json::FastWriter  jWriter;
    Json::Value       jRoot;
    Json::Value       jArrData;
    Json::Value       jValue;

    // 构建包头
    if (!OnBuildHeader(strSID, ECommand::CMD_SEARCH_DEVICE, eEC, jRoot)) return false;

    jRoot[JSON_KEY_SUBSYSTEM_ID] = lstInfo->SysID;
    // 添加查询结果
    for (auto &item : lstInfo->lstDevice)
    {
        jValue.clear();

        if (!item.first.empty() && !item.second.empty())
        {
            jValue[JSON_KEY_DEVICE_ID] = item.first;
            jValue[JSON_KEY_DEVICENAME] = item.second;
            jArrData.append(jValue);
        }
    }
    // 添加Data组
    jRoot[JSON_KEY_DATA] = jArrData;
    // 转换为字符串
    strSend = jWriter.write(jRoot);
    return true;
}

bool CBMSInfoPackage::BuildSearchTagVariableInfo(std::string &strSID, EErrCode eEC, const shared_ptr<TagVarInfo> &lstInfo, std::string &strSend, ECommand command)
{
    if (strSID.empty()) return false;

    Json::FastWriter  jWriter;
    Json::Value       jRoot;
    Json::Value       jArrData;
    Json::Value       jValue;

    // 构建包头
    if (!OnBuildHeader(strSID, command, eEC, jRoot)) return false;

    jRoot[JSON_KEY_DEVICE_ID] = lstInfo->DevID;
    // 添加查询结果
    for (auto &item : lstInfo->lstTag)
    {
        jValue.clear();

        if (!item.first.empty() && !item.second.empty())
        {
            jValue[JSON_KEY_TAG_ID] = item.first;
            jValue[JSON_KEY_TAG_NAME] = item.second;
            jArrData.append(jValue);
        }
    }
    // 添加Data组
    jRoot[JSON_KEY_DATA] = jArrData;
    // 转换为字符串
    strSend = jWriter.write(jRoot);
    return true;
}

//
bool CBMSInfoPackage::BuildSearchOverViewInfo(std::string &strSID, EErrCode eEC, const shared_ptr<OverViewInfo> &lstInfo, std::string &strSend)
{
	if (strSID.empty()) return false;

	Json::FastWriter  jWriter;
	Json::Value       jRoot;
	Json::Value       jArrData;
	Json::Value       jValue;

	// 构建包头
	if (!OnBuildHeader(strSID, ECommand::CMD_SEARCH_OVERVIEW, eEC, jRoot)) return false;
	jRoot[JSON_KEY_OVERVIEW_SUBSYSTEM_COUNT] = lstInfo->overData.size();
	// 添加查询结果
	for (auto &item : lstInfo->overData)
	{
		jValue.clear();
		list<shared_ptr<CommonData>>::iterator iter;
		for (iter = lstInfo->overData.begin(); iter != lstInfo->overData.end(); ++iter)
		{
			jValue[JSON_KEY_SUBSYSTEM_ID] = (*iter)->subsystemId;
			jValue[JSON_KEY_OVERVIEW_DEVICE_COUNT] = (*iter)->deviceCount;
			jValue[JSON_KEY_OVERVIEW_TAG_COUNT] = (*iter)->tagCount;
			jArrData.append(jValue);
		}
	}
	// 添加Data组
	jRoot[JSON_KEY_DATA] = jArrData;
	// 转换为字符串
	strSend = jWriter.write(jRoot);
	return true;
}


bool CBMSInfoPackage::BuildSearchSystemAlarmCountInfo(std::string &strSID, EErrCode eEC, const shared_ptr<Subsystem_Alarm> &lstInfo, std::string &strSend)
{
	if (strSID.empty()) return false;

	Json::FastWriter  jWriter;
	Json::Value       jRoot;
	Json::Value       jArrData;
	Json::Value       jValue;
 
  	// 构建包头
	if (!OnBuildHeader(strSID, ECommand::CMD_SEARCH_SUBSYSTEM_ALARM_COUNT, eEC, jRoot)) return false;
  	// 添加查询结果
  	for (auto &item : lstInfo->alarmInfo)
  	{
  		jValue.clear();
		jValue[JSON_KEY_SUBSYSTEM_ID] = item.first;
		jValue[JSON_KEY_OVERVIEW_ALARM_COUNT] = item.second;
		jArrData.append(jValue);
  	}
	// 添加Data组
	jRoot[JSON_KEY_DATA] = jArrData;
	// 转换为字符串
	strSend = jWriter.write(jRoot);
	return true;
}

bool CBMSInfoPackage::BuildSearchMeterValueInfo(std::string &strSID, EErrCode eEC, const shared_ptr<ResponseMeterValueInfo> &lstInfo, std::string &strSend)
{
    if (strSID.empty()) return false;

    Json::FastWriter  jWriter;
    Json::Value       jRoot;
    Json::Value       jArrData;
    Json::Value       jValue;

    // 构建包头
    if (!OnBuildHeader(strSID, ECommand::CMD_SEARCH_METERVALUE, eEC, jRoot)) return false;

    jRoot[JSON_KEY_PAGE_SIZE] = lstInfo->pageSize;
    jRoot[JSON_KEY_PAGE_NUM] = lstInfo->pageNum;
    jRoot[JSON_KEY_TOTAL_PAGES] = lstInfo->pageTotal;
    // 添加查询结果
    for (auto &item : lstInfo->lstValueInfo)
    {
        jValue.clear();

        if (!item.meterNumber.empty())
        {
            jValue[JSON_KEY_METERNUM] = item.meterNumber;
            jValue[JSON_KEY_METERTYPE] = item.meterType;
            if (!OnBuildGlobalID(item.valueInfo.ulId, jValue[JSON_KEY_ID])) return false;
            jValue[JSON_KEY_TAG_NAME] = item.valueInfo.tagName;
            jValue[JSON_KEY_TAG_VALUE] = item.valueInfo.tagValue;
            jValue[JSON_KEY_COLLECTIONTIME] = item.valueInfo.collectionTime;
            jArrData.append(jValue);
        }
    }
    // 添加Data组
    jRoot[JSON_KEY_DATA] = jArrData;
    // 转换为字符串
    strSend = jWriter.write(jRoot);
    return true;
}

bool CBMSInfoPackage::BuildSearchDeviceValueInfo(std::string &strSID, EErrCode eEC, const shared_ptr<DeviceValueInfo> &lstInfo, std::string &strSend)
{
    if (strSID.empty()) return false;

    Json::FastWriter  jWriter;
    Json::Value       jRoot;
    Json::Value       jArrData;
    Json::Value       jValue;

    // 构建包头
    if (!OnBuildHeader(strSID, ECommand::CMD_SEARCH_DEVICEVALUE, eEC, jRoot)) return false;

    jRoot[JSON_KEY_PAGE_SIZE] = lstInfo->pageSize;
    jRoot[JSON_KEY_PAGE_NUM] = lstInfo->pageNum;
    jRoot[JSON_KEY_TOTAL_PAGES] = lstInfo->pageTotal;
    // 添加查询结果
    for (auto &item : lstInfo->lstValueInfo)
    {
        jValue.clear();

        if (!OnBuildGlobalID(item.ulId, jValue[JSON_KEY_ID])) return false;
        jValue[JSON_KEY_TAG_NAME] = item.tagName;
        jValue[JSON_KEY_TAG_VALUE] = item.tagValue;
        jValue[JSON_KEY_COLLECTIONTIME] = item.collectionTime;
        jArrData.append(jValue);
    }
    // 添加Data组
    jRoot[JSON_KEY_DATA] = jArrData;
    // 转换为字符串
    strSend = jWriter.write(jRoot);
    return true;
}

bool CBMSInfoPackage::BuildSearchVariableValueInfo(std::string &strSID, EErrCode eEC, const shared_ptr<DeviceValueInfo> &lstInfo, std::string &strSend)
{
    if (strSID.empty()) return false;

    Json::FastWriter  jWriter;
    Json::Value       jRoot;
    Json::Value       jArrData;
    Json::Value       jValue;

    // 构建包头
    if (!OnBuildHeader(strSID, ECommand::CMD_SEARCH_TAGVALUE, eEC, jRoot)) return false;

    jRoot[JSON_KEY_PAGE_SIZE] = lstInfo->pageSize;
    jRoot[JSON_KEY_PAGE_NUM] = lstInfo->pageNum;
    jRoot[JSON_KEY_TOTAL_PAGES] = lstInfo->pageTotal;
    // 添加查询结果
    for (auto &item : lstInfo->lstValueInfo)
    {
        jValue.clear();
        if (!OnBuildGlobalID(item.ulId, jValue[JSON_KEY_ID])) return false;
        jValue[JSON_KEY_TAG_NAME] = item.tagName;
        jValue[JSON_KEY_TAG_VALUE] = item.tagValue;
        jValue[JSON_KEY_COLLECTIONTIME] = item.collectionTime;
        jArrData.append(jValue);
    }
    // 添加Data组
    jRoot[JSON_KEY_DATA] = jArrData;
    // 转换为字符串
    strSend = jWriter.write(jRoot);
    return true;
}

bool CBMSInfoPackage::BuildSearchHistoryEventExInfo(std::string &strSID, EErrCode eEC, const shared_ptr<HistoryEventInfo> &lstInfo, std::string &strSend)
{
    if (strSID.empty()) return false;

    Json::FastWriter  jWriter;
    Json::Value       jRoot;
    Json::Value       jArrData;
    Json::Value       jValue;

    // 构建包头
    if (!OnBuildHeader(strSID, ECommand::CMD_READ_HISTORY_EVENT_EX, eEC, jRoot)) return false;

    jRoot[JSON_KEY_PAGE_SIZE] = lstInfo->pageSize;
    jRoot[JSON_KEY_PAGE_NUM] = lstInfo->pageNum;
    jRoot[JSON_KEY_TOTAL_PAGES] = lstInfo->pageTotal;
    // 添加查询结果
    for (auto &item : lstInfo->lstValueInfo)
    {
        jValue.clear();
        // 创建Time对象
        if (!OnBuildDateTime(item.ullTimeMs, jValue[JSON_KEY_TIME])) return false;

        // EventID
        jValue[JSON_KEY_EVENT_ID] = item.ulID;
        // UserName
        jValue[JSON_KEY_USER_NAME] = item.strUserName;
        // Record
        jValue[JSON_KEY_RECORD] = item.strRecord;
        // AccessLevel
        jValue[JSON_KEY_ACCESS_LEVEL] = item.ulAccessLevel;
        // Type
        jValue[JSON_KEY_TYPE] = item.eType;

        jArrData.append(jValue);
    }
    // 添加Data组
    jRoot[JSON_KEY_EVENT] = jArrData;
    // 转换为字符串
    strSend = jWriter.write(jRoot);
    return true;
}

bool CBMSInfoPackage::BuildSearchHistoryAlarmExInfo(std::string &strSID, EErrCode eEC, const shared_ptr<HistoryAlarmInfo> &lstInfo, std::string &strSend)
{
    if (strSID.empty()) return false;

    Json::FastWriter  jWriter;
    Json::Value       jRoot;
    Json::Value       jArrData;
    Json::Value       jValue;

    // 构建包头
    if (!OnBuildHeader(strSID, ECommand::CMD_READ_HISTORY_ALARM_EX, eEC, jRoot)) return false;

    jRoot[JSON_KEY_PAGE_SIZE] = lstInfo->pageSize;
    jRoot[JSON_KEY_PAGE_NUM] = lstInfo->pageNum;
    jRoot[JSON_KEY_TOTAL_PAGES] = lstInfo->pageTotal;
    // 添加查询结果
    for (auto &item : lstInfo->lstValueInfo)
    {
        jValue.clear();
        // 创建ID的对象
        if (!OnBuildGlobalID(item.ulGID, jValue[JSON_KEY_ID])) return false;
        // 创建Time对象
        if (!OnBuildDateTime(item.ullTimeMs, jValue[JSON_KEY_TIME])) return false;

        // Alarm其他值
        jValue[JSON_KEY_ALARM_ID] = item.ulID;
        jValue[JSON_KEY_ALARMCATEGORY] = item.ulCategory;
        jValue[JSON_KEY_ALARMTYPE] = item.ulType;
        jValue[JSON_KEY_ALARMPRIORITY] = item.ulPriority;
        jValue[JSON_KEY_ALARMSOURCE] = item.strSource;
        jValue[JSON_KEY_ALARMVALUE] = item.strValue;
        jValue[JSON_KEY_ALARMMESSAGE] = item.strMessage;
        jValue[JSON_KEY_ALARMLIMIT] = item.strLimit;
        jValue[JSON_KEY_ALARMACK] = item.bAck;

        jArrData.append(jValue);
    }
    // 添加Data组
    jRoot[JSON_KEY_ALARM] = jArrData;
    // 转换为字符串
    strSend = jWriter.write(jRoot);
    return true;
}