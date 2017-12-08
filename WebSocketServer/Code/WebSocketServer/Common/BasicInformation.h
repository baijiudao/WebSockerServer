//------------------------------------------------------------------------------
// 创建日期：2017-10-18 16:18:12
// 文件描述：主要保存一些基础信息
//------------------------------------------------------------------------------
#ifndef _BASIC_INFORMATION_H_
#define _BASIC_INFORMATION_H_

#include <string>
#include "Singleton.h"

// 获取到的所有的基础信息
typedef struct _BasicInfo
{
  std::string strHostName;        // 主机名（本机计算机名）
  std::string strProductVersion;  // 发布版本号
  std::string strModulePathName;  // 当前模块全路径名
  std::string strModuleDirName;   // 当前模块的目录名（不包括文件名）
  std::string strNodePathName;    // node.exe的全路径名
  std::string strJSmpegPathName;  // websocket-relay.js的全路径名
  std::string strFFmpegPathName;  // ffmpeg.exe的全路径名
  std::string strCamConfPathName; // 摄像头流服务启动配置文件路径名
  std::string strWSSConfPathName; // wsserver配置文件路径名
  _BasicInfo()
  {
    strHostName = "";
    strProductVersion = "";
    strModulePathName = "";
    strModuleDirName = "";
    strNodePathName = "";
    strJSmpegPathName = "";
    strFFmpegPathName = "";
    strCamConfPathName = "";
    strWSSConfPathName = "";
  }
}KBasicInfo;

//------------------------------------------------------------------------------
// 创建日期：2017-10-20 13:23:41 
// 类 描 述：当路径名用于命令行时，需要将string类型的路径名
//          转换为boost::filesystem::path格式，然后穿给组装命令行的stringstream类型
//
//          主要原因：win中有些文件夹带空格，但是cmd命令中不允许带空格，所以要么自己将
//          string类型的路径名中加上双引号，要么使用boost::filesystem::path格式，因为
//          boost::filesystem::path格式传输到stringstream中会自动判断路径
//------------------------------------------------------------------------------
class CBasicInformation : public ISingleton<CBasicInformation>
{
protected:
  bool  OnGenerateHostName(std::string &strHostName);
  bool  OnGenerateModulePath(std::string &strModulePathName, std::string &strModuleDirName);
  void  OnGenerateNodePath(const std::string &strModuleDirName, std::string &strNodePathName);
  void  OnGenerateJSmpegPath(const std::string &strModuleDirName, std::string &strJSmpegPathName);
  void  OnGenerateFFmpegPath(const std::string &strModuleDirName, std::string &strFFmpegPathName);
  void  OnGenerateCamConfPath(const std::string &strModuleDirName, std::string &strCamConfPathName);
  void  OnGenerateWSSConfPath(const std::string &strModuleDirName, std::string &strWSSConfPathName);
public:
  bool  Generate(std::shared_ptr<KBasicInfo> ptBasicInfo);
};

#endif // !_BASIC_INFORMATION_H_
