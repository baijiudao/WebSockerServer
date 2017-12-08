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

  // ��������
  if (!jReader.parse(strRecv, jRoot)) return false;
  // ������ͷ
  if (!OnParseHeader(jRoot, strSID)) return false;

  // ��ȡAlarm
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

  // ������ͷ
  if (!OnBuildHeader(strSID, ECommand::CMD_READ_HISTORY_ALARM, eEC, jRoot)) return false;

  // ����Alarm��
  for (auto &item : lstInfo)
  {
    jValue.clear();
    // ����ID�Ķ���
    if (!OnBuildGlobalID(item->ulGID, jValue[JSON_KEY_ID])) return false;
    // ����Time����
    if (!OnBuildDateTime(item->ullTimeMs, jValue[JSON_KEY_TIME])) return false;

    // Alarm����ֵ
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

  // ����Alarm����
  jRoot[JSON_KEY_ALARM] = jArrData;

  // ת��Ϊ�ַ���
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

  // ������ͷ
  if (!OnBuildHeader(strSID, ECommand::CMD_PUSH_ALARM, eEC, jRoot)) return false;

  // ����Alarm��
  for (auto &item : lstInfo)
  {
    jValue.clear();
    // ����ID�Ķ���
    if (!OnBuildGlobalID(item->ulGID, jValue[JSON_KEY_ID])) return false;
    // ����Time����
    if (!OnBuildDateTime(item->ullTimeMs, jValue[JSON_KEY_TIME])) return false;

    // Alarm����ֵ
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

  // ����Alarm����
  jRoot[JSON_KEY_ALARM] = jArrData;

  // ת��Ϊ�ַ���
  strSend = jWriter.write(jRoot);
  return true;
}
