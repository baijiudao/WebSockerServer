//------------------------------------------------------------------------------
// 创建日期：2017-09-11 17:41:21
// 文件描述：组包/解包的基类，主要是实现了一些通用的解析函数
//------------------------------------------------------------------------------
#ifndef _PACKAGE_BASE_H_
#define _PACKAGE_BASE_H_

#include <memory>
#include <string>
#include <mutex>
#include <list>
#include <json/json.h>

#include <Singleton.h>

#include "../DatabaseManager/DatabaseManager.h"

#include "../Define.h"

#include "../UnitConvert.h"
#include "../CommonUnit.h"

// JSON中的键名
#define JSON_KEY_COMMAND         "Command"
#define JSON_KEY_SESSION_ID      "Session_value"
#define JSON_KEY_STATUS_CODE     "StatusCode"

#define JSON_KEY_USER_NAME      "UserName"
#define JSON_KEY_PASSWORD       "Password"
#define JSON_KEY_AUTHORITY      "Authority"
#define JSON_KEY_TAG            "Tag"
#define JSON_KEY_EVENT          "Event"
#define JSON_KEY_ALARM          "Alarm"
#define JSON_KEY_ID             "ID"
#define JSON_KEY_SUBSYSTEM_ID   "SubsystemID"
#define JSON_KEY_DEVICE_ID      "DeviceID"
#define JSON_KEY_TAG_ID         "TagID"
#define JSON_KEY_TYPE           "Type"
#define JSON_KEY_VALUE          "Value"
#define JSON_KEY_TIME           "Time"
#define JSON_KEY_STARTTIME      "startTime"
#define JSON_KEY_ENDTIME        "endTime"
#define JSON_KEY_YEAR           "Year"
#define JSON_KEY_MONTH          "Month"
#define JSON_KEY_DAY            "Day"
#define JSON_KEY_HOUR           "Hour"
#define JSON_KEY_MINUTE         "Minute"
#define JSON_KEY_SECOND         "Second"
#define JSON_KEY_MINSEC         "MinSec"
#define JSON_KEY_EVENT_ID       "EventID"
#define JSON_KEY_ALARM_ID       "AlarmID"
#define JSON_KEY_RECORD         "Record"
#define JSON_KEY_ACCESS_LEVEL   "AccessLevel"
#define JSON_KEY_CATEGORY       "Category"
#define JSON_KEY_AREA           "Area"
#define JSON_KEY_DESCRIBE       "Describe"
#define JSON_KEY_ALARM_TAG      "AlarmTag"
#define JSON_KEY_CURRENT_VALUE  "CurrentValue"
#define JSON_KEY_PRIORITY       "Priority"
#define JSON_KEY_ALARM_EVENT    "AlarmEvent"
#define JSON_KEY_ACK            "AlarmACK"
#define JSON_KEY_SOURCE         "Source"
#define JSON_KEY_VALID          "Valid"
#define JSON_KEY_

#define JSON_KEY_DATA                           "Data"

#define JSON_KEY_DEVICE_NAME                    "DeviceName"
#define JSON_KEY_CAMERA_IP                      "CameraIP"
#define JSON_KEY_CAMERA_USER_NAME               "CameraUserName"
#define JSON_KEY_CAMERA_PASSWORD                "CameraPassword"
#define JSON_KEY_EMERGENCY_PLAN_FILE_NAME       "EmergencyPlanFileName"
#define JSON_KEY_EMERGENCY_PLAN_STORAGE_ADDRESS "EmergencyPlanStorageAddress"
#define JSON_KEY_LINKAGE_SCHEME_NAME            "LinkageSchemeName"
#define JSON_KEY_LINKAGE_SCHEME_DESCRIPTION     "LinkageSchemeDescription"
#define JSON_KEY_COUNT                          "Count"
#define JSON_KEY_START_NUM                      "StartNum"
#define JSON_KEY_TOTAL                          "Total"
#define JSON_KEY_PAGE_SIZE                      "PageSize"
#define JSON_KEY_PAGE_NUM                       "PageNum"
#define JSON_KEY_TOTAL_PAGES                    "TotalPages"

