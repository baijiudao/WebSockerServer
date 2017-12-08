#ifndef _PARAMETER_H_
#define _PARAMETER_H_

#include <memory>
#include "PushDataCache.h"
#include "SessionManager.h"
#include "Optimize.h"
#include "DatabaseManager/DatabaseManager.h"
#include "BasicInformation.h"
#include "Config/Config.h"

using namespace std;

// 用来存放参数
class CParameter
{
private:
  shared_ptr<CSessionManager>     m_pSManager;
  shared_ptr<CSocketRelativeData> m_pSocketData;
  shared_ptr<CPushDataCache>      m_pPushDataCache;
  shared_ptr<CDataCache>          m_pDataCache;
  shared_ptr<COptimize>           m_pOptimize;
  shared_ptr<CDatabaseManager>    m_pDBManager;
  shared_ptr<KBasicInfo>          m_ptBasicInfo;
  shared_ptr<KConf>               m_ptConf;
public:
  CParameter(shared_ptr<CSessionManager> pSManager,
    shared_ptr<CSocketRelativeData> pSocketData,
    shared_ptr<CPushDataCache>  pPushDataCache,
    shared_ptr<CDataCache> pDataCache,
    shared_ptr<COptimize> pOptimize,
    shared_ptr<CDatabaseManager>  pDBManager,
    shared_ptr<KBasicInfo>  ptBasicInfo,
    shared_ptr<KConf> ptConf);
  ~CParameter();
  shared_ptr<CSessionManager>     GetSManager();
  shared_ptr<CSocketRelativeData> GetSocketData();
  shared_ptr<CPushDataCache>      GetPushDataCache();
  shared_ptr<CDataCache>          GetDataCache();
  shared_ptr<COptimize>           GetOptimize();
  shared_ptr<CDatabaseManager>    GetDBManager();
  shared_ptr<KBasicInfo>          GetBasicInfo();
  shared_ptr<KConf>               GetConfig();
};

#endif
