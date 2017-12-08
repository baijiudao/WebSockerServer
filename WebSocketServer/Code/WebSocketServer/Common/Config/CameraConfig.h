//------------------------------------------------------------------------------
// �������ڣ�2017-09-27 15:13:45
// �ļ�����������ͷ��������Ϣ
//------------------------------------------------------------------------------
#ifndef _CAMERA_CONFIG_H_
#define _CAMERA_CONFIG_H_

#include <stdint.h>
#include <string>
#include <memory>
#include <mutex>
#include <list>

#include <Singleton.h>

// ����ͷ��������Ϣ
typedef struct _CamConf
{
  uint32_t    ulID;
  std::string strURL;
  uint16_t    sOutputPort;   // ����˿�
  uint16_t    sInputPort;    // ����˿�
  uint32_t    ulVideoWidth;   // ��Ƶ��
  uint32_t    ulVideoHeight;  // ��Ƶ��
  _CamConf()
  {
    ulID = 0;
    strURL = "";
    sOutputPort = 0;
    sInputPort = 0;
    ulVideoWidth = 0;
    ulVideoHeight = 0;
  }
}KCamConf;

class CCameraConfig : public ISingleton<CCameraConfig>
{
public:
  bool  Load(const std::string &strPathName, std::list<std::shared_ptr<KCamConf>> &lstCamConf);
};

#endif // !_CAMERA_CONFIG_H_
