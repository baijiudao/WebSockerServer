//------------------------------------------------------------------------------
// �������ڣ�2017-11-02 14:58:43
// �ļ����������е�������Ϣ
//------------------------------------------------------------------------------
#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "CameraConfig.h"
#include "WSServerConfig.h"

// ���е�������Ϣ��ÿ����������һ�������ļ���
typedef struct _Conf
{
  std::list<std::shared_ptr<KCamConf>>  lstCamConf;   // ����ͷ�����ļ�
  std::shared_ptr<KWSSConf>   ptWSSConf;    // WSServer�������������ļ�
  _Conf()
  {
    ptWSSConf = std::make_shared<KWSSConf>();
  }
}KConf;

class CConfig
{
public:
  CConfig();
  ~CConfig();
};

#endif // !_CONFIG_H_
