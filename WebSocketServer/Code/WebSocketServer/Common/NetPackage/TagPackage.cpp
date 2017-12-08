#include "stdafx.h"
#include "TagPackage.h"


CTagPackage::CTagPackage()
{
}


CTagPackage::~CTagPackage()
{
}

bool CTagPackage::ParseRead(const std::string &strRecv, std::string &strSID, std::list<shared_ptr<KTagInfo>> &lstInfo)
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

  // 获取Tag组
  jArrData = jRoot[JSON_KEY_TAG];
  if (!jArrData.isArray()) return false;
  jIndex = jArrData.size();

  // 获取每个的内容
  shared_ptr<KTagInfo> ptInfo = nullptr;
  for (Json::ArrayIndex i = 0; i < jIndex; i++)
  {
    // isValidIndex 判断索引是否有效
    if (!jArrData.isValidIndex(i)) return false;
    // 实例化对象
    ptInfo = make_shared<KTagInfo>();

    // 获取一个ID项
    if (!OnParseGlobalID(jArrData[i][JSON_KEY_ID], ptInfo->ulGID)) return false;
    lstInfo.push_back(ptInfo);
  }
  return true;
}

bool CTagPackage::ParseWrite(const std::string &strRecv, std::string &strSID, std::list<shared_ptr<KTagInfo>> &lstInfo)
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

  // 获取Tag组
  jArrData = jRoot[JSON_KEY_TAG];
  if (!jArrData.isArray()) return false;
  jIndex = jArrData.size();

  // 获取每个的内容
  shared_ptr<KTagInfo> ptInfo = nullptr;
  for (Json::ArrayIndex i = 0; i < jIndex; i++)
  {
    // isValidIndex 判断索引是否有效
    if (!jArrData.isValidIndex(i)) return false;
    // 实例化对象
    ptInfo = make_shared<KTagInfo>();

    // 获取一个ID项
    if (!OnParseGlobalID(jArrData[i][JSON_KEY_ID], ptInfo->ulGID)) return false;

    // 获取变量类型
    if (!jArrData[i][JSON_KEY_TYPE].isUInt()) return false;
    switch (static_cast<ETagType>(jArrData[i][JSON_KEY_TYPE].asUInt()))
    {
    case TYPE_BOOL:
    {
      if (!jArrData[i][JSON_KEY_VALUE].isBool()) return false;
      ptInfo->anyValue = jArrData[i][JSON_KEY_VALUE].asBool();
    } break;

    case TYPE_INTEGER:
    {
      if (!jArrData[i][JSON_KEY_VALUE].isUInt()) return false;
      ptInfo->anyValue = jArrData[i][JSON_KEY_VALUE].asUInt();
    } break;

    case TYPE_FLOAT:
    {
      if (!jArrData[i][JSON_KEY_VALUE].isDouble()) return false;
      ptInfo->anyValue = (float)jArrData[i][JSON_KEY_VALUE].asDouble();
    } break;

    case TYPE_STRING:
    {
      if (!jArrData[i][JSON_KEY_VALUE].isString()) return false;
      string str = jArrData[i][JSON_KEY_VALUE].asString();
      ptInfo->anyValue = str;
    } break;

    default: return false;
    }

    lstInfo.push_back(ptInfo);
  }

  return true;
}

bool CTagPackage::BuildRead(const std::string &strSID, EErrCode eEC, const std::list<shared_ptr<KTagInfo>> &lstInfo, std::string &strSend)
{
  if (strSID.empty()) return false;

  Json::FastWriter  jWriter;
  Json::Value       jRoot;
  Json::Value       jArrData;
  Json::Value       jValue;

  // 构建包头
  if (!OnBuildHeader(strSID, ECommand::CMD_READ_TAG, eEC, jRoot)) return false;

  // 构建ID组
  for (auto &item : lstInfo)
  {
    jValue.clear();
    // 添加ID值
    if (!OnBuildGlobalID(item->ulGID, jValue[JSON_KEY_ID])) return false;

    // 判断value是否有效
    if (!item->anyValue.empty())
    {
      // 添加value值和类型值
      if (!OnBuildValue(item->anyValue, jValue[JSON_KEY_VALUE])) return false;
      jValue[JSON_KEY_TYPE] = CCommonUnit::Inst()->GetTagType(item->anyValue);
    }

    jArrData.append(jValue);
  }

  // 添加tag组
  jRoot[JSON_KEY_TAG] = jArrData;

  // 转换为字符串
  strSend = jWriter.write(jRoot);
  return true;
}

bool CTagPackage::BuildWrite(const std::string &strSID, EErrCode eEC, const std::list<shared_ptr<KTagInfo>> &lstInfo, std::string &strSend)
{
  if (strSID.empty()) return false;

  Json::FastWriter  jWriter;
  Json::Value       jRoot;
  Json::Value       jArrData;
  Json::Value       jValue;

  // 构建包头
  if (!OnBuildHeader(strSID, ECommand::CMD_WRITE_TAG, eEC, jRoot)) return false;

  // 构建ID组
  for (auto &item : lstInfo)
  {
    jValue.clear();
    // 添加ID值
    if (!OnBuildGlobalID(item->ulGID, jValue[JSON_KEY_ID])) return false;

    jArrData.append(jValue);
  }

  // 添加tag组
  jRoot[JSON_KEY_TAG] = jArrData;

  // 转换为字符串
  strSend = jWriter.write(jRoot);
  return true;
}

bool CTagPackage::BuildPush(const std::string &strSID, EErrCode eEC, const std::list<shared_ptr<KTagInfo>> &lstInfo, std::string &strSend)
{
  if (strSID.empty()) return false;

  Json::FastWriter  jWriter;
  Json::Value       jRoot;
  Json::Value       jArrData;
  Json::Value       jValue;

  // 构建包头
  if (!OnBuildHeader(strSID, ECommand::CMD_PUSH_TAG, eEC, jRoot)) return false;

  // 构建ID组
  for (auto &item : lstInfo)
  {
    jValue.clear();
    // 添加ID值
    if (!OnBuildGlobalID(item->ulGID, jValue[JSON_KEY_ID])) return false;

    // 判断value是否有效
    if (!item->anyValue.empty())
    {
      // 添加time对象中的值
      if (!OnBuildDateTime(item->ullTimeMs, jValue[JSON_KEY_TIME])) return false;

      // 添加value值和类型值
      if (!OnBuildValue(item->anyValue, jValue[JSON_KEY_VALUE])) return false;
      jValue[JSON_KEY_TYPE] = CCommonUnit::Inst()->GetTagType(item->anyValue);
    }

    jArrData.append(jValue);
  }

  // 添加tag组
  jRoot[JSON_KEY_TAG] = jArrData;

  // 转换为字符串
  strSend = jWriter.write(jRoot);
  return true;
}
