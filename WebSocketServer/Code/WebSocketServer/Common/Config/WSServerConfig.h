//------------------------------------------------------------------------------
// 创建日期：2017-09-27 13:19:38
// 文件描述：WSServer的配置信息
//------------------------------------------------------------------------------
#ifndef _WSSERVER_CONFIG_H_
#define _WSSERVER_CONFIG_H_

#include <string>
#include <memory>
#include <mutex>
#include <Singleton.h>

// 数据库配置信息
typedef struct _DBConf
{
  std::string strUserName;
  std::string strPassword;
  std::string strDSN;   // 数据源名字
  _DBConf()
  {
    strUserName = "";
    strPassword = "";
    strDSN = "";
  }
}KDBConf;

// 配置信息
typedef struct _WSSConf
{
  std::shared_ptr<KDBConf> ptDBConf;
  _WSSConf()
  {
    ptDBConf = std::make_shared<KDBConf>();
  }
}KWSSConf;


class CWSServerConfig : public ISingleton<CWSServerConfig>
{
public:
  CWSServerConfig();
  ~CWSServerConfig();
  bool  Load(const std::string &strPathName, std::shared_ptr<KWSSConf> pConfig);
  bool  Save(const std::string &strPathName, const std::shared_ptr<KWSSConf> pConfig);
};

#endif // !_WSSERVER_CONFIG_H_