#define JSON_KEY_SUCCESSION_TIME                "SuccessionTime"  // 交班时间
#define JSON_KEY_SHIFT_TIME                     "ShiftTime"       // 接班时间
#define JSON_KEY_ALARM_PROCESSING               "AlarmProcessing"       // 告警处理情况
#define JSON_KEY_COMMUNICATION_MONITORING       "CommunicationMonitoring"       // 通信监测
#define JSON_KEY_LOG_TEXT                       "LogText"       // 日志文本

#define JSON_KEY_STATUS                         "Status"        // 状态
#define JSON_KEY_NAME                           "Name"          // 名字
#define JSON_KEY_GLOBAL_ID                      "GlobalID"      // 全局ID
//begin add by gaoxin 2017.10.13
//供应商
#define JSON_KEY_SUPPLIERCODE                    "SupplierCode"
#define JSON_KEY_SUPPLIERNAME                    "SupplierName"
#define JSON_KEY_CONPERSON                       "ConPerson"
#define JSON_KEY_CONDEPARTMENT                   "ConDepartment"
#define JSON_KEY_CONTELEPHONE                    "ConTelephone"
#define JSON_KEY_FAX                             "Fax"
#define JSON_KEY_ADDRESS                         "Address"
#define JSON_KEY_ZIPCODE                         "ZipCode"
#define JSON_KEY_E_MAIL                          "E-Mail"
#define JSON_KEY_URL                             "URL"
#define JSON_KEY_REMARKS                         "Remarks"

//计量表类型
#define JSON_KEY_METERTYPE                       "MeterType"
#define JSON_KEY_TYPEDES                         "TypeDes"

//耗能类别
#define JSON_KEY_ENERGYCATE                      "EnergyCate"

//设备故障类别
#define JSON_KEY_FAULTCATE                       "FaultCate"
#define JSON_KEY_CATEGORYDES                     "CategoryDes"
#define JSON_KEY_FAULTFEATURE                    "FaultFeature"

//设备档案
#define JSON_KEY_DEVICENUM                      "DeviceNum"
#define JSON_KEY_DEVICENAME                     "DeviceName"
#define JSON_KEY_INSTALLLOCATION                "InstallLocation"
#define JSON_KEY_USEDATE                        "UseDate"
#define JSON_KEY_USAGEDEPART                    "UsageDepart"
#define JSON_KEY_DEVICESTATUS                   "DeviceStatus"
#define JSON_KEY_PRODUCTIONDATE                 "ProductionDate"
#define JSON_KEY_DEVICESUPPLIER                 "DeviceSupplier"
#define JSON_KEY_ALARMUPLIMIT                   "AlarmUpLimit"
#define JSON_KEY_ALARMDOWNLIMIT                 "AlarmDownLimit"
#define JSON_KEY_RUNCTRLUPLIMIT                 "RunCtrlUpLimit"
#define JSON_KEY_RUNCTRLDOWNLIMIT               "RunCtrlDownLimit"

//计量表档案
#define JSON_KEY_METERNUM                       "MeterNumber"
#define JSON_KEY_METERMODEL                     "MeterModel"
#define JSON_KEY_USEUNITS                       "UseUnits"
#define JSON_KEY_RATESTANDARD                   "RateStandard"
#define JSON_KEY_STANDARDUNIT                   "StandardUnit"
#define JSON_KEY_INSTALLDATE                    "InstallDate"
#define JSON_KEY_PRODUCTIONFACTORY              "ProductionFactory"
#define JSON_KEY_METERMAGNIFICATION             "MeterMagnification"
#define JSON_KEY_TRANMETERBASE                  "TranMeterBase"
#define JSON_KEY_METERINITVALUE                 "MeterInitValue"
#define JSON_KEY_TRANMETERVALUE                 "TranMeterValue"
#define JSON_KEY_OLDMETERNUMBER                 "OldMeterNumber"

