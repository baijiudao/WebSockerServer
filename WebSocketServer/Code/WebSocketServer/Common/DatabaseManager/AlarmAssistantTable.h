//------------------------------------------------------------------------------
// �������ڣ�2017-09-11 14:00:45
// �ļ������������������ݱ�AlarmAssistant
//------------------------------------------------------------------------------
#ifndef _ALARM_ASSISTANT_TABLE_H_
#define _ALARM_ASSISTANT_TABLE_H_

#include "TableBase.h"

//------------------------------------------------------------------------------
// �������ڣ�2017-09-06 10:49:45 
// ��    ���������ݿ��е�AlarmAssistant���澯���������Ӧ
//------------------------------------------------------------------------------
typedef struct _TBAlarmAssistant
{
  std::pair<uint32_t, bool>     lID;                            // �澯������ϢID
  std::pair<uint32_t, bool>     lGlobalID;                      // �豸ID
  std::pair<std::string, bool>  strDeviceName;                  // �豸����
  std::pair<uint32_t, bool>     lCameraIP;                      // ��������ͷIP��IPV4��
  std::pair<std::string, bool>  strCameraUserName;              // ����ͷ�����û���
  std::pair<std::string, bool>  strCameraPassword;              // ����ͷ��������
  std::pair<std::string, bool>  strEmergencyPlanFileName;       // Ӧ��Ԥ���ļ���
  std::pair<std::string, bool>  strEmergencyPlanStorageAddress; // Ӧ��Ԥ����ŵ�ַ
  std::pair<std::string, bool>  strLinkageSchemeName;           // ������������
  std::pair<std::string, bool>  strLinkageSchemeDescription;    // ��������˵��
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

// AlarmAssistant�Ĳ�ѯ����
typedef struct _TBAlarmAssistantSearchParameter
{
  std::pair<uint32_t, bool>     lGlobalID;            // �豸ID����ѡ��
  std::pair<std::string, bool>  strDeviceName;        // �豸������ѡ��
  std::pair<std::string, bool>  strLinkageSchemeName; // �����������ơ���ѡ��
  _TBAlarmAssistantSearchParameter()
  {
    lGlobalID = { 0, false };
    strDeviceName = { "", false };
    strLinkageSchemeName = { "", false };
  }
}KTBAlarmAssistantSearchParameter;

// AlarmAssistant��ѯ���������������أ�
typedef struct _TBAlarmAssistantSearchCondition_Num
{
  uint32_t  lCount;                             // ����Ҫ��ȡ������
  uint32_t  lStartNum;                          // ����������ʼ����
  KTBAlarmAssistantSearchParameter  tParameter; // �����ѯ����
  _TBAlarmAssistantSearchCondition_Num()
  {
    lCount = 0;
    lStartNum = 0;
  }
}KTBAlarmAssistantSearchCondition_Num;

// AlarmAssistant��ѯ��������ҳ���أ�
typedef struct _TBAlarmAssistantSearchCondition
{
  uint32_t  lPageSize;                          // ҳ��С������������
  uint32_t  lPageNum;                           // ����ڼ�ҳ
  KTBAlarmAssistantSearchParameter  tParameter; // �����ѯ����
  _TBAlarmAssistantSearchCondition()
  {
    lPageSize = 0;
    lPageNum = 0;
  }
}KTBAlarmAssistantSearchCondition;

// AlarmAssistant��ѯ��������������أ�
typedef struct _TBAlarmAssistantSearchResult_Num
{
  uint32_t  lStartNum;                                    // ��ѯ�����һ�����������е�λ��
  uint32_t  lTotal;                                       // ��ѯ���������
  std::list<std::shared_ptr<KTBAlarmAssistant>> lstData;  // ��ǰ���ص�����
  _TBAlarmAssistantSearchResult_Num()
  {
    lStartNum = 0;
    lTotal = 0;
  }
}KTBAlarmAssistantSearchResult_Num;

// AlarmAssistant��ѯ�������ҳ���أ�
typedef struct _TBAlarmAssistantSearchResult
{
  uint32_t  lPageNum;                                     // ��ѯ������صĵڼ�ҳ
  uint32_t  lTotalPages;                                  // ��ѯ�������ҳ��
  std::list<std::shared_ptr<KTBAlarmAssistant>> lstData;  // ��ǰ���ص�����
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
