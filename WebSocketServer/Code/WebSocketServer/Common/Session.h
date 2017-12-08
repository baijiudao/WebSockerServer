#ifndef _SESSION_H_
#define _SESSION_H_

#include <string>
#include <mutex>
#include <memory>
#include "../WebSocketServer/RDBSInterface.h"

using namespace std;

typedef shared_ptr<CComObject<CRDBSInterface>> RDBSInterface_ptr;
typedef map<connection_hdl, int32_t, std::owner_less<connection_hdl>> MapSocket;    // <Socket, SocketStatus>


//------------------------------------------------------------------------------
// 创建日期：2017-08-10 09:21:18 
// 类 描 述：Session信息
//          一个SESSION，保存了用户的一些信息
//          注：在使用公有成员变量时必须使用该类中的锁来保证资源互斥
//------------------------------------------------------------------------------
class CSessionData
{
private:
  string            m_strSID;     // SessionID
public:
  recursive_mutex   m_mutexRc;    // 当将对象调用到外部使用时需要使用这个锁来对资源加锁
  RDBSInterface_ptr m_pInterface; // 与RDBS通信的类
  MapSocket         m_mapSocket;  // 用来保存该用户已连接Socket的相关信息（其中SocketStatus为预留）
public:
  CSessionData(const string & strSID);
  ~CSessionData();
  string  GetSID();
};

typedef list<DWORD> ListGID;
typedef list<connection_hdl> ListSocket;
typedef map<connection_hdl, string, std::owner_less<connection_hdl>>  MapSktSID;  // <Socket, SID>
typedef map<connection_hdl, shared_ptr<ListGID>, std::owner_less<connection_hdl>> MapSktLstGID;  // <Socket, list<GID>>
typedef map<DWORD, shared_ptr<ListSocket>> MapGIDLstSkt;  // <GID, list<Socket>>

//------------------------------------------------------------------------------
// 创建日期：2017-08-10 09:21:59 
// 类 描 述：Socket与之相关的信息的保存与操作
//          1、Socket与SID数据的关联
//          2、Socket与GID数据的关联
//------------------------------------------------------------------------------
class CSocketRelativeData
{
private:
  recursive_mutex m_mutexSID;         // 读写SID相关数据加锁
  MapSktSID       m_mapSktSID;        // 用来保存所有Socket与SID之间的关系

  recursive_mutex m_mutexGID;         // 读写GID相关数据加锁
  MapSktLstGID    m_mapSktGID;        // Socket和GID的一对多关系
  MapGIDLstSkt    m_mapGIDSkt;        // GID和Socket的一对多关系
protected:
  bool  OnSetGIDSkt(const DWORD dwGID, const connection_hdl hdl);
  bool  OnSetSktGID(const connection_hdl hdl, const DWORD dwGID);
  void  OnDelSktSID(const connection_hdl hdl);
  void  OnDelSktGID(const connection_hdl hdl);
public:
  CSocketRelativeData();
  ~CSocketRelativeData();
  bool  SetSID(const connection_hdl hdl, const string & strSID);
  bool  GetSID(const connection_hdl hdl, string & strSID);
  bool  SetGID(const connection_hdl hdl, const DWORD dwGID);
  bool  GetSocketList(shared_ptr<list<connection_hdl>> plstHdl);
  bool  GetGIDListBySocket(const connection_hdl hdl, list<DWORD> &lstGID);
  bool  GetTagPushDataColl(const DWORD dwGID, const shared_ptr<KTagInfo> pInfo, shared_ptr<CTagPushDataQueue> pQueue);
  bool  GetEvtPushDataColl(const shared_ptr<KEventInfo> pInfo, shared_ptr<CEventPushDataQueue> pQueue);
  bool  GetAlmPushDataColl(const shared_ptr<KAlarmInfo> pInfo, shared_ptr<CAlarmPushDataQueue> pQueue);
  void  Delete(const connection_hdl hdl);
};

#endif


