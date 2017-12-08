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
// �������ڣ�2017-08-10 09:21:18 
// �� �� ����Session��Ϣ
//          һ��SESSION���������û���һЩ��Ϣ
//          ע����ʹ�ù��г�Ա����ʱ����ʹ�ø����е�������֤��Դ����
//------------------------------------------------------------------------------
class CSessionData
{
private:
  string            m_strSID;     // SessionID
public:
  recursive_mutex   m_mutexRc;    // ����������õ��ⲿʹ��ʱ��Ҫʹ�������������Դ����
  RDBSInterface_ptr m_pInterface; // ��RDBSͨ�ŵ���
  MapSocket         m_mapSocket;  // ����������û�������Socket�������Ϣ������SocketStatusΪԤ����
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
// �������ڣ�2017-08-10 09:21:59 
// �� �� ����Socket��֮��ص���Ϣ�ı��������
//          1��Socket��SID���ݵĹ���
//          2��Socket��GID���ݵĹ���
//------------------------------------------------------------------------------
class CSocketRelativeData
{
private:
  recursive_mutex m_mutexSID;         // ��дSID������ݼ���
  MapSktSID       m_mapSktSID;        // ������������Socket��SID֮��Ĺ�ϵ

  recursive_mutex m_mutexGID;         // ��дGID������ݼ���
  MapSktLstGID    m_mapSktGID;        // Socket��GID��һ�Զ��ϵ
  MapGIDLstSkt    m_mapGIDSkt;        // GID��Socket��һ�Զ��ϵ
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


