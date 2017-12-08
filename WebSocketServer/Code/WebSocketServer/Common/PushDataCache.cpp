#include "stdafx.h"
#include "PushDataCache.h"


// -- CTagPushData --

//------------------------------------------------------------------------------
CTagPushData::CTagPushData(shared_ptr<KTagInfo> ptInfo,
  connection_hdl pSocket, const string & strSID)
{
  m_strSID = strSID;
  m_pSocket = pSocket;
  m_ptInfo = ptInfo;
}

//------------------------------------------------------------------------------
CTagPushData::~CTagPushData()
{
}

//------------------------------------------------------------------------------
shared_ptr<KTagInfo> CTagPushData::GetInfo()
{
  return m_ptInfo;
}

//------------------------------------------------------------------------------
connection_hdl CTagPushData::GetSocket()
{
  return m_pSocket;
}

//------------------------------------------------------------------------------
std::string CTagPushData::GetSID()
{
  return m_strSID;
}


// -- CEventPushData --

//------------------------------------------------------------------------------
CEventPushData::CEventPushData(shared_ptr<KEventInfo> ptInfo,
  connection_hdl pSocket, const string & strSID)
{
  m_strSID = strSID;
  m_pSocket = pSocket;
  m_ptInfo = ptInfo;
}

//------------------------------------------------------------------------------
CEventPushData::~CEventPushData()
{
}

//------------------------------------------------------------------------------
shared_ptr<KEventInfo> CEventPushData::GetInfo()
{
  return m_ptInfo;
}

//------------------------------------------------------------------------------
connection_hdl CEventPushData::GetSocket()
{
  return m_pSocket;
}

//------------------------------------------------------------------------------
std::string CEventPushData::GetSID()
{
  return m_strSID;
}


// -- CAlarmPushData --

//------------------------------------------------------------------------------
CAlarmPushData::CAlarmPushData(shared_ptr<KAlarmInfo> ptInfo,
  connection_hdl pSocket, const string & strSID)
{
  m_strSID = strSID;
  m_pSocket = pSocket;
  m_ptInfo = ptInfo;
}

CAlarmPushData::~CAlarmPushData()
{
}

//------------------------------------------------------------------------------
shared_ptr<KAlarmInfo> CAlarmPushData::GetInfo()
{
  return m_ptInfo;
}

//------------------------------------------------------------------------------
connection_hdl CAlarmPushData::GetSocket()
{
  return m_pSocket;
}

//------------------------------------------------------------------------------
std::string CAlarmPushData::GetSID()
{
  return m_strSID;
}


// -- CTagPushDataQueue --

//------------------------------------------------------------------------------
CTagPushDataQueue::CTagPushDataQueue()
{
}

//------------------------------------------------------------------------------
CTagPushDataQueue::~CTagPushDataQueue()
{
}

//------------------------------------------------------------------------------
bool CTagPushDataQueue::PushInto(const shared_ptr<CTagPushData> pData)
{
  // 判断数据有效性
  if (!pData) return false;

  // 加锁
  lock_guard<recursive_mutex> alock(m_mutexData);
  // 添加数据
  m_queData.push(pData);
  return true;
}

//------------------------------------------------------------------------------
shared_ptr<CTagPushData> CTagPushDataQueue::PopUp()
{
  // 加锁
  lock_guard<recursive_mutex> alock(m_mutexData);

  // 判断是否还有数据
  if (m_queData.empty()) return nullptr;

  // 取出数据
  shared_ptr<CTagPushData>  pData = m_queData.front();
  m_queData.pop();
  return pData;
}


// -- CEventPushDataQueue --

//------------------------------------------------------------------------------
CEventPushDataQueue::CEventPushDataQueue()
{
}

//------------------------------------------------------------------------------
CEventPushDataQueue::~CEventPushDataQueue()
{
}

//------------------------------------------------------------------------------
bool CEventPushDataQueue::PushInto(const shared_ptr<CEventPushData> pData)
{
  // 判断数据有效性
  if (!pData) return false;

  // 加锁
  lock_guard<recursive_mutex> alock(m_mutexData);
  // 添加数据
  m_queData.push(pData);
  return true;
}

//------------------------------------------------------------------------------
shared_ptr<CEventPushData> CEventPushDataQueue::PopUp()
{
  // 加锁
  lock_guard<recursive_mutex> alock(m_mutexData);

  // 判断是否还有数据
  if (m_queData.empty()) return nullptr;

  // 取出数据
  shared_ptr<CEventPushData>  pData = m_queData.front();
  m_queData.pop();
  return pData;
}


// -- CAlarmPushDataQueue --

//------------------------------------------------------------------------------
CAlarmPushDataQueue::CAlarmPushDataQueue()
{
}

//------------------------------------------------------------------------------
CAlarmPushDataQueue::~CAlarmPushDataQueue()
{
}

//------------------------------------------------------------------------------
bool CAlarmPushDataQueue::PushInto(const shared_ptr<CAlarmPushData> pData)
{
  // 判断数据有效性
  if (!pData) return false;

  // 加锁
  lock_guard<recursive_mutex> alock(m_mutexData);
  // 添加数据
  m_queData.push(pData);
  return true;
}

//------------------------------------------------------------------------------
shared_ptr<CAlarmPushData> CAlarmPushDataQueue::PopUp()
{
  // 加锁
  lock_guard<recursive_mutex> alock(m_mutexData);

  // 判断是否还有数据
  if (m_queData.empty()) return nullptr;

  // 取出数据
  shared_ptr<CAlarmPushData>  pData = m_queData.front();
  m_queData.pop();
  return pData;
}


// -- CPushDataCache --

//------------------------------------------------------------------------------
CPushDataCache::CPushDataCache()
{
  m_pTagQueue = make_shared<CTagPushDataQueue>();
  m_pEvtQueue = make_shared<CEventPushDataQueue>();
  m_pAlmQueue = make_shared<CAlarmPushDataQueue>();
}

//------------------------------------------------------------------------------
CPushDataCache::~CPushDataCache()
{
}

//------------------------------------------------------------------------------
shared_ptr<CTagPushDataQueue> CPushDataCache::GetTagQueue()
{
  return m_pTagQueue;
}

//------------------------------------------------------------------------------
shared_ptr<CEventPushDataQueue> CPushDataCache::GetEvtQueue()
{
  return m_pEvtQueue;
}

//------------------------------------------------------------------------------
shared_ptr<CAlarmPushDataQueue> CPushDataCache::GetAlmQueue()
{
  return m_pAlmQueue;
}
