#include "stdafx.h"
#include "CheckStatusPackage.h"

//------------------------------------------------------------------------------
CCheckStatusPackage::CCheckStatusPackage()
{
}

//------------------------------------------------------------------------------
CCheckStatusPackage::~CCheckStatusPackage()
{
}

//------------------------------------------------------------------------------
bool CCheckStatusPackage::ParseDB(const std::string &strRecv, std::string &strSID)
{
  if (strRecv.empty()) return false;

  Json::Reader  jReader;
  Json::Value   jRoot;

  // 解析数据
  if (!jReader.parse(strRecv, jRoot)) return false;
  // 解析包头
  if (!OnParseHeader(jRoot, strSID)) return false;

  return true;
}

//------------------------------------------------------------------------------
bool CCheckStatusPackage::ParseOPC(const std::string &strRecv, std::string &strSID)
{
  if (strRecv.empty()) return false;

  Json::Reader  jReader;
  Json::Value   jRoot;

  // 解析数据
  if (!jReader.parse(strRecv, jRoot)) return false;
  // 解析包头
  if (!OnParseHeader(jRoot, strSID)) return false;

  return true;
}

//------------------------------------------------------------------------------
bool CCheckStatusPackage::BuildDB(const std::string &strSID, EErrCode eEC,
  const std::list<std::shared_ptr<KDBStatusInfo>> &lstInfo, std::string &strSend)
{
  if (strSID.empty()) return false;

  Json::FastWriter  jWriter;
  Json::Value       jRoot;
  Json::Value       jArrData;
  Json::Value       jValue;

  // 构建包头
  if (!OnBuildHeader(strSID, ECommand::CMD_CHECK_DB_STATUS, eEC, jRoot)) return false;

  // 构建ID组
  for (auto &item : lstInfo)
  {
    jValue.clear();
    jValue[JSON_KEY_DESCRIBE] = item->strDescribe;
    jValue[JSON_KEY_STATUS] = static_cast<uint32_t>(item->eStatus);
    jArrData.append(jValue);
  }

  // 添加data组
  jRoot[JSON_KEY_DATA] = jArrData;

  // 转换为字符串
  strSend = jWriter.write(jRoot);
  return true;
}

//------------------------------------------------------------------------------
bool CCheckStatusPackage::BuildOPC(const std::string &strSID, EErrCode eEC,
  const std::list<std::shared_ptr<KOPCStatusInfo>> &lstInfo, std::string &strSend)
{
  if (strSID.empty()) return false;

  Json::FastWriter  jWriter;
  Json::Value       jRoot;
  Json::Value       jArrData;
  Json::Value       jValue;

  // 构建包头
  if (!OnBuildHeader(strSID, ECommand::CMD_CHECK_OPC_STATUS, eEC, jRoot)) return false;

  // 构建ID组
  for (auto &item : lstInfo)
  {
    jValue.clear();
    jValue[JSON_KEY_ID] = item->ulID;
    jValue[JSON_KEY_NAME] = item->strName;
    jValue[JSON_KEY_STATUS] = static_cast<uint32_t>(item->eStatus);
    jArrData.append(jValue);
  }

  // 添加data组
  jRoot[JSON_KEY_DATA] = jArrData;

  // 转换为字符串
  strSend = jWriter.write(jRoot);
  return true;
}
