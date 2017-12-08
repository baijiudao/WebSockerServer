#ifndef _DATA_CACHE_H_
#define _DATA_CACHE_H_

#include "Define.h"
#include <unordered_map>
#include <mutex>

using namespace std;

//------------------------------------------------------------------------------
// 创建日期：2017-09-25 10:06:16 
// 类 描 述：保存tag的表
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
  // 这个函数是用来监测OPC的状态的（暂时放在这里，后面会迁移位置）
  bool  CheckOPCStatus(list<shared_ptr<KOPCStatusInfo>> &lstInfo);
};

//------------------------------------------------------------------------------
// 创建日期：2017-08-11 16:37:29 
// 类 描 述：保存event的队列
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
// 创建日期：2017-08-11 16:37:43 
// 类 描 述：保存alarm的队列
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
// 创建日期：2017-08-14 10:22:42 
// 类 描 述：用来创建并保存所有缓存数据的类（里面有每种的数据的队列）
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

