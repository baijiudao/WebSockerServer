//------------------------------------------------------------------------------
// 创建日期：2017-09-12 09:09:56
// 文件描述：与tag（变量）有关的包的解析/构建
//------------------------------------------------------------------------------
#ifndef _TAG_PACKAGE_H_
#define _TAG_PACKAGE_H_

#include "PackageBase.h"

class CTagPackage : public CPackageBase, public ISingleton<CTagPackage>
{
public:
  CTagPackage();
  ~CTagPackage();
  bool  ParseRead(const std::string &strRecv, std::string &strSID, std::list<shared_ptr<KTagInfo>> &lstInfo);
  bool  ParseWrite(const std::string &strRecv, std::string &strSID, std::list<shared_ptr<KTagInfo>> &lstInfo);
  bool  BuildRead(const std::string &strSID, EErrCode eEC, const std::list<shared_ptr<KTagInfo>> &lstInfo, std::string &strSend);
  bool  BuildWrite(const std::string &strSID, EErrCode eEC, const std::list<shared_ptr<KTagInfo>> &lstInfo, std::string &strSend);
  bool  BuildPush(const std::string &strSID, EErrCode eEC, const std::list<shared_ptr<KTagInfo>> &lstInfo, std::string &strSend);
};

#endif // !_TAG_PACKAGE_H_

