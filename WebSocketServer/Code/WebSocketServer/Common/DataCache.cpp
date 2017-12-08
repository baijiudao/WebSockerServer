#include "stdafx.h"
#include <memory>
#include <assert.h>
#include "DataCache.h"
#include "UnitConvert.h"

// ������Чʱ�䣨�������ʱ���ж�Ϊ��Ч��
const uint32_t  TAG_TIME_OUT_M = 10;

// -- CTagDataMap --

//------------------------------------------------------------------------------
CTagDataMap::CTagDataMap()
{
}

//------------------------------------------------------------------------------
CTagDataMap::~CTagDataMap()
{
}

//------------------------------------------------------------------------------
// ��tag���ݲ������
//------------------------------------------------------------------------------
bool CTagDataMap::Insert(shared_ptr<KTagInfo> ptInfo)
{
  if (!ptInfo) return false;
  
  lock_guard<recursive_mutex> alock(m_mutex);
  // �����Ƿ����������������ھ��滻
  auto iter = m_Map.find(ptInfo->ulGID);
  if (iter == m_Map.end())
    m_Map.insert(make_pair(ptInfo->ulGID, ptInfo));
  else
    iter->second = ptInfo;
  return true;
}

//------------------------------------------------------------------------------
// ��ȡ����ֵ
//------------------------------------------------------------------------------
bool CTagDataMap::Search(const list<DWORD> &lstGID,
  list<shared_ptr<KTagInfo>> &lstInfo)
{
  lock_guard<recursive_mutex> alock(m_mutex);
  time_t  curTime = time(nullptr);
  shared_ptr<KTagInfo> ptInfo = nullptr;

  for (auto &item : lstGID)
  {
    ptInfo = make_shared<KTagInfo>();
    // ���GID
    ptInfo->ulGID = item;

    // ��ӵ�list��
    lstInfo.push_back(ptInfo);

    auto iter = m_Map.find(item);
    if (iter != m_Map.end())
    {
      // �ж��Ƿ�ʱ
      if ((curTime - (iter->second->ullTimeMs / 1000)) <= TAG_TIME_OUT_M)
      {
        // û�г�ʱ
        ptInfo->anyValue = iter->second->anyValue;
        ptInfo->sQuality = iter->second->sQuality;
        ptInfo->ullTimeMs = iter->second->ullTimeMs;
      }
    }
  }

  return true;
}

//------------------------------------------------------------------------------
// �������ʵ�ʻ�ȡ��OPC״̬����׼ȷ����Ϊ�޷�������OPCͨ�ţ����ֻ���������ַ�ʽ
//------------------------------------------------------------------------------
bool CTagDataMap::CheckOPCStatus(list<shared_ptr<KOPCStatusInfo>> &lstInfo)
{
  unordered_map<uint8_t, bool>  mapOPCStaus;

  // ���������Ϊ�����Զ�����map���������ͷ�
  {
    uint8_t ucID = 0;
    bool  bVaild = false;

    // ��������ȡ��ǰʱ��
    lock_guard<recursive_mutex> alock(m_mutex);
    time_t  curTime = time(nullptr);

    // ��ȡÿ��tag����ϵͳID��״̬
    for (auto &item : m_Map)
    {
      ucID = (item.first >> 24) & 0x0FF;
      bVaild = ((curTime - (item.second->ullTimeMs / 1000)) <= TAG_TIME_OUT_M);

      // �ж��Ƿ��Ѿ��������ϵͳID�ˣ�����оͽ�vaild�ȽϺ�Ž�ȥ
      auto iter = mapOPCStaus.find(ucID);
      if (iter == mapOPCStaus.end())
        mapOPCStaus.insert(make_pair(ucID, bVaild));
      else
        iter->second = (iter->second && bVaild);
    }
  }

  // �����ݷ��뵽opc�б���
  shared_ptr<KOPCStatusInfo>  ptInfo = nullptr;

  for (auto &item : mapOPCStaus)
  {
    ptInfo = make_shared<KOPCStatusInfo>();
    ptInfo->ulID = item.first;
    ptInfo->eStatus = (true == item.second) ? EOPCStatus::OK : EOPCStatus::UNCONNECT;
    ptInfo->strName = "opc_" + to_string(item.first);
    lstInfo.push_back(ptInfo);
  }

  return true;
}

// -- CEventDataQueue --

//------------------------------------------------------------------------------
CEventDataQueue::CEventDataQueue()
{
}

//------------------------------------------------------------------------------
CEventDataQueue::~CEventDataQueue()
{
}

//------------------------------------------------------------------------------
bool CEventDataQueue::PushInto(shared_ptr<KEventInfo> ptInfo)
{
  // ���������Ч��
  if (!ptInfo) return false;

  lock_guard<recursive_mutex> alock(m_mutex);
  m_Queue.push(ptInfo);
  return true;
}

//------------------------------------------------------------------------------
shared_ptr<KEventInfo> CEventDataQueue::PopUp()
{
  lock_guard<recursive_mutex> alock(m_mutex);
  if (m_Queue.empty()) return nullptr;

  shared_ptr<KEventInfo>  pInfo = m_Queue.front();
  m_Queue.pop();
  return pInfo;
}

//------------------------------------------------------------------------------
bool CEventDataQueue::Empty()
{
  return m_Queue.empty();
}


// -- CAlarmDataQueue --

//------------------------------------------------------------------------------
CAlarmDataQueue::CAlarmDataQueue()
{
}

//------------------------------------------------------------------------------
CAlarmDataQueue::~CAlarmDataQueue()
{
}

//------------------------------------------------------------------------------
bool CAlarmDataQueue::PushInto(shared_ptr<KAlarmInfo> ptInfo)
{
  // ���������Ч��
  if (!ptInfo) return false;

  lock_guard<recursive_mutex> alock(m_mutex);
  m_Queue.push(ptInfo);
  return true;
}

//------------------------------------------------------------------------------
shared_ptr<KAlarmInfo> CAlarmDataQueue::PopUp()
{
  lock_guard<recursive_mutex> alock(m_mutex);
  if (m_Queue.empty()) return nullptr;

  shared_ptr<KAlarmInfo>  pInfo = m_Queue.front();
  m_Queue.pop();
  return pInfo;
}

//------------------------------------------------------------------------------
bool CAlarmDataQueue::Empty()
{
  return m_Queue.empty();
}


// -- CDataCache --

//------------------------------------------------------------------------------
CDataCache::CDataCache()
{
  m_pTagMap = make_shared<CTagDataMap>();
  m_pEvtQueue = make_shared<CEventDataQueue>();
  m_pAlmQueue = make_shared<CAlarmDataQueue>();
}

//------------------------------------------------------------------------------
CDataCache::~CDataCache()
{
}

//------------------------------------------------------------------------------
shared_ptr<CTagDataMap> CDataCache::GetTagMap()
{
  return m_pTagMap;
}

//------------------------------------------------------------------------------
shared_ptr<CEventDataQueue> CDataCache::GetEvtQueue()
{
  return m_pEvtQueue;
}

//------------------------------------------------------------------------------
shared_ptr<CAlarmDataQueue> CDataCache::GetAlmQueue()
{
  return m_pAlmQueue;
}

