//------------------------------------------------------------------------------
// �������ڣ�2017-09-26 13:19:22
// �ļ���������������һЩͨ�ú���ģ��
//------------------------------------------------------------------------------
#ifndef _COMMON_UNIT_H_
#define _COMMON_UNIT_H_

#include "Define.h"

#include <mutex>
#include <memory>

#include <Singleton.h>
#include <boost/any.hpp>

class CCommonUnit : public ISingleton<CCommonUnit>
{
public:
  ETagType  GetTagType(const boost::any &anyValue);
};

#endif // !_COMMON_UNIT_H_

