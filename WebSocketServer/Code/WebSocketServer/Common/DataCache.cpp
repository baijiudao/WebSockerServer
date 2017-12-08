#include "stdafx.h"
#include <memory>
#include <assert.h>
#include "DataCache.h"
#include "UnitConvert.h"

// 变量无效时间（超过这个时间判定为无效）
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
// 将tag数据插入表中
//------------------------------------------------------------------------------
bool CTagDataMap::Insert(shared_ptr<KTagInfo> ptInfo)
{
  if (!ptInfo) return false;
  
  lock_guard<recursive_mutex> alock(m_mutex);
  // 查找是否存在这个变量，存在就替换
  auto iter = m_Map.find(ptInfo->ulGID);
  if (iter == m_Map.end())
    m_Map.insert(make_pair(ptInfo->ulGID, ptInfo));
  else
    iter->second = ptInfo;
  return true;
}

//------------------------------------------------------------------------------
// 获取变量值
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
    // 添加GID
    ptInfo->ulGID = item;

    // 添加到list中
    lstInfo.push_back(ptInfo);

    auto iter = m_Map.find(item);
    if (iter != m_Map.end())
    {
      // 判断是否超时
      if ((curTime - (iter->second->ullTimeMs / 1000)) <= TAG_TIME_OUT_M)
      {
        // 没有超时
        ptInfo->anyValue = iter->second->anyValue;
        ptInfo->sQuality = iter->second->sQuality;
        ptInfo->ullTimeMs = iter->second->ullTimeMs;
      }
    }
  }

  return true;
}

//------------------------------------------------------------------------------
// 这个函数实际获取的OPC状态并不准确，因为无法真正与OPC通信，因此只能先用这种方式
//------------------------------------------------------------------------------
bool CTagDataMap::CheckOPCStatus(list<shared_ptr<KOPCStatusInfo>> &lstInfo)
{
  unordered_map<uint8_t, bool>  mapOPCStaus;

  // 这个大括号为了让自动锁在map操作完后就释放
  {
    uint8_t ucID = 0;
    bool  bVaild = false;

    // 加锁并获取当前时间
    lock_guard<recursive_mutex> alock(m_mutex);
    time_t  curTime = time(nullptr);

    // 获取每个tag的子系统ID和状态
    for (auto &item : m_Map)
    {
      ucID = (item.first >> 24) & 0x0FF;
      bVaild = ((curTime - (item.second->ullTimeMs / 1000)) <= TAG_TIME_OUT_M);

      // 判断是否已经有这个子系统ID了，如果有就将vaild比较后放进去
      auto iter = mapOPCStaus.find(ucID);
      if (iter == mapOPCStaus.end())
        mapOPCStaus.insert(make_pair(ucID, bVaild));
      else
        iter->second = (iter->second && bVaild);
    }
  }

  // 将数据放入到opc列表中
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
  // 检查数据有效性
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
  // 检查数据有效性
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

