#ifndef _DATA_CACHE_H_
#define _DATA_CACHE_H_

#include "Define.h"
#include <unordered_map>
#include <mutex>

using namespace std;

//------------------------------------------------------------------------------
// �������ڣ�2017-09-25 10:06:16 
// �� �� ��������tag�ı�
//------------------------------------------------------------------------------
class CTagDataMap
{
private:
  recursive_mutex m_mutex;
  std::unordered_map<DWORD, shared_ptr<KTagInfo>> m_Map;
public:
  CTagDataMap();
  ~CTagDataMap();
  bool  Insert(shared_ptr<KTagInfo> ptInfo);
  bool  Search(const list<DWORD> &lstGID, list<shared_ptr<KTagInfo>> &lstInfo);
  // ����������������OPC��״̬�ģ���ʱ������������Ǩ��λ�ã�
  bool  CheckOPCStatus(list<shared_ptr<KOPCStatusInfo>> &lstInfo);
};

//------------------------------------------------------------------------------
// �������ڣ�2017-08-11 16:37:29 
// �� �� ��������event�Ķ���
//------------------------------------------------------------------------------
class CEventDataQueue
{
private:
  recursive_mutex m_mutex;
  queue<shared_ptr<KEventInfo>> m_Queue;
public:
  CEventDataQueue();
  ~CEventDataQueue();
  bool  PushInto(shared_ptr<KEventInfo> ptInfo);
  shared_ptr<KEventInfo> PopUp();
  bool  Empty();
};

//------------------------------------------------------------------------------
// �������ڣ�2017-08-11 16:37:43 
// �� �� ��������alarm�Ķ���
//------------------------------------------------------------------------------
class CAlarmDataQueue
{
private:
  recursive_mutex m_mutex;
  queue<shared_ptr<KAlarmInfo>> m_Queue;
public:
  CAlarmDataQueue();
  ~CAlarmDataQueue();
  bool  PushInto(shared_ptr<KAlarmInfo> ptInfo);
  shared_ptr<KAlarmInfo> PopUp();
  bool  Empty();
};


//------------------------------------------------------------------------------
// �������ڣ�2017-08-14 10:22:42 
// �� �� ���������������������л������ݵ��ࣨ������ÿ�ֵ����ݵĶ��У�
//------------------------------------------------------------------------------
class CDataCache
{
protected:
  shared_ptr<CTagDataMap>   m_pTagMap;
  shared_ptr<CEventDataQueue> m_pEvtQueue;
  shared_ptr<CAlarmDataQueue> m_pAlmQueue;
public:
  CDataCache();
  ~CDataCache();
  shared_ptr<CTagDataMap>     GetTagMap();
  shared_ptr<CEventDataQueue> GetEvtQueue();
  shared_ptr<CAlarmDataQueue> GetAlmQueue();
};

#endif // !_DATA_CACHE_H_

