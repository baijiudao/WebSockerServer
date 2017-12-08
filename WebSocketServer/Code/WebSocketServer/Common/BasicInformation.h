//------------------------------------------------------------------------------
// �������ڣ�2017-10-18 16:18:12
// �ļ���������Ҫ����һЩ������Ϣ
//------------------------------------------------------------------------------
#ifndef _BASIC_INFORMATION_H_
#define _BASIC_INFORMATION_H_

#include <string>
#include "Singleton.h"

// ��ȡ�������еĻ�����Ϣ
typedef struct _BasicInfo
{
  std::string strHostName;        // ���������������������
  std::string strProductVersion;  // �����汾��
  std::string strModulePathName;  // ��ǰģ��ȫ·����
  std::string strModuleDirName;   // ��ǰģ���Ŀ¼�����������ļ�����
  std::string strNodePathName;    // node.exe��ȫ·����
  std::string strJSmpegPathName;  // websocket-relay.js��ȫ·����
  std::string strFFmpegPathName;  // ffmpeg.exe��ȫ·����
  std::string strCamConfPathName; // ����ͷ���������������ļ�·����
  std::string strWSSConfPathName; // wsserver�����ļ�·����
  _BasicInfo()
  {
    strHostName = "";
    strProductVersion = "";
    strModulePathName = "";
    strModuleDirName = "";
    strNodePathName = "";
    strJSmpegPathName = "";
    strFFmpegPathName = "";
    strCamConfPathName = "";
    strWSSConfPathName = "";
  }
}KBasicInfo;

//------------------------------------------------------------------------------
// �������ڣ�2017-10-20 13:23:41 
// �� �� ������·��������������ʱ����Ҫ��string���͵�·����
//          ת��Ϊboost::filesystem::path��ʽ��Ȼ�󴩸���װ�����е�stringstream����
//
//          ��Ҫԭ��win����Щ�ļ��д��ո񣬵���cmd�����в�������ո�����Ҫô�Լ���
//          string���͵�·�����м���˫���ţ�Ҫôʹ��boost::filesystem::path��ʽ����Ϊ
//          boost::filesystem::path��ʽ���䵽stringstream�л��Զ��ж�·��
//------------------------------------------------------------------------------
class CBasicInformation : public ISingleton<CBasicInformation>
{
protected:
  bool  OnGenerateHostName(std::string &strHostName);
  bool  OnGenerateModulePath(std::string &strModulePathName, std::string &strModuleDirName);
  void  OnGenerateNodePath(const std::string &strModuleDirName, std::string &strNodePathName);
  void  OnGenerateJSmpegPath(const std::string &strModuleDirName, std::string &strJSmpegPathName);
  void  OnGenerateFFmpegPath(const std::string &strModuleDirName, std::string &strFFmpegPathName);
  void  OnGenerateCamConfPath(const std::string &strModuleDirName, std::string &strCamConfPathName);
  void  OnGenerateWSSConfPath(const std::string &strModuleDirName, std::string &strWSSConfPathName);
public:
  bool  Generate(std::shared_ptr<KBasicInfo> ptBasicInfo);
};

#endif // !_BASIC_INFORMATION_H_
