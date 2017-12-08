//------------------------------------------------------------------------------
// �������ڣ�2017-10-19 09:46:04
// �ļ���������Ƶ�ؼ����/����
//------------------------------------------------------------------------------
#ifndef _CAMERA_CONTROL_PACKAGE_H_
#define _CAMERA_CONTROL_PACKAGE_H_

#include "PackageBase.h"
#include "../CameraScreenshot.h"
#include "../Config/Config.h"

class CCameraControlPackage : public CPackageBase, public ISingleton<CCameraControlPackage>
{
public:
  CCameraControlPackage();
  ~CCameraControlPackage();
  bool  ParseScreenshot(const std::string &strRecv, std::string &strSID, std::shared_ptr<KCamScreenshotInfo> ptInfo);
  bool  ParseSearchConfig(const std::string &strRecv, std::string &strSID);
  bool  BuildScreenshot(const std::string &strSID, EErrCode eEC, std::string &strSend);
  bool  BuildSearchConfig(const std::string &strSID, EErrCode eEC,
    const std::list<std::shared_ptr<KCamConf>> &lstCamConf, std::string &strSend);
};

#endif // !_CAMERA_CONTROL_PACKAGE_H_
