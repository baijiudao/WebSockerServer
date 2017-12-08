//------------------------------------------------------------------------------
// 创建日期：2017-09-06 13:09:39
// 文件描述：告警辅助相关包的编解码
//------------------------------------------------------------------------------
#ifndef _ALARM_ASSISTANT_PACKAGE_H_
#define _ALARM_ASSISTANT_PACKAGE_H_

#include "PackageBase.h"

//------------------------------------------------------------------------------
// 创建日期：2017-09-06 13:10:03 
// 类 描 述：用来解析/打包告警辅助的相关操作的包
//------------------------------------------------------------------------------
class CAlarmAssistantPackage : public CPackageBase, public ISingleton<CAlarmAssistantPackage>
{
public:
  ~CAlarmAssistantPackage();
  CAlarmAssistantPackage();
  bool  ParseAdd(const std::string &strRecv, std::string &strSID, std::list<std::shared_ptr<KTBAlarmAssistant>> &lstInfo);
  bool  ParseDelete(const std::string &strRecv, std::string &strSID, std::list<uint32_t> &lstID);
  bool  ParseModify(const std::string &strRecv, std::string &strSID, std::list<std::shared_ptr<KTBAlarmAssistant>> &lstInfo);
  bool  ParseSearch(const std::string &strRecv, std::string &strSID, uint32_t &ulGlobalID);
  bool  ParseSearch_Num(const std::string &strRecv, std::string &strSID, std::shared_ptr<KTBAlarmAssistantSearchCondition_Num> ptCondition);
  bool  ParseSearchPaging(const std::string &strRecv, std::string &strSID, std::shared_ptr<KTBAlarmAssistantSearchCondition> ptCondition);
  bool  BuildAdd(const std::string &strSID, EErrCode eEC, const std::list<uint32_t> &lstID, std::string &strSend);
  bool  BuildDelete(const std::string &strSID, EErrCode eEC, const std::list<uint32_t> &lstID, std::string &strSend);
  bool  BuildModify(const std::string &strSID, EErrCode eEC, const std::list<uint32_t> &lstID, std::string &strSend);
  bool  BuildSearch(const std::string &strSID, EErrCode eEC, const std::list<std::shared_ptr<KTBAlarmAssistant>> &lstInfo, std::string &strSend);
  bool  BuildSearch_Num(const std::string &strSID, EErrCode eEC, const std::shared_ptr<KTBAlarmAssistantSearchResult_Num> ptResult, std::string &strSend);
  bool  BuildSearchPaging(const std::string &strSID, EErrCode eEC, const std::shared_ptr<KTBAlarmAssistantSearchResult> ptResult, std::string &strSend);
};

#endif // !_ALARM_ASSISTANT_PACKAGE_H_
