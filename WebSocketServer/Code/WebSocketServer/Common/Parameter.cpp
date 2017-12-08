#include "stdafx.h"
#include <assert.h>
#include "Parameter.h"


// -- CParameter --

//------------------------------------------------------------------------------
CParameter::CParameter(shared_ptr<CSessionManager> pSManager,
  shared_ptr<CSocketRelativeData> pSocketData,
  shared_ptr<CPushDataCache>  pPushDataCache,
  shared_ptr<CDataCache> pDataCache,
  shared_ptr<COptimize> pOptimize,
  shared_ptr<CDatabaseManager>  pDBManager,
  shared_ptr<KBasicInfo>  ptBasicInfo,
  shared_ptr<KConf> ptConf)
{
  assert(pSManager);
  assert(pSocketData);
  assert(pPushDataCache);
  assert(pDataCache);
  assert(pOptimize);
  assert(pDBManager);
  assert(ptBasicInfo);
  assert(ptConf);

  m_pSManager = pSManager;
  m_pSocketData = pSocketData;
  m_pPushDataCache = pPushDataCache;
  m_pDataCache = pDataCache;
  m_pOptimize = pOptimize;
  m_pDBManager = pDBManager;
  m_ptBasicInfo = ptBasicInfo;
  m_ptConf = ptConf;
}

//------------------------------------------------------------------------------
CParameter::~CParameter()
{
}

//------------------------------------------------------------------------------
shared_ptr<CSessionManager> CParameter::GetSManager()
{
  return m_pSManager;
}

//------------------------------------------------------------------------------
shared_ptr<CSocketRelativeData> CParameter::GetSocketData()
{
  return m_pSocketData;
}

//------------------------------------------------------------------------------
shared_ptr<CPushDataCache> CParameter::GetPushDataCache()
{
  return m_pPushDataCache;
}

//------------------------------------------------------------------------------
shared_ptr<CDataCache> CParameter::GetDataCache()
{
  return m_pDataCache;
}

//------------------------------------------------------------------------------
shared_ptr<COptimize> CParameter::GetOptimize()
{
  return m_pOptimize;
}

//------------------------------------------------------------------------------
shared_ptr<CDatabaseManager> CParameter::GetDBManager()
{
  return m_pDBManager;
}

//------------------------------------------------------------------------------
shared_ptr<KBasicInfo> CParameter::GetBasicInfo()
{
  return m_ptBasicInfo;
}

std::shared_ptr<KConf> CParameter::GetConfig()
{
  return m_ptConf;
}

