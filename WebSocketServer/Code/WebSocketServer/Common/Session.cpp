#include "stdafx.h"
#include <assert.h>
#include "Session.h"

// -- CSessionData --

//------------------------------------------------------------------------------
CSessionData::CSessionData(const string & strSID)
{
  m_strSID = strSID;
  m_pInterface = nullptr;
}

//------------------------------------------------------------------------------
CSessionData::~CSessionData()
{
}

//------------------------------------------------------------------------------
string  CSessionData::GetSID()
{
  return m_strSID;
}


// -- CSocketRelativeData --

//------------------------------------------------------------------------------
CSocketRelativeData::CSocketRelativeData()
{
}

//------------------------------------------------------------------------------
CSocketRelativeData::~CSocketRelativeData()
{
}

//------------------------------------------------------------------------------
bool CSocketRelativeData::OnSetGIDSkt(const DWORD dwGID, const connection_hdl hdl)
{
  // 加锁
  lock_guard<recursive_mutex> alock(m_mutexGID);

  // 在MapGIDLstSkt中查找数据
  auto iter = m_mapGIDSkt.find(dwGID);
  if (iter == m_mapGIDSkt.end())
  {
    // 找不到就创建一个新list
    shared_ptr<ListSocket> plst = make_shared<ListSocket>();
    m_mapGIDSkt.insert(make_pair(dwGID, plst));

    // 将Socket添加到list中
    plst->push_back(hdl);
    return true;
  }

  // 存在就添加数据
  iter->second->push_back(hdl);
  return true;
}

//------------------------------------------------------------------------------
bool CSocketRelativeData::OnSetSktGID(const connection_hdl hdl, const DWORD dwGID)
{
  // 加锁
  lock_guard<recursive_mutex> alock(m_mutexGID);

  // 在MapSktLstGID中查找数据
  auto iter = m_mapSktGID.find(hdl);
  if (iter == m_mapSktGID.end())
  {
    // 找不到就创建一个新list
    shared_ptr<list<DWORD>> plst = make_shared<list<DWORD>>();
    m_mapSktGID.insert(make_pair(hdl, plst));

    // 将GID添加到list中
    plst->push_back(dwGID);
    return true;
  }

  // 存在就添加数据
  // 首先判断GID是否重复
  for (auto &item : *(iter->second))
    if (item == dwGID) return true;

  // 如果没有找到重复的就直接添加数据
  iter->second->push_back(dwGID);
  return true;
}

//------------------------------------------------------------------------------
void CSocketRelativeData::OnDelSktSID(const connection_hdl hdl)
{
  // 检查数据有效性
  if (!(hdl.lock())) return;

  // 加锁
  lock_guard<recursive_mutex> alock(m_mutexSID);

  // 删除数据
  m_mapSktSID.erase(hdl);
}

//------------------------------------------------------------------------------
void CSocketRelativeData::OnDelSktGID(const connection_hdl hdl)
{
  // 检查数据有效性
  if (!(hdl.lock())) return;

  // 加锁
  lock_guard<recursive_mutex> alock(m_mutexGID);

  // 通过MapSktLstGID 获取Socket对应的GID list
  auto iterSktGID = m_mapSktGID.find(hdl);
  if (iterSktGID == m_mapSktGID.end()) return;
  auto plstGID = iterSktGID->second;  // shared_ptr<list<int32_t> >

  // 从MapSktLstGID中删除这个Socket相关信息
  m_mapSktGID.erase(iterSktGID);

  // 根据GID删除对应的list
  for (auto iterGID = plstGID->begin(); iterGID != plstGID->end(); iterGID++)
  {
    // 通过MapGIDLstSkt 找到对应的Socket list
    auto iterGIDSkt = m_mapGIDSkt.find(*iterGID);
    if (iterGIDSkt == m_mapGIDSkt.end()) continue;
    auto plstSkt = iterGIDSkt->second;

    // 删除对应Socket
    for (auto iterSkt = plstSkt->begin(); iterSkt != plstSkt->end(); iterSkt++)
    {
      // 两个Socket链接不同则不进行下面的操作
      if (!hdl.owner_before(*iterSkt)) continue;
      plstSkt->erase(iterSkt);
      break;
    }

    // 判断Socket list是否还有数据，没有就直接删除
    if (plstSkt->empty()) m_mapGIDSkt.erase(iterGIDSkt);
  }
}

//------------------------------------------------------------------------------
bool CSocketRelativeData::SetSID(const connection_hdl hdl, const string & strSID)
{
  // 检查数据有效性
  if (!(hdl.lock()) || strSID.empty()) return false;

  // 加锁
  lock_guard<recursive_mutex> alock(m_mutexSID);

  // 添加数据
  m_mapSktSID.insert(make_pair(hdl, strSID));
  return true;
}

//------------------------------------------------------------------------------
bool  CSocketRelativeData::GetSID(const connection_hdl hdl, string & strSID)
{
  // 检查数据有效性
  if (!(hdl.lock())) return false;

  // 加锁
  lock_guard<recursive_mutex> alock(m_mutexSID);

  // 获取对应的SID
  auto iter = m_mapSktSID.find(hdl);
  if (iter == m_mapSktSID.end()) return false;

  // 传出SID
  strSID = iter->second;
  return true;
}

