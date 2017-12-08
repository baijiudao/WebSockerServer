//------------------------------------------------------------------------------
// �������ڣ�2017-09-18 14:31:16
// �ļ�����������ʵ��һЩ��������
//------------------------------------------------------------------------------
#ifndef _TABLE_BASE_H_
#define _TABLE_BASE_H_

#include <utility>
#include <string>
#include <stdint.h>
#include <list>
#include <memory>
#include <atomic>
#include <time.h>

#define OTL_ODBC_MSSQL_2005
#define OTL_STL
#define OTL_STREAM_POOLING_ON
#define OTL_CONNECT_POOL_ON

#include <otlv4.h>
#include <Logger.h>
#include <Define.h>

typedef otl_connect_pool<otl_connect, otl_exception>  COTLConnectPool;

// std::pair<��������, �����Ƿ���Ч>

// otl�쳣��ʾ��Ϣ
#define OTL_CUSTOM_EXCEPTION_IDENTITY_INVALID   "otl custom exception : Identity Invalid"

// otlʱ�侫��
#define OTL_CUSTOM_TIME_PRECISION   3   // 3��ʾ����Ϊ���루���Ϊ9��

class CTableBase
{
protected:
  bool OnInitialise(const otl_connect &dbconnect, const std::string &strSQL);
public:
  CTableBase();
  ~CTableBase();
};

#endif // !_TABLE_BASE_H_
