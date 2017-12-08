//------------------------------------------------------------------------------
// �������ڣ�2017-06-19 11:04:39
// �ļ�������socket��recvive�����֣���Ϊ̫�࣬���Էֿ�����������
//          ������WebSocket�ı�д�߳�ͻ��С
//------------------------------------------------------------------------------
#include "stdafx.h"

#include "NetPackage/NetPackage.h"
#include "SessionManager.h"

#include "UnitConvert.h"
#include "RDBSInterface.h"
#include "Parameter.h"
#include "WebSocketHandler.h"

using namespace std;

recursive_mutex g_mutexLogon; // ������������֤��¼����

//------------------------------------------------------------------------------
// 1��ɾ��Session
// 2��ɾ����Session������Socket
//------------------------------------------------------------------------------
void Destroy(shared_ptr<CSessionManager> pSManager, shared_ptr<CSessionData> pSession,
  shared_ptr<CSocketRelativeData> pSocketData)
{
  if (!pSManager || !pSession || !pSocketData) return;

  // ɾ��Session
  pSManager->DelSession(pSession->GetSID());

  // ɾ����Socket��ص�����
  for (auto iter = pSession->m_mapSocket.begin(); iter != pSession->m_mapSocket.end(); iter++)
    pSocketData->Delete(iter->first);
}

//------------------------------------------------------------------------------
// ����strSession��nSeq���ڴ���ʧ�ܵ�ʱ���ⲿִ��Ĭ�ϴ���ʱ��Ҫ��
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

  // �������ݰ�
  if (!pPackage->ParseLogin(strSrc, strAccount, strPassword))
    return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // �жϸ�Socket�Ƿ��Ѿ���¼����û���򴴽�Session
  lock_guard<recursive_mutex> alock(g_mutexLogon);
  string  strSIDTmp = "";
  if (pSocketData->GetSID(hdl, strSIDTmp)) return EErrCode::EC_LOGGED_IN;

  // ����Session
  pSession = pSManager->CreSession();
  if (!pSession) return EErrCode::EC_CREATE_SESSION_FAILD;

