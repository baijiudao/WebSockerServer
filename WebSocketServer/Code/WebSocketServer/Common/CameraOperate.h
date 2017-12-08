//------------------------------------------------------------------------------
// 创建日期：2017-09-27 17:13:51
// 文件描述：用来操作摄像头的类
//------------------------------------------------------------------------------
#ifndef _CAMERA_OPERATE_H_
#define _CAMERA_OPERATE_H_

#include <list>
#include <memory>
#include "Config/CameraConfig.h"

class CCameraOperate
{
private:
  std::list<std::shared_ptr<SHELLEXECUTEINFO>>  m_lstProc;  // 启动的进程的信息
protected:
  bool  ShellRun(const std::string &strExe, const std::string &strParam, DWORD nShow = SW_SHOW);
  void  OnStop();
public:
  CCameraOperate();
  ~CCameraOperate();
  bool  Start(const std::list<std::shared_ptr<KCamConf>> &lstCamConf,
    const std::string &strNodePathName, const std::string &strJSmpegPathName,
    const std::string &strFFmpegPathName);
};

#endif // !_CAMERA_OPERATE_H_
