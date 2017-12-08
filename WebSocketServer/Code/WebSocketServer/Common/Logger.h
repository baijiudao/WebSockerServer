//------------------------------------------------------------------------------
// �������ڣ�2017-10-17 11:12:25
// �ļ���������־��
//  trace��������Ϣ�������ڿ����׶���ʾ��������
//  debug��������Ϣ��������ӡ������Ϣ����ʾ��������)
//  info����ͨ��Ϣ������չʾһЩ��Ҫ��Ϣ���汾��Ϣ��������Ϣ�ȣ�
//  warning��������Ϣ������������ֵfalseʱ�������ں����ڲ���ʾ��Щ��Ϣ
//  error��������Ϣ�������������׳��쳣����ʾ�쳣��Ϣ
//  fatal���ش������Ϣ�����������һЩ������Ҫ��ֹ����ʱ����ʹ��
// һ�㷢��������־����������õ�error
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

//#define _LOG_SIMPLE_PRINT_CONSOLE_  // �Ƚϼ򵥵�ֻ���������̨�ķ�ʽ

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

// ����������־ģʽ
typedef enum class _eLogMode : uint32_t
{
  CONSOLE = 0,  // ֻ���������̨
  FILE,         // ֻ������ļ�
  ALL           // ͬʱ���������̨���ļ�
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