//设备维修记录
#define JSON_KEY_RECORDINGID                    "recordingID"
#define JSON_KEY_REPAIRCONTENT                  "RepairContent"
#define JSON_KEY_REPAIRTIME                     "RepairTime"
#define JSON_KEY_FAULTREASON                    "FaultReason"
#define JSON_KEY_REPAIRRESULTS                  "RepairResults"
#define JSON_KEY_PEPAIRMANAGER                  "RepairManager"
#define JSON_KEY_REPAIRUNIT                     "RepairUnit"

//设备运行记录
#define JSON_KEY_STARTTIME_                     "StartTime"
#define JSON_KEY_ENDTIME_                       "EndTime"
#define JSON_KEY_OPERATION                      "Operation"
#define JSON_KEY_RECORDER                       "Recorder"

#define JSON_KEY_URL                            "URL"
#define JSON_KEY_CAMERA_NAME                    "CameraName"
#define JSON_KEY_DIR_NAME                       "DirName"

#define JSON_KEY_ITEM_ID                        "ItemID"
#define JSON_KEY_LINKAGE_NAME                   "LinkageName"
#define JSON_KEY_TASK_STATUS                    "TaskStatus"
#define JSON_KEY_VALID_START_TIME               "ValidStartTime"
#define JSON_KEY_VALID_END_TIME                 "ValidEndTime"
#define JSON_KEY_ENABLE                         "Enable"
#define JSON_KEY_MANUAL_CONFIRMATION            "ManualConfirmation"

//设备参数
#define JSON_KEY_TAG_NAME                       "TagName"
#define JSON_KEY_TAG_VALUE                      "TagValue"
#define JSON_KEY_COLLECTIONTIME                 "CollectionTime"
#define JSON_KEY_SUBSTYTEM_NAME                 "SubsystemName"

#define JSON_KEY_PRE_RUN_TIME                   "PreRunTime"
#define JSON_KEY_NEXT_RUN_TIME                  "NextRunTime"
#define JSON_KEY_TERMINATED                     "Terminated"

#define JSON_KEY_Id                             "Id"
#define JSON_KEY_URL                            "URL"
#define JSON_KEY_INPUT_PORT                     "InputPort"
#define JSON_KEY_OUTPUT_PORT                    "OutputPort"
#define JSON_KEY_VIDEO_WIDTH                    "VideoWidth"
#define JSON_KEY_VIDEO_HEIGHT                   "VideoHeight"

//历史报警
#define JSON_KEY_ALARMCATEGORY                  "AlarmCategory"
#define JSON_KEY_ALARMTYPE                      "AlarmType"
#define JSON_KEY_ALARMPRIORITY                  "AlarmPriority"
#define JSON_KEY_ALARMSOURCE                    "AlarmSource"
#define JSON_KEY_ALARMVALUE                     "AlarmValue"
#define JSON_KEY_ALARMMESSAGE                   "AlarmMessage"
#define JSON_KEY_ALARMLIMIT                     "AlarmLimit"
#define JSON_KEY_ALARMACK                       "AlarmACK"

#define JSON_KEY_OVERVIEW_SUBSYSTEM_COUNT       "SystemNum"
#define JSON_KEY_OVERVIEW_DEVICE_COUNT          "DeviceNum"
#define JSON_KEY_OVERVIEW_TAG_COUNT             "TagNum"
#define JSON_KEY_OVERVIEW_ALARM_COUNT           "AlarmNum"
//end add by gaoxin 2017.10.13

class CPackageBase
{
protected:
  bool  OnParseHeader(const Json::Value &jObject, std::string &strSID);
  bool  OnParseGlobalID(const Json::Value &jObject, uint32_t &ulGID);
  bool  OnParseDateTime(const Json::Value &jObject, uint64_t &ullTimeMs);
  bool  OnParseValue(const Json::Value &jObject, boost::any &anyValue);
  bool  OnBuildHeader(const std::string &strSID, ECommand eCmd, EErrCode eEC, Json::Value &jObject);
  bool  OnBuildGlobalID(uint32_t ulGID, Json::Value &jObject);
  bool  OnBuildDateTime(uint64_t ullTimeMs, Json::Value &jObject);
  bool  OnBuildValue(const boost::any &anyValue, Json::Value &jObject);
public:
  CPackageBase();
  ~CPackageBase();
};

#endif // !_PACKAGE_BASE_H_
