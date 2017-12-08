#include "stdafx.h"
#include "UnitConvert.h"

// -- CUnitConvert --

//------------------------------------------------------------------------------
CUnitConvert::CUnitConvert()
{
}

//------------------------------------------------------------------------------
CUnitConvert::~CUnitConvert()
{
}

//------------------------------------------------------------------------------
string CUnitConvert::WCharToMultiByte(const wstring &wstrValue, unsigned int nCodePage)
{
  // 获取有多少个字符，并不是字符串的长度，当一个字符占2个位置的时候当作长度就会出错
  int nCount = WideCharToMultiByte(nCodePage, 0, wstrValue.data(), -1, NULL, 0, NULL, 0);
  // 给要转换的数据申请空间，多申请一位，用来放结束符
  char* pcValue = new char[(nCount + 1) * sizeof(char)];
  memset(pcValue, 0, (nCount + 1) * sizeof(char));

  //将宽字节字符串转换为多字节字符串  
  WideCharToMultiByte(nCodePage, 0, wstrValue.data(), -1, pcValue, nCount, NULL, 0);
  string strValue = pcValue;

  // 释放空间
  delete[] pcValue;
  return strValue;
}

//------------------------------------------------------------------------------
wstring CUnitConvert::MultiByteToWChar(const string &strValue, unsigned int nCodePage)
{
  int nCount = MultiByteToWideChar(nCodePage, 0, strValue.data(), -1, NULL, 0);
  wchar_t*  pwcValue = new wchar_t[(nCount + 1) * sizeof(wchar_t)];
  memset(pwcValue, 0, (nCount + 1) * sizeof(wchar_t));

  //多字节转换为宽字节  
  MultiByteToWideChar(nCodePage, 0, strValue.data(), -1, pwcValue, nCount);
  wstring wstrValue = pwcValue;

  // 释放空间
  delete[] pwcValue;
  return wstrValue;
}

//------------------------------------------------------------------------------
string CUnitConvert::WStringToString(const wstring &wstrValue)
{
  return WCharToMultiByte(wstrValue, CP_ACP);
}

//------------------------------------------------------------------------------
wstring CUnitConvert::StringToWString(const string &strValue)
{
  return MultiByteToWChar(strValue, CP_ACP);
}

//------------------------------------------------------------------------------
std::string CUnitConvert::AnsiToUTF8(const string &strValue)
{
  wstring wstrValue = MultiByteToWChar(strValue, CP_ACP);
  return WCharToMultiByte(wstrValue, CP_UTF8);
}

//------------------------------------------------------------------------------
std::string CUnitConvert::UTF8ToAnsi(const string &strValue)
{
  wstring wstrValue = MultiByteToWChar(strValue, CP_UTF8);
  return WCharToMultiByte(wstrValue, CP_ACP);
}

//------------------------------------------------------------------------------
// 将Unicode（默认UTF-16（UCS-2））编码格式的转换为Ansi编码格式
//------------------------------------------------------------------------------
std::string CUnitConvert::UnicodeToAnsi(std::wstring wstrValue)
{
  return WCharToMultiByte(wstrValue, CP_ACP);
}

//------------------------------------------------------------------------------
// 将Ansi编码格式的转换为Unicode（默认UTF-16（UCS-2））编码格式
//------------------------------------------------------------------------------
std::wstring CUnitConvert::AnsiToUnicode(std::string strValue)
{
  return MultiByteToWChar(strValue, CP_ACP);
}

//------------------------------------------------------------------------------
// 将Unicode字符串转换为UTF-8字符串
//------------------------------------------------------------------------------
std::string CUnitConvert::UnicodeToUTF8(std::wstring wstrValue)
{
  return WCharToMultiByte(wstrValue, CP_UTF8);
}

//------------------------------------------------------------------------------
// 将UTF-8字符串转换为Unicode字符串
//------------------------------------------------------------------------------
std::wstring CUnitConvert::UTF8ToUnicode(std::string strValue)
{
  return MultiByteToWChar(strValue, CP_UTF8);
}

//------------------------------------------------------------------------------
// 将无符号64位整型的毫秒级时间转换为VariantTime
//------------------------------------------------------------------------------
double CUnitConvert::UINT64ToVariantTime(uint64_t ullTimeMs)
{
  time_t  tmp = ullTimeMs / 1000;
  struct tm *ptm = localtime(&tmp);
  SYSTEMTIME st = { 1900 + ptm->tm_year,
    1 + ptm->tm_mon,
    ptm->tm_wday,
    ptm->tm_mday,
    ptm->tm_hour,
    ptm->tm_min,
    ptm->tm_sec,
    ullTimeMs % 1000 };

  double vTime = 0.0;
  SystemTimeToVariantTime(&st, &vTime);
  return vTime;
}

//------------------------------------------------------------------------------
// 将时间转换为一个无符号64位整型的毫秒级时间
//------------------------------------------------------------------------------
uint64_t CUnitConvert::VariantTimeToUINT64(double vTime)
{
  SYSTEMTIME st;
  uint64_t ullTimeMs = 0;
  ::ZeroMemory(&st, sizeof(SYSTEMTIME));
  if (VariantTimeToSystemTime(vTime, &st))
  {
    struct tm gm = { st.wSecond,
      st.wMinute,
      st.wHour,
      st.wDay,
      st.wMonth - 1,
      st.wYear - 1900,
      st.wDayOfWeek,
      0, 0 };

    time_t t = mktime(&gm);
    ullTimeMs = (t * 1000) + st.wMilliseconds;
  }

  return ullTimeMs;
}

