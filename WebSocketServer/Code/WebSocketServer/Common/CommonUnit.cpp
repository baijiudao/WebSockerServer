#include "stdafx.h"
#include "CommonUnit.h"

//------------------------------------------------------------------------------
ETagType CCommonUnit::GetTagType(const boost::any &anyValue)
{
  if (anyValue.type() == typeid(bool)) return TYPE_BOOL;
  if (anyValue.type() == typeid(uint32_t)) return TYPE_INTEGER;
  if (anyValue.type() == typeid(float)) return TYPE_FLOAT;
  if (anyValue.type() == typeid(string)) return TYPE_STRING;
  return TYPE_LAST;
}
