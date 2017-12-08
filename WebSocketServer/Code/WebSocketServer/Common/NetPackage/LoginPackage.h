#ifndef _LOGIN_PACKAGE_H_
#define _LOGIN_PACKAGE_H_

#include "PackageBase.h"

class CLoginPackage : public CPackageBase, public ISingleton<CLoginPackage>
{
public:
  CLoginPackage();
  ~CLoginPackage();
  bool  ParseLogin(const std::string &strRecv, std::string &strUserName, std::string &strPassword);
  bool  ParseLogOut(const std::string &strRecv, std::string &strSID);
  bool  BuildLogin(const std::string &strSID, EErrCode eEC, EUserType eType, std::string &strSend);
  bool  BuildLogOut(EErrCode eEC, std::string &strSend);
};

#endif // !_LOGIN_PACKAGE_H_
