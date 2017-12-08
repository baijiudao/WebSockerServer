//------------------------------------------------------------------------------
// �������ڣ�2017-10-11 14:00:45
// �ļ������������������ݱ�AlarmAssistant
//------------------------------------------------------------------------------
#ifndef _DATABASEREPORTFORMS_H_
#define _DATABASEREPORTFORMS_H_

#include "TableBase.h"

//------------------------------------------------------------------------------
// �������ڣ�2017-10-11 10:49:45 
// ��    ����������������ϻ�����ת��
//------------------------------------------------------------------------------

#define CHECK_SPACE_MINUTES 45
#define CHECK_TIME_POS       2
#define OLD_DATA_LIMIT_DAY  90

//IOValue�����ݽṹ
typedef struct _IOValueData
{
    uint32_t ID;                                    //�����ڱ��еĴ洢id
    uint32_t IOGlobalId;                            //������ȫ��id
    uint32_t TagType;                               //����������
    string IOValue;                                 //����ֵ
    string IOTime;                                  //�����ɼ�ʱ��
    uint32_t quality;                               //����Ȩ��
    _IOValueData()
    {
        ID = -1;
        IOGlobalId = 0;
        IOGlobalId = -1;
    }
}IOValueData;


class DatabaseReportForms : public CTableBase
{
public:
    DatabaseReportForms();
    ~DatabaseReportForms();
    void startCheck(std::shared_ptr<COTLConnectPool> pDBConnectPool);
private:
    bool CheckReportFormsData(const otl_connect &dbconnect);
    bool InsertDataToOldForms(const std::list<std::shared_ptr<IOValueData>> &ioInfo, const otl_connect &dbconnect);
    bool DeleteDataFromIOForms(const std::list<std::shared_ptr<IOValueData>> &ioInfo, const otl_connect &dbconnect);
private:
    std::shared_ptr<COTLConnectPool> m_pDBConnectPool;
};

#endif // !_DATABASEREPORTFORMS_H_
