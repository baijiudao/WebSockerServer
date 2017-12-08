//------------------------------------------------------------------------------
// �������ڣ�2017-07-14 13:39:26
// �ļ�������һЩʵʱ���ݶ���
//------------------------------------------------------------------------------
#ifndef _PUSH_DATA_CACHE_H_
#define _PUSH_DATA_CACHE_H_

#include "Define.h"
#include <mutex>

//------------------------------------------------------------------------------
// �������ڣ�2017-08-11 09:37:19 
// �� �� ������Ҫ���͵�Tag����
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
// �������ڣ�2017-08-11 09:40:15 
// �� �� ������Ҫ���͵�Event����
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
// �������ڣ�2017-08-11 09:37:30 
// �� �� ������Ҫ���͵�Alm����
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
// �������ڣ�2017-08-11 09:38:21 
// �� �� ������Ҫ���͵�Tag���ݶ���
//------------------------------------------------------------------------------
class CTagPushDataQueue
{
private:
  recursive_mutex                 m_mutexData;    // �����ݶ���ʹ�õ���
  queue<shared_ptr<CTagPushData>> m_queData;      // ���ݶ�����
public:
  CTagPushDataQueue();
  ~CTagPushDataQueue();
  bool  PushInto(const shared_ptr<CTagPushData> pData);   // ����
  shared_ptr<CTagPushData>  PopUp();                      // ȡ��
};


//------------------------------------------------------------------------------
// �������ڣ�2017-08-11 09:49:55 
// �� �� ������Ҫ���͵�Event���ݶ���
//------------------------------------------------------------------------------
class CEventPushDataQueue
{
private:
  recursive_mutex                 m_mutexData;    // �����ݶ���ʹ�õ���
  queue<shared_ptr<CEventPushData>> m_queData;      // ���ݶ�����
public:
  CEventPushDataQueue();
  ~CEventPushDataQueue();
  bool  PushInto(const shared_ptr<CEventPushData> pData);   // ����
  shared_ptr<CEventPushData>  PopUp();                      // ȡ��
};


//------------------------------------------------------------------------------
// �������ڣ�2017-08-11 09:52:36 
// �� �� ������Ҫ���͵�Alarm���ݶ���
//------------------------------------------------------------------------------
class CAlarmPushDataQueue
{
private:
  recursive_mutex                 m_mutexData;    // �����ݶ���ʹ�õ���
  queue<shared_ptr<CAlarmPushData>> m_queData;      // ���ݶ�����
public:
  CAlarmPushDataQueue();
  ~CAlarmPushDataQueue();
  bool  PushInto(const shared_ptr<CAlarmPushData> pData);   // ����
  shared_ptr<CAlarmPushData>  PopUp();                      // ȡ��
};


//------------------------------------------------------------------------------
// �������ڣ�2017-08-14 10:10:27 
// �� �� �����������������������������ݵ��ࣨ������ÿ�ֵ����ݵĶ��У�
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
