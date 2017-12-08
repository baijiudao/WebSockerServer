//------------------------------------------------------------------------------
// 创建日期：2017-10-11 14:00:45
// 文件描述：用来操作数据表AlarmAssistant
//------------------------------------------------------------------------------
#ifndef _DATABASEREPORTFORMS_H_
#define _DATABASEREPORTFORMS_H_

#include "TableBase.h"

//------------------------------------------------------------------------------
// 创建日期：2017-10-11 10:49:45 
// 描    述：报表的生成与老化数据转储
//------------------------------------------------------------------------------

#define CHECK_SPACE_MINUTES 45
#define CHECK_TIME_POS       2
#define OLD_DATA_LIMIT_DAY  90

//IOValue表数据结构
typedef struct _IOValueData
{
    uint32_t ID;                                    //变量在表中的存储id
    uint32_t IOGlobalId;                            //变量的全局id
    uint32_t TagType;                               //变量的类型
    string IOValue;                                 //变量值
    string IOTime;                                  //变量采集时间
    uint32_t quality;                               //变量权重
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
