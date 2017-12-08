#include "stdafx.h"
#include "CameraConfig.h"
#include <json/json.h>
#include <iostream>
#include <fstream>

#include <boost/filesystem.hpp>

namespace _namespace_CameraConfig
{
  const char  JSON_KEY_CAMERA[] = "Camera";
  const char  JSON_KEY_ID[] = "Id";
  const char  JSON_KEY_URL[] = "URL";
  const char  JSON_KEY_INPUT_PORT[] = "InputPort";
  const char  JSON_KEY_OUTPUT_PORT[] = "OutputPort";
  const char  JSON_KEY_VIDEO_WIDTH[] = "VideoWidth";
  const char  JSON_KEY_VIDEO_HEIGHT[] = "VideoHeight";
}

using namespace _namespace_CameraConfig;
using namespace std;

//------------------------------------------------------------------------------
// 函数功能：从配置文件中获取所有的摄像头配置信息
// 参数列表：
//      [IN]：strPathName：配置文件路径名
//            lstCamConf：获取到的所有配置信息
// 返 回 值：true :表示成功；  false :表示失败；
// 其他说明：因为WSServer只负责加载这个配置文件，不负责写，因此文件不存在就不读取
//------------------------------------------------------------------------------
bool CCameraConfig::Load(const std::string &strPathName,
  std::list<std::shared_ptr<KCamConf>> &lstCamConf)
{
  // 打开文件
  ifstream  ifstr(strPathName, std::ios::in | std::ios::binary);
  if (ifstr.fail()) return true;

  // 用来处理失败时的操作
  auto  RET_FAIL = [&ifstr]{
    ifstr.close();
    return false;
  };

  Json::ArrayIndex  jIndex = 0;
  Json::Reader  jReader;
  Json::Value   jRoot;
  Json::Value   jArrData;

  if (!jReader.parse(ifstr, jRoot)) return RET_FAIL();
  ifstr.close();

  // 获取camera信息
  jArrData = jRoot[JSON_KEY_CAMERA];
  if (!jArrData.isArray()) return false;

  // 获取配置信息
  jIndex = jArrData.size();
  shared_ptr<KCamConf> ptCamConf = nullptr;
  for (Json::ArrayIndex i = 0; i < jIndex; i++)
  {
    ptCamConf = make_shared<KCamConf>();
    lstCamConf.push_back(ptCamConf);

    // ID
    if (!jArrData[i][JSON_KEY_ID].isUInt()) return false;
    ptCamConf->ulID = jArrData[i][JSON_KEY_ID].asUInt();

    // URL
    if (!jArrData[i][JSON_KEY_URL].isString()) return false;
    ptCamConf->strURL = jArrData[i][JSON_KEY_URL].asString();

    // OutputPort
    if (!jArrData[i][JSON_KEY_INPUT_PORT].isUInt()) return false;
    ptCamConf->sInputPort = jArrData[i][JSON_KEY_INPUT_PORT].asUInt();

    // InputPort
    if (!jArrData[i][JSON_KEY_OUTPUT_PORT].isUInt()) return false;
    ptCamConf->sOutputPort = jArrData[i][JSON_KEY_OUTPUT_PORT].asUInt();

    // VideoWidth
    if (!jArrData[i][JSON_KEY_VIDEO_WIDTH].isUInt()) return false;
    ptCamConf->ulVideoWidth = jArrData[i][JSON_KEY_VIDEO_WIDTH].asUInt();

    // VideoHeight
    if (!jArrData[i][JSON_KEY_VIDEO_HEIGHT].isUInt()) return false;
    ptCamConf->ulVideoHeight = jArrData[i][JSON_KEY_VIDEO_HEIGHT].asUInt();
  }

  return true;
}
