//------------------------------------------------------------------------------
// �������ڣ�2017-09-12 09:09:56
// �ļ���������tag���������йصİ��Ľ���/����
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

