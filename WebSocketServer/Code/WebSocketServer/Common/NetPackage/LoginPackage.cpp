#include "stdafx.h"
#include "LoginPackage.h"


CLoginPackage::CLoginPackage()
{
}


CLoginPackage::~CLoginPackage()
{
}

bool CLoginPackage::ParseLogin(const std::string &strRecv, std::string &strUserName, std::string &strPassword)
{
  // 检查数据有效性
  if (strRecv.empty()) return false;

  Json::Reader  jReader;
  Json::Value   jRoot;

  // 解析数据
  if (!jReader.parse(strRecv, jRoot)) return false;

  // 判断数据有效性并获取数据
  // 用户名
  if (!jRoot[JSON_KEY_USER_NAME].isString()) return false;
  strUserName = jRoot[JSON_KEY_USER_NAME].asString();

  // 密码
  if (!jRoot[JSON_KEY_PASSWORD].isString()) return false;
  strPassword = jRoot[JSON_KEY_PASSWORD].asString();

  return true;
}

bool CLoginPackage::ParseLogOut(const std::string &strRecv, std::string &strSID)
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

bool CLoginPackage::BuildLogin(const std::string &strSID, EErrCode eEC, EUserType eType, std::string &strSend)
{
  if (strSID.empty()) return false;

  Json::FastWriter  jWriter;
  Json::Value       jRoot;

  // 构建包头
  if (!OnBuildHeader(strSID, ECommand::CMD_LOGIN, eEC, jRoot)) return false;

  // 添加权限
  jRoot[JSON_KEY_AUTHORITY] = (uint32_t)eType;

  // 转换为字符串
  strSend = jWriter.write(jRoot);
  return true;
}

bool CLoginPackage::BuildLogOut(EErrCode eEC, std::string &strSend)
{
  Json::FastWriter  jWriter;
  Json::Value       jRoot;

  // 添加命令类型
  jRoot[JSON_KEY_COMMAND] = (uint32_t)ECommand::CMD_LOGOUT;
  // 添加错误码
  jRoot[JSON_KEY_STATUS_CODE] = (uint32_t)eEC;

  // 转换为字符串
  strSend = jWriter.write(jRoot);
  return true;
}
