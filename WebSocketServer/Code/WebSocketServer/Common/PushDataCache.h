//------------------------------------------------------------------------------
// 创建日期：2017-07-14 13:39:26
// 文件描述：一些实时数据队列
//------------------------------------------------------------------------------
#ifndef _PUSH_DATA_CACHE_H_
#define _PUSH_DATA_CACHE_H_

#include "Define.h"
#include <mutex>

//------------------------------------------------------------------------------
// 创建日期：2017-08-11 09:37:19 
// 类 描 述：需要推送的Tag数据
//------------------------------------------------------------------------------
class CTagPushData
{
private:
  shared_ptr<KTagInfo>  m_ptInfo;
  connection_hdl        m_pSocket;
  string                m_strSID;
public:
  CTagPushData(shared_ptr<KTagInfo> ptInfo, connection_hdl pSocket, const string & strSID);
  ~CTagPushData();
  shared_ptr<KTagInfo>  GetInfo();
  connection_hdl        GetSocket();
  string                GetSID();
};

//------------------------------------------------------------------------------
// 创建日期：2017-08-11 09:40:15 
// 类 描 述：需要推送的Event数据
//------------------------------------------------------------------------------
class CEventPushData
{
private:
  shared_ptr<KEventInfo>  m_ptInfo;
  connection_hdl          m_pSocket;
  string                  m_strSID;
public:
  CEventPushData(shared_ptr<KEventInfo> ptInfo, connection_hdl pSocket, const string & strSID);
  ~CEventPushData();
  shared_ptr<KEventInfo>  GetInfo();
  connection_hdl          GetSocket();
  string                  GetSID();
};

//------------------------------------------------------------------------------
// 创建日期：2017-08-11 09:37:30 
// 类 描 述：需要推送的Alm数据
//------------------------------------------------------------------------------
class CAlarmPushData
{
private:
  shared_ptr<KAlarmInfo>  m_ptInfo;
  connection_hdl          m_pSocket;
  string                  m_strSID;
public:
  CAlarmPushData(shared_ptr<KAlarmInfo> ptInfo, connection_hdl pSocket, const string & strSID);
  ~CAlarmPushData();
  shared_ptr<KAlarmInfo>  GetInfo();
  connection_hdl          GetSocket();
  string                  GetSID();
};


//------------------------------------------------------------------------------
// 创建日期：2017-08-11 09:38:21 
// 类 描 述：需要推送的Tag数据队列
//------------------------------------------------------------------------------
class CTagPushDataQueue
{
private:
  recursive_mutex                 m_mutexData;    // 给数据队列使用的锁
  queue<shared_ptr<CTagPushData>> m_queData;      // 数据队列锁
public:
  CTagPushDataQueue();
  ~CTagPushDataQueue();
  bool  PushInto(const shared_ptr<CTagPushData> pData);   // 推入
  shared_ptr<CTagPushData>  PopUp();                      // 取出
};


//------------------------------------------------------------------------------
// 创建日期：2017-08-11 09:49:55 
// 类 描 述：需要推送的Event数据队列
//------------------------------------------------------------------------------
class CEventPushDataQueue
{
private:
  recursive_mutex                 m_mutexData;    // 给数据队列使用的锁
  queue<shared_ptr<CEventPushData>> m_queData;      // 数据队列锁
public:
  CEventPushDataQueue();
  ~CEventPushDataQueue();
  bool  PushInto(const shared_ptr<CEventPushData> pData);   // 推入
  shared_ptr<CEventPushData>  PopUp();                      // 取出
};


//------------------------------------------------------------------------------
// 创建日期：2017-08-11 09:52:36 
// 类 描 述：需要推送的Alarm数据队列
//------------------------------------------------------------------------------
class CAlarmPushDataQueue
{
private:
  recursive_mutex                 m_mutexData;    // 给数据队列使用的锁
  queue<shared_ptr<CAlarmPushData>> m_queData;      // 数据队列锁
public:
  CAlarmPushDataQueue();
  ~CAlarmPushDataQueue();
  bool  PushInto(const shared_ptr<CAlarmPushData> pData);   // 推入
  shared_ptr<CAlarmPushData>  PopUp();                      // 取出
};


//------------------------------------------------------------------------------
// 创建日期：2017-08-14 10:10:27 
// 类 描 述：用来创建并保存所有推送数据的类（里面有每种的数据的队列）
//------------------------------------------------------------------------------
class CPushDataCache
{
private:
  shared_ptr<CTagPushDataQueue> m_pTagQueue;
  shared_ptr<CEventPushDataQueue> m_pEvtQueue;
  shared_ptr<CAlarmPushDataQueue> m_pAlmQueue;
public:
  CPushDataCache();
  ~CPushDataCache();
  shared_ptr<CTagPushDataQueue> GetTagQueue();
  shared_ptr<CEventPushDataQueue> GetEvtQueue();
  shared_ptr<CAlarmPushDataQueue> GetAlmQueue();
};

#endif // !_PUSH_DATA_CACHE_H_
