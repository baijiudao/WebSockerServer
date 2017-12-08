//------------------------------------------------------------------------------
// 创建日期：2017-09-27 15:13:45
// 文件描述：摄像头的配置信息
//------------------------------------------------------------------------------
#ifndef _CAMERA_CONFIG_H_
#define _CAMERA_CONFIG_H_

#include <stdint.h>
#include <string>
#include <memory>
#include <mutex>
#include <list>

#include <Singleton.h>

// 摄像头的配置信息
typedef struct _CamConf
{
  uint32_t    ulID;
  std::string strURL;
  uint16_t    sOutputPort;   // 输出端口
  uint16_t    sInputPort;    // 输入端口
  uint32_t    ulVideoWidth;   // 视频宽
  uint32_t    ulVideoHeight;  // 视频高
  _CamConf()
  {
    ulID = 0;
    strURL = "";
    sOutputPort = 0;
    sInputPort = 0;
    ulVideoWidth = 0;
    ulVideoHeight = 0;
  }
}KCamConf;

class CCameraConfig : public ISingleton<CCameraConfig>
{
public:
  bool  Load(const std::string &strPathName, std::list<std::shared_ptr<KCamConf>> &lstCamConf);
};

#endif // !_CAMERA_CONFIG_H_
