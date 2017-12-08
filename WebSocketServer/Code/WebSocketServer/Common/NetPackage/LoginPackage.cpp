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
  // ���������Ч��
  if (strRecv.empty()) return false;

  Json::Reader  jReader;
  Json::Value   jRoot;

  // ��������
  if (!jReader.parse(strRecv, jRoot)) return false;

  // �ж�������Ч�Բ���ȡ����
  // �û���
  if (!jRoot[JSON_KEY_USER_NAME].isString()) return false;
  strUserName = jRoot[JSON_KEY_USER_NAME].asString();

  // ����
  if (!jRoot[JSON_KEY_PASSWORD].isString()) return false;
  strPassword = jRoot[JSON_KEY_PASSWORD].asString();

  return true;
}

bool CLoginPackage::ParseLogOut(const std::string &strRecv, std::string &strSID)
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

bool CLoginPackage::BuildLogin(const std::string &strSID, EErrCode eEC, EUserType eType, std::string &strSend)
{
  if (strSID.empty()) return false;

  Json::FastWriter  jWriter;
  Json::Value       jRoot;

  // ������ͷ
  if (!OnBuildHeader(strSID, ECommand::CMD_LOGIN, eEC, jRoot)) return false;

  // ���Ȩ��
  jRoot[JSON_KEY_AUTHORITY] = (uint32_t)eType;

  // ת��Ϊ�ַ���
  strSend = jWriter.write(jRoot);
  return true;
}

bool CLoginPackage::BuildLogOut(EErrCode eEC, std::string &strSend)
{
  Json::FastWriter  jWriter;
  Json::Value       jRoot;

  // �����������
  jRoot[JSON_KEY_COMMAND] = (uint32_t)ECommand::CMD_LOGOUT;
  // ��Ӵ�����
  jRoot[JSON_KEY_STATUS_CODE] = (uint32_t)eEC;

  // ת��Ϊ�ַ���
  strSend = jWriter.write(jRoot);
  return true;
}
