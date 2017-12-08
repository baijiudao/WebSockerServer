#include "stdafx.h"
#include "CameraControlPackage.h"


//------------------------------------------------------------------------------
CCameraControlPackage::CCameraControlPackage()
{
}

//------------------------------------------------------------------------------
CCameraControlPackage::~CCameraControlPackage()
{
}

//------------------------------------------------------------------------------
bool CCameraControlPackage::ParseScreenshot(const std::string &strRecv,
  std::string &strSID, std::shared_ptr<KCamScreenshotInfo> ptInfo)
{
  if (strRecv.empty() || (!ptInfo)) return false;

  Json::Reader  jReader;
  Json::Value   jRoot;

  // ��������
  if (!jReader.parse(strRecv, jRoot)) return false;
  // ������ͷ
  if (!OnParseHeader(jRoot, strSID)) return false;

  // �ж�������Ч�Բ���ȡ����
  // URL
  if (!jRoot[JSON_KEY_URL].isString()) return false;
  ptInfo->strURL = jRoot[JSON_KEY_URL].asString();

  // Camera Name
  if (!jRoot[JSON_KEY_CAMERA_NAME].isString()) return false;
  ptInfo->strCameraName = jRoot[JSON_KEY_CAMERA_NAME].asString();

  // ��ͼ����·����
  if (!jRoot[JSON_KEY_DIR_NAME].isString()) return false;
  ptInfo->strDirName = jRoot[JSON_KEY_DIR_NAME].asString();

  return true;
}

//------------------------------------------------------------------------------
bool CCameraControlPackage::ParseSearchConfig(const std::string &strRecv,
  std::string &strSID)
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
bool CCameraControlPackage::BuildScreenshot(const std::string &strSID,
  EErrCode eEC, std::string &strSend)
{
  if (strSID.empty()) return false;

  Json::FastWriter  jWriter;
  Json::Value       jRoot;

  // ������ͷ
  if (!OnBuildHeader(strSID, ECommand::CMD_CAMERA_SCREENSHOT, eEC, jRoot)) return false;

  // ת��Ϊ�ַ���
  strSend = jWriter.write(jRoot);
  return true;
}

//------------------------------------------------------------------------------
bool CCameraControlPackage::BuildSearchConfig(const std::string &strSID,
  EErrCode eEC, const std::list<std::shared_ptr<KCamConf>> &lstCamConf,
  std::string &strSend)
{
  if (strSID.empty()) return false;

  Json::FastWriter  jWriter;
  Json::Value       jRoot;
  Json::Value       jArrData;
  Json::Value       jValue;

  // ������ͷ
  if (!OnBuildHeader(strSID, ECommand::CMD_SEARCH_CAMERA_CONFIG, eEC, jRoot)) return false;

  // ����ID��
  for (auto &item : lstCamConf)
  {
    jValue.clear();
    jValue[JSON_KEY_Id] = item->ulID;
    jValue[JSON_KEY_URL] = item->strURL;
    jValue[JSON_KEY_INPUT_PORT] = item->sInputPort;
    jValue[JSON_KEY_OUTPUT_PORT] = item->sOutputPort;
    jValue[JSON_KEY_VIDEO_WIDTH] = item->ulVideoWidth;
    jValue[JSON_KEY_VIDEO_HEIGHT] = item->ulVideoHeight;
    jArrData.append(jValue);
  }

  // ���data��
  jRoot[JSON_KEY_DATA] = jArrData;

  // ת��Ϊ�ַ���
  strSend = jWriter.write(jRoot);
  return true;
}
