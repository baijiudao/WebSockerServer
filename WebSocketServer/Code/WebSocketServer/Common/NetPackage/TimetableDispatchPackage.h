//------------------------------------------------------------------------------
// 创建日期：2017-10-30 13:12:24
// 文件描述：时间表调度相关的打包/解析
//------------------------------------------------------------------------------
#ifndef _TIMETABLE_DISPATCH_PACKAGE_H_
#define _TIMETABLE_DISPATCH_PACKAGE_H_

#include "PackageBase.h"

class CTimetableDispatchPackage : public CPackageBase, public ISingleton<CTimetableDispatchPackage>
{
public:
  CTimetableDispatchPackage();
  ~CTimetableDispatchPackage();
  bool  ParseReadWatchInfo(const std::string &strRecv, std::string &strSID);
  bool  ParseTerminateTask(const std::string &strRecv, std::string &strSID, uint32_t &ulItemID);
  bool  BuildReadWatchInfo(const std::string &strSID, EErrCode eEC,
    const std::list<shared_ptr<KTimetableDispatchWatchInfo>> &lstInfo,
    std::string &strSend);
  bool  BuildTerminateTask(const std::string &strSID, EErrCode eEC, std::string &strSend);
};

#endif // !_TIMETABLE_DISPATCH_PACKAGE_H_