//------------------------------------------------------------------------------
void CUnitConvert::UINT64ToFileTime(uint64_t ullTimeMs, FILETIME &ft)
{
  time_t  tmp = ullTimeMs / 1000;
  struct tm *ptm = localtime(&tmp);
  SYSTEMTIME st = { 1900 + ptm->tm_year,
    1 + ptm->tm_mon,
    ptm->tm_wday,
    ptm->tm_mday,
    ptm->tm_hour,
    ptm->tm_min,
    ptm->tm_sec,
    ullTimeMs % 1000 };

  FileTimeToSystemTime((FILETIME*)&ft, &st);
}

//------------------------------------------------------------------------------
void CUnitConvert::FileTimeToUINT64(const FILETIME &ft, uint64_t &ullTimeMs)
{
  SYSTEMTIME st;
  ::ZeroMemory(&st, sizeof(SYSTEMTIME));
  if (SystemTimeToFileTime(&st, (FILETIME*)&ft))
  {
    struct tm gm = { st.wSecond,
      st.wMinute,
      st.wHour,
      st.wDay,
      st.wMonth - 1,
      st.wYear - 1900,
      st.wDayOfWeek,
      0, 0 };

    time_t t = mktime(&gm);
    ullTimeMs = (t * 1000) + st.wMilliseconds;
  }
}

//------------------------------------------------------------------------------
TupleTime CUnitConvert::UINT64ToTupleTime(uint64_t ullTimeMs)
{
  uint64_t  ullM = ullTimeMs / 1000;
  struct tm *ptmTmp = localtime((const time_t*)&ullM);

  return make_tuple(uint16_t(ptmTmp->tm_year + 1900),
    uint8_t(ptmTmp->tm_mon + 1),
    uint8_t(ptmTmp->tm_mday),
    uint8_t(ptmTmp->tm_hour),
    uint8_t(ptmTmp->tm_min),
    uint8_t(ptmTmp->tm_sec),
    uint32_t(ullTimeMs % 1000));
}

//------------------------------------------------------------------------------
uint64_t CUnitConvert::TupleTimeToUINT64(TupleTime tupTime)
{
  struct tm tmTmp;
  uint64_t  ullTimeMs = 0;

  tmTmp.tm_year = get<(const uint8_t)ETupleTimeField::YEAR>(tupTime) -1900;
  tmTmp.tm_mon = get<(const uint8_t)ETupleTimeField::MONTH>(tupTime) -1;
  tmTmp.tm_mday = get<(const uint8_t)ETupleTimeField::DAY>(tupTime);
  tmTmp.tm_hour = get<(const uint8_t)ETupleTimeField::HOUR>(tupTime);
  tmTmp.tm_min = get<(const uint8_t)ETupleTimeField::MINUTE>(tupTime);
  tmTmp.tm_sec = get<(const uint8_t)ETupleTimeField::SECOND>(tupTime);

  ullTimeMs = mktime(&tmTmp);
  ullTimeMs *= 1000;  // 转换为毫秒
  ullTimeMs += get<(const uint8_t)ETupleTimeField::MILLISECOND>(tupTime);
  return ullTimeMs;
}

//------------------------------------------------------------------------------
void CUnitConvert::UINT64ToTime(uint64_t ullTimeMs, uint16_t &usYear,
  uint8_t &ucMonth, uint8_t &ucDay, uint8_t &ucHour, uint8_t &ucMinute,
  uint8_t &ucSecond, uint32_t &ulMillisecond)
{
  uint64_t  ullM = ullTimeMs / 1000;
  struct tm *ptmTmp = localtime((const time_t*)&ullM);

  usYear = ptmTmp->tm_year + 1900;
  ucMonth = ptmTmp->tm_mon + 1;
  ucDay = ptmTmp->tm_mday;
  ucHour = ptmTmp->tm_hour;
  ucMinute = ptmTmp->tm_min;
  ucSecond = ptmTmp->tm_sec;
  ulMillisecond = ullTimeMs % 1000;
}

//------------------------------------------------------------------------------
void CUnitConvert::TimeToUINT64(uint16_t usYear, uint8_t ucMonth, uint8_t ucDay,
  uint8_t ucHour, uint8_t ucMinute, uint8_t ucSecond, uint32_t ulMillisecond,
  uint64_t &ullTimeMs)
{
  struct tm tmTmp;

  tmTmp.tm_year = usYear - 1900;
  tmTmp.tm_mon = ucMonth - 1;
  tmTmp.tm_mday = ucDay;
  tmTmp.tm_hour = ucHour;
  tmTmp.tm_min = ucMinute;
  tmTmp.tm_sec = ucSecond;

  ullTimeMs = mktime(&tmTmp);
  ullTimeMs *= 1000;  // 转换为毫秒
  ullTimeMs += ulMillisecond;
}
