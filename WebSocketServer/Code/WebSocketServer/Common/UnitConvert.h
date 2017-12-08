//------------------------------------------------------------------------------
// 创建日期：2017-07-10 16:42:05
// 文件描述：该工程所需要使用的转换函数
//------------------------------------------------------------------------------
#ifndef _UNIT_CONVERT_H_
#define _UNIT_CONVERT_H_

#include <string>
#include <memory>
#include <tuple>
#include "Singleton.h"

//#include "Define.h"

using namespace std;

typedef std::tuple < uint16_t, uint8_t, uint8_t,  // 年 月 日
  uint8_t, uint8_t, uint8_t,  // 时 分 秒
  uint32_t > TupleTime; // 毫秒

// 代表TupleTime对象中时间的每个字段
typedef enum class _eTupleTimeField : uint8_t
{
  YEAR = 0,
  MONTH,
  DAY,
  HOUR,
  MINUTE,
  SECOND,
  MILLISECOND
}ETupleTimeField;

// 单例模式
class CUnitConvert : public ISingleton<CUnitConvert>
{
protected:
  string  WCharToMultiByte(const wstring &wstrValue, unsigned int nCodePage);
  wstring MultiByteToWChar(const string &strValue, unsigned int nCodePage);
public:
  CUnitConvert();
  ~CUnitConvert();
  // 字符编码转换
  string  WStringToString(const wstring &wstrValue);
  wstring StringToWString(const string &strValue);
  string  AnsiToUTF8(const string &strValue);
  string  UTF8ToAnsi(const string &strValue);
  std::string   UnicodeToAnsi(std::wstring wstrValue);
  std::wstring  AnsiToUnicode(std::string strValue);
  std::string   UnicodeToUTF8(std::wstring wstrValue);
  std::wstring  UTF8ToUnicode(std::string strValue);
  // 将整型时间和DCOM的date时间相互转换
  double    UINT64ToVariantTime(uint64_t ullTimeMs);
  uint64_t  VariantTimeToUINT64(double vTime);
  // 整型时间和文件时间相互转换
  void  UINT64ToFileTime(uint64_t ullTimeMs, FILETIME &ft);
  void  FileTimeToUINT64(const FILETIME &ft, uint64_t &ullTimeMs);
  // 将整型时间和tuple类型相互转换
  TupleTime UINT64ToTupleTime(uint64_t ullTimeMs);
  uint64_t  TupleTimeToUINT64(TupleTime tupTime);
  // 将整型时间和分段式时间互相转换
  void  UINT64ToTime(uint64_t ullTimeMs, uint16_t &usYear, uint8_t &ucMonth,
    uint8_t &ucDay, uint8_t &ucHour, uint8_t &ucMinute, uint8_t &ucSecond,
    uint32_t &ulMillisecond);
  void  TimeToUINT64(uint16_t usYear, uint8_t ucMonth, uint8_t ucDay,
    uint8_t ucHour, uint8_t ucMinute, uint8_t ucSecond, uint32_t ulMillisecond,
    uint64_t &ullTimeMs);
};

#endif // !_UNIT_CONVERT_H_

