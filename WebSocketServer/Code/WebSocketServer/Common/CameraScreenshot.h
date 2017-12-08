//------------------------------------------------------------------------------
// 创建日期：2017-10-18 14:46:43
// 文件描述：对摄像头的视频进行截图
//------------------------------------------------------------------------------
#ifndef _CAMERAM_SCREENSHOT_H_
#define _CAMERAM_SCREENSHOT_H_

#include <string>
#include <memory>
#include "Singleton.h"

// 视频截图信息
typedef struct _CamScreenshotInfo
{
  std::string strURL;         // 摄像头网络流的URL
  std::string strCameraName;  // 摄像头名字（用来保存截图命名使用）
  std::string strDirName;     // 截图保存的目录名
  _CamScreenshotInfo()
  {
    strURL = "";
    strCameraName = "";
    strDirName = "";
  }
}KCamScreenshotInfo;

class CCameraScreenshot : public ISingleton<CCameraScreenshot>
{
protected:
  bool ShellRun(const std::string &strExe, const std::string &strParam, DWORD nShow = SW_SHOW);
public:
  CCameraScreenshot();
  ~CCameraScreenshot();
  bool  Screenshot(const std::string &strFFmpegPathName, const std::shared_ptr<KCamScreenshotInfo> ptInfo);
};

#endif // !_CAMERAM_SCREENSHOT_H_
