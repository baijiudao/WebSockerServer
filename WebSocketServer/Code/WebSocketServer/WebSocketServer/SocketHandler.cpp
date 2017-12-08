//------------------------------------------------------------------------------
// 创建日期：2017-06-19 11:04:39
// 文件描述：socket的recvive处理部分，因为太多，所以分开来处理，而且
//          这样对WebSocket的编写者冲突最小
//------------------------------------------------------------------------------
#include "stdafx.h"

#include "NetPackage/NetPackage.h"
#include "SessionManager.h"

#include "UnitConvert.h"
#include "RDBSInterface.h"
#include "Parameter.h"
#include "WebSocketHandler.h"

using namespace std;

recursive_mutex g_mutexLogon; // 加锁，用来保证登录互斥

//------------------------------------------------------------------------------
// 1、删除Session
// 2、删除与Session关联的Socket
//------------------------------------------------------------------------------
void Destroy(shared_ptr<CSessionManager> pSManager, shared_ptr<CSessionData> pSession,
  shared_ptr<CSocketRelativeData> pSocketData)
{
  if (!pSManager || !pSession || !pSocketData) return;

  // 删除Session
  pSManager->DelSession(pSession->GetSID());

  // 删除与Socket相关的数据
  for (auto iter = pSession->m_mapSocket.begin(); iter != pSession->m_mapSocket.end(); iter++)
    pSocketData->Delete(iter->first);
}

