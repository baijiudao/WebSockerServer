#include "stdafx.h"
#include "CameraScreenshot.h"
#include <chrono>
#include <boost/filesystem.hpp>
#include <Logger.h>
#include <boost/date_time.hpp>
#include "CommonTool.h"

namespace _namespace_CCameraScreenshot
{
  const char NODEJS_FOLDER_NAME[] = "nodejs";  // nodejs���ļ�����
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
// �������ܣ���������һ����ִ�г��򣬲������������ʾ״̬
// �����б�
//      [IN]��strExe��Ҫ���еĿ�ִ�г���
//            strParam��Ҫ����Ĳ���
//            nShow����ʾģʽ���磺SW_SHOW��
// �� �� ֵ��true :��ʾ�ɹ���  false :��ʾʧ�ܣ�
// ����˵����
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

  // ��������
  ShellExecuteEx(&ShExecInfo);
  return true;
}

//------------------------------------------------------------------------------
// �������ܣ���ͼ
// �����б�
//      [IN]��strFFmpegPathName��ffmpeg.exe��·����
//            ptInfo��Ҫ��ͼ����Ϣ
// �� �� ֵ��true :��ʾ�ɹ���  false :��ʾʧ�ܣ�
// ����˵������ͼ�Ƿ�ɹ��޷��жϣ�ֻ���ж�����һ����ͼ�����Ƿ�ɹ�
//------------------------------------------------------------------------------
bool CCameraScreenshot::Screenshot(const std::string &strFFmpegPathName,
  const std::shared_ptr<KCamScreenshotInfo> ptInfo)
{
  if (!ptInfo) return false;
  // �ж�·�����Ƿ����
  if (!boost::filesystem::exists(boost::filesystem::path(strFFmpegPathName)))
  {
    LOG_WARN << "FFmpeg �ļ�������...";
    return false;
  }

  // ��ͼ��·����
  boost::filesystem::path pathImage = ptInfo->strDirName;
  string  strFileName = ""; // ͼƬ���ļ���
  stringstream  strmScreenshotCmd;

  try
  {
    // �ж�·�����Ƿ���ڣ������ھʹ���
    if (!boost::filesystem::is_directory(pathImage))
      if (!boost::filesystem::create_directories(pathImage)) return false;
  }
  catch (std::exception &e)
  {
    LOG_ERROR << e.what();
    return false;
  }

  // ��ȡʱ��ṹ��
  std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
  boost::posix_time::ptime ptmTime = boost::posix_time::from_time_t(std::chrono::system_clock::to_time_t(tp));
  struct tm tmp = boost::posix_time::to_tm(ptmTime);

  // �ļ�������ʱ��
  strFileName = ptInfo->strCameraName
    + "_" + to_string(tmp.tm_year + 1900)
    + "-" + to_string(tmp.tm_mon + 1)
    + "-" + to_string(tmp.tm_mday)
    + "_" + to_string(tmp.tm_hour)
    + "-" + to_string(tmp.tm_min)
    + "-" + to_string(tmp.tm_sec);

  // �ļ������Ϻ�׺��
  strFileName += ".jpg";

  // �ж��ļ����Ƿ�Ϸ�
  if (!boost::filesystem::native(strFileName)) return false;

  // ����·����
  pathImage /= strFileName;

  // ��װ����
  strmScreenshotCmd.str("");

  // ffmpeg.exe -i rtsp://admin:Hikvision@192.168.100.64:554/h264/ch1/main/av_stream -rtsp_transport tcp  -r 25 -ss 00:00:00.000 -t 00:00:00.040 -f image2 -y D:\\pic\\%10d.jpg
  strmScreenshotCmd << " -i " << ptInfo->strURL
    << " -rtsp_transport tcp  -r 25 -ss 00:00:00.000 -t 00:00:00.040 -f image2 -y "
    << pathImage.string();

  // ִ������
  if (!ShellRun(strFFmpegPathName, strmScreenshotCmd.str()))
  {
    LOG_WARN << "Start Screenshot Cmd Fail...";
    return false;
  }

  return true;
}
