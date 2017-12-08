#include "stdafx.h"
#include "CameraScreenshot.h"
#include <chrono>
#include <boost/filesystem.hpp>
#include <Logger.h>
#include <boost/date_time.hpp>
#include "CommonTool.h"

namespace _namespace_CCameraScreenshot
{
  const char NODEJS_FOLDER_NAME[] = "nodejs";  // nodejs的文件夹名
}

using namespace std;
using namespace _namespace_CCameraScreenshot;

//------------------------------------------------------------------------------
CCameraScreenshot::CCameraScreenshot()
{
}

//------------------------------------------------------------------------------
CCameraScreenshot::~CCameraScreenshot()
{
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
bool CCameraScreenshot::ShellRun(const std::string &strExe,
  const std::string &strParam, DWORD nShow)
{
  SHELLEXECUTEINFO  ShExecInfo;
  ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
  ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
  ShExecInfo.hwnd = NULL;
  ShExecInfo.lpVerb = NULL;
  ShExecInfo.lpFile = strExe.c_str(); //can be a file as well
  ShExecInfo.lpParameters = strParam.c_str();
  ShExecInfo.lpDirectory = NULL;
  ShExecInfo.nShow = nShow;
  ShExecInfo.hInstApp = NULL;

  // 启动进程
  ShellExecuteEx(&ShExecInfo);
  return true;
}

//------------------------------------------------------------------------------
// 函数功能：截图
// 参数列表：
//      [IN]：strFFmpegPathName：ffmpeg.exe的路径名
//            ptInfo：要截图的信息
// 返 回 值：true :表示成功；  false :表示失败；
// 其他说明：截图是否成功无法判断，只能判断启动一个截图进程是否成功
//------------------------------------------------------------------------------
bool CCameraScreenshot::Screenshot(const std::string &strFFmpegPathName,
  const std::shared_ptr<KCamScreenshotInfo> ptInfo)
{
  if (!ptInfo) return false;
  // 判断路径名是否存在
  if (!boost::filesystem::exists(boost::filesystem::path(strFFmpegPathName)))
  {
    LOG_WARN << "FFmpeg 文件不存在...";
    return false;
  }

  // 截图的路径名
  boost::filesystem::path pathImage = ptInfo->strDirName;
  string  strFileName = ""; // 图片的文件名
  stringstream  strmScreenshotCmd;

  try
  {
    // 判断路径名是否存在，不存在就创建
    if (!boost::filesystem::is_directory(pathImage))
      if (!boost::filesystem::create_directories(pathImage)) return false;
  }
  catch (std::exception &e)
  {
    LOG_ERROR << e.what();
    return false;
  }

  // 获取时间结构体
  std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
  boost::posix_time::ptime ptmTime = boost::posix_time::from_time_t(std::chrono::system_clock::to_time_t(tp));
  struct tm tmp = boost::posix_time::to_tm(ptmTime);

  // 文件名带上时间
  strFileName = ptInfo->strCameraName
    + "_" + to_string(tmp.tm_year + 1900)
    + "-" + to_string(tmp.tm_mon + 1)
    + "-" + to_string(tmp.tm_mday)
    + "_" + to_string(tmp.tm_hour)
    + "-" + to_string(tmp.tm_min)
    + "-" + to_string(tmp.tm_sec);

  // 文件名带上后缀名
  strFileName += ".jpg";

  // 判断文件名是否合法
  if (!boost::filesystem::native(strFileName)) return false;

  // 赋给路径名
  pathImage /= strFileName;

  // 组装命令
  strmScreenshotCmd.str("");

  // ffmpeg.exe -i rtsp://admin:Hikvision@192.168.100.64:554/h264/ch1/main/av_stream -rtsp_transport tcp  -r 25 -ss 00:00:00.000 -t 00:00:00.040 -f image2 -y D:\\pic\\%10d.jpg
  strmScreenshotCmd << " -i " << ptInfo->strURL
    << " -rtsp_transport tcp  -r 25 -ss 00:00:00.000 -t 00:00:00.040 -f image2 -y "
    << pathImage.string();

  // 执行命令
  if (!ShellRun(strFFmpegPathName, strmScreenshotCmd.str()))
  {
    LOG_WARN << "Start Screenshot Cmd Fail...";
    return false;
  }

  return true;
}
