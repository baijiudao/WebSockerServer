//------------------------------------------------------------------------------
// 创建日期：2017-09-11 14:00:45
// 文件描述：用来操作数据表AlarmAssistant
//------------------------------------------------------------------------------
#ifndef _ALARM_ASSISTANT_TABLE_H_
#define _ALARM_ASSISTANT_TABLE_H_

#include "TableBase.h"

//------------------------------------------------------------------------------
// 创建日期：2017-09-06 10:49:45 
// 描    述：与数据库中的AlarmAssistant（告警辅助）表对应
//------------------------------------------------------------------------------
typedef struct _TBAlarmAssistant
{
  std::pair<uint32_t, bool>     lID;                            // 告警辅助信息ID
  std::pair<uint32_t, bool>     lGlobalID;                      // 设备ID
  std::pair<std::string, bool>  strDeviceName;                  // 设备名称
  std::pair<uint32_t, bool>     lCameraIP;                      // 关联摄像头IP（IPV4）
  std::pair<std::string, bool>  strCameraUserName;              // 摄像头访问用户名
  std::pair<std::string, bool>  strCameraPassword;              // 摄像头访问密码
  std::pair<std::string, bool>  strEmergencyPlanFileName;       // 应急预案文件名
  std::pair<std::string, bool>  strEmergencyPlanStorageAddress; // 应急预案存放地址
  std::pair<std::string, bool>  strLinkageSchemeName;           // 联动方案名称
  std::pair<std::string, bool>  strLinkageSchemeDescription;    // 联动方案说明
  _TBAlarmAssistant()
  {
    lID = { 0, false };
    lGlobalID = { 0, false };
    strDeviceName = { "", false };
    lCameraIP = { 0, false };
    strCameraUserName = { "", false };
    strCameraPassword = { "", false };
    strEmergencyPlanFileName = { "", false };
    strEmergencyPlanStorageAddress = { "", false };
    strLinkageSchemeName = { "", false };
    strLinkageSchemeDescription = { "", false };
  }
}KTBAlarmAssistant;

// AlarmAssistant的查询参数
typedef struct _TBAlarmAssistantSearchParameter
{
  std::pair<uint32_t, bool>     lGlobalID;            // 设备ID【可选】
  std::pair<std::string, bool>  strDeviceName;        // 设备名【可选】
  std::pair<std::string, bool>  strLinkageSchemeName; // 联动方案名称【可选】
  _TBAlarmAssistantSearchParameter()
  {
    lGlobalID = { 0, false };
    strDeviceName = { "", false };
    strLinkageSchemeName = { "", false };
  }
}KTBAlarmAssistantSearchParameter;

// AlarmAssistant查询条件（按数量返回）
typedef struct _TBAlarmAssistantSearchCondition_Num
{
  uint32_t  lCount;                             // 请求要获取的数量
  uint32_t  lStartNum;                          // 请求数据起始索引
  KTBAlarmAssistantSearchParameter  tParameter; // 请求查询参数
  _TBAlarmAssistantSearchCondition_Num()
  {
    lCount = 0;
    lStartNum = 0;
  }
}KTBAlarmAssistantSearchCondition_Num;

// AlarmAssistant查询条件（按页返回）
typedef struct _TBAlarmAssistantSearchCondition
{
  uint32_t  lPageSize;                          // 页大小（请求数量）
  uint32_t  lPageNum;                           // 请求第几页
  KTBAlarmAssistantSearchParameter  tParameter; // 请求查询参数
  _TBAlarmAssistantSearchCondition()
  {
    lPageSize = 0;
    lPageNum = 0;
  }
}KTBAlarmAssistantSearchCondition;

// AlarmAssistant查询结果（按数量返回）
typedef struct _TBAlarmAssistantSearchResult_Num
{
  uint32_t  lStartNum;                                    // 查询结果第一条在总条数中的位置
  uint32_t  lTotal;                                       // 查询结果的总数
  std::list<std::shared_ptr<KTBAlarmAssistant>> lstData;  // 当前返回的数据
  _TBAlarmAssistantSearchResult_Num()
  {
    lStartNum = 0;
    lTotal = 0;
  }
}KTBAlarmAssistantSearchResult_Num;

// AlarmAssistant查询结果（按页返回）
typedef struct _TBAlarmAssistantSearchResult
{
  uint32_t  lPageNum;                                     // 查询结果返回的第几页
  uint32_t  lTotalPages;                                  // 查询结果的总页数
  std::list<std::shared_ptr<KTBAlarmAssistant>> lstData;  // 当前返回的数据
  _TBAlarmAssistantSearchResult()
  {
    lPageNum = 0;
    lTotalPages = 0;
  }
}KTBAlarmAssistantSearchResult;

class CAlarmAssistantTable : public CTableBase
{
public:
  CAlarmAssistantTable();
  ~CAlarmAssistantTable();
  bool  Initialise(const otl_connect &dbconnect);
  bool  Add(const otl_connect &dbconnect, const std::list<std::shared_ptr<KTBAlarmAssistant>> &lstInfo, std::list<uint32_t> &lstID);
  bool  Delete(const otl_connect &dbconnect, const std::list<uint32_t> &lstID);
  bool  Modify(const otl_connect &dbconnect, const std::list<std::shared_ptr<KTBAlarmAssistant>> &lstInfo);
  bool  Search(const otl_connect &dbconnect, uint32_t ulGlobalID, std::list<std::shared_ptr<KTBAlarmAssistant>> &lstInfo);
  bool  Search_Num(const otl_connect &dbconnect, const std::shared_ptr<KTBAlarmAssistantSearchCondition_Num> ptCondition, std::shared_ptr<KTBAlarmAssistantSearchResult_Num> ptResult);
  bool  SearchPaging(const otl_connect &dbconnect, const std::shared_ptr<KTBAlarmAssistantSearchCondition> ptCondition, std::shared_ptr<KTBAlarmAssistantSearchResult> ptResult);
};

#endif // !_ALARM_ASSISTANT_TABLE_H_
