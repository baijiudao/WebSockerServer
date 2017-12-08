#include "stdafx.h"
#include <assert.h>
#include <random>
#include "SessionManager.h"
#include <iomanip>

// -- CSessionManager --

//------------------------------------------------------------------------------
CSessionManager::CSessionManager()
{
}

//------------------------------------------------------------------------------
CSessionManager::~CSessionManager()
{
}

//------------------------------------------------------------------------------
std::string CSessionManager::OnCreSID()
{
#ifdef _DEBUG_CODE_
  return "8SDF52E6S3";
#else
  std::random_device  rd;
  stringstream  strStream;
  strStream << std::hex << std::uppercase << std::setw(sizeof(int32_t)) << std::setfill('0') << rd();
  return strStream.str();
#endif // _DEBUG_CODE_
}

//------------------------------------------------------------------------------
shared_ptr<CSessionData> CSessionManager::CreSession()
{
  shared_ptr<CSessionData> pSession = nullptr;
  string  strSID = OnCreSID();

  // �����ݲ�������
  lock_guard<recursive_mutex> alock(m_mutexRc);

  // �ж�SessionID�Ƿ��ظ�,����ظ���������SID
  if (m_mapSession.find(strSID) != m_mapSession.end())
  {
    strSID = OnCreSID();
    // ����ٴ����ɵĻ��ظ����򷵻�ʧ��
    if (m_mapSession.find(strSID) != m_mapSession.end())
      return nullptr;
  }

  // �ж�SID��Ч��
  if (strSID.empty()) return nullptr;

  // ����һ��Session
  pSession = make_shared<CSessionData>(strSID);
  if (!pSession) return nullptr;

  // ����Ϣ��ӵ�mapSession��
  m_mapSession.insert(make_pair(strSID, pSession));
  return pSession;
}

//------------------------------------------------------------------------------
shared_ptr<CSessionData> CSessionManager::GetSession(const string & strSID)
{
  // �ж�������Ч��
  if (strSID.empty()) return nullptr;

  // ����
  lock_guard<recursive_mutex> alock(m_mutexRc);

  // ͨ��SID����Session
  auto iter = m_mapSession.find(strSID);
  if (iter == m_mapSession.end()) return nullptr;
  return iter->second;
}

//------------------------------------------------------------------------------
void CSessionManager::DelSession(const string & strSID)
{
  // �ж�������Ч��
  if (strSID.empty()) return;

  // ����
  lock_guard<recursive_mutex> alock(m_mutexRc);

  // ͨ��SID����Session
  auto iter = m_mapSession.find(strSID);

  // ɾ���ҵ���Session
  if (iter != m_mapSession.end())
    m_mapSession.erase(iter);
}


// -- CAuthorManager --

//------------------------------------------------------------------------------
CAuthorManager::CAuthorManager()
{
}

//------------------------------------------------------------------------------
CAuthorManager::~CAuthorManager()
{
}

