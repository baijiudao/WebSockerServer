//------------------------------------------------------------------------------
// 创建日期：2017-09-12 10:13:41
// 文件描述：alarm相关的数据包的解析/组建
//------------------------------------------------------------------------------
#ifndef _ALARM_PACKAGE_H_
#define _ALARM_PACKAGE_H_

#include "PackageBase.h"

class CAlarmPackage : public CPackageBase, public ISingleton<CAlarmPackage>
{
public:
  CAlarmPackage();
  ~CAlarmPackage();
  bool  ParseReadHistory(const std::string &strRecv, std::string &strSID, uint64_t &ullStartTime, uint64_t &ullEndTime);
  bool  BuildReadHistory(const std::string &strSID, EErrCode eEC, const std::list<shared_ptr<KAlarmInfo>> &lstInfo, std::string &strSend);
  bool  BuildPush(const std::string &strSID, EErrCode eEC, const std::list<shared_ptr<KAlarmInfo>> &lstInfo, std::string &strSend);
};

#endif // !_ALARM_PACKAGE_H_
