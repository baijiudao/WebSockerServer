#include "stdafx.h"
#include "AlarmPackage.h"


CAlarmPackage::CAlarmPackage()
{
}


CAlarmPackage::~CAlarmPackage()
{
}

//------------------------------------------------------------------------------
bool CAlarmPackage::ParseReadHistory(const std::string &strRecv,
  std::string &strSID, uint64_t &ullStartTime, uint64_t &ullEndTime)
{
  if (strRecv.empty()) return false;

  Json::Reader  jReader;
  Json::Value   jRoot;
  Json::Value   jValue;

  // 解析数据
  if (!jReader.parse(strRecv, jRoot)) return false;
  // 解析包头
  if (!OnParseHeader(jRoot, strSID)) return false;

  // 获取Alarm
  jValue = jRoot[JSON_KEY_ALARM];

  // StartTime
  if (!OnParseDateTime(jValue[JSON_KEY_STARTTIME], ullStartTime)) return false;
  // EndTime
  if (!OnParseDateTime(jValue[JSON_KEY_ENDTIME], ullEndTime)) return false;

  return true;
}

//------------------------------------------------------------------------------
bool CAlarmPackage::BuildReadHistory(const std::string &strSID, EErrCode eEC,
  const std::list<shared_ptr<KAlarmInfo>> &lstInfo, std::string &strSend)
{
  if (strSID.empty()) return false;

  Json::FastWriter  jWriter;
  Json::Value       jRoot;
  Json::Value       jArrData;
  Json::Value       jValue;

  // 构建包头
  if (!OnBuildHeader(strSID, ECommand::CMD_READ_HISTORY_ALARM, eEC, jRoot)) return false;

  // 构建Alarm组
  for (auto &item : lstInfo)
  {
    jValue.clear();
    // 创建ID的对象
    if (!OnBuildGlobalID(item->ulGID, jValue[JSON_KEY_ID])) return false;
    // 创建Time对象
    if (!OnBuildDateTime(item->ullTimeMs, jValue[JSON_KEY_TIME])) return false;

    // Alarm其他值
    jValue[JSON_KEY_ALARM_ID] = item->ulID;
    jValue[JSON_KEY_USER_NAME] = item->strUserName;
    jValue[JSON_KEY_CATEGORY] = item->ulCategory;
    jValue[JSON_KEY_DESCRIBE] = item->strArea;
    jValue[JSON_KEY_ALARM_TAG] = item->strDescribe;
    jValue[JSON_KEY_CURRENT_VALUE] = item->strTag;
    jValue[JSON_KEY_PRIORITY] = item->ulPriority;
    jValue[JSON_KEY_ALARM_EVENT] = item->strAlarmEvent;
    jValue[JSON_KEY_ACK] = item->bAck;

    jArrData.append(jValue);
  }

  // 创建Alarm对象
  jRoot[JSON_KEY_ALARM] = jArrData;

  // 转换为字符串
  strSend = jWriter.write(jRoot);
  return true;
}

//------------------------------------------------------------------------------
bool CAlarmPackage::BuildPush(const std::string &strSID, EErrCode eEC,
  const std::list<shared_ptr<KAlarmInfo>> &lstInfo, std::string &strSend)
{
  if (strSID.empty()) return false;

  Json::FastWriter  jWriter;
  Json::Value       jRoot;
  Json::Value       jArrData;
  Json::Value       jValue;

  // 构建包头
  if (!OnBuildHeader(strSID, ECommand::CMD_PUSH_ALARM, eEC, jRoot)) return false;

  // 构建Alarm组
  for (auto &item : lstInfo)
  {
    jValue.clear();
    // 创建ID的对象
    if (!OnBuildGlobalID(item->ulGID, jValue[JSON_KEY_ID])) return false;
    // 创建Time对象
    if (!OnBuildDateTime(item->ullTimeMs, jValue[JSON_KEY_TIME])) return false;

    // Alarm其他值
    jValue[JSON_KEY_ALARM_ID] = item->ulID;
    jValue[JSON_KEY_USER_NAME] = item->strUserName;
    jValue[JSON_KEY_CATEGORY] = item->ulCategory;
    jValue[JSON_KEY_DESCRIBE] = item->strArea;
    jValue[JSON_KEY_ALARM_TAG] = item->strDescribe;
    jValue[JSON_KEY_CURRENT_VALUE] = item->strTag;
    jValue[JSON_KEY_PRIORITY] = item->ulPriority;
    jValue[JSON_KEY_ALARM_EVENT] = item->strAlarmEvent;
    jValue[JSON_KEY_ACK] = item->bAck;

    jArrData.append(jValue);
  }

  // 创建Alarm对象
  jRoot[JSON_KEY_ALARM] = jArrData;

  // 转换为字符串
  strSend = jWriter.write(jRoot);
  return true;
}
