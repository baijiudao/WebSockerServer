#include "stdafx.h"
#include "EventPackage.h"


CEventPackage::CEventPackage()
{
}


CEventPackage::~CEventPackage()
{
}

bool CEventPackage::ParseWrite(const std::string &strRecv, std::string &strSID, std::list<shared_ptr<KEventInfo>> &lstInfo)
{
  if (strRecv.empty()) return false;

  Json::ArrayIndex  jIndex = 0;
  Json::Reader  jReader;
  Json::Value   jRoot;
  Json::Value   jArrData;
  string  str;

  // ��������
  if (!jReader.parse(strRecv, jRoot)) return false;
  // ������ͷ
  if (!OnParseHeader(jRoot, strSID)) return false;

  // ��ȡEvent��
  jArrData = jRoot[JSON_KEY_EVENT];
  if (!jArrData.isArray()) return false;
  jIndex = jArrData.size();

  // ��ȡÿ��������
  shared_ptr<KEventInfo> ptInfo = nullptr;
  for (Json::ArrayIndex i = 0; i < jIndex; i++)
  {
    // isValidIndex �ж������Ƿ���Ч
    if (!jArrData.isValidIndex(i)) return false;
    // ʵ��������
    ptInfo = make_shared<KEventInfo>();

    // ��ȡʱ����
    if (!OnParseDateTime(jArrData[i][JSON_KEY_TIME], ptInfo->ullTimeMs)) return false;

    // ��ȡ�¼�����
    if (!jArrData[i][JSON_KEY_TYPE].isUInt()) return false;
    ptInfo->eType = jArrData[i][JSON_KEY_TYPE].asUInt();

    // ��ȡ���ȼ�
    if (!jArrData[i][JSON_KEY_ACCESS_LEVEL].isUInt()) return false;
    ptInfo->ulAccessLevel = jArrData[i][JSON_KEY_ACCESS_LEVEL].asUInt();

    // ��ȡ�û���
    if (!jArrData[i][JSON_KEY_USER_NAME].isString()) return false;
    ptInfo->strUserName = jArrData[i][JSON_KEY_USER_NAME].asString();

    // ��ȡ��¼
    if (!jArrData[i][JSON_KEY_RECORD].isString()) return false;
    ptInfo->strRecord = jArrData[i][JSON_KEY_RECORD].asString();

    lstInfo.push_back(ptInfo);
  }

  return true;
}

//------------------------------------------------------------------------------
bool CEventPackage::ParseReadHistory(const std::string &strRecv,
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
  jValue = jRoot[JSON_KEY_EVENT];

  // StartTime
  if (!OnParseDateTime(jValue[JSON_KEY_STARTTIME], ullStartTime)) return false;
  // EndTime
  if (!OnParseDateTime(jValue[JSON_KEY_ENDTIME], ullEndTime)) return false;

  return true;
}

//------------------------------------------------------------------------------
bool CEventPackage::BuildWrite(const std::string &strSID,
  EErrCode eEC, std::string &strSend)
{
  if (strSID.empty()) return false;

  Json::FastWriter  jWriter;
  Json::Value       jRoot;

  // ������ͷ
  if (!OnBuildHeader(strSID, ECommand::CMD_WRITE_EVENT, eEC, jRoot)) return false;

  // ת��Ϊ�ַ���
  strSend = jWriter.write(jRoot);
  return true;
}

//------------------------------------------------------------------------------
bool CEventPackage::BuildReadHistory(const std::string &strSID, EErrCode eEC,
  const std::list<shared_ptr<KEventInfo>> &lstInfo, std::string &strSend)
{
  if (strSID.empty()) return false;

  Json::FastWriter  jWriter;
  Json::Value       jRoot;
  Json::Value       jArrData;
  Json::Value       jValue;

  // ������ͷ
  if (!OnBuildHeader(strSID, ECommand::CMD_READ_HISTORY_EVENT, eEC, jRoot)) return false;

  // ����Alarm��
  for (auto &item : lstInfo)
  {
    jValue.clear();
    // ����Time����
    if (!OnBuildDateTime(item->ullTimeMs, jValue[JSON_KEY_TIME])) return false;

    // EventID
    jValue[JSON_KEY_EVENT_ID] = item->ulID;
    // UserName
    jValue[JSON_KEY_USER_NAME] = item->strUserName;
    // Record
    jValue[JSON_KEY_RECORD] = item->strRecord;
    // AccessLevel
    jValue[JSON_KEY_ACCESS_LEVEL] = item->ulAccessLevel;
    // Type
    jValue[JSON_KEY_TYPE] = item->eType;

    jArrData.append(jValue);
  }

  // ����Alarm����
  jRoot[JSON_KEY_EVENT] = jArrData;

  // ת��Ϊ�ַ���
  strSend = jWriter.write(jRoot);
  return true;
}

//------------------------------------------------------------------------------
bool CEventPackage::BuildPush(const std::string &strSID, EErrCode eEC,
  const std::list<shared_ptr<KEventInfo>> &lstInfo, std::string &strSend)
{
  if (strSID.empty()) return false;

  Json::FastWriter  jWriter;
  Json::Value       jRoot;
  Json::Value       jArrData;
  Json::Value       jValue;

  // ������ͷ
  if (!OnBuildHeader(strSID, ECommand::CMD_PUSH_EVENT, eEC, jRoot)) return false;

  // ����Alarm��
  for (auto &item : lstInfo)
  {
    jValue.clear();
    // ����Time����
    if (!OnBuildDateTime(item->ullTimeMs, jValue[JSON_KEY_TIME])) return false;

    // EventID
    jValue[JSON_KEY_EVENT_ID] = item->ulID;
    // UserName
    jValue[JSON_KEY_USER_NAME] = item->strUserName;
    // Record
    jValue[JSON_KEY_RECORD] = item->strRecord;
    // AccessLevel
    jValue[JSON_KEY_ACCESS_LEVEL] = item->ulAccessLevel;
    // Type
    jValue[JSON_KEY_TYPE] = item->eType;

    jArrData.append(jValue);
  }

  // ����Alarm����
  jRoot[JSON_KEY_EVENT] = jArrData;

  // ת��Ϊ�ַ���
  strSend = jWriter.write(jRoot);
  return true;
}
