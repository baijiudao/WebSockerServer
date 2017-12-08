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
// �������ܣ��������ļ��л�ȡ���е�����ͷ������Ϣ
// �����б�
//      [IN]��strPathName�������ļ�·����
//            lstCamConf����ȡ��������������Ϣ
// �� �� ֵ��true :��ʾ�ɹ���  false :��ʾʧ�ܣ�
// ����˵������ΪWSServerֻ���������������ļ���������д������ļ������ھͲ���ȡ
//------------------------------------------------------------------------------
bool CCameraConfig::Load(const std::string &strPathName,
  std::list<std::shared_ptr<KCamConf>> &lstCamConf)
{
  // ���ļ�
  ifstream  ifstr(strPathName, std::ios::in | std::ios::binary);
  if (ifstr.fail()) return true;

  // ��������ʧ��ʱ�Ĳ���
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

  // ��ȡcamera��Ϣ
  jArrData = jRoot[JSON_KEY_CAMERA];
  if (!jArrData.isArray()) return false;

  // ��ȡ������Ϣ
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
