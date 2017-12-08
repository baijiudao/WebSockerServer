//------------------------------------------------------------------------------
// 创建日期：2017-09-18 14:31:16
// 文件描述：用来实现一些公共函数
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

// std::pair<数据类型, 数据是否有效>

// otl异常提示信息
#define OTL_CUSTOM_EXCEPTION_IDENTITY_INVALID   "otl custom exception : Identity Invalid"

// otl时间精度
#define OTL_CUSTOM_TIME_PRECISION   3   // 3表示精度为毫秒（最大为9）

class CTableBase
{
protected:
  bool OnInitialise(const otl_connect &dbconnect, const std::string &strSQL);
public:
  CTableBase();
  ~CTableBase();
};

#endif // !_TABLE_BASE_H_
