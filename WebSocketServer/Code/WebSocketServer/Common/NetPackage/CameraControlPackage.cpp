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

  // 解析数据
  if (!jReader.parse(strRecv, jRoot)) return false;
  // 解析包头
  if (!OnParseHeader(jRoot, strSID)) return false;

  // 判断数据有效性并获取数据
  // URL
  if (!jRoot[JSON_KEY_URL].isString()) return false;
  ptInfo->strURL = jRoot[JSON_KEY_URL].asString();

  // Camera Name
  if (!jRoot[JSON_KEY_CAMERA_NAME].isString()) return false;
  ptInfo->strCameraName = jRoot[JSON_KEY_CAMERA_NAME].asString();

  // 截图保存路径名
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

  // 解析数据
  if (!jReader.parse(strRecv, jRoot)) return false;
  // 解析包头
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

  // 构建包头
  if (!OnBuildHeader(strSID, ECommand::CMD_CAMERA_SCREENSHOT, eEC, jRoot)) return false;

  // 转换为字符串
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

  // 构建包头
  if (!OnBuildHeader(strSID, ECommand::CMD_SEARCH_CAMERA_CONFIG, eEC, jRoot)) return false;

  // 构建ID组
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

  // 添加data组
  jRoot[JSON_KEY_DATA] = jArrData;

  // 转换为字符串
  strSend = jWriter.write(jRoot);
  return true;
}
