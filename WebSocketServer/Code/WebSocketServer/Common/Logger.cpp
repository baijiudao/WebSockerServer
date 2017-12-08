#include "stdafx.h"
#include "Logger.h"
#include <windows.h>
#include <stdio.h>


#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/thread/thread.hpp>
#include <boost/log/core.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/formatter_parser.hpp>

namespace attrs = boost::log::attributes;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;

// -- CLogger --

#ifdef _LOG_SIMPLE_PRINT_CONSOLE_
//------------------------------------------------------------------------------
CLogger::CLogger(logging::trivial::severity_level elevel)
{
  logging::core::get()->set_filter(logging::trivial::severity >= elevel);
}
#else
//------------------------------------------------------------------------------
// 可以同时打印到控制台和文件
//------------------------------------------------------------------------------
CLogger::CLogger(logging::trivial::severity_level elevel, ELogMode eLM)
{
  // 输出到控制台（只输出debug及其以上等级的）
  if ((ELogMode::ALL == eLM) || (ELogMode::CONSOLE == eLM))
    OnLogConsole((logging::trivial::trace == elevel) ? logging::trivial::debug : elevel);

  // 输出到文件
  if ((ELogMode::ALL == eLM) || (ELogMode::FILE == eLM)) OnLogFile(elevel, "");

  // 通用配置
  logging::add_common_attributes();
}

//------------------------------------------------------------------------------
// 可以同时打印到控制台和文件（生成到指定文件中，如果文件名为空，则文件名为进程名）
//------------------------------------------------------------------------------
CLogger::CLogger(const std::string &strFileName,
  logging::trivial::severity_level elevel, ELogMode eLM)
{
  // 输出到控制台（只输出debug及其以上等级的）
  if ((ELogMode::ALL == eLM) || (ELogMode::CONSOLE == eLM))
    OnLogConsole((logging::trivial::trace == elevel) ? logging::trivial::debug : elevel);

  // 输出到单个文件
  if ((ELogMode::ALL == eLM) || (ELogMode::FILE == eLM))
  {
    std::string strLogName = strFileName;

#ifndef _UNICODE
    // 获取当前模块文件名
    if (strLogName.empty())
    {
      TCHAR szPath[MAX_PATH] = { 0 };
      if (GetModuleFileName(NULL, szPath, MAX_PATH))
        strLogName = szPath;

      boost::filesystem::path pathName(strLogName.c_str());
      strLogName = pathName.stem().string();
    }
#endif // _UNICODE

    // 如果没有获取路径名
    if (strLogName.empty()) strLogName = "Logger";
    // 添加后缀名
    strLogName += ".log";

    // 输出到文件
    OnLogFile(elevel, strLogName);
  }

  // 通用配置
  logging::add_common_attributes();
}

//------------------------------------------------------------------------------
// 输出到控制台
//------------------------------------------------------------------------------
void CLogger::OnLogConsole(logging::trivial::severity_level elevel)
{
  logging::register_simple_formatter_factory< logging::trivial::severity_level, char >("Severity");
  // 如果是ATL工程，则必须使用std::cout，否则无法输出到控制台
  // 如果是其他工程，则推荐使用std::clog
  auto pCons = logging::add_console_log(std::cout, keywords::format = "[%TimeStamp%] [%ThreadID%] [%Severity%] %Message%");
  pCons->set_filter(logging::trivial::severity >= elevel);
}

//------------------------------------------------------------------------------
// 与CLogger构造函数不同，strFileName为空则log会生成到多个文件中
//------------------------------------------------------------------------------
void CLogger::OnLogFile(logging::trivial::severity_level elevel,
  const std::string &strFileName)
{
  if (strFileName.empty())
  {
    // 输出到文件
    auto pSink = logging::add_file_log
      (
      keywords::file_name = "%Y-%m-%d_%H-%M-%S.%N.log",
      keywords::rotation_size = 10 * 1024 * 1024,
      keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
      keywords::min_free_space = 30 * 1024 * 1024,
      keywords::auto_flush = true,
      keywords::format =
      (
      expr::format("[%1%] [%2%] [%3%] %4%")
      % expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S.%f")
      % expr::attr<attrs::current_thread_id::value_type >("ThreadID")
      % logging::trivial::severity
      % expr::smessage
      )
      );

    pSink->locked_backend()->set_file_collector(sinks::file::make_collector(
      keywords::target = "logs",  //文件夹名
      keywords::max_files = 10    // 日志文件最大数量
      ));

    pSink->set_filter(logging::trivial::severity >= elevel);
  }
  else
  {
    auto pSink = logging::add_file_log
      (
      keywords::file_name = strFileName,
      keywords::auto_flush = true,
      keywords::format =
      (
      expr::format("[%1%] [%2%] [%3%] %4%")
      % expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S.%f")
      % expr::attr<attrs::current_thread_id::value_type >("ThreadID")
      % logging::trivial::severity
      % expr::smessage
      )
      );

    pSink->set_filter(logging::trivial::severity >= elevel);
  }
}
#endif // _LOG_SIMPLE_PRINT_CONSOLE_

//------------------------------------------------------------------------------
CLogger::~CLogger()
{
  logging::core::get()->remove_all_sinks();
}

