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
  // �ж�������Ч��
  if (!pData) return false;

  // ����
  lock_guard<recursive_mutex> alock(m_mutexData);
  // �������
  m_queData.push(pData);
  return true;
}

//------------------------------------------------------------------------------
shared_ptr<CTagPushData> CTagPushDataQueue::PopUp()
{
  // ����
  lock_guard<recursive_mutex> alock(m_mutexData);

  // �ж��Ƿ�������
  if (m_queData.empty()) return nullptr;

  // ȡ������
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
  // �ж�������Ч��
  if (!pData) return false;

  // ����
  lock_guard<recursive_mutex> alock(m_mutexData);
  // �������
  m_queData.push(pData);
  return true;
}

//------------------------------------------------------------------------------
shared_ptr<CEventPushData> CEventPushDataQueue::PopUp()
{
  // ����
  lock_guard<recursive_mutex> alock(m_mutexData);

  // �ж��Ƿ�������
  if (m_queData.empty()) return nullptr;

  // ȡ������
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
  // �ж�������Ч��
  if (!pData) return false;

  // ����
  lock_guard<recursive_mutex> alock(m_mutexData);
  // �������
  m_queData.push(pData);
  return true;
}

//------------------------------------------------------------------------------
shared_ptr<CAlarmPushData> CAlarmPushDataQueue::PopUp()
{
  // ����
  lock_guard<recursive_mutex> alock(m_mutexData);

  // �ж��Ƿ�������
  if (m_queData.empty()) return nullptr;

  // ȡ������
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
