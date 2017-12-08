//------------------------------------------------------------------------------
// �������ڣ�2017-10-18 14:46:43
// �ļ�������������ͷ����Ƶ���н�ͼ
//------------------------------------------------------------------------------
#ifndef _CAMERAM_SCREENSHOT_H_
#define _CAMERAM_SCREENSHOT_H_

#include <string>
#include <memory>
#include "Singleton.h"

// ��Ƶ��ͼ��Ϣ
typedef struct _CamScreenshotInfo
{
  std::string strURL;         // ����ͷ��������URL
  std::string strCameraName;  // ����ͷ���֣����������ͼ����ʹ�ã�
  std::string strDirName;     // ��ͼ�����Ŀ¼��
  _CamScreenshotInfo()
  {
    strURL = "";
    strCameraName = "";
    strDirName = "";
  }
}KCamScreenshotInfo;

class CCameraScreenshot : public ISingleton<CCameraScreenshot>
{
protected:
  bool ShellRun(const std::string &strExe, const std::string &strParam, DWORD nShow = SW_SHOW);
public:
  CCameraScreenshot();
  ~CCameraScreenshot();
  bool  Screenshot(const std::string &strFFmpegPathName, const std::shared_ptr<KCamScreenshotInfo> ptInfo);
};

#endif // !_CAMERAM_SCREENSHOT_H_
