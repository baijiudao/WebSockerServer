#include "stdafx.h"
#include "TimetableDispatchPackage.h"


//------------------------------------------------------------------------------
CTimetableDispatchPackage::CTimetableDispatchPackage()
{
}

//------------------------------------------------------------------------------
CTimetableDispatchPackage::~CTimetableDispatchPackage()
{
}

//------------------------------------------------------------------------------
bool CTimetableDispatchPackage::ParseReadWatchInfo(const std::string &strRecv,
  std::string &strSID)
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
bool CTimetableDispatchPackage::ParseTerminateTask(const std::string &strRecv,
  std::string &strSID, uint32_t &ulItemID)
{
  if (strRecv.empty()) return false;

  Json::Reader  jReader;
  Json::Value   jRoot;

  // 解析数据
  if (!jReader.parse(strRecv, jRoot)) return false;
  // 解析包头
  if (!OnParseHeader(jRoot, strSID)) return false;

  // ItemID
  if (!jRoot[JSON_KEY_ITEM_ID].isUInt()) return false;
  ulItemID = jRoot[JSON_KEY_ITEM_ID].asUInt();

  return true;
}

//------------------------------------------------------------------------------
bool CTimetableDispatchPackage::BuildReadWatchInfo(const std::string &strSID,
  EErrCode eEC, const std::list<shared_ptr<KTimetableDispatchWatchInfo>> &lstInfo,
  std::string &strSend)
{
  if (strSID.empty()) return false;

  Json::FastWriter  jWriter;
  Json::Value       jRoot;
  Json::Value       jArrData;
  Json::Value       jValue;

  // 构建包头
  if (!OnBuildHeader(strSID, ECommand::CMD_READ_TIMETABLE_DISPATCH_WATCH_INFO,
    eEC, jRoot)) return false;

  // 添加查询到的数据
  for (auto &item : lstInfo)
  {
    jValue.clear();

    jValue[JSON_KEY_ITEM_ID] = item->ulItemID;
    jValue[JSON_KEY_NAME] = item->strName;
    if (!OnBuildDateTime(item->ullPreRunTime, jValue[JSON_KEY_PRE_RUN_TIME])) return false;
    if (!OnBuildDateTime(item->ullNextRunTime, jValue[JSON_KEY_NEXT_RUN_TIME])) return false;
    jValue[JSON_KEY_ENABLE] = item->bEnable;
    jValue[JSON_KEY_TERMINATED] = item->bTerminated;

    jArrData.append(jValue);
  }

  // 添加data组
  jRoot[JSON_KEY_DATA] = jArrData;

  // 转换为字符串
  strSend = jWriter.write(jRoot);
  return true;
}

//------------------------------------------------------------------------------
bool CTimetableDispatchPackage::BuildTerminateTask(const std::string &strSID,
  EErrCode eEC, std::string &strSend)
{
  if (strSID.empty()) return false;

  Json::FastWriter  jWriter;
  Json::Value       jRoot;

  // 构建包头
  if (!OnBuildHeader(strSID, ECommand::CMD_TIMETABLE_DISPATCH_TERMINATE_TASK, eEC, jRoot)) return false;

  // 转换为字符串
  strSend = jWriter.write(jRoot);
  return true;
}
