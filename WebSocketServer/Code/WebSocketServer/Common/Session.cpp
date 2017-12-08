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
  // ����
  lock_guard<recursive_mutex> alock(m_mutexGID);

  // ��MapGIDLstSkt�в�������
  auto iter = m_mapGIDSkt.find(dwGID);
  if (iter == m_mapGIDSkt.end())
  {
    // �Ҳ����ʹ���һ����list
    shared_ptr<ListSocket> plst = make_shared<ListSocket>();
    m_mapGIDSkt.insert(make_pair(dwGID, plst));

    // ��Socket��ӵ�list��
    plst->push_back(hdl);
    return true;
  }

  // ���ھ��������
  iter->second->push_back(hdl);
  return true;
}

//------------------------------------------------------------------------------
bool CSocketRelativeData::OnSetSktGID(const connection_hdl hdl, const DWORD dwGID)
{
  // ����
  lock_guard<recursive_mutex> alock(m_mutexGID);

  // ��MapSktLstGID�в�������
  auto iter = m_mapSktGID.find(hdl);
  if (iter == m_mapSktGID.end())
  {
    // �Ҳ����ʹ���һ����list
    shared_ptr<list<DWORD>> plst = make_shared<list<DWORD>>();
    m_mapSktGID.insert(make_pair(hdl, plst));

    // ��GID��ӵ�list��
    plst->push_back(dwGID);
    return true;
  }

  // ���ھ��������
  // �����ж�GID�Ƿ��ظ�
  for (auto &item : *(iter->second))
    if (item == dwGID) return true;

  // ���û���ҵ��ظ��ľ�ֱ���������
  iter->second->push_back(dwGID);
  return true;
}

//------------------------------------------------------------------------------
void CSocketRelativeData::OnDelSktSID(const connection_hdl hdl)
{
  // ���������Ч��
  if (!(hdl.lock())) return;

  // ����
  lock_guard<recursive_mutex> alock(m_mutexSID);

  // ɾ������
  m_mapSktSID.erase(hdl);
}

//------------------------------------------------------------------------------
void CSocketRelativeData::OnDelSktGID(const connection_hdl hdl)
{
  // ���������Ч��
  if (!(hdl.lock())) return;

  // ����
  lock_guard<recursive_mutex> alock(m_mutexGID);

  // ͨ��MapSktLstGID ��ȡSocket��Ӧ��GID list
  auto iterSktGID = m_mapSktGID.find(hdl);
  if (iterSktGID == m_mapSktGID.end()) return;
  auto plstGID = iterSktGID->second;  // shared_ptr<list<int32_t> >

  // ��MapSktLstGID��ɾ�����Socket�����Ϣ
  m_mapSktGID.erase(iterSktGID);

  // ����GIDɾ����Ӧ��list
  for (auto iterGID = plstGID->begin(); iterGID != plstGID->end(); iterGID++)
  {
    // ͨ��MapGIDLstSkt �ҵ���Ӧ��Socket list
    auto iterGIDSkt = m_mapGIDSkt.find(*iterGID);
    if (iterGIDSkt == m_mapGIDSkt.end()) continue;
    auto plstSkt = iterGIDSkt->second;

    // ɾ����ӦSocket
    for (auto iterSkt = plstSkt->begin(); iterSkt != plstSkt->end(); iterSkt++)
    {
      // ����Socket���Ӳ�ͬ�򲻽�������Ĳ���
      if (!hdl.owner_before(*iterSkt)) continue;
      plstSkt->erase(iterSkt);
      break;
    }

    // �ж�Socket list�Ƿ������ݣ�û�о�ֱ��ɾ��
    if (plstSkt->empty()) m_mapGIDSkt.erase(iterGIDSkt);
  }
}

//------------------------------------------------------------------------------
bool CSocketRelativeData::SetSID(const connection_hdl hdl, const string & strSID)
{
  // ���������Ч��
  if (!(hdl.lock()) || strSID.empty()) return false;

  // ����
  lock_guard<recursive_mutex> alock(m_mutexSID);

  // �������
  m_mapSktSID.insert(make_pair(hdl, strSID));
  return true;
}

//------------------------------------------------------------------------------
bool  CSocketRelativeData::GetSID(const connection_hdl hdl, string & strSID)
{
  // ���������Ч��
  if (!(hdl.lock())) return false;

  // ����
  lock_guard<recursive_mutex> alock(m_mutexSID);

  // ��ȡ��Ӧ��SID
  auto iter = m_mapSktSID.find(hdl);
  if (iter == m_mapSktSID.end()) return false;

  // ����SID
  strSID = iter->second;
  return true;
}

