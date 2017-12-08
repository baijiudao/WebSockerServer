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
// ����ͬʱ��ӡ������̨���ļ�
//------------------------------------------------------------------------------
CLogger::CLogger(logging::trivial::severity_level elevel, ELogMode eLM)
{
  // ���������̨��ֻ���debug�������ϵȼ��ģ�
  if ((ELogMode::ALL == eLM) || (ELogMode::CONSOLE == eLM))
    OnLogConsole((logging::trivial::trace == elevel) ? logging::trivial::debug : elevel);

  // ������ļ�
  if ((ELogMode::ALL == eLM) || (ELogMode::FILE == eLM)) OnLogFile(elevel, "");

  // ͨ������
  logging::add_common_attributes();
}

//------------------------------------------------------------------------------
// ����ͬʱ��ӡ������̨���ļ������ɵ�ָ���ļ��У�����ļ���Ϊ�գ����ļ���Ϊ��������
//------------------------------------------------------------------------------
CLogger::CLogger(const std::string &strFileName,
  logging::trivial::severity_level elevel, ELogMode eLM)
{
  // ���������̨��ֻ���debug�������ϵȼ��ģ�
  if ((ELogMode::ALL == eLM) || (ELogMode::CONSOLE == eLM))
    OnLogConsole((logging::trivial::trace == elevel) ? logging::trivial::debug : elevel);

  // ����������ļ�
  if ((ELogMode::ALL == eLM) || (ELogMode::FILE == eLM))
  {
    std::string strLogName = strFileName;

#ifndef _UNICODE
    // ��ȡ��ǰģ���ļ���
    if (strLogName.empty())
    {
      TCHAR szPath[MAX_PATH] = { 0 };
      if (GetModuleFileName(NULL, szPath, MAX_PATH))
        strLogName = szPath;

      boost::filesystem::path pathName(strLogName.c_str());
      strLogName = pathName.stem().string();
    }
#endif // _UNICODE

    // ���û�л�ȡ·����
    if (strLogName.empty()) strLogName = "Logger";
    // ��Ӻ�׺��
    strLogName += ".log";

    // ������ļ�
    OnLogFile(elevel, strLogName);
  }

  // ͨ������
  logging::add_common_attributes();
}

//------------------------------------------------------------------------------
// ���������̨
//------------------------------------------------------------------------------
void CLogger::OnLogConsole(logging::trivial::severity_level elevel)
{
  logging::register_simple_formatter_factory< logging::trivial::severity_level, char >("Severity");
  // �����ATL���̣������ʹ��std::cout�������޷����������̨
  // ������������̣����Ƽ�ʹ��std::clog
  auto pCons = logging::add_console_log(std::cout, keywords::format = "[%TimeStamp%] [%ThreadID%] [%Severity%] %Message%");
  pCons->set_filter(logging::trivial::severity >= elevel);
}

//------------------------------------------------------------------------------
// ��CLogger���캯����ͬ��strFileNameΪ����log�����ɵ�����ļ���
//------------------------------------------------------------------------------
void CLogger::OnLogFile(logging::trivial::severity_level elevel,
  const std::string &strFileName)
{
  if (strFileName.empty())
  {
    // ������ļ�
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
      keywords::target = "logs",  //�ļ�����
      keywords::max_files = 10    // ��־�ļ��������
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

