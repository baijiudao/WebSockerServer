#include "stdafx.h"
#include "CameraOperate.h"

#include <iostream>
#include <sstream>
#include <boost/filesystem.hpp>
#include <Logger.h>
#include "CommonTool.h"

namespace _namespace_CCameraOperate
{
  const char NODEJS_FOLDER_NAME[] = "nodejs";  // nodejs���ļ�����
}

using namespace std;
using namespace _namespace_CCameraOperate;

//------------------------------------------------------------------------------
CCameraOperate::CCameraOperate()
{
}

//------------------------------------------------------------------------------
CCameraOperate::~CCameraOperate()
{
  // �ر���������ͷ��������
  OnStop();
}

//------------------------------------------------------------------------------
// �������ܣ���������һ����ִ�г��򣬲������������ʾ״̬
// �����б�
//      [IN]��strExe��Ҫ���еĿ�ִ�г���
//            strParam��Ҫ����Ĳ���
//            nShow����ʾģʽ���磺SW_SHOW��
// �� �� ֵ��true :��ʾ�ɹ���  false :��ʾʧ�ܣ�
// ����˵����
//------------------------------------------------------------------------------
bool CCameraOperate::ShellRun(const std::string &strExe,
  const std::string &strParam, DWORD nShow)
{
  shared_ptr<SHELLEXECUTEINFO>  pShExecInfo = make_shared<SHELLEXECUTEINFO>();
  pShExecInfo->cbSize = sizeof(SHELLEXECUTEINFO);
  pShExecInfo->fMask = SEE_MASK_NOCLOSEPROCESS;
  pShExecInfo->hwnd = NULL;
  pShExecInfo->lpVerb = NULL;
  pShExecInfo->lpFile = strExe.c_str(); //can be a file as well
  pShExecInfo->lpParameters = strParam.c_str();
  pShExecInfo->lpDirectory = NULL;
  pShExecInfo->nShow = nShow;
  pShExecInfo->hInstApp = NULL;

  // ��������
  ShellExecuteEx(pShExecInfo.get());

  // ������������Ϣ��ӵ��б���
  m_lstProc.push_back(pShExecInfo);
  return true;
}

//------------------------------------------------------------------------------
// ֹͣ�Ѿ���������������ͷ
//------------------------------------------------------------------------------
void CCameraOperate::OnStop()
{
  // �ر����еĽ���
  for (auto iter = m_lstProc.begin(); iter != m_lstProc.end(); iter++)
  {
    TerminateProcess((*iter)->hProcess, 0);
    CloseHandle((*iter)->hProcess);

    // �������
    iter = m_lstProc.erase(iter);
  }
}

//------------------------------------------------------------------------------
// �������ܣ�����������Ϣ��������ͷ
// �����б�
//      [IN]��lstCamConf�����е�������Ϣ
//            strNodePathName��node.exe��·����
//            strJSmpegPathName��websocket-relay.js��·����
//            strFFmpegPathName��ffmpeg.exe��·����
// �� �� ֵ��true :��ʾ�ɹ���  false :��ʾʧ�ܣ�
// ����˵����
//------------------------------------------------------------------------------
bool CCameraOperate::Start(const std::list<std::shared_ptr<KCamConf>> &lstCamConf,
  const std::string &strNodePathName, const std::string &strJSmpegPathName,
  const std::string &strFFmpegPathName)
{
  // �ж�·�����Ƿ����
  if (!boost::filesystem::exists(boost::filesystem::path(strFFmpegPathName))
    || !boost::filesystem::exists(boost::filesystem::path(strJSmpegPathName))
    || !boost::filesystem::exists(boost::filesystem::path(strFFmpegPathName)))
  {
    LOG_WARN << "����ͷ������������ļ�������...";
    return false;
  }

  std::stringstream  strmJSmpegCmd;
  std::stringstream  strmFFmpegCmd;

  // ����������װ���еĲ���
  for (auto &item : lstCamConf)
  {
    // �����
    strmJSmpegCmd.str("");
    strmFFmpegCmd.str("");

    // ������������
    // node websocket-relay.js supersecret 8081 8082
    // �˴��������е�·����������boost::filesystem::path���󣬷���·�����ո�ͻ������
    strmJSmpegCmd << boost::filesystem::path(strJSmpegPathName) << " supersecret " << item->sInputPort << " " << item->sOutputPort;

    // ffmpeg -rtsp_transport tcp -i rtsp://admin:Hikvision@192.168.100.64:554/h264/ch1/main/av_stream -f mpegts -codec:v mpeg1video -s 960x540 -b:v 2048k -bf 0 http://localhost:8081/supersecret
    strmFFmpegCmd << "-rtsp_transport tcp -i " << item->strURL << " -f mpegts -codec:v mpeg1video -s "
      << item->ulVideoWidth << "x" << item->ulVideoHeight << " -b:v 2048k -bf 0 http://localhost:" << item->sInputPort << "/supersecret";

    // ִ����������
    LOG_DEBUG << strNodePathName << " " << strmJSmpegCmd.str();
    if (!ShellRun(strNodePathName, strmJSmpegCmd.str()))
    {
      LOG_WARN << "Start JSmpeg Cmd Fail...";
      OnStop();
      return false;
    }

    LOG_DEBUG << strFFmpegPathName << " " << strmFFmpegCmd.str();
    if (!ShellRun(strFFmpegPathName, strmFFmpegCmd.str()))
    {
      LOG_WARN << "Start FFmpeg Cmd Fail...";
      OnStop();
      return false;
    }
  }

  return true;
}
