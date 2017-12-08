#include "stdafx.h"
#include "LinkagePackage.h"


//------------------------------------------------------------------------------
CLinkagePackage::CLinkagePackage()
{
}

//------------------------------------------------------------------------------
CLinkagePackage::~CLinkagePackage()
{
}

//------------------------------------------------------------------------------
bool CLinkagePackage::ParseReadWatchInfo(const std::string &strRecv, std::string &strSID)
{
  if (strRecv.empty()) return false;

  Json::Reader  jReader;
  Json::Value   jRoot;

  // ��������
  if (!jReader.parse(strRecv, jRoot)) return false;
  // ������ͷ
  if (!OnParseHeader(jRoot, strSID)) return false;

  return true;
}

//------------------------------------------------------------------------------
bool CLinkagePackage::ParseManualConfirmation(const std::string &strRecv,
  std::string &strSID, uint32_t &ulItemID)
{
  if (strRecv.empty()) return false;

  Json::Reader  jReader;
  Json::Value   jRoot;

  // ��������
  if (!jReader.parse(strRecv, jRoot)) return false;
  // ������ͷ
  if (!OnParseHeader(jRoot, strSID)) return false;

  // ItemID
  if (!jRoot[JSON_KEY_ITEM_ID].isUInt()) return false;
  ulItemID = jRoot[JSON_KEY_ITEM_ID].asUInt();

  return true;
}

//------------------------------------------------------------------------------
bool CLinkagePackage::ParseTerminateTask(const std::string &strRecv,
  std::string &strSID, uint32_t &ulItemID)
{
  if (strRecv.empty()) return false;

  Json::Reader  jReader;
  Json::Value   jRoot;

  // ��������
  if (!jReader.parse(strRecv, jRoot)) return false;
  // ������ͷ
  if (!OnParseHeader(jRoot, strSID)) return false;

  // ItemID
  if (!jRoot[JSON_KEY_ITEM_ID].isUInt()) return false;
  ulItemID = jRoot[JSON_KEY_ITEM_ID].asUInt();

  return true;
}

//------------------------------------------------------------------------------
bool CLinkagePackage::BuildReadWatchInfo(const std::string &strSID, EErrCode eEC,
  const std::list<shared_ptr<KLinkageWatchInfo>> &lstInfo, std::string &strSend)
{
  if (strSID.empty()) return false;

  Json::FastWriter  jWriter;
  Json::Value       jRoot;
  Json::Value       jArrData;
  Json::Value       jValue;

  // ������ͷ
  if (!OnBuildHeader(strSID, ECommand::CMD_READ_LINKAGE_WATCH_INFO, eEC, jRoot)) return false;

  // ��Ӳ�ѯ��������
  for (auto &item : lstInfo)
  {
    jValue.clear();

    jValue[JSON_KEY_ITEM_ID] = item->ulItemID;
    jValue[JSON_KEY_LINKAGE_NAME] = item->strName;
    jValue[JSON_KEY_TASK_STATUS] = item->ulTaskStatus;
    jValue[JSON_KEY_VALID_START_TIME] = item->ulValidStartTime;
    jValue[JSON_KEY_VALID_END_TIME] = item->ulValidEndTime;
    jValue[JSON_KEY_ENABLE] = item->bEnable;
    jValue[JSON_KEY_MANUAL_CONFIRMATION] = item->bManualConfirmation;

    jArrData.append(jValue);
  }

  // ���data��
  jRoot[JSON_KEY_DATA] = jArrData;

  // ת��Ϊ�ַ���
  strSend = jWriter.write(jRoot);
  return true;
}

//------------------------------------------------------------------------------
bool CLinkagePackage::BuildManualConfirmation(const std::string &strSID,
  EErrCode eEC, std::string &strSend)
{
  if (strSID.empty()) return false;

  Json::FastWriter  jWriter;
  Json::Value       jRoot;

  // ������ͷ
  if (!OnBuildHeader(strSID, ECommand::CMD_LINKAGE_MANUAL_CONFIRMATION, eEC, jRoot)) return false;

  // ת��Ϊ�ַ���
  strSend = jWriter.write(jRoot);
  return true;
}

//------------------------------------------------------------------------------
bool CLinkagePackage::BuildTerminateTask(const std::string &strSID,
  EErrCode eEC, std::string &strSend)
{
  if (strSID.empty()) return false;

  Json::FastWriter  jWriter;
  Json::Value       jRoot;

  // ������ͷ
  if (!OnBuildHeader(strSID, ECommand::CMD_LINKAGE_TERMINATE_TASK, eEC, jRoot)) return false;

  // ת��Ϊ�ַ���
  strSend = jWriter.write(jRoot);
  return true;
}