//------------------------------------------------------------------------------
bool CSocketRelativeData::SetGID(const connection_hdl hdl, const DWORD dwGID)
{
  // 检查数据有效性
  if (!(hdl.lock())) return false;

  // 将数据添加到map中
  if (!OnSetGIDSkt(dwGID, hdl)) return false;
  if (!OnSetSktGID(hdl, dwGID)) return false;
  return true;
}

//------------------------------------------------------------------------------
bool CSocketRelativeData::GetSocketList(shared_ptr<list<connection_hdl>> plstHdl)
{
  if (!plstHdl) return false;

  lock_guard<recursive_mutex> alock(m_mutexSID);
  // 获取所有socket
  for (auto iter = m_mapSktSID.begin(); iter != m_mapSktSID.end(); iter++)
    plstHdl->push_back(iter->first);

  return true;
}

//------------------------------------------------------------------------------
// 获得一个socket上绑定的所有GID
//------------------------------------------------------------------------------
bool CSocketRelativeData::GetGIDListBySocket(const connection_hdl hdl,
  list<DWORD> &lstGID)
{
  // 加锁
  lock_guard<recursive_mutex> alockSID(m_mutexSID);
  lock_guard<recursive_mutex> alockGID(m_mutexGID);

  // 查找hdl对应的所有GID
  auto iter = m_mapSktGID.find(hdl);
  if (iter == m_mapSktGID.end()) return false;

  for (auto &item : *(iter->second))
    lstGID.push_back(item);

  return true;
}

//------------------------------------------------------------------------------
bool CSocketRelativeData::GetTagPushDataColl(const DWORD dwGID,
  const shared_ptr<KTagInfo> pInfo, shared_ptr<CTagPushDataQueue> pQueue)
{
  // 检查数据有效性
  if (!pInfo || !pQueue) return false;

  // 局部变量
  shared_ptr<CTagPushData>  pData = nullptr;

  // 加锁
  lock_guard<recursive_mutex> alockSID(m_mutexSID);
  lock_guard<recursive_mutex> alockGID(m_mutexGID);

  // 查找Socket List
  auto iter = m_mapGIDSkt.find(dwGID);
  if (iter == m_mapGIDSkt.end()) return false;
  shared_ptr<ListSocket> plst = iter->second;

  // 给每个Socket添加匹配数据
  for (auto iterSkt = plst->begin(); iterSkt != plst->end(); iterSkt++)
  {
    // 通过Socket找到SID
    auto iterSktSID = m_mapSktSID.find(*iterSkt);
    if (iterSktSID == m_mapSktSID.end()) continue;

    // 创建数据
    pData = make_shared<CTagPushData>(pInfo, iterSktSID->first, iterSktSID->second);
    // 将数据添加到队列中
    if (!pQueue->PushInto(pData)) return false;
  }

  return true;
}

//------------------------------------------------------------------------------
bool CSocketRelativeData::GetEvtPushDataColl(const shared_ptr<KEventInfo> pInfo,
  shared_ptr<CEventPushDataQueue> pQueue)
{
  // 检查数据有效性
  if (!pInfo || !pQueue) return false;

  // 局部变量
  shared_ptr<CEventPushData>  pData = nullptr;

  // 加锁
  lock_guard<recursive_mutex> alockSID(m_mutexSID);

  // 给每个Socket添加匹配数据
  for (auto iter = m_mapSktSID.begin(); iter != m_mapSktSID.end(); iter++)
  {
    // 创建数据
    pData = make_shared<CEventPushData>(pInfo, iter->first, iter->second);
    // 将数据添加到队列中
    if (!pQueue->PushInto(pData)) return false;
  }

  return true;
}

//------------------------------------------------------------------------------
bool CSocketRelativeData::GetAlmPushDataColl(const shared_ptr<KAlarmInfo> pInfo,
  shared_ptr<CAlarmPushDataQueue> pQueue)
{
  // 检查数据有效性
  if (!pInfo || !pQueue) return false;

  // 局部变量
  shared_ptr<CAlarmPushData>  pData = nullptr;

  // 加锁
  lock_guard<recursive_mutex> alockSID(m_mutexSID);

  // 给每个Socket添加匹配数据
  for (auto iter = m_mapSktSID.begin(); iter != m_mapSktSID.end(); iter++)
  {
    // 创建数据
    pData = make_shared<CAlarmPushData>(pInfo, iter->first, iter->second);
    // 将数据添加到队列中
    if (!pQueue->PushInto(pData)) return false;
  }

  return true;
}

//------------------------------------------------------------------------------
void CSocketRelativeData::Delete(const connection_hdl hdl)
{
  // 检查数据有效性
  if (!(hdl.lock())) return;

  // 删除与Socket相关的SID数据
  OnDelSktSID(hdl);
  // 删除与Socket相关的GID数据
  OnDelSktGID(hdl);
}