//------------------------------------------------------------------------------
bool CSocketRelativeData::SetGID(const connection_hdl hdl, const DWORD dwGID)
{
  // ���������Ч��
  if (!(hdl.lock())) return false;

  // ��������ӵ�map��
  if (!OnSetGIDSkt(dwGID, hdl)) return false;
  if (!OnSetSktGID(hdl, dwGID)) return false;
  return true;
}

//------------------------------------------------------------------------------
bool CSocketRelativeData::GetSocketList(shared_ptr<list<connection_hdl>> plstHdl)
{
  if (!plstHdl) return false;

  lock_guard<recursive_mutex> alock(m_mutexSID);
  // ��ȡ����socket
  for (auto iter = m_mapSktSID.begin(); iter != m_mapSktSID.end(); iter++)
    plstHdl->push_back(iter->first);

  return true;
}

//------------------------------------------------------------------------------
// ���һ��socket�ϰ󶨵�����GID
//------------------------------------------------------------------------------
bool CSocketRelativeData::GetGIDListBySocket(const connection_hdl hdl,
  list<DWORD> &lstGID)
{
  // ����
  lock_guard<recursive_mutex> alockSID(m_mutexSID);
  lock_guard<recursive_mutex> alockGID(m_mutexGID);

  // ����hdl��Ӧ������GID
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
  // ���������Ч��
  if (!pInfo || !pQueue) return false;

  // �ֲ�����
  shared_ptr<CTagPushData>  pData = nullptr;

  // ����
  lock_guard<recursive_mutex> alockSID(m_mutexSID);
  lock_guard<recursive_mutex> alockGID(m_mutexGID);

  // ����Socket List
  auto iter = m_mapGIDSkt.find(dwGID);
  if (iter == m_mapGIDSkt.end()) return false;
  shared_ptr<ListSocket> plst = iter->second;

  // ��ÿ��Socket���ƥ������
  for (auto iterSkt = plst->begin(); iterSkt != plst->end(); iterSkt++)
  {
    // ͨ��Socket�ҵ�SID
    auto iterSktSID = m_mapSktSID.find(*iterSkt);
    if (iterSktSID == m_mapSktSID.end()) continue;

    // ��������
    pData = make_shared<CTagPushData>(pInfo, iterSktSID->first, iterSktSID->second);
    // ��������ӵ�������
    if (!pQueue->PushInto(pData)) return false;
  }

  return true;
}

//------------------------------------------------------------------------------
bool CSocketRelativeData::GetEvtPushDataColl(const shared_ptr<KEventInfo> pInfo,
  shared_ptr<CEventPushDataQueue> pQueue)
{
  // ���������Ч��
  if (!pInfo || !pQueue) return false;

  // �ֲ�����
  shared_ptr<CEventPushData>  pData = nullptr;

  // ����
  lock_guard<recursive_mutex> alockSID(m_mutexSID);

  // ��ÿ��Socket���ƥ������
  for (auto iter = m_mapSktSID.begin(); iter != m_mapSktSID.end(); iter++)
  {
    // ��������
    pData = make_shared<CEventPushData>(pInfo, iter->first, iter->second);
    // ��������ӵ�������
    if (!pQueue->PushInto(pData)) return false;
  }

  return true;
}

//------------------------------------------------------------------------------
bool CSocketRelativeData::GetAlmPushDataColl(const shared_ptr<KAlarmInfo> pInfo,
  shared_ptr<CAlarmPushDataQueue> pQueue)
{
  // ���������Ч��
  if (!pInfo || !pQueue) return false;

  // �ֲ�����
  shared_ptr<CAlarmPushData>  pData = nullptr;

  // ����
  lock_guard<recursive_mutex> alockSID(m_mutexSID);

  // ��ÿ��Socket���ƥ������
  for (auto iter = m_mapSktSID.begin(); iter != m_mapSktSID.end(); iter++)
  {
    // ��������
    pData = make_shared<CAlarmPushData>(pInfo, iter->first, iter->second);
    // ��������ӵ�������
    if (!pQueue->PushInto(pData)) return false;
  }

  return true;
}

//------------------------------------------------------------------------------
void CSocketRelativeData::Delete(const connection_hdl hdl)
{
  // ���������Ч��
  if (!(hdl.lock())) return;

  // ɾ����Socket��ص�SID����
  OnDelSktSID(hdl);
  // ɾ����Socket��ص�GID����
  OnDelSktGID(hdl);
}
