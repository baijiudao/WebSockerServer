//------------------------------------------------------------------------------
// 创建日期：2017-07-18 15:33:08
// 文件描述：用来推送数据的类，里面有一个线程，专门用来推送数据
//------------------------------------------------------------------------------
#ifndef _PUSH_THREAD_H_
#define _PUSH_THREAD_H_

#include <thread>
#include <memory>
#include "Define.h"
#include "DataCache.h"
#include "SessionManager.h"
#include "Logger.h"

//------------------------------------------------------------------------------
// 创建日期：2017-08-14 09:17:58 
// 类 描 述：变量数据推送线程
//------------------------------------------------------------------------------
class CTagPushThread
{
private:
  std::atomic_bool  m_bRuning;  // 线程是否运行
  shared_ptr<std::thread> m_pthThread;
  shared_ptr<CTagDataMap> m_pMap;
  shared_ptr<CTagPushDataQueue> m_pPushQueue;
  shared_ptr<CSocketRelativeData> m_pSocketData;
  server &  m_wsserver;
protected:
  void  OnThreadPrc();
  void  OnPushData(const connection_hdl hdl);
public:
  CTagPushThread(shared_ptr<CTagDataMap> pMap, shared_ptr<CSocketRelativeData> pSocketData, server &wsserver);
  ~CTagPushThread();
};


//------------------------------------------------------------------------------
// 创建日期：2017-08-14 10:37:02 
// 类 描 述：事件数据推送线程
//------------------------------------------------------------------------------
class CEventPushThread
{
private:
  std::atomic_bool  m_bRuning;  // 线程是否运行
  shared_ptr<std::thread> m_pthThread;
  shared_ptr<CEventDataQueue> m_pQueue;
  shared_ptr<CEventPushDataQueue> m_pPushQueue;
  shared_ptr<CSocketRelativeData> m_pSocketData;
  server &  m_wsserver;
protected:
  void  OnThreadPrc();
  void  OnPushData(list<shared_ptr<KEventInfo>> &lstInfo);
public:
  CEventPushThread(shared_ptr<CEventDataQueue> pQueue, shared_ptr<CSocketRelativeData> pSocketData, server &wsserver);
  ~CEventPushThread();
};

//------------------------------------------------------------------------------
// 创建日期：2017-08-14 09:18:20 
// 类 描 述：告警数据推送线程
//------------------------------------------------------------------------------
class CAlarmPushThread
{
private:
  std::atomic_bool  m_bRuning;  // 线程是否运行
  shared_ptr<std::thread> m_pthThread;
  shared_ptr<CAlarmDataQueue> m_pQueue;
  shared_ptr<CAlarmPushDataQueue> m_pPushQueue;
  shared_ptr<CSocketRelativeData> m_pSocketData;
  server &  m_wsserver;
protected:
  void  OnThreadPrc();
  void  OnPushData(list<shared_ptr<KAlarmInfo>> &lstInfo);
public:
  CAlarmPushThread(shared_ptr<CAlarmDataQueue> pQueue, shared_ptr<CSocketRelativeData> pSocketData, server &wsserver);
  ~CAlarmPushThread();
};


//------------------------------------------------------------------------------
// 创建日期：2017-08-14 10:31:47 
// 类 描 述：推送线程（推送所有需要推送的数据）
//------------------------------------------------------------------------------
class CPushThread
{
private:
  shared_ptr<CTagPushThread>    m_pTagThread;
  shared_ptr<CEventPushThread>  m_pEvtThread;
  shared_ptr<CAlarmPushThread>  m_pAlmThread;
public:
  CPushThread(shared_ptr<CDataCache> pCache, shared_ptr<CSocketRelativeData> pSocketData, server &wsserver);
  ~CPushThread();
};

#endif // !_PUSH_THREAD_H_
