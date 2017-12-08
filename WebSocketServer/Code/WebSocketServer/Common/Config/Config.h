//------------------------------------------------------------------------------
// 创建日期：2017-11-02 14:58:43
// 文件描述：所有的配置信息
//------------------------------------------------------------------------------
#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "CameraConfig.h"
#include "WSServerConfig.h"

// 所有的配置信息（每个参数代表一个配置文件）
typedef struct _Conf
{
  std::list<std::shared_ptr<KCamConf>>  lstCamConf;   // 摄像头配置文件
  std::shared_ptr<KWSSConf>   ptWSSConf;    // WSServer的主程序配置文件
  _Conf()
  {
    ptWSSConf = std::make_shared<KWSSConf>();
  }
}KConf;

class CConfig
{
public:
  CConfig();
  ~CConfig();
};

#endif // !_CONFIG_H_