//------------------------------------------------------------------------------
// 其中strSession和nSeq是在处理失败的时候外部执行默认处理时需要的
//------------------------------------------------------------------------------
EErrCode MsgLogin(connection_hdl hdl, shared_ptr<CParameter> pParameter,
  const string &strSrc, string &strDst, string &strSessionID)
{
  if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
  shared_ptr<CNetPackage>  pPackage = CNetPackage::Inst();
  shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
  shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();

  shared_ptr<CSessionData>    pSession = nullptr;
  RDBSInterface_ptr           pInterface = nullptr;
  CComObject<CRDBSInterface>* pInterfaceTmp = nullptr;
  std::string strAccount = "";
  std::string strPassword = "";
  EUserType eUserType = EUserType::LAST;
  EErrCode eErr = EErrCode::EC_UNKNOW;

  // 解析数据包
  if (!pPackage->ParseLogin(strSrc, strAccount, strPassword))
    return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // 判断该Socket是否已经登录过，没有则创建Session
  lock_guard<recursive_mutex> alock(g_mutexLogon);
  string  strSIDTmp = "";
  if (pSocketData->GetSID(hdl, strSIDTmp)) return EErrCode::EC_LOGGED_IN;

  // 生成Session
  pSession = pSManager->CreSession();
  if (!pSession) return EErrCode::EC_CREATE_SESSION_FAILD;

//   // 对Session操作加锁
//   lock_guard<recursive_mutex> galock(pSession->m_mutexRc);
// 
//   // 创建接口对象将对象指针交给智能指针
//   CComObject<CRDBSInterface>::CreateInstance(&pInterfaceTmp);
//   pInterface.reset(pInterfaceTmp);
// 
//   // 给Session添加信息
//   pSession->m_pInterface = pInterface;
//   pSession->m_mapSocket.insert(make_pair(hdl, 0));
// 
//   // 初始化接口
//   eErr = pInterface->Init(pParameter->GetBasicInfo()->strHostName,
//     pParameter->GetDataCache(), pParameter->GetOptimize());
//   if (EErrCode::EC_SUCCESS != eErr) return eErr;
// 
//   // 登录操作
//   eErr = pInterface->Login(strAccount, strPassword, eUserType);
//   if (EErrCode::EC_SUCCESS != eErr) return eErr;

  // 组建数据包
  if (!pPackage->BuildLogin(pSession->GetSID(), EErrCode::EC_SUCCESS, eUserType, strDst))
    return EErrCode::EC_PACKAGE_BUILD_FAILD;

  // 将Session添加到CSocketRelativeData中
  pSocketData->SetSID(hdl, pSession->GetSID());

  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode MsgLogout(connection_hdl hdl, shared_ptr<CParameter> pParameter,
  const string &strSrc, string &strDst, string &strSessionID)
{
  if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
  shared_ptr<CNetPackage>  pPackage = CNetPackage::Inst();
  shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
  shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();

  shared_ptr<CSessionData>    pSession = nullptr;
  RDBSInterface_ptr           pInterface = nullptr;
  EErrCode eErr = EErrCode::EC_UNKNOW;

  // 解析数据包
  if (!pPackage->ParseLogout(strSrc, strSessionID)) return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // 获取Session相关数据
  pSession = pSManager->GetSession(strSessionID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;
  
  // 对Session操作加锁
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);
  pInterface = pSession->m_pInterface;
  if (!pInterface)  return EErrCode::EC_INTERFACE_OBJ_UNINVALID;

  // 退出登录
  eErr = pInterface->Logout();
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  // 销毁Session及其相关信息
  Destroy(pSManager, pSession, pSocketData);

  // 组建数据包
  if (!pPackage->BuildLogout(EErrCode::EC_SUCCESS, strDst))
    return EErrCode::EC_PACKAGE_BUILD_FAILD;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode MsgReadTag(connection_hdl hdl, shared_ptr<CParameter> pParameter,
  const string &strSrc, string &strDst, string &strSessionID)
{
  if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
  shared_ptr<CNetPackage>  pPackage = CNetPackage::Inst();
  shared_ptr<CUnitConvert> pConv = CUnitConvert::Inst();
  shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
  shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();

  shared_ptr<CSessionData>    pSession = nullptr;
  RDBSInterface_ptr           pInterface = nullptr;
  list<shared_ptr<KTagInfo>>  lstTag;
  EErrCode eErr = EErrCode::EC_UNKNOW;

  // 解析数据包
  if (!pPackage->ParseReadTag(strSrc, strSessionID, lstTag)) return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // 获取Session相关数据
  pSession = pSManager->GetSession(strSessionID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // 对session操作加锁
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);
  pInterface = pSession->m_pInterface;
  if (!pInterface) return EErrCode::EC_INTERFACE_OBJ_UNINVALID;

  // 单点登录实现位置，添加这个Socket到数据中
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  // 获取变量数据
  eErr = pInterface->ReadTag(lstTag);
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  //  将变量ID放入SocketData中
  for (auto iter = lstTag.begin(); iter != lstTag.end(); iter++)
    if (!pSocketData->SetGID(hdl, (*iter)->ulGID)) return EErrCode::EC_GID_SET_DATA_FAILD;

  // 组建数据包
  if (!pPackage->BuildReadTag(strSessionID, EErrCode::EC_SUCCESS, lstTag, strDst))
    return EErrCode::EC_PACKAGE_BUILD_FAILD;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode MsgWriteTag(connection_hdl hdl, shared_ptr<CParameter> pParameter,
  const string &strSrc, string &strDst, string &strSessionID)
{
  if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
  shared_ptr<CNetPackage>  pPackage = CNetPackage::Inst();
  shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
  shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();

  shared_ptr<CSessionData>    pSession = nullptr;
  RDBSInterface_ptr           pInterface = nullptr;
  list<shared_ptr<KTagInfo>>  lstTag;
  EErrCode eErr = EErrCode::EC_UNKNOW;

  // 解析数据包
  if (!pPackage->ParseWriteTag(strSrc, strSessionID, lstTag)) return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // 获取Session相关数据
  pSession = pSManager->GetSession(strSessionID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // 对session操作加锁
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);
  pInterface = pSession->m_pInterface;
  if (!pInterface) return EErrCode::EC_INTERFACE_OBJ_UNINVALID;

  // 单点登录实现位置，添加这个Socket到数据中
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  // 获取变量数据
  eErr = pInterface->WriteTag(lstTag);
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  // 组建数据包
  if (!pPackage->BuildWriteTag(strSessionID, EErrCode::EC_SUCCESS, lstTag, strDst))
    return EErrCode::EC_PACKAGE_BUILD_FAILD;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode MsgWriteEvent(connection_hdl hdl, shared_ptr<CParameter> pParameter,
  const string &strSrc, string &strDst, string &strSessionID)
{
  if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
  shared_ptr<CNetPackage>  pPackage = CNetPackage::Inst();
  shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
  shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();

  shared_ptr<CSessionData>    pSession = nullptr;
  RDBSInterface_ptr           pInterface = nullptr;
  list<shared_ptr<KEventInfo>>  lstInfo;
  EErrCode eErr = EErrCode::EC_UNKNOW;

  // 解析数据包
  if (!pPackage->ParseWriteEvent(strSrc, strSessionID, lstInfo)) return EErrCode::EC_PACKAGE_PARSE_FAILD;
  // 获取Session相关数据
  pSession = pSManager->GetSession(strSessionID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // 对session操作加锁
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);
  pInterface = pSession->m_pInterface;
  if (!pInterface) return EErrCode::EC_INTERFACE_OBJ_UNINVALID;

  // 单点登录实现位置，添加这个Socket到数据中
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  // 获取变量数据
  eErr = pInterface->WriteEvent(lstInfo);
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  // 组建数据包
  if (!pPackage->BuildWriteEvent(strSessionID, EErrCode::EC_SUCCESS, strDst))
    return EErrCode::EC_PACKAGE_BUILD_FAILD;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode MsgReadHistoryEvent(connection_hdl hdl, shared_ptr<CParameter> pParameter,
  const string &strSrc, string &strDst, string &strSessionID)
{
  if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
  shared_ptr<CNetPackage>  pPackage = CNetPackage::Inst();
  shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
  shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();

  shared_ptr<CSessionData>    pSession = nullptr;
  RDBSInterface_ptr           pInterface = nullptr;
  list<shared_ptr<KEventInfo>>  lstEvent;
  EErrCode eErr = EErrCode::EC_UNKNOW;

  uint64_t  ullStartTime = 0;
  uint64_t  ullEndTime = 0;

  // 解析数据包
  if (!pPackage->ParseReadHistoryEvent(strSrc, strSessionID, ullStartTime, ullEndTime))
    return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // 获取Session相关数据
  pSession = pSManager->GetSession(strSessionID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // 对session操作加锁
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);
  pInterface = pSession->m_pInterface;
  if (!pInterface) return EErrCode::EC_INTERFACE_OBJ_UNINVALID;

  // 单点登录实现位置，添加这个Socket到数据中
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  eErr = pInterface->ReadHistroyEvent(ullStartTime, ullEndTime, lstEvent);
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  // 组建数据包
  if (!pPackage->BuildReadHistoryEvent(strSessionID, EErrCode::EC_SUCCESS, lstEvent, strDst))
    return EErrCode::EC_PACKAGE_BUILD_FAILD;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode MsgReadHistoryAlarm(connection_hdl hdl, shared_ptr<CParameter> pParameter,
  const string &strSrc, string &strDst, string &strSessionID)
{
  if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
  shared_ptr<CNetPackage>  pPackage = CNetPackage::Inst();
  shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
  shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();

  shared_ptr<CSessionData>    pSession = nullptr;
  RDBSInterface_ptr           pInterface = nullptr;
  list<shared_ptr<KAlarmInfo>>  lstAlarm;
  EErrCode eErr = EErrCode::EC_UNKNOW;

  uint64_t  ullStartTime = 0;
  uint64_t  ullEndTime = 0;

  // 解析数据包
  if (!pPackage->ParseReadHistoryAlarm(strSrc, strSessionID, ullStartTime, ullEndTime))
    return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // 获取Session相关数据
  pSession = pSManager->GetSession(strSessionID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // 对session操作加锁
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);
  pInterface = pSession->m_pInterface;
  if (!pInterface) return EErrCode::EC_INTERFACE_OBJ_UNINVALID;

  // 单点登录实现位置，添加这个Socket到数据中
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  eErr = pInterface->ReadHistroyAlarm(ullStartTime, ullEndTime, lstAlarm);
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  // 组建数据包
  if (!pPackage->BuildReadHistoryAlarm(strSessionID, EErrCode::EC_SUCCESS, lstAlarm, strDst))
    return EErrCode::EC_PACKAGE_BUILD_FAILD;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgAddAlarmAssistant(connection_hdl hdl, shared_ptr<CParameter> pParameter,
  const string &strRecv, string &strSend, string &strSID)
{
  if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
  shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
  shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
  shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
  shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

  shared_ptr<CSessionData>    pSession = nullptr;
  EErrCode eErr = EErrCode::EC_UNKNOW;

  std::list<std::shared_ptr<KTBAlarmAssistant>> lstInfo;
  std::list<uint32_t> lstID;

  // 解析数据包
  if (!pPackage->ParseAdd_AlarmAssistant(strRecv, strSID, lstInfo))
    return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // 获取Session相关数据
  pSession = pSManager->GetSession(strSID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // 对session操作加锁
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

  // 单点登录实现位置，添加这个Socket到数据中
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  // 数据库操作
  if (!pDBManager->AddAlarmAssistant(lstInfo, lstID)) return EErrCode::EC_DATABASE_FAIL;

  // 组建数据包
  if (!pPackage->BuildAdd_AlarmAssistant(strSID, EErrCode::EC_SUCCESS, lstID, strSend))
    return EErrCode::EC_PACKAGE_BUILD_FAILD;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgDeleteAlarmAssistant(connection_hdl hdl, shared_ptr<CParameter> pParameter,
  const string &strRecv, string &strSend, string &strSID)
{
  if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
  shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
  shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
  shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
  shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

  shared_ptr<CSessionData>    pSession = nullptr;
  EErrCode eErr = EErrCode::EC_UNKNOW;
  std::list<uint32_t> lstID;

  // 解析数据包
  if (!pPackage->ParseDelete_AlarmAssistant(strRecv, strSID, lstID))
    return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // 获取Session相关数据
  pSession = pSManager->GetSession(strSID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // 对session操作加锁
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

  // 单点登录实现位置，添加这个Socket到数据中
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  // 数据库操作
  if (!pDBManager->DeleteAlarmAssistant(lstID)) return EErrCode::EC_DATABASE_FAIL;

  // 组建数据包
  if (!pPackage->BuildDelete_AlarmAssistant(strSID, EErrCode::EC_SUCCESS, lstID, strSend))
    return EErrCode::EC_PACKAGE_BUILD_FAILD;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgModifyAlarmAssistant(connection_hdl hdl, shared_ptr<CParameter> pParameter,
  const string &strRecv, string &strSend, string &strSID)
{
  if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
  shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
  shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
  shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
  shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

  shared_ptr<CSessionData>    pSession = nullptr;
  EErrCode eErr = EErrCode::EC_UNKNOW;

  std::list<std::shared_ptr<KTBAlarmAssistant>> lstInfo;
  std::list<uint32_t> lstID;

  // 解析数据包
  if (!pPackage->ParseModify_AlarmAssistant(strRecv, strSID, lstInfo))
    return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // 获取Session相关数据
  pSession = pSManager->GetSession(strSID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // 对session操作加锁
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

  // 单点登录实现位置，添加这个Socket到数据中
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  // 数据库操作
  if (!pDBManager->ModifyAlarmAssistant(lstInfo)) return EErrCode::EC_DATABASE_FAIL;

  // 获取所有修改成功的ID
  for (auto &itemInfo : lstInfo)
    lstID.push_back(itemInfo->lID.first);

  // 组建数据包
  if (!pPackage->BuildModify_AlarmAssistant(strSID, EErrCode::EC_SUCCESS, lstID, strSend))
    return EErrCode::EC_PACKAGE_BUILD_FAILD;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgSearchAlarmAssistant(connection_hdl hdl,
  shared_ptr<CParameter> pParameter, const string &strRecv,
  string &strSend, string &strSID)
{
  if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
  shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
  shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
  shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
  shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

  shared_ptr<CSessionData>    pSession = nullptr;
  EErrCode eErr = EErrCode::EC_UNKNOW;

  uint32_t  ulGlobalID = 0;
  list<shared_ptr<KTBAlarmAssistant>> lstInfo;

  // 解析数据包
  if (!pPackage->ParseSearch_AlarmAssistant(strRecv, strSID, ulGlobalID))
    return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // 获取Session相关数据
  pSession = pSManager->GetSession(strSID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // 对session操作加锁
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

  // 单点登录实现位置，添加这个Socket到数据中
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  // 数据库操作
  if (!pDBManager->SearchAlarmAssistant(ulGlobalID, lstInfo)) return EErrCode::EC_DATABASE_FAIL;

  // 组建数据包
  if (!pPackage->BuildSearch_AlarmAssistant(strSID, EErrCode::EC_SUCCESS, lstInfo, strSend))
    return EErrCode::EC_PACKAGE_BUILD_FAILD;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgSearchAlarmAssistant_Num(connection_hdl hdl,
  shared_ptr<CParameter> pParameter, const string &strRecv,
  string &strSend, string &strSID)
{
  if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
  shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
  shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
  shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
  shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

  shared_ptr<CSessionData>    pSession = nullptr;
  EErrCode eErr = EErrCode::EC_UNKNOW;

  shared_ptr<KTBAlarmAssistantSearchCondition_Num>  ptConfition = make_shared<KTBAlarmAssistantSearchCondition_Num>();
  shared_ptr<KTBAlarmAssistantSearchResult_Num> ptResult = make_shared<KTBAlarmAssistantSearchResult_Num>();

  // 解析数据包
  if (!pPackage->ParseSearch_Num_AlarmAssistant(strRecv, strSID, ptConfition))
    return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // 获取Session相关数据
  pSession = pSManager->GetSession(strSID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // 对session操作加锁
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

  // 单点登录实现位置，添加这个Socket到数据中
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  // 数据库操作
  if (!pDBManager->SearchAlarmAssistant_Num(ptConfition, ptResult)) return EErrCode::EC_DATABASE_FAIL;

  // 组建数据包
  if (!pPackage->BuildSearch_Num_AlarmAssistant(strSID, EErrCode::EC_SUCCESS, ptResult, strSend))
    return EErrCode::EC_PACKAGE_BUILD_FAILD;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgSearchPagingAlarmAssistant(connection_hdl hdl,
  shared_ptr<CParameter> pParameter, const string &strRecv,
  string &strSend, string &strSID)
{
  if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
  shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
  shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
  shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
  shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

  shared_ptr<CSessionData>    pSession = nullptr;
  EErrCode eErr = EErrCode::EC_UNKNOW;

  shared_ptr<KTBAlarmAssistantSearchCondition>  ptConfition = make_shared<KTBAlarmAssistantSearchCondition>();
  shared_ptr<KTBAlarmAssistantSearchResult> ptResult = make_shared<KTBAlarmAssistantSearchResult>();

  // 解析数据包
  if (!pPackage->ParseSearchPaging_AlarmAssistant(strRecv, strSID, ptConfition))
    return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // 获取Session相关数据
  pSession = pSManager->GetSession(strSID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // 对session操作加锁
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

  // 单点登录实现位置，添加这个Socket到数据中
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  // 数据库操作
  if (!pDBManager->SearchPagingAlarmAssistant(ptConfition, ptResult)) return EErrCode::EC_DATABASE_FAIL;

  // 组建数据包
  if (!pPackage->BuildSearchPaging_AlarmAssistant(strSID, EErrCode::EC_SUCCESS, ptResult, strSend))
    return EErrCode::EC_PACKAGE_BUILD_FAILD;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgAddChangeShifts(connection_hdl hdl, shared_ptr<CParameter> pParameter,
  const string &strRecv, string &strSend, string &strSID)
{
  if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
  shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
  shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
  shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
  shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

  shared_ptr<CSessionData>    pSession = nullptr;
  EErrCode eErr = EErrCode::EC_UNKNOW;

  list<shared_ptr<KTBChgShifts>>  lstInfo;
  list<uint32_t>  lstID;

  // 解析数据包
  if (!pPackage->ParseAdd_ChangeShifts(strRecv, strSID, lstInfo))
    return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // 获取Session相关数据
  pSession = pSManager->GetSession(strSID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // 对session操作加锁
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

  // 单点登录实现位置，添加这个Socket到数据中
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  // 数据库操作
  if (!pDBManager->AddChangeShifts(lstInfo, lstID)) return EErrCode::EC_DATABASE_FAIL;

  // 组建数据包
  if (!pPackage->BuildAdd_ChangeShifts(strSID, EErrCode::EC_SUCCESS, lstID, strSend))
    return EErrCode::EC_PACKAGE_BUILD_FAILD;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgCheckDBStatus(connection_hdl hdl, shared_ptr<CParameter> pParameter,
  const string &strRecv, string &strSend, string &strSID)
{
  if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
  shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
  shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
  shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
  shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

  shared_ptr<CSessionData>    pSession = nullptr;
  EErrCode eErr = EErrCode::EC_UNKNOW;

  RDBSInterface_ptr   pInterface = nullptr;
  list<shared_ptr<KDBStatusInfo>>  lstInfo;

  // 解析数据包
  if (!pPackage->ParseCheckDBStatus(strRecv, strSID))
    return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // 获取Session相关数据
  pSession = pSManager->GetSession(strSID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // 对session操作加锁
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);
  pInterface = pSession->m_pInterface;

  // 单点登录实现位置，添加这个Socket到数据中
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  // 获取数据库连通状态
  if (!pDBManager->CheckDBStatus(lstInfo)) return EErrCode::EC_DATABASE_FAIL;
  if (!pInterface->CheckDBStatus(lstInfo)) return EErrCode::EC_DATABASE_FAIL;

  // 组建数据包
  if (!pPackage->BuildCheckDBStatus(strSID, EErrCode::EC_SUCCESS, lstInfo, strSend))
    return EErrCode::EC_PACKAGE_BUILD_FAILD;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgCheckOPCStatus(connection_hdl hdl, shared_ptr<CParameter> pParameter,
  const string &strRecv, string &strSend, string &strSID)
{
  if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
  shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
  shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
  shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
  shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();
  // 暂时用来获取opc状态
  shared_ptr<CTagDataMap> pTagDataMap = pParameter->GetDataCache()->GetTagMap();

  shared_ptr<CSessionData>    pSession = nullptr;
  EErrCode eErr = EErrCode::EC_UNKNOW;

  list<shared_ptr<KOPCStatusInfo>>  lstInfo;

  // 解析数据包
  if (!pPackage->ParseCheckOPCStatus(strRecv, strSID))
    return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // 获取Session相关数据
  pSession = pSManager->GetSession(strSID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // 对session操作加锁
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

  // 单点登录实现位置，添加这个Socket到数据中
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  // 获取OPC连通状态
  if (!pTagDataMap->CheckOPCStatus(lstInfo)) return EErrCode::EC_CHECK_OPC_STATUS_FAILD;

  // 组建数据包
  if (!pPackage->BuildCheckOPCStatus(strSID, EErrCode::EC_SUCCESS, lstInfo, strSend))
    return EErrCode::EC_PACKAGE_BUILD_FAILD;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgSearchSupplierInfForTagID(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    shared_ptr<SupplierInfoAndID>  ptResult = make_shared<SupplierInfoAndID>();
    uint32_t lstInfo;

    // 解析数据包
    if (!pPackage->ParseSearchID(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->SearchSupplierForTagID(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildSearchSupplierForTagID(strSID, EErrCode::EC_SUCCESS, ptResult, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgSearchDeviceInfForTagID(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    shared_ptr<DeviceInfoAndID>  ptResult = make_shared<DeviceInfoAndID>();
    uint32_t lstInfo;
    // 解析数据包
    if (!pPackage->ParseSearchID(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->SearchDeviceInfForTagID(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildSearchDeviceInfForTagID(strSID, EErrCode::EC_SUCCESS, ptResult, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgSearchRepairInfForTagID(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    shared_ptr<RepairInfoAndID>  ptResult = make_shared<RepairInfoAndID>();
    uint32_t lstInfo;

    // 解析数据包
    if (!pPackage->ParseSearchID(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->SearchRepairInfForTagID(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildSearchRepairInfForTagID(strSID, EErrCode::EC_SUCCESS, ptResult, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgSearchRunInfForTagID(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    shared_ptr<DeviceRunInfoAndID>  ptResult = make_shared<DeviceRunInfoAndID>();
    uint32_t lstInfo;

    // 解析数据包
    if (!pPackage->ParseSearchID(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->SearchRunInfForTagID(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildSearchRunInfForTagID(strSID, EErrCode::EC_SUCCESS, ptResult, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgAddSupplier(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    shared_ptr<SupplierInfo>  ptResult = make_shared<SupplierInfo>();

    // 解析数据包
    if (!pPackage->ParseAddOrModSupplier(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->AddSupplier(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildOnlyHead(strSID, EErrCode::EC_SUCCESS, ECommand::CMD_ADD_SUPPLIER, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgDeleteSupplier(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    string  ptResult = "";

    // 解析数据包
    if (!pPackage->ParseDelOrSelSupplier(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->DeleteSupplier(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildOnlyHead(strSID, EErrCode::EC_SUCCESS, ECommand::CMD_DELETE_SUPPLIER, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgModifySupplier(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    shared_ptr<SupplierInfo>  ptResult = make_shared<SupplierInfo>();

    // 解析数据包
    if (!pPackage->ParseAddOrModSupplier(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->ModifySuplier(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildOnlyHead(strSID, EErrCode::EC_SUCCESS, ECommand::CMD_MODIFY_SUPPLIER, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgSearchSupplier(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    list<shared_ptr<SupplierInfo>>  ptResult;
    string lstInfo;

    // 解析数据包
    if (!pPackage->ParseDelOrSelSupplier(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->SearchSuplier(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildSearchSupplier(strSID, EErrCode::EC_SUCCESS, ptResult, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgAddMeterType(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    shared_ptr<MeterType>  ptResult = make_shared<MeterType>();

    // 解析数据包
    if (!pPackage->ParseAddOrModMeterType(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->AddMeterType(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildOnlyHead(strSID, EErrCode::EC_SUCCESS, ECommand::CMD_ADD_METERTYPE, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgDeleteMeterType(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    string  ptResult = "";

    // 解析数据包
    if (!pPackage->ParseDelOrSelMeterType(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->DeleteMeterType(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildOnlyHead(strSID, EErrCode::EC_SUCCESS, ECommand::CMD_DELETE_METERTYPE, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgModifyMeterType(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    shared_ptr<MeterType>  ptResult = make_shared<MeterType>();

    // 解析数据包
    if (!pPackage->ParseAddOrModMeterType(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->ModifyMeterType(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildOnlyHead(strSID, EErrCode::EC_SUCCESS, ECommand::CMD_MODIFY_METERTYPE, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgSearchMeterType(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    list<shared_ptr<MeterType>>  ptResult;
    string lstInfo;

    // 解析数据包
    if (!pPackage->ParseDelOrSelMeterType(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->SearchMeterType(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildSearchMeterType(strSID, EErrCode::EC_SUCCESS, ptResult, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgAddEnergyCate(connection_hdl hdl, shared_ptr<CParameter> pParameter,
const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    shared_ptr<EnergyCate>  ptResult = make_shared<EnergyCate>();

    // 解析数据包
    if (!pPackage->ParseAddOrModEnergyCate(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->AddEnergyCate(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildOnlyHead(strSID, EErrCode::EC_SUCCESS, ECommand::CMD_ADD_ENERGYCATE, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgDeleteEnergyCate(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    string  ptResult = "";

    // 解析数据包
    if (!pPackage->ParseDelOrSelEnergyCate(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->DeleteEnergyCate(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildOnlyHead(strSID, EErrCode::EC_SUCCESS, ECommand::CMD_DELETE_ENERGYCATE, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgModifyEnergyCate(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    shared_ptr<EnergyCate>  ptResult = make_shared<EnergyCate>();

    // 解析数据包
    if (!pPackage->ParseAddOrModEnergyCate(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->ModifyEnergyCate(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildOnlyHead(strSID, EErrCode::EC_SUCCESS, ECommand::CMD_MODIFY_ENERGYCATE, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgSearchEnergyCate(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    list<shared_ptr<EnergyCate>>  ptResult;
    string lstInfo;

    // 解析数据包
    if (!pPackage->ParseDelOrSelEnergyCate(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->SearchEnergyCate(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildSearchEnergyCate(strSID, EErrCode::EC_SUCCESS, ptResult, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgAddDevFaultCate(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    shared_ptr<DevFaultCate>  ptResult = make_shared<DevFaultCate>();

    // 解析数据包
    if (!pPackage->ParseAddOrModDevFaultCate(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->AddDevFaultCate(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildOnlyHead(strSID, EErrCode::EC_SUCCESS, ECommand::CMD_ADD_FAULTCATE, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgDeleteDevFaultCate(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    string  ptResult = "";

    // 解析数据包
    if (!pPackage->ParseDelOrSelDevFaultCate(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->DeleteDevFaultCate(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildOnlyHead(strSID, EErrCode::EC_SUCCESS, ECommand::CMD_DELETE_FAULTCATE, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgModifyDevFaultCate(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    shared_ptr<DevFaultCate>  ptResult = make_shared<DevFaultCate>();

    // 解析数据包
    if (!pPackage->ParseAddOrModDevFaultCate(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->ModifyDevFaultCate(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildOnlyHead(strSID, EErrCode::EC_SUCCESS, ECommand::CMD_MODIFY_FAULTCATE, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgSearchDevFaultCate(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    list<shared_ptr<DevFaultCate>>  ptResult;
    string lstInfo;

    // 解析数据包
    if (!pPackage->ParseDelOrSelDevFaultCate(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->SearchDevFaultCate(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildSearchDevFaultCate(strSID, EErrCode::EC_SUCCESS, ptResult, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgAddDeviceInfo(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    shared_ptr<DeviceInfo>  ptResult = make_shared<DeviceInfo>();

    // 解析数据包
    if (!pPackage->ParseAddOrModDeviceInfo(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->AddDeviceInfo(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildOnlyHead(strSID, EErrCode::EC_SUCCESS, ECommand::CMD_ADD_DEVICEINF, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgDeleteDeviceInfo(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    string  ptResult = "";

    // 解析数据包
    if (!pPackage->ParseDelOrSelDeviceInfo(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->DeleteDeviceInfo(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildOnlyHead(strSID, EErrCode::EC_SUCCESS, ECommand::CMD_DELETE_DEVICEINF, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgModifyDeviceInfo(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    shared_ptr<DeviceInfo>  ptResult = make_shared<DeviceInfo>();

    // 解析数据包
    if (!pPackage->ParseAddOrModDeviceInfo(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->ModifyDeviceInfo(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildOnlyHead(strSID, EErrCode::EC_SUCCESS, ECommand::CMD_MODIFY_DEVICEINF, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgSearchDeviceInfo(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    list<shared_ptr<DeviceInfo>>  ptResult;
    string lstInfo;

    // 解析数据包
    if (!pPackage->ParseDelOrSelDeviceInfo(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->SearchDeviceInfo(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildSearchDeviceInfo(strSID, EErrCode::EC_SUCCESS, ptResult, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgAddMeterInfo(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    shared_ptr<MeterInfo>  ptResult = make_shared<MeterInfo>();

    // 解析数据包
    if (!pPackage->ParseAddOrModMeterInfo(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->AddMeterInfo(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildOnlyHead(strSID, EErrCode::EC_SUCCESS, ECommand::CMD_ADD_METERINF, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgDeleteMeterInfo(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    string  ptResult = "";

    // 解析数据包
    if (!pPackage->ParseDelOrSelMeterInfo(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->DeleteMeterInfo(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildOnlyHead(strSID, EErrCode::EC_SUCCESS, ECommand::CMD_DELETE_METERINF, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgModifyMeterInfo(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    shared_ptr<MeterInfo>  ptResult = make_shared<MeterInfo>();

    // 解析数据包
    if (!pPackage->ParseAddOrModMeterInfo(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->ModifyMeterInfo(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildOnlyHead(strSID, EErrCode::EC_SUCCESS, ECommand::CMD_MODIFY_METERINF, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgSearchMeterInfo(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    list<shared_ptr<MeterInfo>>  ptResult;
    string lstInfo;

    // 解析数据包
    if (!pPackage->ParseDelOrSelMeterInfo(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->SearchMeterInfo(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildSearchMeterInfo(strSID, EErrCode::EC_SUCCESS, ptResult, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}
//------------------------------------------------------------------------------
EErrCode  MsgAddDeviceParameter(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    shared_ptr<DeviceParameter>  ptResult = make_shared<DeviceParameter>();

    // 解析数据包
    if (!pPackage->ParseAddDeviceParameter(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->AddDeviceParameter(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildOnlyHead(strSID, EErrCode::EC_SUCCESS, ECommand::CMD_ADD_DEVICEPARAMETER, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgDeleteDeviceParameter(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    shared_ptr<DeviceParameter>  ptResult = make_shared<DeviceParameter>();

    // 解析数据包
    if (!pPackage->ParseDelDeviceParameter(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->DeleteDeviceParameter(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildOnlyHead(strSID, EErrCode::EC_SUCCESS, ECommand::CMD_DELETE_DEVICEPARAMETER, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgSearchDeviceParameter(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    shared_ptr<DeviceParameter>  ptResult = make_shared<DeviceParameter>();
    string lstInfo;

    // 解析数据包
    if (!pPackage->ParseSearchDeviceParameter(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->SearchDeviceParameter(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildSearchDeviceParameter(strSID, EErrCode::EC_SUCCESS, ptResult, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgAddMeterParameter(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    shared_ptr<MeterParameter>  ptResult = make_shared<MeterParameter>();

    // 解析数据包
    if (!pPackage->ParseAddMeterParameter(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->AddMeterParameter(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildOnlyHead(strSID, EErrCode::EC_SUCCESS, ECommand::CMD_ADD_METERPARAMETER, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgDeleteMeterParameter(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    shared_ptr<MeterParameter>  ptResult = make_shared<MeterParameter>();

    // 解析数据包
    if (!pPackage->ParseDelMeterParameter(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->DeleteMeterParameter(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildOnlyHead(strSID, EErrCode::EC_SUCCESS, ECommand::CMD_DELETE_METERPARAMETER, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgSearchMeterParameter(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    shared_ptr<MeterParameter>  ptResult = make_shared<MeterParameter>();
    string lstInfo;

    // 解析数据包
    if (!pPackage->ParseSearchMeterParameter(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->SearchMeterParameter(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildSearchMeterParameter(strSID, EErrCode::EC_SUCCESS, ptResult, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgAddDeviceRepairInfo(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    shared_ptr<RepairInfo>  ptResult = make_shared<RepairInfo>();
    uint32_t ulRecordingID = 0;

    // 解析数据包
    if (!pPackage->ParseAddOrModDeviceRepair(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->AddDeviceRepair(ptResult, ulRecordingID);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildAddDeviceRecordInfo(strSID, EErrCode::EC_SUCCESS, ECommand::CMD_ADD_DEVICEREPAIRINF, ulRecordingID, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgDeleteDeviceRepairInfo(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    int  ptResult = 0;

    // 解析数据包
    if (!pPackage->ParseDelOrSelDeviceRepair(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->DeleteDeviceRepair(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildOnlyHead(strSID, EErrCode::EC_SUCCESS, ECommand::CMD_DELETE_DEVICEREPAIRINF, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgModifyDeviceRepairInfo(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    shared_ptr<RepairInfo>  ptResult = make_shared<RepairInfo>();

    // 解析数据包
    if (!pPackage->ParseAddOrModDeviceRepair(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->ModifyDeviceRepair(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildOnlyHead(strSID, EErrCode::EC_SUCCESS, ECommand::CMD_MODIFY_DEVICEREPAIRINF, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgSearchDeviceRepairInfo(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    //shared_ptr<RepairInfo>  ptResult;
    list<shared_ptr<RepairInfo>>  ptResult;
    int lstInfo;

    // 解析数据包
    if (!pPackage->ParseDelOrSelDeviceRepair(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->SearchDeviceRepair(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildSearchDeviceRepair(strSID, EErrCode::EC_SUCCESS, ECommand::CMD_SEARCH_DEVICEREPAIRINF, ptResult, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

EErrCode  MsgSearchRepairInfFromDevName(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    list<shared_ptr<RepairInfo>>  ptResult;
    string lstInfo;

    // 解析数据包
    if (!pPackage->ParseSearchDeviceRepair(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->SearchDeviceRepair(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildSearchDeviceRepair(strSID, EErrCode::EC_SUCCESS, ECommand::CMD_SEARCH_REPAIRINF_FORDEVICE, ptResult, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgAddDeviceRunInfo(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    shared_ptr<DeviceRunInfo>  ptResult = make_shared<DeviceRunInfo>();
    uint32_t ulRecordingID = 0;
    // 解析数据包
    if (!pPackage->ParseAddOrModDeviceRun(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->AddDeviceRun(ptResult, ulRecordingID);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildAddDeviceRecordInfo(strSID, EErrCode::EC_SUCCESS, ECommand::CMD_ADD_RUNINF, ulRecordingID, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgDeleteDeviceRunInfo(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    int  ptResult = 0;

    // 解析数据包
    if (!pPackage->ParseDelOrSelDeviceRun(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->DeleteDeviceRun(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildOnlyHead(strSID, EErrCode::EC_SUCCESS, ECommand::CMD_DELETE_RUNINF, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgModifyDeviceRunInfo(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    shared_ptr<DeviceRunInfo>  ptResult = make_shared<DeviceRunInfo>();

    // 解析数据包
    if (!pPackage->ParseAddOrModDeviceRun(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->ModifyDeviceRun(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildOnlyHead(strSID, EErrCode::EC_SUCCESS, ECommand::CMD_MODIFY_RUNINF, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode  MsgSearchDeviceRunInfo(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    //shared_ptr<RepairInfo>  ptResult;
    list<shared_ptr<DeviceRunInfo>>  ptResult;
    int lstInfo;

    // 解析数据包
    if (!pPackage->ParseDelOrSelDeviceRun(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->SearchDeviceRun(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildSearchDeviceRun(strSID, EErrCode::EC_SUCCESS, ECommand::CMD_SEARCH_RUNINF, ptResult, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

EErrCode  MsgSearchRunInfFromDevName(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    list<shared_ptr<DeviceRunInfo>>  ptResult;
    string lstInfo;

    // 解析数据包
    if (!pPackage->ParseSearchDeviceRun(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->SearchDeviceRun(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildSearchDeviceRun(strSID, EErrCode::EC_SUCCESS, ECommand::CMD_SEARCH_RUNINF_FORDEVICE, ptResult, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}


EErrCode MsgSearchSubSysVariableInfo(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    shared_ptr<SubSysInfo>  ptResult = make_shared<SubSysInfo>();

    // 解析数据包
    if (!pPackage->ParseSearchSubSysVariableInfo(strRecv, strSID))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->SearchSubSysVariableInfo(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildSearchSubSysVariableInfo(strSID, EErrCode::EC_SUCCESS, ptResult, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

EErrCode MsgSearchDeviceVariableInfo(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    shared_ptr<DeviceVarInfo>  ptResult = make_shared<DeviceVarInfo>();
    string lstInfo;
    // 解析数据包
    if (!pPackage->ParseSearchDeviceVariableInfo(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->SearchDeviceVariableInfo(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildSearchDeviceVariableInfo(strSID, EErrCode::EC_SUCCESS, ptResult, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

EErrCode MsgSearchTagVariableInfo(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    shared_ptr<TagVarInfo>  ptResult = make_shared<TagVarInfo>();
    string lstInfo;
    // 解析数据包
    if (!pPackage->ParseSearchTagVariableInfo(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->SearchTagVariableInfo(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildSearchTagVariableInfo(strSID, EErrCode::EC_SUCCESS, ptResult, strSend, ECommand::CMD_SEARCH_TAG))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}


EErrCode MsgSearchOverViewInfo(connection_hdl hdl, shared_ptr<CParameter> pParameter,
	const string &strRecv, string &strSend, string &strSID)
{
	if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
	shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
	shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
	shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
	shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

	shared_ptr<CSessionData>    pSession = nullptr;
	EErrCode eErr = EErrCode::EC_UNKNOW;

	shared_ptr<OverViewInfo>  ptResult = make_shared<OverViewInfo>();
	string lstInfo;
	// 解析数据包
	if (!pPackage->ParseSearchOverViewInfo(strRecv, strSID, lstInfo))
		return EErrCode::EC_PACKAGE_PARSE_FAILD;

	// 获取Session相关数据
	pSession = pSManager->GetSession(strSID);
	if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

	// 对session操作加锁
	lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

	// 单点登录实现位置，添加这个Socket到数据中
	if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
	{
		pSession->m_mapSocket.insert(make_pair(hdl, 0));
		pSocketData->SetSID(hdl, pSession->GetSID());
	}
	//数据库操作
	EErrCode status = pDBManager->SearchOverViewInfo(ptResult);
 	if (EErrCode::EC_SUCCESS != status) return status;
 
 	// 组建数据包
	if (!pPackage->BuildSearchOverViewInfo(strSID, EErrCode::EC_SUCCESS, ptResult, strSend))
 		return EErrCode::EC_PACKAGE_BUILD_FAILD;
	return EErrCode::EC_SUCCESS;
}



EErrCode MsgSearchSystemAlarmCountInfo(connection_hdl hdl, shared_ptr<CParameter> pParameter,
	const string &strRecv, string &strSend, string &strSID)
{
	if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
	shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
	shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
	shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
	shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

	shared_ptr<CSessionData>    pSession = nullptr;
	EErrCode eErr = EErrCode::EC_UNKNOW;

	shared_ptr<Subsystem_Alarm> ptResult = make_shared<Subsystem_Alarm>();
	string lstInfo;
	// 解析数据包
	if (!pPackage->ParseSearchOverViewInfo(strRecv, strSID, lstInfo))
		return EErrCode::EC_PACKAGE_PARSE_FAILD;

	// 获取Session相关数据
	pSession = pSManager->GetSession(strSID);
	if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

	// 对session操作加锁
	lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

	// 单点登录实现位置，添加这个Socket到数据中
	if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
	{
		pSession->m_mapSocket.insert(make_pair(hdl, 0));
		pSocketData->SetSID(hdl, pSession->GetSID());
	}
	//数据库操作
	EErrCode status = pDBManager->SearchAlarmCountInfo(ptResult);
	if (EErrCode::EC_SUCCESS != status) return status;

	// 组建数据包
	if (!pPackage->BuildSearchSystemAlarmCountInfo(strSID, EErrCode::EC_SUCCESS, ptResult, strSend))
		return EErrCode::EC_PACKAGE_BUILD_FAILD;
	return EErrCode::EC_SUCCESS;
}

EErrCode MsgSearchTagVarWithoutPar(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    shared_ptr<TagVarInfo>  ptResult = make_shared<TagVarInfo>();
    string lstInfo;
    // 解析数据包
    if (!pPackage->ParseSearchTagVariableInfo(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->SearchTagVariableInfo(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildSearchTagVariableInfo(strSID, EErrCode::EC_SUCCESS, ptResult, strSend, ECommand::CMD_SEARCH_TAG_WITHOUTPARAMETER))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

EErrCode MsgSearchMeterValueInfo(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    shared_ptr<ResponseMeterValueInfo>  ptResult = make_shared<ResponseMeterValueInfo>();
    shared_ptr<ValueRequestInfo>  lstInfo = make_shared<ValueRequestInfo>();
    // 解析数据包
    if (!pPackage->ParseSearchMeterValueInfo(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->SearchMeterValueInfo(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildSearchMeterValueInfo(strSID, EErrCode::EC_SUCCESS, ptResult, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

EErrCode MsgSearchDeviceValueInfo(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    shared_ptr<DeviceValueInfo>  ptResult = make_shared<DeviceValueInfo>();
    shared_ptr<ValueRequestInfo>  lstInfo = make_shared<ValueRequestInfo>();
    // 解析数据包
    if (!pPackage->ParseSearchDeviceValueInfo(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->SearchDeviceValueInfo(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildSearchDeviceValueInfo(strSID, EErrCode::EC_SUCCESS, ptResult, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

EErrCode MsgSearchVariableValueInfo(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    shared_ptr<DeviceValueInfo>  ptResult = make_shared<DeviceValueInfo>();
    shared_ptr<ValueRequestInfo>  lstInfo = make_shared<ValueRequestInfo>();
    // 解析数据包
    if (!pPackage->ParseSearchVariableValueInfo(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->SearchVariableValueInfo(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildSearchVariableValueInfo(strSID, EErrCode::EC_SUCCESS, ptResult, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}

EErrCode MsgSearchHistoryEventInfoEx(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    shared_ptr<HistoryEventInfo>  ptResult = make_shared<HistoryEventInfo>();
    shared_ptr<HistoryEventRequestInfo>  lstInfo = make_shared<HistoryEventRequestInfo>();
    // 解析数据包
    if (!pPackage->ParseSearchHistoryEventExInfo(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->SearchHistoryEventExInfo(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildSearchHistoryEventExInfo(strSID, EErrCode::EC_SUCCESS, ptResult, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}
EErrCode MsgSearchHistoryAlarmInfoEx(connection_hdl hdl, shared_ptr<CParameter> pParameter,
    const string &strRecv, string &strSend, string &strSID)
{
    if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
    shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
    shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
    shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
    shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();

    shared_ptr<CSessionData>    pSession = nullptr;
    EErrCode eErr = EErrCode::EC_UNKNOW;

    shared_ptr<HistoryAlarmInfo>  ptResult = make_shared<HistoryAlarmInfo>();
    shared_ptr<HistoryAlarmRequestInfo>  lstInfo = make_shared<HistoryAlarmRequestInfo>();
    // 解析数据包
    if (!pPackage->ParseSearchHistoryAlarmExInfo(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // 单点登录实现位置，添加这个Socket到数据中
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // 数据库操作
    EErrCode status = pDBManager->SearchHistoryAlarmExInfo(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // 组建数据包
    if (!pPackage->BuildSearchHistoryAlarmExInfo(strSID, EErrCode::EC_SUCCESS, ptResult, strSend))
        return EErrCode::EC_PACKAGE_BUILD_FAILD;
    return EErrCode::EC_SUCCESS;
}
//------------------------------------------------------------------------------
EErrCode  MsgCameraScreenshot(connection_hdl hdl, shared_ptr<CParameter> pParameter,
  const string &strRecv, string &strSend, string &strSID)
{
  if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
  shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
  shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
  shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
  shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();
  // 暂时用来获取opc状态
  shared_ptr<CTagDataMap> pTagDataMap = pParameter->GetDataCache()->GetTagMap();

  shared_ptr<CSessionData>    pSession = nullptr;
  EErrCode eErr = EErrCode::EC_UNKNOW;

  shared_ptr<KCamScreenshotInfo>  ptInfo = make_shared<KCamScreenshotInfo>();

  // 解析数据包
  if (!pPackage->ParseCameraScreenshot(strRecv, strSID, ptInfo))
    return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // 获取Session相关数据
  pSession = pSManager->GetSession(strSID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // 对session操作加锁
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

  // 单点登录实现位置，添加这个Socket到数据中
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  // 执行截图操作
  if (!CCameraScreenshot::Inst()->Screenshot(pParameter->GetBasicInfo()->strFFmpegPathName, ptInfo))
    return EErrCode::EC_CHECK_OPC_STATUS_FAILD;

  // 组建数据包
  if (!pPackage->BuildCameraScreenshot(strSID, EErrCode::EC_SUCCESS, strSend))
    return EErrCode::EC_PACKAGE_BUILD_FAILD;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
// 获取摄像头配置信息
//------------------------------------------------------------------------------
EErrCode  MsgSearchCameraConfig(connection_hdl hdl, shared_ptr<CParameter> pParameter,
  const string &strRecv, string &strSend, string &strSID)
{
  if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
  shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
  shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
  shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();

  shared_ptr<CSessionData>    pSession = nullptr;
  EErrCode eErr = EErrCode::EC_UNKNOW;

  // 解析数据包
  if (!pPackage->ParseSearchCameraConfig(strRecv, strSID))
    return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // 获取Session相关数据
  pSession = pSManager->GetSession(strSID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // 对session操作加锁
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

  // 单点登录实现位置，添加这个Socket到数据中
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  // 组建数据包
  if (!pPackage->BuildSearchCameraConfig(strSID, EErrCode::EC_SUCCESS,
    pParameter->GetConfig()->lstCamConf, strSend))
    return EErrCode::EC_PACKAGE_BUILD_FAILD;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
// 获取联动监视信息
//------------------------------------------------------------------------------
EErrCode  MsgReadLinkageWatchInfo(connection_hdl hdl, shared_ptr<CParameter> pParameter,
  const string &strRecv, string &strSend, string &strSID)
{
  if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
  shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
  shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
  shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
  shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();
  // 暂时用来获取opc状态
  shared_ptr<CTagDataMap> pTagDataMap = pParameter->GetDataCache()->GetTagMap();

  shared_ptr<CSessionData>    pSession = nullptr;
  RDBSInterface_ptr           pInterface = nullptr;
  EErrCode eErr = EErrCode::EC_UNKNOW;

  list<shared_ptr<KLinkageWatchInfo>> lstInfo;

  // 解析数据包
  if (!pPackage->ParseReadLinkageWatchInfo(strRecv, strSID))
    return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // 获取Session相关数据
  pSession = pSManager->GetSession(strSID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // 对session操作加锁
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);
  pInterface = pSession->m_pInterface;
  if (!pInterface) return EErrCode::EC_INTERFACE_OBJ_UNINVALID;

  // 单点登录实现位置，添加这个Socket到数据中
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  // 获取联动信息
  eErr = pInterface->ReadLinkageWatchInfo(lstInfo);
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  // 组建数据包
  if (!pPackage->BuildReadLinkageWatchInfo(strSID, EErrCode::EC_SUCCESS, lstInfo, strSend))
    return EErrCode::EC_PACKAGE_BUILD_FAILD;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
// 人工确认联动任务
//------------------------------------------------------------------------------
EErrCode  MsgLinkageManualConfirmation(connection_hdl hdl, shared_ptr<CParameter> pParameter,
  const string &strRecv, string &strSend, string &strSID)
{
  if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
  shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
  shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
  shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
  shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();
  // 暂时用来获取opc状态
  shared_ptr<CTagDataMap> pTagDataMap = pParameter->GetDataCache()->GetTagMap();

  shared_ptr<CSessionData>    pSession = nullptr;
  RDBSInterface_ptr           pInterface = nullptr;
  EErrCode eErr = EErrCode::EC_UNKNOW;
  uint32_t  ulItemID = 0;

  // 解析数据包
  if (!pPackage->ParseLinkageManualConfirmation(strRecv, strSID, ulItemID))
    return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // 获取Session相关数据
  pSession = pSManager->GetSession(strSID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // 对session操作加锁
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);
  pInterface = pSession->m_pInterface;
  if (!pInterface) return EErrCode::EC_INTERFACE_OBJ_UNINVALID;

  // 单点登录实现位置，添加这个Socket到数据中
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  // 获取联动信息
  eErr = pInterface->LinkageManualConfirmation(ulItemID);
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  // 组建数据包
  if (!pPackage->BuildLinkageManualConfirmation(strSID, EErrCode::EC_SUCCESS, strSend))
    return EErrCode::EC_PACKAGE_BUILD_FAILD;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
// 终止联动任务
//------------------------------------------------------------------------------
EErrCode  MsgLinkageTerminateTask(connection_hdl hdl, shared_ptr<CParameter> pParameter,
  const string &strRecv, string &strSend, string &strSID)
{
  if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
  shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
  shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
  shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
  shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();
  // 暂时用来获取opc状态
  shared_ptr<CTagDataMap> pTagDataMap = pParameter->GetDataCache()->GetTagMap();

  shared_ptr<CSessionData>    pSession = nullptr;
  RDBSInterface_ptr           pInterface = nullptr;
  EErrCode eErr = EErrCode::EC_UNKNOW;
  uint32_t  ulItemID = 0;

  // 解析数据包
  if (!pPackage->ParseLinkageTerminateTask(strRecv, strSID, ulItemID))
    return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // 获取Session相关数据
  pSession = pSManager->GetSession(strSID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // 对session操作加锁
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);
  pInterface = pSession->m_pInterface;
  if (!pInterface) return EErrCode::EC_INTERFACE_OBJ_UNINVALID;

  // 单点登录实现位置，添加这个Socket到数据中
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  // 获取联动信息
  eErr = pInterface->LinkageTerminateTask(ulItemID);
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  // 组建数据包
  if (!pPackage->BuildLinkageTerminateTask(strSID, EErrCode::EC_SUCCESS, strSend))
    return EErrCode::EC_PACKAGE_BUILD_FAILD;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
// 获取时间表调度监视信息
//------------------------------------------------------------------------------
EErrCode  MsgReadTimetableDispatchWatchInfo(connection_hdl hdl, shared_ptr<CParameter> pParameter,
  const string &strRecv, string &strSend, string &strSID)
{
  if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
  shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
  shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
  shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
  shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();
  // 暂时用来获取opc状态
  shared_ptr<CTagDataMap> pTagDataMap = pParameter->GetDataCache()->GetTagMap();

  shared_ptr<CSessionData>    pSession = nullptr;
  RDBSInterface_ptr           pInterface = nullptr;
  EErrCode eErr = EErrCode::EC_UNKNOW;

  list<shared_ptr<KTimetableDispatchWatchInfo>> lstInfo;

  // 解析数据包
  if (!pPackage->ParseReadTimetableDispatchWatchInfo(strRecv, strSID))
    return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // 获取Session相关数据
  pSession = pSManager->GetSession(strSID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // 对session操作加锁
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);
  pInterface = pSession->m_pInterface;
  if (!pInterface) return EErrCode::EC_INTERFACE_OBJ_UNINVALID;

  // 单点登录实现位置，添加这个Socket到数据中
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  // 获取联动信息
  eErr = pInterface->ReadTimetableDispatchWatchInfo(lstInfo);
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  // 组建数据包
  if (!pPackage->BuildReadTimetableDispatchInfo(strSID, EErrCode::EC_SUCCESS, lstInfo, strSend))
    return EErrCode::EC_PACKAGE_BUILD_FAILD;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
// 终止时间表调度任务
//------------------------------------------------------------------------------
EErrCode  MsgTimetableDispatchTerminateTask(connection_hdl hdl, shared_ptr<CParameter> pParameter,
  const string &strRecv, string &strSend, string &strSID)
{
  if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
  shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
  shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
  shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
  shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();
  // 暂时用来获取opc状态
  shared_ptr<CTagDataMap> pTagDataMap = pParameter->GetDataCache()->GetTagMap();

  shared_ptr<CSessionData>    pSession = nullptr;
  RDBSInterface_ptr           pInterface = nullptr;
  EErrCode eErr = EErrCode::EC_UNKNOW;
  uint32_t  ulItemID = 0;

  // 解析数据包
  if (!pPackage->ParseTimetableDispatchTerminateTask(strRecv, strSID, ulItemID))
    return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // 获取Session相关数据
  pSession = pSManager->GetSession(strSID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // 对session操作加锁
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);
  pInterface = pSession->m_pInterface;
  if (!pInterface) return EErrCode::EC_INTERFACE_OBJ_UNINVALID;

  // 单点登录实现位置，添加这个Socket到数据中
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  // 获取联动信息
  eErr = pInterface->TimetableDispatchTerminateTask(ulItemID);
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  // 组建数据包
  if (!pPackage->BuildTimetableDispatchTerminateTask(strSID, EErrCode::EC_SUCCESS, strSend))
    return EErrCode::EC_PACKAGE_BUILD_FAILD;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
// 默认处理
//------------------------------------------------------------------------------
void MsgDefault(const string &strSessionID, EErrCode eEC,
  ECommand eCmd, string &strDst)
{
  shared_ptr<CNetPackage>  pPackage = CNetPackage::Inst();
  pPackage->BuildDefault(strSessionID, eCmd, eEC, strDst);
}

//------------------------------------------------------------------------------
// WebSocket出错时的回调（带外部传入的自定义参数）
//------------------------------------------------------------------------------
void CWebSocketHandler::_OnConnect(connection_hdl hdl,
  shared_ptr<CParameter> pParameter)
{
}

//------------------------------------------------------------------------------
// WebSocket出错时的回调（带外部传入的自定义参数）
//------------------------------------------------------------------------------
void CWebSocketHandler::_OnDisconnect(connection_hdl hdl,
  shared_ptr<CParameter> pParameter)
{
  if (hdl.lock() == nullptr) return;

  shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
  shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();

  shared_ptr<CSessionData>    pSession = nullptr;
  string  strSID = "";

  // 通过Socket找到对应Session
  if (pSocketData->GetSID(hdl, strSID))
  {
    // 获取Session相关数据
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return;

    // 对session操作加锁
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);
    // 判断Session中的Socket有几个，如果就剩一个则直接删除Session
    if (pSession->m_mapSocket.size() > 1)
    {
      pSocketData->Delete(hdl);
      pSession->m_mapSocket.erase(hdl);
    }
    else
      Destroy(pSManager, pSession, pSocketData);

    LOG_DEBUG << "Delete Over";
  }
  LOG_DEBUG << "DelSession Success";
}

//------------------------------------------------------------------------------
// WebSocket出错时的回调（带外部传入的自定义参数）
//------------------------------------------------------------------------------
void CWebSocketHandler::_OnMessage(connection_hdl hdl,
  server::message_ptr msg, shared_ptr<CParameter> pParameter)
{
  // 检查数据有效性
  if (hdl.lock() == nullptr) return;

  shared_ptr<CNetPackage>  pPackage = CNetPackage::Inst();
  std::string strNetData = CUnitConvert::Inst()->UTF8ToAnsi(msg->get_payload());
  string  strSessionID = "";
  int nSeq = -1;

  ECommand  eCmd = pPackage->GetCommand(strNetData);

  LOG_DEBUG << "====Recvice Data=====";
  LOG_DEBUG << "Command = " << (uint32_t)eCmd;
  LOG_DEBUG << "vvvvvvvvvvvvvvvvvvvvvvvvvvvvvv";
  LOG_DEBUG << strNetData;
  LOG_DEBUG << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^";

  // 解析出来的数据所需要的接收变量
  std::string strResult = "";
  EErrCode eEC = EErrCode::EC_UNKNOW;

  switch (eCmd)
  {
  case ECommand::CMD_LOGIN: eEC = MsgLogin(hdl, pParameter, strNetData, strResult, strSessionID); break;
  case ECommand::CMD_LOGOUT:eEC = MsgLogout(hdl, pParameter, strNetData, strResult, strSessionID); break;
  case ECommand::CMD_READ_TAG:  eEC = MsgReadTag(hdl, pParameter, strNetData, strResult, strSessionID); break;
  case ECommand::CMD_READ_HISTORY_EVENT: eEC = MsgReadHistoryEvent(hdl, pParameter, strNetData, strResult, strSessionID); break;
  case ECommand::CMD_READ_HISTORY_ALARM: eEC = MsgReadHistoryAlarm(hdl, pParameter, strNetData, strResult, strSessionID); break;
  case ECommand::CMD_WRITE_TAG: eEC = MsgWriteTag(hdl, pParameter, strNetData, strResult, strSessionID); break;
  case ECommand::CMD_WRITE_EVENT:eEC = MsgWriteEvent(hdl, pParameter, strNetData, strResult, strSessionID); break;

  case ECommand::CMD_ADD_ALARM_ASSISTANT:eEC = MsgAddAlarmAssistant(hdl, pParameter, strNetData, strResult, strSessionID); break;
  case ECommand::CMD_DEL_ALARM_ASSISTANT:eEC = MsgDeleteAlarmAssistant(hdl, pParameter, strNetData, strResult, strSessionID); break;
  case ECommand::CMD_MODIFY_ALARM_ASSISTANT:eEC = MsgModifyAlarmAssistant(hdl, pParameter, strNetData, strResult, strSessionID); break;
  case ECommand::CMD_SEARCH_ALARM_ASSISTANT:eEC = MsgSearchAlarmAssistant(hdl, pParameter, strNetData, strResult, strSessionID);break;
  case ECommand::CMD_SEARCH_NUM_ALARM_ASSISTANT:eEC = MsgSearchAlarmAssistant_Num(hdl, pParameter, strNetData, strResult, strSessionID); break;
  case ECommand::CMD_SEARCH_PAGING_ALARM_ASSISTANT:eEC = MsgSearchPagingAlarmAssistant(hdl, pParameter, strNetData, strResult, strSessionID); break;

  case ECommand::CMD_ADD_CHANGE_SHIFTS:eEC = MsgAddChangeShifts(hdl, pParameter, strNetData, strResult, strSessionID); break;

  case ECommand::CMD_CHECK_DB_STATUS:eEC = MsgCheckDBStatus(hdl, pParameter, strNetData, strResult, strSessionID); break;
  case ECommand::CMD_CHECK_OPC_STATUS:eEC = MsgCheckOPCStatus(hdl, pParameter, strNetData, strResult, strSessionID); break;

  //设备相关信息查询
  case ECommand::CMD_SEARCH_SUPPLIERINF_FORTAGID:eEC = MsgSearchSupplierInfForTagID(hdl, pParameter, strNetData, strResult, strSessionID); break;
  case ECommand::CMD_SEARCH_DEVICEINF_FORTAGID: eEC = MsgSearchDeviceInfForTagID(hdl, pParameter, strNetData, strResult, strSessionID); break;
  case ECommand::CMD_SEARCH_REPAIRINF_FORTAGID: eEC = MsgSearchRepairInfForTagID(hdl, pParameter, strNetData, strResult, strSessionID); break;
  case ECommand::CMD_SEARCH_RUNINF_FORTAGID: eEC = MsgSearchRunInfForTagID(hdl, pParameter, strNetData, strResult, strSessionID); break;
  //信息管理接口
        /*供应商*/
    case ECommand::CMD_ADD_SUPPLIER: eEC = MsgAddSupplier(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_DELETE_SUPPLIER: eEC = MsgDeleteSupplier(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_MODIFY_SUPPLIER: eEC = MsgModifySupplier(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_SEARCH_SUPPLIER: eEC = MsgSearchSupplier(hdl, pParameter, strNetData, strResult, strSessionID); break;
        /*计量表*/
    case ECommand::CMD_ADD_METERTYPE: eEC = MsgAddMeterType(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_DELETE_METERTYPE: eEC = MsgDeleteMeterType(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_MODIFY_METERTYPE: eEC = MsgModifyMeterType(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_SEARCH_METERTYPE: eEC = MsgSearchMeterType(hdl, pParameter, strNetData, strResult, strSessionID); break;
        /*耗能类别*/
    case ECommand::CMD_ADD_ENERGYCATE: eEC = MsgAddEnergyCate(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_DELETE_ENERGYCATE: eEC = MsgDeleteEnergyCate(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_MODIFY_ENERGYCATE: eEC = MsgModifyEnergyCate(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_SEARCH_ENERGYCATE: eEC = MsgSearchEnergyCate(hdl, pParameter, strNetData, strResult, strSessionID); break;
        /*故障类别*/
    case ECommand::CMD_ADD_FAULTCATE:    eEC = MsgAddDevFaultCate(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_DELETE_FAULTCATE: eEC = MsgDeleteDevFaultCate(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_MODIFY_FAULTCATE: eEC = MsgModifyDevFaultCate(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_SEARCH_FAULTCATE: eEC = MsgSearchDevFaultCate(hdl, pParameter, strNetData, strResult, strSessionID); break;
        /*设备档案*/
    case ECommand::CMD_ADD_DEVICEINF: eEC = MsgAddDeviceInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_DELETE_DEVICEINF: eEC = MsgDeleteDeviceInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_MODIFY_DEVICEINF: eEC = MsgModifyDeviceInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_SEARCH_DEVICEINF: eEC = MsgSearchDeviceInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
        /*计量表档案*/
    case ECommand::CMD_ADD_METERINF: eEC = MsgAddMeterInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_DELETE_METERINF: eEC = MsgDeleteMeterInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_MODIFY_METERINF: eEC = MsgModifyMeterInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_SEARCH_METERINF: eEC = MsgSearchMeterInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
        /*设备参数*/
    case ECommand::CMD_ADD_DEVICEPARAMETER: eEC = MsgAddDeviceParameter(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_DELETE_DEVICEPARAMETER: eEC = MsgDeleteDeviceParameter(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_SEARCH_DEVICEPARAMETER: eEC = MsgSearchDeviceParameter(hdl, pParameter, strNetData, strResult, strSessionID); break;
        /*能源采集参数*/
    case ECommand::CMD_ADD_METERPARAMETER: eEC = MsgAddMeterParameter(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_DELETE_METERPARAMETER: eEC = MsgDeleteMeterParameter(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_SEARCH_METERPARAMETER: eEC = MsgSearchMeterParameter(hdl, pParameter, strNetData, strResult, strSessionID); break;
        /*设备维修记录*/
    case ECommand::CMD_ADD_DEVICEREPAIRINF: eEC = MsgAddDeviceRepairInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_DELETE_DEVICEREPAIRINF: eEC = MsgDeleteDeviceRepairInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_MODIFY_DEVICEREPAIRINF: eEC = MsgModifyDeviceRepairInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_SEARCH_DEVICEREPAIRINF: eEC = MsgSearchDeviceRepairInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_SEARCH_REPAIRINF_FORDEVICE: eEC = MsgSearchRepairInfFromDevName(hdl, pParameter, strNetData, strResult, strSessionID); break;
        /*设备运行记录*/
    case ECommand::CMD_ADD_RUNINF: eEC = MsgAddDeviceRunInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_DELETE_RUNINF: eEC = MsgDeleteDeviceRunInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_MODIFY_RUNINF: eEC = MsgModifyDeviceRunInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_SEARCH_RUNINF: eEC = MsgSearchDeviceRunInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_SEARCH_RUNINF_FORDEVICE: eEC = MsgSearchRunInfFromDevName(hdl, pParameter, strNetData, strResult, strSessionID); break;
        /*查询计量表读数*/
    case ECommand::CMD_SEARCH_METERVALUE: eEC = MsgSearchMeterValueInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
        /*按设备查询变量值*/
    case ECommand::CMD_SEARCH_DEVICEVALUE: eEC = MsgSearchDeviceValueInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
        /*查询能源统计结果*/
    case ECommand::CMD_SEARCH_ENERGYSTATISTICS:
        /*查询变量值*/
    case ECommand::CMD_SEARCH_TAGVALUE: eEC = MsgSearchVariableValueInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
        /*查询点表信息*/
    case ECommand::CMD_SEARCH_SYSTEM: eEC = MsgSearchSubSysVariableInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_SEARCH_DEVICE: eEC = MsgSearchDeviceVariableInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_SEARCH_TAG: eEC = MsgSearchTagVariableInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
 
	/*查询总览信息*/
	case ECommand::CMD_SEARCH_OVERVIEW: eEC = MsgSearchOverViewInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;

	case  ECommand::CMD_SEARCH_SUBSYSTEM_ALARM_COUNT: eEC = MsgSearchSystemAlarmCountInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
	case ECommand::CMD_SEARCH_TAG_WITHOUTPARAMETER: MsgSearchTagVarWithoutPar(hdl, pParameter, strNetData, strResult, strSessionID); break;
       
		/*历史事件*/
    case ECommand::CMD_READ_HISTORY_EVENT_EX: eEC = MsgSearchHistoryEventInfoEx(hdl, pParameter, strNetData, strResult, strSessionID); break;
        /*历史告警*/
    case ECommand::CMD_READ_HISTORY_ALARM_EX: eEC = MsgSearchHistoryAlarmInfoEx(hdl, pParameter, strNetData, strResult, strSessionID); break;

  case ECommand::CMD_CAMERA_SCREENSHOT: eEC = MsgCameraScreenshot(hdl, pParameter, strNetData, strResult, strSessionID);break;
  case ECommand::CMD_SEARCH_CAMERA_CONFIG: eEC = MsgSearchCameraConfig(hdl, pParameter, strNetData, strResult, strSessionID); break;

  // 联动信息
  case ECommand::CMD_READ_LINKAGE_WATCH_INFO: eEC = MsgReadLinkageWatchInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
  case ECommand::CMD_LINKAGE_MANUAL_CONFIRMATION:eEC = MsgLinkageManualConfirmation(hdl, pParameter, strNetData, strResult, strSessionID); break;
  case ECommand::CMD_LINKAGE_TERMINATE_TASK:eEC = MsgLinkageTerminateTask(hdl, pParameter, strNetData, strResult, strSessionID); break;
  // 时间表调度信息
  case ECommand::CMD_READ_TIMETABLE_DISPATCH_WATCH_INFO:eEC = MsgReadTimetableDispatchWatchInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
  case ECommand::CMD_TIMETABLE_DISPATCH_TERMINATE_TASK:eEC = MsgTimetableDispatchTerminateTask(hdl, pParameter, strNetData, strResult, strSessionID); break;
  default: eEC = EErrCode::EC_COMMAND_FAILD; break;
  }

  // 如果上面处理结果出错，则打包一个默认的处理结果
  if (EErrCode::EC_SUCCESS != eEC) MsgDefault(strSessionID, eEC, eCmd, strResult);

  // 发送结果数据
  m_server.send(hdl, CUnitConvert::Inst()->AnsiToUTF8(strResult), websocketpp::frame::opcode::text);

  LOG_DEBUG << "StatusCode = " << (uint32_t)eEC;
  LOG_DEBUG << "====Send Data=====";
  LOG_DEBUG << "Data Size = " << strResult.size();
  LOG_DEBUG << "vvvvvvvvvvvvvvvvvvvvvvvvvvvvvv";
  LOG_DEBUG << strResult;
  LOG_DEBUG << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^";
}
