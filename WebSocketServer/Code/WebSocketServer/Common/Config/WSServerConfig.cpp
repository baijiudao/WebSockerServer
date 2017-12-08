#include "stdafx.h"
#include "WSServerConfig.h"
#include <json/json.h>
#include <fstream>
#include "CommonTool.h"
#include <boost/filesystem.hpp>

namespace _namespace_WSServerConfig
{
  const char  JSON_KEY_DATABASE[] = "Database";
  const char  JSON_KEY_USER_NAME[] = "UserName";
  const char  JSON_KEY_PASSWORD[] = "Password";
  const char  JSON_KEY_DSN[] = "DSN";
}

using namespace _namespace_WSServerConfig;
using namespace std;

//------------------------------------------------------------------------------
CWSServerConfig::CWSServerConfig()
{
}

//------------------------------------------------------------------------------
CWSServerConfig::~CWSServerConfig()
{
}

//------------------------------------------------------------------------------
bool CWSServerConfig::Load(const std::string &strPathName,
  std::shared_ptr<KWSSConf> pConfig)
{
  // 判断文件是否存在，且是否为文件
  if (!boost::filesystem::exists(boost::filesystem::path(strPathName)))
  {
    // 配置一个默认配置
    std::shared_ptr<KWSSConf> pDefaultConf = make_shared<KWSSConf>();
    pDefaultConf->ptDBConf->strUserName = "sa";
    pDefaultConf->ptDBConf->strPassword = "123456";
    pDefaultConf->ptDBConf->strDSN = "YLIIS";

    // 保存一个默认配置
    if (!Save(strPathName, pDefaultConf)) return false;
  }

  // 打开文件
  ifstream  ifstr(strPathName, std::ios::in | std::ios::binary);
  if (ifstr.fail()) return false;
  // 用来处理失败时的操作
  auto  RET_FAIL = [&ifstr]{
    ifstr.close();
    return false;
  };

  Json::Reader  jReader;
  Json::Value   jRoot;
  Json::Value   jValue;

  if (!jReader.parse(ifstr, jRoot)) return RET_FAIL();
  ifstr.close();

  // 获取Database信息
  jValue = jRoot[JSON_KEY_DATABASE];
  if (!jValue.isObject()) return false;

  // 获取Database的UserName
  if (!jValue[JSON_KEY_USER_NAME].isString()) return false;
  pConfig->ptDBConf->strUserName = jValue[JSON_KEY_USER_NAME].asString();

  // 获取Database的Password
  if (!jValue[JSON_KEY_PASSWORD].isString()) return false;
  pConfig->ptDBConf->strPassword = jValue[JSON_KEY_PASSWORD].asString();

  // 获取Database的DSN
  if (!jValue[JSON_KEY_DSN].isString()) return false;
  pConfig->ptDBConf->strDSN = jValue[JSON_KEY_DSN].asString();

  return true;
}

//------------------------------------------------------------------------------
bool CWSServerConfig::Save(const std::string &strPathName,
  const std::shared_ptr<KWSSConf> pConfig)
{
  if (!pConfig) return false;

  Json::StyledStreamWriter  jWriter;
  Json::Value               jRoot;
  Json::Value               jValue;

  // Database的UserName
  jValue[JSON_KEY_USER_NAME] = pConfig->ptDBConf->strUserName;
  // Database的Password
  jValue[JSON_KEY_PASSWORD] = pConfig->ptDBConf->strPassword;
  // Database的DSN
  jValue[JSON_KEY_DSN] = pConfig->ptDBConf->strDSN;

  // 创建Database对象
  jRoot[JSON_KEY_DATABASE] = jValue;

  // 打开文件
  ofstream  ofstr(strPathName, std::ios::out | std::ios::binary | std::ios::trunc);
  if (ofstr.fail()) return false;

  // 将数据写出到文件流中
  jWriter.write(ofstr, jRoot);
  ofstr.close();

  return true;
}
