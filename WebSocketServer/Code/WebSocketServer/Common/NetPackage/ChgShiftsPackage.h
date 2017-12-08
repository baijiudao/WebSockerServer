//------------------------------------------------------------------------------
// 创建日期：2017-09-18 17:53:20
// 文件描述：打包解包数据
//------------------------------------------------------------------------------
#ifndef _CHANGE_SHIFTS_PACKAGE_H_
#define _CHANGE_SHIFTS_PACKAGE_H_

#include "PackageBase.h"

class CChgShiftsPackage : public CPackageBase, public ISingleton<CChgShiftsPackage>
{
public:
  CChgShiftsPackage();
  ~CChgShiftsPackage();
  bool  ParseAdd(const std::string &strRecv, std::string &strSID, std::list<std::shared_ptr<KTBChgShifts>> &lstInfo);
  bool  BuildAdd(const std::string &strSID, EErrCode eEC, const std::list<uint32_t> &lstID, std::string &strSend);
};

#endif // !_CHANGE_SHIFTS_H_
