#include "stdafx.h"
#include "DatabaseReportForms.h"

using namespace std;

//------------------------------------------------------------------------------
DatabaseReportForms::DatabaseReportForms()
{
}

//------------------------------------------------------------------------------
DatabaseReportForms::~DatabaseReportForms()
{
}

void DatabaseReportForms::startCheck(std::shared_ptr<COTLConnectPool> pDBConnectPool)
{
    m_pDBConnectPool = pDBConnectPool;
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    CheckReportFormsData(const_cast<otl_connect &>(*pDBConnect));
    m_pDBConnectPool->put(std::move(pDBConnect));
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::minutes(CHECK_SPACE_MINUTES));
        SYSTEMTIME stNow;
        ::GetLocalTime(&stNow);
        if (CHECK_TIME_POS == stNow.wHour)
        {
            COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
            CheckReportFormsData(const_cast<otl_connect &>(*pDBConnect));
            m_pDBConnectPool->put(std::move(pDBConnect));
        }
    }
}

//------------------------------------------------------------------------------
bool DatabaseReportForms::CheckReportFormsData(const otl_connect &dbconnect)
{
    try
    {
        SYSTEMTIME stNow, stTmp;
        ULARGE_INTEGER ftNow;/*FILETIME*/

        ::GetLocalTime(&stNow);
        stNow.wSecond = stNow.wMilliseconds = 0;
        SystemTimeToFileTime(&stNow, (FILETIME*)&ftNow);
        unsigned __int64 dft = OLD_DATA_LIMIT_DAY * 864000000000;
        ftNow.QuadPart = ftNow.QuadPart - dft;
        if (ftNow.QuadPart < 0)
        {
            return false;
        }
        FileTimeToSystemTime((FILETIME*)&ftNow, &stTmp);
        stTmp.wHour = stTmp.wMinute = stTmp.wSecond = stTmp.wMilliseconds = 0;

        string  strSQL = R"(select * from BMS_IOValue where IOTime between '2010-01-01 00:00:00.000' and ')";
        //2011-01-01 00:00:00.000'";
        char buffer[64] = { 0 };
        sprintf_s(buffer, "%u", stTmp.wYear);
        string strTemp = buffer;
        if (4 == strTemp.size())
        {
            strSQL += strTemp + R"(-)";
        }
        else
        {
            return false;
        }
        memset(buffer, 0, sizeof(buffer) / sizeof(char));
        sprintf_s(buffer, "%u", stTmp.wMonth);
        strTemp = buffer;
        if (2 == strTemp.size())
        {
            strSQL += strTemp + R"(-)";
        }
        else if (1 == strTemp.size())
        {
            strSQL += R"(0)" + strTemp + R"(-)";
        }
        else
        {
            return false;
        }
        memset(buffer, 0, sizeof(buffer) / sizeof(char));
        sprintf_s(buffer, "%u", stTmp.wDay);
        strTemp = buffer;
        if (2 == strTemp.size())
        {
            strSQL += strTemp;
        }
        else if (1 == strTemp.size())
        {
            strSQL += R"(0)" + strTemp;
        }
        else
        {
            return false;
        }
        strSQL += R"( 00:00:00.000')";

        otl_connect &db = const_cast<otl_connect &>(dbconnect);
        otl_nocommit_stream strm;
        std::list<std::shared_ptr<IOValueData>> ioValueList;
        try
        {
            strm.open(strSQL.size(), strSQL.c_str(), db);

            // 开始转储老化数据
            while (!strm.eof())
            {
                shared_ptr<IOValueData>  pIoValue = make_shared<IOValueData>();
                strm >> pIoValue->ID;
                strm >> pIoValue->IOGlobalId;
                strm >> pIoValue->TagType;
                strm >> pIoValue->IOValue;
                strm >> pIoValue->IOTime;
                strm >> pIoValue->quality;
                ioValueList.push_back(pIoValue);
            }
            strm.close();
        }
        catch (otl_exception &e)
        {
            // 回滚事务
            db.rollback();
            strm.close();
            cout << e.code << "：" << e.msg << " " << e.stm_text << " " << e.var_info << endl;
            LOG_ERROR << e.code << "：" << e.msg << " " << e.stm_text << " " << e.var_info;
            return false;
        }
        InsertDataToOldForms(ioValueList, dbconnect);
        DeleteDataFromIOForms(ioValueList, dbconnect);
        // 将连接归还给连接池

    }
    catch (otl_exception &e)
    {
        LOG_ERROR << e.code << "：" << e.msg << " " << e.stm_text << " " << e.var_info;
    }
    return true;
}

//------------------------------------------------------------------------------
bool DatabaseReportForms::InsertDataToOldForms(const std::list<std::shared_ptr<IOValueData>> &ioInfo, const otl_connect &dbconnect)
{
    // 如果没有要添加的数据，则返回成功
    if (ioInfo.empty())
    {
        return true;
    }
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(
INSERT INTO BMS_IOHistroyValue
           (IOGlobalId,
            TagType,
            IOValue,
            IOTime,
            quality)
      VALUES
           (:v_IOGlobalId<int>,
            :v_TagType<int>,
            :v_IOValue<char[32]>,
            :v_IOTime<char[32]>,
            :v_quality<int>)
)";

    try
    {
        int nID = 0;
        strm.open(strSQL.size(), strSQL.c_str(), db, otl_implicit_select);

        // 添加所有数据
        for (auto &itemInfo : ioInfo)
        {
            strm << itemInfo->IOGlobalId;
            strm << itemInfo->TagType;
            strm << itemInfo->IOValue;
            strm << itemInfo->IOTime;
            strm << itemInfo->quality;
        }

        strm.flush();
        strm.close();
        // 提交事务
        db.commit();
    }
    catch (otl_exception &e)
    {
        strm.close();
        // 回滚事务
        db.rollback();
        cout << e.code << "：" << e.msg << " " << e.stm_text << " " << e.var_info << endl;
        LOG_ERROR << e.code << "：" << e.msg << " " << e.stm_text << " " << e.var_info;
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
bool DatabaseReportForms::DeleteDataFromIOForms(const std::list<std::shared_ptr<IOValueData>> &ioInfo, const otl_connect &dbconnect)
{
    // 如果没有要添加的数据，则返回成功
    if (ioInfo.empty())
    {
        return true;
    }
    // 获取一个连接
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(
DELETE FROM BMS_IOValue
WHERE ID = :v_ID<int>
)";
    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);

        // 添加所有数据
        for (auto &itemID : ioInfo)
            strm << itemID->ID;

        strm.flush();
        // 提交事务
        db.commit();
        strm.close();
    }
    catch (otl_exception &e)
    {
        // 回滚事务
        db.rollback();
        strm.close();
        cout << e.code << "：" << e.msg << " " << e.stm_text << " " << e.var_info << endl;
        LOG_ERROR << e.code << "：" << e.msg << " " << e.stm_text << " " << e.var_info;
        return false;
    }

    return true;
}