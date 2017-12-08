#include "stdafx.h"
#include "CameraOperate.h"

#include <iostream>
#include <sstream>
#include <boost/filesystem.hpp>
#include <Logger.h>
#include "CommonTool.h"

namespace _namespace_CCameraOperate
{
  const char NODEJS_FOLDER_NAME[] = "nodejs";  // nodejs的文件夹名
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
  // 关闭所有摄像头推流窗口
  OnStop();
}

//------------------------------------------------------------------------------
// 函数功能：用来运行一个可执行程序，并传入参数和显示状态
// 参数列表：
//      [IN]：strExe：要运行的可执行程序
//            strParam：要传入的参数
//            nShow：显示模式（如：SW_SHOW）
// 返 回 值：true :表示成功；  false :表示失败；
// 其他说明：
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

  // 启动进程
  ShellExecuteEx(pShExecInfo.get());

  // 将启动进程信息添加到列表中
  m_lstProc.push_back(pShExecInfo);
  return true;
}

//------------------------------------------------------------------------------
// 停止已经启动的所有摄像头
//------------------------------------------------------------------------------
void CCameraOperate::OnStop()
{
  // 关闭所有的进程
  for (auto iter = m_lstProc.begin(); iter != m_lstProc.end(); iter++)
  {
    TerminateProcess((*iter)->hProcess, 0);
    CloseHandle((*iter)->hProcess);

    // 清除对象
    iter = m_lstProc.erase(iter);
  }
}

//------------------------------------------------------------------------------
// 函数功能：根据配置信息启动摄像头
// 参数列表：
//      [IN]：lstCamConf：所有的配置信息
//            strNodePathName：node.exe的路径名
//            strJSmpegPathName：websocket-relay.js的路径名
//            strFFmpegPathName：ffmpeg.exe的路径名
// 返 回 值：true :表示成功；  false :表示失败；
// 其他说明：
//------------------------------------------------------------------------------
bool CCameraOperate::Start(const std::list<std::shared_ptr<KCamConf>> &lstCamConf,
  const std::string &strNodePathName, const std::string &strJSmpegPathName,
  const std::string &strFFmpegPathName)
{
  // 判断路径名是否存在
  if (!boost::filesystem::exists(boost::filesystem::path(strFFmpegPathName))
    || !boost::filesystem::exists(boost::filesystem::path(strJSmpegPathName))
    || !boost::filesystem::exists(boost::filesystem::path(strFFmpegPathName)))
  {
    LOG_WARN << "摄像头推流服务相关文件不存在...";
    return false;
  }

  std::stringstream  strmJSmpegCmd;
  std::stringstream  strmFFmpegCmd;

  // 根据配置组装所有的参数
  for (auto &item : lstCamConf)
  {
    // 清空流
    strmJSmpegCmd.str("");
    strmFFmpegCmd.str("");

    // 创建两个命令
    // node websocket-relay.js supersecret 8081 8082
    // 此处命令行中的路径名必须是boost::filesystem::path对象，否则路径带空格就会出问题
    strmJSmpegCmd << boost::filesystem::path(strJSmpegPathName) << " supersecret " << item->sInputPort << " " << item->sOutputPort;

    // ffmpeg -rtsp_transport tcp -i rtsp://admin:Hikvision@192.168.100.64:554/h264/ch1/main/av_stream -f mpegts -codec:v mpeg1video -s 960x540 -b:v 2048k -bf 0 http://localhost:8081/supersecret
    strmFFmpegCmd << "-rtsp_transport tcp -i " << item->strURL << " -f mpegts -codec:v mpeg1video -s "
      << item->ulVideoWidth << "x" << item->ulVideoHeight << " -b:v 2048k -bf 0 http://localhost:" << item->sInputPort << "/supersecret";

    // 执行两个命令
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
