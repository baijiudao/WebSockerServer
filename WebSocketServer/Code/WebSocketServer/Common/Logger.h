//------------------------------------------------------------------------------
// 创建日期：2017-10-17 11:12:25
// 文件描述：日志类
//  trace：冗余信息，用来在开发阶段显示大量数据
//  debug：调试信息，用来打印调试信息（显示少量数据)
//  info：普通信息，用来展示一些必要信息（版本信息，主机信息等）
//  warning：警告信息，当函数返回值false时，可以在函数内部显示这些信息
//  error：错误信息，当程序运行抛出异常，提示异常信息
//  fatal：重大错误信息，当程序出现一些错误需要终止程序时可以使用
// 一般发布程序日志级别可以设置到error
//------------------------------------------------------------------------------
#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <fstream>
#include <string>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sources/global_logger_storage.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;

//#define _LOG_SIMPLE_PRINT_CONSOLE_  // 比较简单的只输出到控制台的方式

#define CODE_INFORMATION  " [FILE:" << __FILE__ << " LINE:" << __LINE__ << " FUNCTION:" << __FUNCTION__ << "] "

#ifdef _LOG_SIMPLE_PRINT_CONSOLE_
#define LOG_TRACE   BOOST_LOG_TRIVIAL(trace)
#define LOG_DEBUG   BOOST_LOG_TRIVIAL(debug)
#define LOG_INFO    BOOST_LOG_TRIVIAL(info)
#define LOG_WARN    BOOST_LOG_TRIVIAL(warning) << CODE_INFORMATION
#define LOG_ERROR   BOOST_LOG_TRIVIAL(error) << CODE_INFORMATION
#define LOG_FATAL   BOOST_LOG_TRIVIAL(fatal) << CODE_INFORMATION
#else
BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(samplogger, src::severity_logger_mt<logging::trivial::severity_level>)

#define LOG_TRACE   BOOST_LOG_SEV(samplogger::get(), logging::trivial::trace)
#define LOG_DEBUG   BOOST_LOG_SEV(samplogger::get(), logging::trivial::debug)
#define LOG_INFO    BOOST_LOG_SEV(samplogger::get(), logging::trivial::info)
#define LOG_WARN    BOOST_LOG_SEV(samplogger::get(), logging::trivial::warning) << CODE_INFORMATION
#define LOG_ERROR   BOOST_LOG_SEV(samplogger::get(), logging::trivial::error) << CODE_INFORMATION
#define LOG_FATAL   BOOST_LOG_SEV(samplogger::get(), logging::trivial::fatal) << CODE_INFORMATION

// 用来定义日志模式
typedef enum class _eLogMode : uint32_t
{
  CONSOLE = 0,  // 只输出到控制台
  FILE,         // 只输出到文件
  ALL           // 同时输出到控制台和文件
}ELogMode;
#endif // _LOG_SIMPLE_PRINT_CONSOLE_

class CLogger
{
#ifndef _LOG_SIMPLE_PRINT_CONSOLE_
protected:
  void  OnLogConsole(logging::trivial::severity_level elevel);
  void  OnLogFile(logging::trivial::severity_level elevel, const std::string &strFileName);
#endif // !_LOG_SIMPLE_PRINT_CONSOLE_
public:
#ifdef _LOG_SIMPLE_PRINT_CONSOLE_
  CLogger(logging::trivial::severity_level elevel = logging::trivial::trace);
#else
  CLogger(logging::trivial::severity_level elevel = logging::trivial::trace, ELogMode eLM = ELogMode::ALL);
  CLogger(const std::string &strFileName, logging::trivial::severity_level elevel = logging::trivial::trace, ELogMode eLM = ELogMode::ALL);
#endif // _LOG_SIMPLE_PRINT_CONSOLE_
  ~CLogger();
};

#endif // !_LOGGER_H_
