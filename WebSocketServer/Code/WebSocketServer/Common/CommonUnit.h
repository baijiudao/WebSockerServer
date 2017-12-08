//------------------------------------------------------------------------------
// 创建日期：2017-09-26 13:19:22
// 文件描述：用来定义一些通用函数模块
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

