#include "stdafx.h"
#include "TableBase.h"

//------------------------------------------------------------------------------
CTableBase::CTableBase()
{
}

//------------------------------------------------------------------------------
CTableBase::~CTableBase()
{
}

//------------------------------------------------------------------------------
// 创建表
//------------------------------------------------------------------------------
bool CTableBase::OnInitialise(const otl_connect &dbconnect, const std::string &strSQL)
{
  otl_connect &db = const_cast<otl_connect &>(dbconnect);

  try
  {
    // 创建表
    db.direct_exec(strSQL.c_str());
  }
  catch (otl_exception &e)
  {
    LOG_ERROR << e.code << "：" << e.msg << " " << e.stm_text << " " << e.var_info;
    return false;
  }
  return true;
}

