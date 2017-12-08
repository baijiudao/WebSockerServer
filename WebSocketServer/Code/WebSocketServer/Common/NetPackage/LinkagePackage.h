//------------------------------------------------------------------------------
// 创建日期：2017-10-27 10:36:19
// 文件描述：联动信息解析/组建
//------------------------------------------------------------------------------
#ifndef _LINKAGE_PACKAGE_H_
#define _LINKAGE_PACKAGE_H_

#include "PackageBase.h"

class CLinkagePackage : public CPackageBase, public ISingleton<CLinkagePackage>
{
public:
  CLinkagePackage();
  ~CLinkagePackage();
  bool  ParseReadWatchInfo(const std::string &strRecv, std::string &strSID);
  bool  ParseManualConfirmation(const std::string &strRecv, std::string &strSID, uint32_t &ulItemID);
  bool  ParseTerminateTask(const std::string &strRecv, std::string &strSID, uint32_t &ulItemID);
  bool  BuildReadWatchInfo(const std::string &strSID, EErrCode eEC,
    const std::list<shared_ptr<KLinkageWatchInfo>> &lstInfo, std::string &strSend);
  bool  BuildManualConfirmation(const std::string &strSID, EErrCode eEC, std::string &strSend);
  bool  BuildTerminateTask(const std::string &strSID, EErrCode eEC, std::string &strSend);
};

#endif // !_LINKAGE_PACKAGE_H_