//   // ��Session��������
//   lock_guard<recursive_mutex> galock(pSession->m_mutexRc);
// 
//   // �����ӿڶ��󽫶���ָ�뽻������ָ��
//   CComObject<CRDBSInterface>::CreateInstance(&pInterfaceTmp);
//   pInterface.reset(pInterfaceTmp);
// 
//   // ��Session�����Ϣ
//   pSession->m_pInterface = pInterface;
//   pSession->m_mapSocket.insert(make_pair(hdl, 0));
// 
//   // ��ʼ���ӿ�
//   eErr = pInterface->Init(pParameter->GetBasicInfo()->strHostName,
//     pParameter->GetDataCache(), pParameter->GetOptimize());
//   if (EErrCode::EC_SUCCESS != eErr) return eErr;
// 
//   // ��¼����
//   eErr = pInterface->Login(strAccount, strPassword, eUserType);
//   if (EErrCode::EC_SUCCESS != eErr) return eErr;

  // �齨���ݰ�
  if (!pPackage->BuildLogin(pSession->GetSID(), EErrCode::EC_SUCCESS, eUserType, strDst))
    return EErrCode::EC_PACKAGE_BUILD_FAILD;

  // ��Session��ӵ�CSocketRelativeData��
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

  // �������ݰ�
  if (!pPackage->ParseLogout(strSrc, strSessionID)) return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // ��ȡSession�������
  pSession = pSManager->GetSession(strSessionID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;
  
  // ��Session��������
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);
  pInterface = pSession->m_pInterface;
  if (!pInterface)  return EErrCode::EC_INTERFACE_OBJ_UNINVALID;

  // �˳���¼
  eErr = pInterface->Logout();
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  // ����Session���������Ϣ
  Destroy(pSManager, pSession, pSocketData);

  // �齨���ݰ�
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

  // �������ݰ�
  if (!pPackage->ParseReadTag(strSrc, strSessionID, lstTag)) return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // ��ȡSession�������
  pSession = pSManager->GetSession(strSessionID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // ��session��������
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);
  pInterface = pSession->m_pInterface;
  if (!pInterface) return EErrCode::EC_INTERFACE_OBJ_UNINVALID;

  // �����¼ʵ��λ�ã�������Socket��������
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  // ��ȡ��������
  eErr = pInterface->ReadTag(lstTag);
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  //  ������ID����SocketData��
  for (auto iter = lstTag.begin(); iter != lstTag.end(); iter++)
    if (!pSocketData->SetGID(hdl, (*iter)->ulGID)) return EErrCode::EC_GID_SET_DATA_FAILD;

  // �齨���ݰ�
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

  // �������ݰ�
  if (!pPackage->ParseWriteTag(strSrc, strSessionID, lstTag)) return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // ��ȡSession�������
  pSession = pSManager->GetSession(strSessionID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // ��session��������
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);
  pInterface = pSession->m_pInterface;
  if (!pInterface) return EErrCode::EC_INTERFACE_OBJ_UNINVALID;

  // �����¼ʵ��λ�ã�������Socket��������
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  // ��ȡ��������
  eErr = pInterface->WriteTag(lstTag);
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  // �齨���ݰ�
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

  // �������ݰ�
  if (!pPackage->ParseWriteEvent(strSrc, strSessionID, lstInfo)) return EErrCode::EC_PACKAGE_PARSE_FAILD;
  // ��ȡSession�������
  pSession = pSManager->GetSession(strSessionID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // ��session��������
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);
  pInterface = pSession->m_pInterface;
  if (!pInterface) return EErrCode::EC_INTERFACE_OBJ_UNINVALID;

  // �����¼ʵ��λ�ã�������Socket��������
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  // ��ȡ��������
  eErr = pInterface->WriteEvent(lstInfo);
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  // �齨���ݰ�
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

  // �������ݰ�
  if (!pPackage->ParseReadHistoryEvent(strSrc, strSessionID, ullStartTime, ullEndTime))
    return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // ��ȡSession�������
  pSession = pSManager->GetSession(strSessionID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // ��session��������
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);
  pInterface = pSession->m_pInterface;
  if (!pInterface) return EErrCode::EC_INTERFACE_OBJ_UNINVALID;

  // �����¼ʵ��λ�ã�������Socket��������
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  eErr = pInterface->ReadHistroyEvent(ullStartTime, ullEndTime, lstEvent);
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  // �齨���ݰ�
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

  // �������ݰ�
  if (!pPackage->ParseReadHistoryAlarm(strSrc, strSessionID, ullStartTime, ullEndTime))
    return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // ��ȡSession�������
  pSession = pSManager->GetSession(strSessionID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // ��session��������
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);
  pInterface = pSession->m_pInterface;
  if (!pInterface) return EErrCode::EC_INTERFACE_OBJ_UNINVALID;

  // �����¼ʵ��λ�ã�������Socket��������
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  eErr = pInterface->ReadHistroyAlarm(ullStartTime, ullEndTime, lstAlarm);
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  // �齨���ݰ�
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

  // �������ݰ�
  if (!pPackage->ParseAdd_AlarmAssistant(strRecv, strSID, lstInfo))
    return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // ��ȡSession�������
  pSession = pSManager->GetSession(strSID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // ��session��������
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

  // �����¼ʵ��λ�ã�������Socket��������
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  // ���ݿ����
  if (!pDBManager->AddAlarmAssistant(lstInfo, lstID)) return EErrCode::EC_DATABASE_FAIL;

  // �齨���ݰ�
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

  // �������ݰ�
  if (!pPackage->ParseDelete_AlarmAssistant(strRecv, strSID, lstID))
    return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // ��ȡSession�������
  pSession = pSManager->GetSession(strSID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // ��session��������
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

  // �����¼ʵ��λ�ã�������Socket��������
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  // ���ݿ����
  if (!pDBManager->DeleteAlarmAssistant(lstID)) return EErrCode::EC_DATABASE_FAIL;

  // �齨���ݰ�
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

  // �������ݰ�
  if (!pPackage->ParseModify_AlarmAssistant(strRecv, strSID, lstInfo))
    return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // ��ȡSession�������
  pSession = pSManager->GetSession(strSID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // ��session��������
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

  // �����¼ʵ��λ�ã�������Socket��������
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  // ���ݿ����
  if (!pDBManager->ModifyAlarmAssistant(lstInfo)) return EErrCode::EC_DATABASE_FAIL;

  // ��ȡ�����޸ĳɹ���ID
  for (auto &itemInfo : lstInfo)
    lstID.push_back(itemInfo->lID.first);

  // �齨���ݰ�
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

  // �������ݰ�
  if (!pPackage->ParseSearch_AlarmAssistant(strRecv, strSID, ulGlobalID))
    return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // ��ȡSession�������
  pSession = pSManager->GetSession(strSID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // ��session��������
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

  // �����¼ʵ��λ�ã�������Socket��������
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  // ���ݿ����
  if (!pDBManager->SearchAlarmAssistant(ulGlobalID, lstInfo)) return EErrCode::EC_DATABASE_FAIL;

  // �齨���ݰ�
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

  // �������ݰ�
  if (!pPackage->ParseSearch_Num_AlarmAssistant(strRecv, strSID, ptConfition))
    return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // ��ȡSession�������
  pSession = pSManager->GetSession(strSID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // ��session��������
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

  // �����¼ʵ��λ�ã�������Socket��������
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  // ���ݿ����
  if (!pDBManager->SearchAlarmAssistant_Num(ptConfition, ptResult)) return EErrCode::EC_DATABASE_FAIL;

  // �齨���ݰ�
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

  // �������ݰ�
  if (!pPackage->ParseSearchPaging_AlarmAssistant(strRecv, strSID, ptConfition))
    return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // ��ȡSession�������
  pSession = pSManager->GetSession(strSID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // ��session��������
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

  // �����¼ʵ��λ�ã�������Socket��������
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  // ���ݿ����
  if (!pDBManager->SearchPagingAlarmAssistant(ptConfition, ptResult)) return EErrCode::EC_DATABASE_FAIL;

  // �齨���ݰ�
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

  // �������ݰ�
  if (!pPackage->ParseAdd_ChangeShifts(strRecv, strSID, lstInfo))
    return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // ��ȡSession�������
  pSession = pSManager->GetSession(strSID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // ��session��������
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

  // �����¼ʵ��λ�ã�������Socket��������
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  // ���ݿ����
  if (!pDBManager->AddChangeShifts(lstInfo, lstID)) return EErrCode::EC_DATABASE_FAIL;

  // �齨���ݰ�
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

  // �������ݰ�
  if (!pPackage->ParseCheckDBStatus(strRecv, strSID))
    return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // ��ȡSession�������
  pSession = pSManager->GetSession(strSID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // ��session��������
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);
  pInterface = pSession->m_pInterface;

  // �����¼ʵ��λ�ã�������Socket��������
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  // ��ȡ���ݿ���ͨ״̬
  if (!pDBManager->CheckDBStatus(lstInfo)) return EErrCode::EC_DATABASE_FAIL;
  if (!pInterface->CheckDBStatus(lstInfo)) return EErrCode::EC_DATABASE_FAIL;

  // �齨���ݰ�
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
  // ��ʱ������ȡopc״̬
  shared_ptr<CTagDataMap> pTagDataMap = pParameter->GetDataCache()->GetTagMap();

  shared_ptr<CSessionData>    pSession = nullptr;
  EErrCode eErr = EErrCode::EC_UNKNOW;

  list<shared_ptr<KOPCStatusInfo>>  lstInfo;

  // �������ݰ�
  if (!pPackage->ParseCheckOPCStatus(strRecv, strSID))
    return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // ��ȡSession�������
  pSession = pSManager->GetSession(strSID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // ��session��������
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

  // �����¼ʵ��λ�ã�������Socket��������
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  // ��ȡOPC��ͨ״̬
  if (!pTagDataMap->CheckOPCStatus(lstInfo)) return EErrCode::EC_CHECK_OPC_STATUS_FAILD;

  // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseSearchID(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->SearchSupplierForTagID(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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
    // �������ݰ�
    if (!pPackage->ParseSearchID(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->SearchDeviceInfForTagID(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseSearchID(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->SearchRepairInfForTagID(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseSearchID(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->SearchRunInfForTagID(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseAddOrModSupplier(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->AddSupplier(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseDelOrSelSupplier(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->DeleteSupplier(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseAddOrModSupplier(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->ModifySuplier(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseDelOrSelSupplier(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->SearchSuplier(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseAddOrModMeterType(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->AddMeterType(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseDelOrSelMeterType(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->DeleteMeterType(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseAddOrModMeterType(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->ModifyMeterType(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseDelOrSelMeterType(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->SearchMeterType(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseAddOrModEnergyCate(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->AddEnergyCate(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseDelOrSelEnergyCate(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->DeleteEnergyCate(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseAddOrModEnergyCate(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->ModifyEnergyCate(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseDelOrSelEnergyCate(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->SearchEnergyCate(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseAddOrModDevFaultCate(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->AddDevFaultCate(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseDelOrSelDevFaultCate(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->DeleteDevFaultCate(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseAddOrModDevFaultCate(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->ModifyDevFaultCate(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseDelOrSelDevFaultCate(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->SearchDevFaultCate(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseAddOrModDeviceInfo(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->AddDeviceInfo(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseDelOrSelDeviceInfo(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->DeleteDeviceInfo(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseAddOrModDeviceInfo(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->ModifyDeviceInfo(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseDelOrSelDeviceInfo(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->SearchDeviceInfo(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseAddOrModMeterInfo(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->AddMeterInfo(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseDelOrSelMeterInfo(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->DeleteMeterInfo(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseAddOrModMeterInfo(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->ModifyMeterInfo(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseDelOrSelMeterInfo(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->SearchMeterInfo(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseAddDeviceParameter(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->AddDeviceParameter(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseDelDeviceParameter(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->DeleteDeviceParameter(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseSearchDeviceParameter(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->SearchDeviceParameter(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseAddMeterParameter(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->AddMeterParameter(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseDelMeterParameter(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->DeleteMeterParameter(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseSearchMeterParameter(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->SearchMeterParameter(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseAddOrModDeviceRepair(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->AddDeviceRepair(ptResult, ulRecordingID);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseDelOrSelDeviceRepair(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->DeleteDeviceRepair(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseAddOrModDeviceRepair(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->ModifyDeviceRepair(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseDelOrSelDeviceRepair(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->SearchDeviceRepair(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseSearchDeviceRepair(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->SearchDeviceRepair(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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
    // �������ݰ�
    if (!pPackage->ParseAddOrModDeviceRun(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->AddDeviceRun(ptResult, ulRecordingID);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseDelOrSelDeviceRun(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->DeleteDeviceRun(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseAddOrModDeviceRun(strRecv, strSID, ptResult))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->ModifyDeviceRun(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseDelOrSelDeviceRun(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->SearchDeviceRun(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseSearchDeviceRun(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->SearchDeviceRun(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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

    // �������ݰ�
    if (!pPackage->ParseSearchSubSysVariableInfo(strRecv, strSID))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->SearchSubSysVariableInfo(ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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
    // �������ݰ�
    if (!pPackage->ParseSearchDeviceVariableInfo(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->SearchDeviceVariableInfo(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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
    // �������ݰ�
    if (!pPackage->ParseSearchTagVariableInfo(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->SearchTagVariableInfo(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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
	// �������ݰ�
	if (!pPackage->ParseSearchOverViewInfo(strRecv, strSID, lstInfo))
		return EErrCode::EC_PACKAGE_PARSE_FAILD;

	// ��ȡSession�������
	pSession = pSManager->GetSession(strSID);
	if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

	// ��session��������
	lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

	// �����¼ʵ��λ�ã�������Socket��������
	if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
	{
		pSession->m_mapSocket.insert(make_pair(hdl, 0));
		pSocketData->SetSID(hdl, pSession->GetSID());
	}
	//���ݿ����
	EErrCode status = pDBManager->SearchOverViewInfo(ptResult);
 	if (EErrCode::EC_SUCCESS != status) return status;
 
 	// �齨���ݰ�
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
	// �������ݰ�
	if (!pPackage->ParseSearchOverViewInfo(strRecv, strSID, lstInfo))
		return EErrCode::EC_PACKAGE_PARSE_FAILD;

	// ��ȡSession�������
	pSession = pSManager->GetSession(strSID);
	if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

	// ��session��������
	lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

	// �����¼ʵ��λ�ã�������Socket��������
	if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
	{
		pSession->m_mapSocket.insert(make_pair(hdl, 0));
		pSocketData->SetSID(hdl, pSession->GetSID());
	}
	//���ݿ����
	EErrCode status = pDBManager->SearchAlarmCountInfo(ptResult);
	if (EErrCode::EC_SUCCESS != status) return status;

	// �齨���ݰ�
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
    // �������ݰ�
    if (!pPackage->ParseSearchTagVariableInfo(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->SearchTagVariableInfo(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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
    // �������ݰ�
    if (!pPackage->ParseSearchMeterValueInfo(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->SearchMeterValueInfo(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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
    // �������ݰ�
    if (!pPackage->ParseSearchDeviceValueInfo(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->SearchDeviceValueInfo(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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
    // �������ݰ�
    if (!pPackage->ParseSearchVariableValueInfo(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->SearchVariableValueInfo(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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
    // �������ݰ�
    if (!pPackage->ParseSearchHistoryEventExInfo(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->SearchHistoryEventExInfo(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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
    // �������ݰ�
    if (!pPackage->ParseSearchHistoryAlarmExInfo(strRecv, strSID, lstInfo))
        return EErrCode::EC_PACKAGE_PARSE_FAILD;

    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

    // �����¼ʵ��λ�ã�������Socket��������
    if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
    {
        pSession->m_mapSocket.insert(make_pair(hdl, 0));
        pSocketData->SetSID(hdl, pSession->GetSID());
    }

    // ���ݿ����
    EErrCode status = pDBManager->SearchHistoryAlarmExInfo(lstInfo, ptResult);
    if (EErrCode::EC_SUCCESS != status) return status;

    // �齨���ݰ�
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
  // ��ʱ������ȡopc״̬
  shared_ptr<CTagDataMap> pTagDataMap = pParameter->GetDataCache()->GetTagMap();

  shared_ptr<CSessionData>    pSession = nullptr;
  EErrCode eErr = EErrCode::EC_UNKNOW;

  shared_ptr<KCamScreenshotInfo>  ptInfo = make_shared<KCamScreenshotInfo>();

  // �������ݰ�
  if (!pPackage->ParseCameraScreenshot(strRecv, strSID, ptInfo))
    return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // ��ȡSession�������
  pSession = pSManager->GetSession(strSID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // ��session��������
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

  // �����¼ʵ��λ�ã�������Socket��������
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  // ִ�н�ͼ����
  if (!CCameraScreenshot::Inst()->Screenshot(pParameter->GetBasicInfo()->strFFmpegPathName, ptInfo))
    return EErrCode::EC_CHECK_OPC_STATUS_FAILD;

  // �齨���ݰ�
  if (!pPackage->BuildCameraScreenshot(strSID, EErrCode::EC_SUCCESS, strSend))
    return EErrCode::EC_PACKAGE_BUILD_FAILD;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
// ��ȡ����ͷ������Ϣ
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

  // �������ݰ�
  if (!pPackage->ParseSearchCameraConfig(strRecv, strSID))
    return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // ��ȡSession�������
  pSession = pSManager->GetSession(strSID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // ��session��������
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);

  // �����¼ʵ��λ�ã�������Socket��������
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  // �齨���ݰ�
  if (!pPackage->BuildSearchCameraConfig(strSID, EErrCode::EC_SUCCESS,
    pParameter->GetConfig()->lstCamConf, strSend))
    return EErrCode::EC_PACKAGE_BUILD_FAILD;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
// ��ȡ����������Ϣ
//------------------------------------------------------------------------------
EErrCode  MsgReadLinkageWatchInfo(connection_hdl hdl, shared_ptr<CParameter> pParameter,
  const string &strRecv, string &strSend, string &strSID)
{
  if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
  shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
  shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
  shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
  shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();
  // ��ʱ������ȡopc״̬
  shared_ptr<CTagDataMap> pTagDataMap = pParameter->GetDataCache()->GetTagMap();

  shared_ptr<CSessionData>    pSession = nullptr;
  RDBSInterface_ptr           pInterface = nullptr;
  EErrCode eErr = EErrCode::EC_UNKNOW;

  list<shared_ptr<KLinkageWatchInfo>> lstInfo;

  // �������ݰ�
  if (!pPackage->ParseReadLinkageWatchInfo(strRecv, strSID))
    return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // ��ȡSession�������
  pSession = pSManager->GetSession(strSID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // ��session��������
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);
  pInterface = pSession->m_pInterface;
  if (!pInterface) return EErrCode::EC_INTERFACE_OBJ_UNINVALID;

  // �����¼ʵ��λ�ã�������Socket��������
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  // ��ȡ������Ϣ
  eErr = pInterface->ReadLinkageWatchInfo(lstInfo);
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  // �齨���ݰ�
  if (!pPackage->BuildReadLinkageWatchInfo(strSID, EErrCode::EC_SUCCESS, lstInfo, strSend))
    return EErrCode::EC_PACKAGE_BUILD_FAILD;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
// �˹�ȷ����������
//------------------------------------------------------------------------------
EErrCode  MsgLinkageManualConfirmation(connection_hdl hdl, shared_ptr<CParameter> pParameter,
  const string &strRecv, string &strSend, string &strSID)
{
  if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
  shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
  shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
  shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
  shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();
  // ��ʱ������ȡopc״̬
  shared_ptr<CTagDataMap> pTagDataMap = pParameter->GetDataCache()->GetTagMap();

  shared_ptr<CSessionData>    pSession = nullptr;
  RDBSInterface_ptr           pInterface = nullptr;
  EErrCode eErr = EErrCode::EC_UNKNOW;
  uint32_t  ulItemID = 0;

  // �������ݰ�
  if (!pPackage->ParseLinkageManualConfirmation(strRecv, strSID, ulItemID))
    return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // ��ȡSession�������
  pSession = pSManager->GetSession(strSID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // ��session��������
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);
  pInterface = pSession->m_pInterface;
  if (!pInterface) return EErrCode::EC_INTERFACE_OBJ_UNINVALID;

  // �����¼ʵ��λ�ã�������Socket��������
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  // ��ȡ������Ϣ
  eErr = pInterface->LinkageManualConfirmation(ulItemID);
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  // �齨���ݰ�
  if (!pPackage->BuildLinkageManualConfirmation(strSID, EErrCode::EC_SUCCESS, strSend))
    return EErrCode::EC_PACKAGE_BUILD_FAILD;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
// ��ֹ��������
//------------------------------------------------------------------------------
EErrCode  MsgLinkageTerminateTask(connection_hdl hdl, shared_ptr<CParameter> pParameter,
  const string &strRecv, string &strSend, string &strSID)
{
  if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
  shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
  shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
  shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
  shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();
  // ��ʱ������ȡopc״̬
  shared_ptr<CTagDataMap> pTagDataMap = pParameter->GetDataCache()->GetTagMap();

  shared_ptr<CSessionData>    pSession = nullptr;
  RDBSInterface_ptr           pInterface = nullptr;
  EErrCode eErr = EErrCode::EC_UNKNOW;
  uint32_t  ulItemID = 0;

  // �������ݰ�
  if (!pPackage->ParseLinkageTerminateTask(strRecv, strSID, ulItemID))
    return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // ��ȡSession�������
  pSession = pSManager->GetSession(strSID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // ��session��������
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);
  pInterface = pSession->m_pInterface;
  if (!pInterface) return EErrCode::EC_INTERFACE_OBJ_UNINVALID;

  // �����¼ʵ��λ�ã�������Socket��������
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  // ��ȡ������Ϣ
  eErr = pInterface->LinkageTerminateTask(ulItemID);
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  // �齨���ݰ�
  if (!pPackage->BuildLinkageTerminateTask(strSID, EErrCode::EC_SUCCESS, strSend))
    return EErrCode::EC_PACKAGE_BUILD_FAILD;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
// ��ȡʱ�����ȼ�����Ϣ
//------------------------------------------------------------------------------
EErrCode  MsgReadTimetableDispatchWatchInfo(connection_hdl hdl, shared_ptr<CParameter> pParameter,
  const string &strRecv, string &strSend, string &strSID)
{
  if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
  shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
  shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
  shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
  shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();
  // ��ʱ������ȡopc״̬
  shared_ptr<CTagDataMap> pTagDataMap = pParameter->GetDataCache()->GetTagMap();

  shared_ptr<CSessionData>    pSession = nullptr;
  RDBSInterface_ptr           pInterface = nullptr;
  EErrCode eErr = EErrCode::EC_UNKNOW;

  list<shared_ptr<KTimetableDispatchWatchInfo>> lstInfo;

  // �������ݰ�
  if (!pPackage->ParseReadTimetableDispatchWatchInfo(strRecv, strSID))
    return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // ��ȡSession�������
  pSession = pSManager->GetSession(strSID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // ��session��������
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);
  pInterface = pSession->m_pInterface;
  if (!pInterface) return EErrCode::EC_INTERFACE_OBJ_UNINVALID;

  // �����¼ʵ��λ�ã�������Socket��������
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  // ��ȡ������Ϣ
  eErr = pInterface->ReadTimetableDispatchWatchInfo(lstInfo);
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  // �齨���ݰ�
  if (!pPackage->BuildReadTimetableDispatchInfo(strSID, EErrCode::EC_SUCCESS, lstInfo, strSend))
    return EErrCode::EC_PACKAGE_BUILD_FAILD;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
// ��ֹʱ����������
//------------------------------------------------------------------------------
EErrCode  MsgTimetableDispatchTerminateTask(connection_hdl hdl, shared_ptr<CParameter> pParameter,
  const string &strRecv, string &strSend, string &strSID)
{
  if (!pParameter) return EErrCode::EC_PARAM_UNINVALID;
  shared_ptr<CNetPackage> pPackage = CNetPackage::Inst();
  shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
  shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();
  shared_ptr<CDatabaseManager>  pDBManager = pParameter->GetDBManager();
  // ��ʱ������ȡopc״̬
  shared_ptr<CTagDataMap> pTagDataMap = pParameter->GetDataCache()->GetTagMap();

  shared_ptr<CSessionData>    pSession = nullptr;
  RDBSInterface_ptr           pInterface = nullptr;
  EErrCode eErr = EErrCode::EC_UNKNOW;
  uint32_t  ulItemID = 0;

  // �������ݰ�
  if (!pPackage->ParseTimetableDispatchTerminateTask(strRecv, strSID, ulItemID))
    return EErrCode::EC_PACKAGE_PARSE_FAILD;

  // ��ȡSession�������
  pSession = pSManager->GetSession(strSID);
  if (!pSession) return EErrCode::EC_SESSION_UNINVALID;

  // ��session��������
  lock_guard<recursive_mutex> alock(pSession->m_mutexRc);
  pInterface = pSession->m_pInterface;
  if (!pInterface) return EErrCode::EC_INTERFACE_OBJ_UNINVALID;

  // �����¼ʵ��λ�ã�������Socket��������
  if (pSession->m_mapSocket.find(hdl) == pSession->m_mapSocket.end())
  {
    pSession->m_mapSocket.insert(make_pair(hdl, 0));
    pSocketData->SetSID(hdl, pSession->GetSID());
  }

  // ��ȡ������Ϣ
  eErr = pInterface->TimetableDispatchTerminateTask(ulItemID);
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  // �齨���ݰ�
  if (!pPackage->BuildTimetableDispatchTerminateTask(strSID, EErrCode::EC_SUCCESS, strSend))
    return EErrCode::EC_PACKAGE_BUILD_FAILD;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
// Ĭ�ϴ���
//------------------------------------------------------------------------------
void MsgDefault(const string &strSessionID, EErrCode eEC,
  ECommand eCmd, string &strDst)
{
  shared_ptr<CNetPackage>  pPackage = CNetPackage::Inst();
  pPackage->BuildDefault(strSessionID, eCmd, eEC, strDst);
}

//------------------------------------------------------------------------------
// WebSocket����ʱ�Ļص������ⲿ������Զ��������
//------------------------------------------------------------------------------
void CWebSocketHandler::_OnConnect(connection_hdl hdl,
  shared_ptr<CParameter> pParameter)
{
}

//------------------------------------------------------------------------------
// WebSocket����ʱ�Ļص������ⲿ������Զ��������
//------------------------------------------------------------------------------
void CWebSocketHandler::_OnDisconnect(connection_hdl hdl,
  shared_ptr<CParameter> pParameter)
{
  if (hdl.lock() == nullptr) return;

  shared_ptr<CSessionManager> pSManager = pParameter->GetSManager();
  shared_ptr<CSocketRelativeData> pSocketData = pParameter->GetSocketData();

  shared_ptr<CSessionData>    pSession = nullptr;
  string  strSID = "";

  // ͨ��Socket�ҵ���ӦSession
  if (pSocketData->GetSID(hdl, strSID))
  {
    // ��ȡSession�������
    pSession = pSManager->GetSession(strSID);
    if (!pSession) return;

    // ��session��������
    lock_guard<recursive_mutex> alock(pSession->m_mutexRc);
    // �ж�Session�е�Socket�м����������ʣһ����ֱ��ɾ��Session
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
// WebSocket����ʱ�Ļص������ⲿ������Զ��������
//------------------------------------------------------------------------------
void CWebSocketHandler::_OnMessage(connection_hdl hdl,
  server::message_ptr msg, shared_ptr<CParameter> pParameter)
{
  // ���������Ч��
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

  // ������������������Ҫ�Ľ��ձ���
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

  //�豸�����Ϣ��ѯ
  case ECommand::CMD_SEARCH_SUPPLIERINF_FORTAGID:eEC = MsgSearchSupplierInfForTagID(hdl, pParameter, strNetData, strResult, strSessionID); break;
  case ECommand::CMD_SEARCH_DEVICEINF_FORTAGID: eEC = MsgSearchDeviceInfForTagID(hdl, pParameter, strNetData, strResult, strSessionID); break;
  case ECommand::CMD_SEARCH_REPAIRINF_FORTAGID: eEC = MsgSearchRepairInfForTagID(hdl, pParameter, strNetData, strResult, strSessionID); break;
  case ECommand::CMD_SEARCH_RUNINF_FORTAGID: eEC = MsgSearchRunInfForTagID(hdl, pParameter, strNetData, strResult, strSessionID); break;
  //��Ϣ����ӿ�
        /*��Ӧ��*/
    case ECommand::CMD_ADD_SUPPLIER: eEC = MsgAddSupplier(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_DELETE_SUPPLIER: eEC = MsgDeleteSupplier(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_MODIFY_SUPPLIER: eEC = MsgModifySupplier(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_SEARCH_SUPPLIER: eEC = MsgSearchSupplier(hdl, pParameter, strNetData, strResult, strSessionID); break;
        /*������*/
    case ECommand::CMD_ADD_METERTYPE: eEC = MsgAddMeterType(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_DELETE_METERTYPE: eEC = MsgDeleteMeterType(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_MODIFY_METERTYPE: eEC = MsgModifyMeterType(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_SEARCH_METERTYPE: eEC = MsgSearchMeterType(hdl, pParameter, strNetData, strResult, strSessionID); break;
        /*�������*/
    case ECommand::CMD_ADD_ENERGYCATE: eEC = MsgAddEnergyCate(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_DELETE_ENERGYCATE: eEC = MsgDeleteEnergyCate(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_MODIFY_ENERGYCATE: eEC = MsgModifyEnergyCate(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_SEARCH_ENERGYCATE: eEC = MsgSearchEnergyCate(hdl, pParameter, strNetData, strResult, strSessionID); break;
        /*�������*/
    case ECommand::CMD_ADD_FAULTCATE:    eEC = MsgAddDevFaultCate(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_DELETE_FAULTCATE: eEC = MsgDeleteDevFaultCate(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_MODIFY_FAULTCATE: eEC = MsgModifyDevFaultCate(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_SEARCH_FAULTCATE: eEC = MsgSearchDevFaultCate(hdl, pParameter, strNetData, strResult, strSessionID); break;
        /*�豸����*/
    case ECommand::CMD_ADD_DEVICEINF: eEC = MsgAddDeviceInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_DELETE_DEVICEINF: eEC = MsgDeleteDeviceInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_MODIFY_DEVICEINF: eEC = MsgModifyDeviceInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_SEARCH_DEVICEINF: eEC = MsgSearchDeviceInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
        /*��������*/
    case ECommand::CMD_ADD_METERINF: eEC = MsgAddMeterInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_DELETE_METERINF: eEC = MsgDeleteMeterInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_MODIFY_METERINF: eEC = MsgModifyMeterInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_SEARCH_METERINF: eEC = MsgSearchMeterInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
        /*�豸����*/
    case ECommand::CMD_ADD_DEVICEPARAMETER: eEC = MsgAddDeviceParameter(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_DELETE_DEVICEPARAMETER: eEC = MsgDeleteDeviceParameter(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_SEARCH_DEVICEPARAMETER: eEC = MsgSearchDeviceParameter(hdl, pParameter, strNetData, strResult, strSessionID); break;
        /*��Դ�ɼ�����*/
    case ECommand::CMD_ADD_METERPARAMETER: eEC = MsgAddMeterParameter(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_DELETE_METERPARAMETER: eEC = MsgDeleteMeterParameter(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_SEARCH_METERPARAMETER: eEC = MsgSearchMeterParameter(hdl, pParameter, strNetData, strResult, strSessionID); break;
        /*�豸ά�޼�¼*/
    case ECommand::CMD_ADD_DEVICEREPAIRINF: eEC = MsgAddDeviceRepairInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_DELETE_DEVICEREPAIRINF: eEC = MsgDeleteDeviceRepairInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_MODIFY_DEVICEREPAIRINF: eEC = MsgModifyDeviceRepairInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_SEARCH_DEVICEREPAIRINF: eEC = MsgSearchDeviceRepairInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_SEARCH_REPAIRINF_FORDEVICE: eEC = MsgSearchRepairInfFromDevName(hdl, pParameter, strNetData, strResult, strSessionID); break;
        /*�豸���м�¼*/
    case ECommand::CMD_ADD_RUNINF: eEC = MsgAddDeviceRunInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_DELETE_RUNINF: eEC = MsgDeleteDeviceRunInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_MODIFY_RUNINF: eEC = MsgModifyDeviceRunInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_SEARCH_RUNINF: eEC = MsgSearchDeviceRunInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_SEARCH_RUNINF_FORDEVICE: eEC = MsgSearchRunInfFromDevName(hdl, pParameter, strNetData, strResult, strSessionID); break;
        /*��ѯ���������*/
    case ECommand::CMD_SEARCH_METERVALUE: eEC = MsgSearchMeterValueInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
        /*���豸��ѯ����ֵ*/
    case ECommand::CMD_SEARCH_DEVICEVALUE: eEC = MsgSearchDeviceValueInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
        /*��ѯ��Դͳ�ƽ��*/
    case ECommand::CMD_SEARCH_ENERGYSTATISTICS:
        /*��ѯ����ֵ*/
    case ECommand::CMD_SEARCH_TAGVALUE: eEC = MsgSearchVariableValueInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
        /*��ѯ�����Ϣ*/
    case ECommand::CMD_SEARCH_SYSTEM: eEC = MsgSearchSubSysVariableInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_SEARCH_DEVICE: eEC = MsgSearchDeviceVariableInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
    case ECommand::CMD_SEARCH_TAG: eEC = MsgSearchTagVariableInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
 
	/*��ѯ������Ϣ*/
	case ECommand::CMD_SEARCH_OVERVIEW: eEC = MsgSearchOverViewInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;

	case  ECommand::CMD_SEARCH_SUBSYSTEM_ALARM_COUNT: eEC = MsgSearchSystemAlarmCountInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
	case ECommand::CMD_SEARCH_TAG_WITHOUTPARAMETER: MsgSearchTagVarWithoutPar(hdl, pParameter, strNetData, strResult, strSessionID); break;
       
		/*��ʷ�¼�*/
    case ECommand::CMD_READ_HISTORY_EVENT_EX: eEC = MsgSearchHistoryEventInfoEx(hdl, pParameter, strNetData, strResult, strSessionID); break;
        /*��ʷ�澯*/
    case ECommand::CMD_READ_HISTORY_ALARM_EX: eEC = MsgSearchHistoryAlarmInfoEx(hdl, pParameter, strNetData, strResult, strSessionID); break;

  case ECommand::CMD_CAMERA_SCREENSHOT: eEC = MsgCameraScreenshot(hdl, pParameter, strNetData, strResult, strSessionID);break;
  case ECommand::CMD_SEARCH_CAMERA_CONFIG: eEC = MsgSearchCameraConfig(hdl, pParameter, strNetData, strResult, strSessionID); break;

  // ������Ϣ
  case ECommand::CMD_READ_LINKAGE_WATCH_INFO: eEC = MsgReadLinkageWatchInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
  case ECommand::CMD_LINKAGE_MANUAL_CONFIRMATION:eEC = MsgLinkageManualConfirmation(hdl, pParameter, strNetData, strResult, strSessionID); break;
  case ECommand::CMD_LINKAGE_TERMINATE_TASK:eEC = MsgLinkageTerminateTask(hdl, pParameter, strNetData, strResult, strSessionID); break;
  // ʱ��������Ϣ
  case ECommand::CMD_READ_TIMETABLE_DISPATCH_WATCH_INFO:eEC = MsgReadTimetableDispatchWatchInfo(hdl, pParameter, strNetData, strResult, strSessionID); break;
  case ECommand::CMD_TIMETABLE_DISPATCH_TERMINATE_TASK:eEC = MsgTimetableDispatchTerminateTask(hdl, pParameter, strNetData, strResult, strSessionID); break;
  default: eEC = EErrCode::EC_COMMAND_FAILD; break;
  }

  // ������洦������������һ��Ĭ�ϵĴ�����
  if (EErrCode::EC_SUCCESS != eEC) MsgDefault(strSessionID, eEC, eCmd, strResult);

  // ���ͽ������
  m_server.send(hdl, CUnitConvert::Inst()->AnsiToUTF8(strResult), websocketpp::frame::opcode::text);

  LOG_DEBUG << "StatusCode = " << (uint32_t)eEC;
  LOG_DEBUG << "====Send Data=====";
  LOG_DEBUG << "Data Size = " << strResult.size();
  LOG_DEBUG << "vvvvvvvvvvvvvvvvvvvvvvvvvvvvvv";
  LOG_DEBUG << strResult;
  LOG_DEBUG << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^";
}
