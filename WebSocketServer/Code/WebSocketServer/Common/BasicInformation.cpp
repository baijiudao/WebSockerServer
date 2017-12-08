#include "stdafx.h"
#include "BasicInformation.h"
#include <FileVersion/FileVersion.h>

#include <boost/filesystem.hpp>

#include <Logger.h>

namespace _namespace_BasicInformation
{
  const char NODEJS_FOLDER_NAME[] = "nodejs";  // nodejs���ļ�����
}

using namespace std;
using namespace _namespace_BasicInformation;


//------------------------------------------------------------------------------
bool CBasicInformation::OnGenerateHostName(std::string &strHostName)
{
  // ��ȡ������
  const int MAX_BUFFER_LEN = 500;
  char  szBuffer[MAX_BUFFER_LEN] = { 0 };
  DWORD dwNameLen;

  dwNameLen = MAX_BUFFER_LEN;
  if (!GetComputerName(szBuffer, &dwNameLen))
  {
    LOG_WARN << "���������ȡʧ�ܣ�";
    return false;
  }

  strHostName.assign(szBuffer);

  return true;
}

//------------------------------------------------------------------------------
bool CBasicInformation::OnGenerateModulePath(std::string &strModulePathName,
  std::string &strModuleDirName)
{
  // ��ȡ��ǰģ��ȫ·������Ŀ¼��
  char szSelfPathName[MAX_PATH] = { 0 };
  if (0 == ::GetModuleFileName(nullptr, szSelfPathName, MAX_PATH))
  {
    LOG_WARN << "��ǰģ��ȫ·������ȡʧ��...";
    return false;
  }

  // ��ģ�����ŵ�path������
  boost::filesystem::path pathModule = szSelfPathName;
  strModulePathName = pathModule.string();
  strModuleDirName = pathModule.remove_filename().string();

  return true;
}

//------------------------------------------------------------------------------
void CBasicInformation::OnGenerateNodePath(const std::string &strModuleDirName,
  std::string &strNodePathName)
{
  // ��ȡnode.exe��ȫ·����
  boost::filesystem::path pathName = strModuleDirName;
  pathName /= NODEJS_FOLDER_NAME;
  pathName /= "node.exe";

  strNodePathName = pathName.string();
}

//------------------------------------------------------------------------------
void CBasicInformation::OnGenerateJSmpegPath(const std::string &strModuleDirName,
  std::string &strJSmpegPathName)
{
  // ��ȡwebsocket-relay.js��ȫ·����
  boost::filesystem::path pathName = strModuleDirName;
  pathName /= NODEJS_FOLDER_NAME;
  pathName /= "jsmpeg";
  pathName /= "websocket-relay.js";

  strJSmpegPathName = pathName.string();
}

//------------------------------------------------------------------------------
void CBasicInformation::OnGenerateFFmpegPath(const std::string &strModuleDirName,
  std::string &strFFmpegPathName)
{
  // ��ȡffmpeg.exe��ȫ·����
  boost::filesystem::path pathName = strModuleDirName;
  pathName /= NODEJS_FOLDER_NAME;
  pathName /= "ffmpeg";
  pathName /= "ffmpeg.exe";

  strFFmpegPathName = pathName.string();
}

//------------------------------------------------------------------------------
void CBasicInformation::OnGenerateCamConfPath(const std::string &strModuleDirName,
  std::string &strCamConfPathName)
{
  boost::filesystem::path pathName = strModuleDirName;
  pathName /= "camera.json";
  strCamConfPathName = pathName.string();
}

//------------------------------------------------------------------------------
void CBasicInformation::OnGenerateWSSConfPath(const std::string &strModuleDirName,
  std::string &strWSSConfPathName)
{
  boost::filesystem::path pathName = strModuleDirName;
  pathName /= "WSServerConfig.json";
  strWSSConfPathName = pathName.string();
}

//------------------------------------------------------------------------------
// ��ʼ�������ɵ�������Ҫ�Ļ�����Ϣ
//------------------------------------------------------------------------------
bool CBasicInformation::Generate(std::shared_ptr<KBasicInfo> ptBasicInfo)
{
  if (!ptBasicInfo) return false;

  if (!OnGenerateHostName(ptBasicInfo->strHostName)) return false;
  ptBasicInfo->strProductVersion = GetSelfProductVersion();

  if (!OnGenerateModulePath(ptBasicInfo->strModulePathName,
    ptBasicInfo->strModuleDirName)) return false;

  OnGenerateNodePath(ptBasicInfo->strModuleDirName, ptBasicInfo->strNodePathName);
  OnGenerateJSmpegPath(ptBasicInfo->strModuleDirName, ptBasicInfo->strJSmpegPathName);
  OnGenerateFFmpegPath(ptBasicInfo->strModuleDirName, ptBasicInfo->strFFmpegPathName);
  OnGenerateCamConfPath(ptBasicInfo->strModuleDirName, ptBasicInfo->strCamConfPathName);
  OnGenerateWSSConfPath(ptBasicInfo->strModuleDirName, ptBasicInfo->strWSSConfPathName);

  return true;
}
