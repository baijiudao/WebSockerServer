//------------------------------------------------------------------------------
// �������ڣ�2017-07-10 16:42:05
// �ļ��������ù�������Ҫʹ�õ�ת������
//------------------------------------------------------------------------------
#ifndef _UNIT_CONVERT_H_
#define _UNIT_CONVERT_H_

#include <string>
#include <memory>
#include <tuple>
#include "Singleton.h"

//#include "Define.h"

using namespace std;

typedef std::tuple < uint16_t, uint8_t, uint8_t,  // �� �� ��
  uint8_t, uint8_t, uint8_t,  // ʱ �� ��
  uint32_t > TupleTime; // ����

// ����TupleTime������ʱ���ÿ���ֶ�
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

// ����ģʽ
class CUnitConvert : public ISingleton<CUnitConvert>
{
protected:
  string  WCharToMultiByte(const wstring &wstrValue, unsigned int nCodePage);
  wstring MultiByteToWChar(const string &strValue, unsigned int nCodePage);
public:
  CUnitConvert();
  ~CUnitConvert();
  // �ַ�����ת��
  string  WStringToString(const wstring &wstrValue);
  wstring StringToWString(const string &strValue);
  string  AnsiToUTF8(const string &strValue);
  string  UTF8ToAnsi(const string &strValue);
  std::string   UnicodeToAnsi(std::wstring wstrValue);
  std::wstring  AnsiToUnicode(std::string strValue);
  std::string   UnicodeToUTF8(std::wstring wstrValue);
  std::wstring  UTF8ToUnicode(std::string strValue);
  // ������ʱ���DCOM��dateʱ���໥ת��
  double    UINT64ToVariantTime(uint64_t ullTimeMs);
  uint64_t  VariantTimeToUINT64(double vTime);
  // ����ʱ����ļ�ʱ���໥ת��
  void  UINT64ToFileTime(uint64_t ullTimeMs, FILETIME &ft);
  void  FileTimeToUINT64(const FILETIME &ft, uint64_t &ullTimeMs);
  // ������ʱ���tuple�����໥ת��
  TupleTime UINT64ToTupleTime(uint64_t ullTimeMs);
  uint64_t  TupleTimeToUINT64(TupleTime tupTime);
  // ������ʱ��ͷֶ�ʽʱ�以��ת��
  void  UINT64ToTime(uint64_t ullTimeMs, uint16_t &usYear, uint8_t &ucMonth,
    uint8_t &ucDay, uint8_t &ucHour, uint8_t &ucMinute, uint8_t &ucSecond,
    uint32_t &ulMillisecond);
  void  TimeToUINT64(uint16_t usYear, uint8_t ucMonth, uint8_t ucDay,
    uint8_t ucHour, uint8_t ucMinute, uint8_t ucSecond, uint32_t ulMillisecond,
    uint64_t &ullTimeMs);
};

#endif // !_UNIT_CONVERT_H_

