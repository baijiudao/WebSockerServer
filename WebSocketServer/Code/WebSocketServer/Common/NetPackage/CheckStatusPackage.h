//------------------------------------------------------------------------------
// �������ڣ�2017-10-09 14:30:38
// �ļ�������״̬��⣨���ݿ��OPC״̬��⣩Э��Ľ�����
//------------------------------------------------------------------------------
#ifndef _CHECK_STATUS_PACKAGE_H_
#define _CHECK_STATUS_PACKAGE_H_

#include "PackageBase.h"

class CCheckStatusPackage : public CPackageBase, public ISingleton<CCheckStatusPackage>
{
public:
  CCheckStatusPackage();
  ~CCheckStatusPackage();
  bool  ParseDB(const std::string &strRecv, std::string &strSID);
  bool  ParseOPC(const std::string &strRecv, std::string &strSID);
  bool  BuildDB(const std::string &strSID, EErrCode eEC, const std::list<std::shared_ptr<KDBStatusInfo>> &lstInfo, std::string &strSend);
  bool  BuildOPC(const std::string &strSID, EErrCode eEC, const std::list<std::shared_ptr<KOPCStatusInfo>> &lstInfo, std::string &strSend);
};

#endif // !_CHECK_STATUS_PACKAGE_H_
