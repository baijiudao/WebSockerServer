#include "stdafx.h"
#include "PackageBase.h"


CPackageBase::CPackageBase()
{
}


CPackageBase::~CPackageBase()
{
}

//------------------------------------------------------------------------------
// 解析数据包包头
//------------------------------------------------------------------------------
bool CPackageBase::OnParseHeader(const Json::Value &jObject,
  std::string &strSID)
{
  if (!jObject.isObject()) return false;

  // 获取SID
  Json::Value jValue = jObject[JSON_KEY_SESSION_ID];
  if (!jValue.isString()) return false;
  strSID = jValue.asString();

  return true;
}

//------------------------------------------------------------------------------
bool CPackageBase::OnParseGlobalID(const Json::Value &jObject, uint32_t &ulGID)
{
  uint32_t  tmp = 0;

  // 获取ID数据
  if (!jObject[JSON_KEY_SUBSYSTEM_ID].isUInt()) return false;
  tmp |= (jObject[JSON_KEY_SUBSYSTEM_ID].asUInt() << 24);

  if (!jObject[JSON_KEY_DEVICE_ID].isUInt()) return false;
  tmp |= (jObject[JSON_KEY_DEVICE_ID].asUInt() << 16);

  if (!jObject[JSON_KEY_TAG_ID].isUInt()) return false;
  tmp |= jObject[JSON_KEY_TAG_ID].asUInt();

  ulGID = tmp;
  return true;
}

//------------------------------------------------------------------------------
bool CPackageBase::OnParseDateTime(const Json::Value &jObject, uint64_t &ullTimeMs)
{
  struct tm tmTmp;
  uint64_t  tmp = 0;

  // 获取DateTime数据
  if (!jObject[JSON_KEY_YEAR].isUInt()) return false;
  tmTmp.tm_year = jObject[JSON_KEY_YEAR].asUInt() - 1900;

  if (!jObject[JSON_KEY_MONTH].isUInt()) return false;
  tmTmp.tm_mon = jObject[JSON_KEY_MONTH].asUInt() - 1;

  if (!jObject[JSON_KEY_DAY].isUInt()) return false;
  tmTmp.tm_mday = jObject[JSON_KEY_DAY].asUInt();

  if (!jObject[JSON_KEY_HOUR].isUInt()) return false;
  tmTmp.tm_hour = jObject[JSON_KEY_HOUR].asUInt();

  if (!jObject[JSON_KEY_MINUTE].isUInt()) return false;
  tmTmp.tm_min = jObject[JSON_KEY_MINUTE].asUInt();

  if (!jObject[JSON_KEY_SECOND].isUInt()) return false;
  tmTmp.tm_sec = jObject[JSON_KEY_SECOND].asUInt();

  tmp = mktime(&tmTmp);
  tmp *= 1000;  // 转换为毫秒

  if (!jObject[JSON_KEY_MINSEC].isUInt()) return false;
  tmp += jObject[JSON_KEY_MINSEC].asUInt();

  ullTimeMs = tmp;
  return true;
}

//------------------------------------------------------------------------------
bool CPackageBase::OnParseValue(const Json::Value &jObject, boost::any &anyValue)
{
  return false;
}

//------------------------------------------------------------------------------
// 构建数据包包头
//------------------------------------------------------------------------------
bool CPackageBase::OnBuildHeader(const std::string &strSID,
  ECommand eCmd, EErrCode eEC, Json::Value &jObject)
{
  if (strSID.empty()) return false;

  // 添加命令类型
  jObject[JSON_KEY_COMMAND] = (uint32_t)eCmd;

  // 添加SID
  jObject[JSON_KEY_SESSION_ID] = strSID;

  // 添加错误码
  jObject[JSON_KEY_STATUS_CODE] = (uint32_t)eEC;


  auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  struct tm* ptm = localtime(&tt);
  char date[60] = { 0 };
  sprintf_s(date, "%d-%02d-%02d      %02d:%02d:%02d",
    (int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, (int)ptm->tm_mday,
    (int)ptm->tm_hour, (int)ptm->tm_min, (int)ptm->tm_sec);

  // 临时添加一个时间
  jObject["CurTime"] = date;

  return true;
}

//------------------------------------------------------------------------------
bool CPackageBase::OnBuildGlobalID(uint32_t ulGID, Json::Value &jObject)
{
  jObject[JSON_KEY_SUBSYSTEM_ID] = ((ulGID >> 24) & 0x0FF);
  jObject[JSON_KEY_DEVICE_ID] = ((ulGID >> 16) & 0x0FF);
  jObject[JSON_KEY_TAG_ID] = (ulGID & 0x0FFFF);
  return true;
}

//------------------------------------------------------------------------------
bool CPackageBase::OnBuildDateTime(uint64_t ullTimeMs, Json::Value &jObject)
{
  uint64_t  ullM = ullTimeMs / 1000;
  struct tm *ptmTmp = localtime((const time_t*)&ullM);

  jObject[JSON_KEY_YEAR] = ptmTmp->tm_year + 1900;
  jObject[JSON_KEY_MONTH] = ptmTmp->tm_mon + 1;
  jObject[JSON_KEY_DAY] = ptmTmp->tm_mday;
  jObject[JSON_KEY_HOUR] = ptmTmp->tm_hour;
  jObject[JSON_KEY_MINUTE] = ptmTmp->tm_min;
  jObject[JSON_KEY_SECOND] = ptmTmp->tm_sec;
  jObject[JSON_KEY_MINSEC] = ullTimeMs % 1000;

  return true;
}

//------------------------------------------------------------------------------
bool CPackageBase::OnBuildValue(const boost::any &anyValue, Json::Value &jObject)
{
  try
  {
    switch (CCommonUnit::Inst()->GetTagType(anyValue))
    {
    case TYPE_BOOL:   jObject = boost::any_cast<bool>(anyValue); break;
    case TYPE_INTEGER:jObject = boost::any_cast<uint32_t>(anyValue); break;
    case TYPE_FLOAT:  jObject = boost::any_cast<float>(anyValue); break;
    case TYPE_STRING: jObject = boost::any_cast<string>(anyValue); break;
    default:return false;
    }
  }
  catch (boost::bad_any_cast &e)
  {
    LOG_ERROR << e.what();
    return false;
  }

  return true;
}
