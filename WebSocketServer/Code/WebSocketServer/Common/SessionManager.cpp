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

  // 给数据操作加锁
  lock_guard<recursive_mutex> alock(m_mutexRc);

  // 判断SessionID是否重复,如果重复重新生成SID
  if (m_mapSession.find(strSID) != m_mapSession.end())
  {
    strSID = OnCreSID();
    // 如果再次生成的还重复，则返回失败
    if (m_mapSession.find(strSID) != m_mapSession.end())
      return nullptr;
  }

  // 判断SID有效性
  if (strSID.empty()) return nullptr;

  // 创建一个Session
  pSession = make_shared<CSessionData>(strSID);
  if (!pSession) return nullptr;

  // 将信息添加到mapSession中
  m_mapSession.insert(make_pair(strSID, pSession));
  return pSession;
}

//------------------------------------------------------------------------------
shared_ptr<CSessionData> CSessionManager::GetSession(const string & strSID)
{
  // 判断数据有效性
  if (strSID.empty()) return nullptr;

  // 加锁
  lock_guard<recursive_mutex> alock(m_mutexRc);

  // 通过SID查找Session
  auto iter = m_mapSession.find(strSID);
  if (iter == m_mapSession.end()) return nullptr;
  return iter->second;
}

//------------------------------------------------------------------------------
void CSessionManager::DelSession(const string & strSID)
{
  // 判断数据有效性
  if (strSID.empty()) return;

  // 加锁
  lock_guard<recursive_mutex> alock(m_mutexRc);

  // 通过SID查找Session
  auto iter = m_mapSession.find(strSID);

  // 删除找到的Session
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

