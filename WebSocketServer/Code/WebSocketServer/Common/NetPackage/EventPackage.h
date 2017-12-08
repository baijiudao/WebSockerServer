//------------------------------------------------------------------------------
// �������ڣ�2017-09-12 10:30:44
// �ļ��������¼���ص����ݰ��Ľ���/�齨
//------------------------------------------------------------------------------
#ifndef _EVENT_PACKAGE_H_
#define _EVENT_PACKAGE_H_

#include "PackageBase.h"

class CEventPackage : public CPackageBase, public ISingleton<CEventPackage>
{
public:
  CEventPackage();
  ~CEventPackage();
  bool  ParseWrite(const std::string &strRecv, std::string &strSID, std::list<shared_ptr<KEventInfo>> &lstInfo);
  bool  ParseReadHistory(const std::string &strRecv, std::string &strSID, uint64_t &ullStartTime, uint64_t &ullEndTime);
  bool  BuildWrite(const std::string &strSID, EErrCode eEC, std::string &strSend);
  bool  BuildReadHistory(const std::string &strSID, EErrCode eEC, const std::list<shared_ptr<KEventInfo>> &lstInfo, std::string &strSend);
  bool  BuildPush(const std::string &strSID, EErrCode eEC, const std::list<shared_ptr<KEventInfo>> &lstInfo, std::string &strSend);
};

#endif // !_EVENT_PACKAGE_H_
