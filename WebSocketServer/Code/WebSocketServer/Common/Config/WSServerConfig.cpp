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
  // �ж��ļ��Ƿ���ڣ����Ƿ�Ϊ�ļ�
  if (!boost::filesystem::exists(boost::filesystem::path(strPathName)))
  {
    // ����һ��Ĭ������
    std::shared_ptr<KWSSConf> pDefaultConf = make_shared<KWSSConf>();
    pDefaultConf->ptDBConf->strUserName = "sa";
    pDefaultConf->ptDBConf->strPassword = "123456";
    pDefaultConf->ptDBConf->strDSN = "YLIIS";

    // ����һ��Ĭ������
    if (!Save(strPathName, pDefaultConf)) return false;
  }

  // ���ļ�
  ifstream  ifstr(strPathName, std::ios::in | std::ios::binary);
  if (ifstr.fail()) return false;
  // ��������ʧ��ʱ�Ĳ���
  auto  RET_FAIL = [&ifstr]{
    ifstr.close();
    return false;
  };

  Json::Reader  jReader;
  Json::Value   jRoot;
  Json::Value   jValue;

  if (!jReader.parse(ifstr, jRoot)) return RET_FAIL();
  ifstr.close();

  // ��ȡDatabase��Ϣ
  jValue = jRoot[JSON_KEY_DATABASE];
  if (!jValue.isObject()) return false;

  // ��ȡDatabase��UserName
  if (!jValue[JSON_KEY_USER_NAME].isString()) return false;
  pConfig->ptDBConf->strUserName = jValue[JSON_KEY_USER_NAME].asString();

  // ��ȡDatabase��Password
  if (!jValue[JSON_KEY_PASSWORD].isString()) return false;
  pConfig->ptDBConf->strPassword = jValue[JSON_KEY_PASSWORD].asString();

  // ��ȡDatabase��DSN
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

  // Database��UserName
  jValue[JSON_KEY_USER_NAME] = pConfig->ptDBConf->strUserName;
  // Database��Password
  jValue[JSON_KEY_PASSWORD] = pConfig->ptDBConf->strPassword;
  // Database��DSN
  jValue[JSON_KEY_DSN] = pConfig->ptDBConf->strDSN;

  // ����Database����
  jRoot[JSON_KEY_DATABASE] = jValue;

  // ���ļ�
  ofstream  ofstr(strPathName, std::ios::out | std::ios::binary | std::ios::trunc);
  if (ofstr.fail()) return false;

  // ������д�����ļ�����
  jWriter.write(ofstr, jRoot);
  ofstr.close();

  return true;
}
