#include "stdafx.h"
#include "PushThread.h"
#include "NetPackage/NetPackage.h"
#include "UnitConvert.h"
#include <assert.h>
#include <functional>

// 推送间隔时间（微妙）
const   uint32_t  PUSH_INTERVAL_US = 1;
const   uint32_t  PUSH_TAG_INTERVAL_US = 1000 * 1000;


// -- CTagPushThread --

//------------------------------------------------------------------------------
CTagPushThread::CTagPushThread(shared_ptr<CTagDataMap> pMap,
  shared_ptr<CSocketRelativeData> pSocketData, server &wsserver)
  : m_pMap(pMap), m_pSocketData(pSocketData), m_wsserver(wsserver)
{
  assert(pMap);
  assert(pSocketData);

  m_bRuning = true;
  m_pPushQueue = make_shared<CTagPushDataQueue>();

  // 创建一个回调函数并启动线程
  std::function<void(void)> thFunc = std::bind(&CTagPushThread::OnThreadPrc, this);
  m_pthThread = make_shared<std::thread>(thFunc);
}

//------------------------------------------------------------------------------
CTagPushThread::~CTagPushThread()
{
  m_bRuning = false;
  m_pthThread->join();
}

//------------------------------------------------------------------------------
void CTagPushThread::OnThreadPrc()
{
  try
  {
    shared_ptr<list<connection_hdl>> plstHdl = make_shared<list<connection_hdl>>();

    for (; true == m_bRuning;)
    {
      std::this_thread::sleep_for(std::chrono::microseconds(PUSH_TAG_INTERVAL_US));

      plstHdl->clear();
      m_pSocketData->GetSocketList(plstHdl);

      for (auto &item : *plstHdl)
      {
        OnPushData(item);
      }
    }
  }
  catch (std::exception &e)
  {
    LOG_ERROR << e.what();
  }

}

//------------------------------------------------------------------------------
void CTagPushThread::OnPushData(const connection_hdl hdl)
{
  if (!hdl.lock()) return;

  list<shared_ptr<KTagInfo>>  lstInfo;
  string  strDst = "";
  DWORD dwGID = 0;
  string  strSID = "";

  list<DWORD> lstGID;
  // 获取这个socket绑定的GID
  if (!m_pSocketData->GetGIDListBySocket(hdl, lstGID)) return;

  LOG_TRACE << "Find Push Tag User...";
  // 获取变量值
  m_pMap->Search(lstGID, lstInfo);

  // 获取用户SID
  m_pSocketData->GetSID(hdl, strSID);

  LOG_TRACE << "Start Push Tag To User...";
  // 组包
  if (CNetPackage::Inst()->BuildPushTag(strSID, EErrCode::EC_SUCCESS, lstInfo, strDst))
  {
    try
    {
      m_wsserver.send(hdl, CUnitConvert::Inst()->AnsiToUTF8(strDst),
        websocketpp::frame::opcode::value::text); // 推送数据
      LOG_TRACE << strDst;
      LOG_TRACE << "Start Push Tag To User Success";
    }
    catch (std::exception &e)
    {
      LOG_ERROR << e.what();
    }
  }
  else
    LOG_TRACE << "Start Push Tag To User Fail";
}


// -- CEventPushThread --

//------------------------------------------------------------------------------
CEventPushThread::CEventPushThread(shared_ptr<CEventDataQueue> pQueue,
  shared_ptr<CSocketRelativeData> pSocketData, server &wsserver)
  : m_pQueue(pQueue), m_pSocketData(pSocketData), m_wsserver(wsserver)
{
  assert(pQueue);
  assert(pSocketData);

  m_bRuning = true;
  m_pPushQueue = make_shared<CEventPushDataQueue>();

  // 创建一个回调函数并启动线程
  std::function<void(void)> thFunc = std::bind(&CEventPushThread::OnThreadPrc, this);
  m_pthThread = make_shared<std::thread>(thFunc);
}

//------------------------------------------------------------------------------
CEventPushThread::~CEventPushThread()
{
  m_bRuning = false;
  m_pthThread->join();
}

//------------------------------------------------------------------------------
void CEventPushThread::OnThreadPrc()
{
  try
  {
    list<shared_ptr<KEventInfo>>  lstInfo;

    for (; true == m_bRuning;)
    {
      std::this_thread::sleep_for(std::chrono::microseconds(PUSH_INTERVAL_US));

      while (!m_pQueue->Empty())
      {
        lstInfo.clear();

        // 将最多100个数据放入vec中
        for (auto ptInfo = m_pQueue->PopUp(); ptInfo != nullptr; ptInfo = m_pQueue->PopUp())
        {
          lstInfo.push_back(ptInfo);
          if (lstInfo.size() > 100) break;
        }

        // 获取一个推送数据
        OnPushData(lstInfo);
      }
    }
  }
  catch (std::exception &e)
  {
    LOG_ERROR << e.what();
  }
}

//------------------------------------------------------------------------------
void CEventPushThread::OnPushData(list<shared_ptr<KEventInfo>> &lstInfo)
{
  if (lstInfo.empty()) return;
  string  strDst = "";
  string  strSID = "";

  shared_ptr<list<connection_hdl>>  plstHdl = make_shared<list<connection_hdl>>();
  m_pSocketData->GetSocketList(plstHdl);

  LOG_TRACE << "Start Push Event Msg To All User...";
  // 给每个Socket发送
  for (auto iter = plstHdl->begin(); iter != plstHdl->end(); iter++)
  {
    // 获取SID
    m_pSocketData->GetSID(*iter, strSID);
    // 组包
    if (CNetPackage::Inst()->BuildPushEvent(strSID, EErrCode::EC_SUCCESS, lstInfo, strDst))
    {
      try
      {
        LOG_TRACE << "Start Push Event Msg To User...";
        // 发送失败时会抛出异常
        m_wsserver.send(*iter, CUnitConvert::Inst()->AnsiToUTF8(strDst),
          websocketpp::frame::opcode::value::text); // 推送数据
        LOG_TRACE << strDst;
        LOG_TRACE << "Start Push Event Msg To User Success";
      }
      catch (std::exception &e)
      {
        LOG_ERROR << e.what();
        // 不对异常进行处理
        std::ignore = e;  // 不会出现unused的警告
      }
    }
  }
}


// -- CAlarmPushThread --

//------------------------------------------------------------------------------
CAlarmPushThread::CAlarmPushThread(shared_ptr<CAlarmDataQueue> pQueue,
  shared_ptr<CSocketRelativeData> pSocketData, server &wsserver)
  : m_pQueue(pQueue), m_pSocketData(pSocketData), m_wsserver(wsserver)
{
  assert(pQueue);
  assert(pSocketData);

  m_bRuning = true;
  m_pPushQueue = make_shared<CAlarmPushDataQueue>();

  // 创建一个回调函数并启动线程
  std::function<void(void)> thFunc = std::bind(&CAlarmPushThread::OnThreadPrc, this);
  m_pthThread = make_shared<std::thread>(thFunc);
}

//------------------------------------------------------------------------------
CAlarmPushThread::~CAlarmPushThread()
{
  m_bRuning = false;
  m_pthThread->join();
}

//------------------------------------------------------------------------------
void CAlarmPushThread::OnThreadPrc()
{
  try
  {
    list<shared_ptr<KAlarmInfo>>  lstInfo;

    for (; true == m_bRuning;)
    {
      std::this_thread::sleep_for(std::chrono::microseconds(PUSH_INTERVAL_US));

      while (!m_pQueue->Empty())
      {
        lstInfo.clear();

        // 将最多100个数据放入vec中
        for (auto ptInfo = m_pQueue->PopUp(); ptInfo != nullptr; ptInfo = m_pQueue->PopUp())
        {
          lstInfo.push_back(ptInfo);
          if (lstInfo.size() > 100) break;
        }

        // 获取一个推送数据
        OnPushData(lstInfo);
      }
    }
  }
  catch (std::exception &e)
  {
    LOG_ERROR << e.what();
  }

}

//------------------------------------------------------------------------------
void CAlarmPushThread::OnPushData(list<shared_ptr<KAlarmInfo>> &lstInfo)
{
  if (lstInfo.empty()) return;
  string  strDst = "";
  string  strSID = "";

  shared_ptr<list<connection_hdl>>  plstHdl = make_shared<list<connection_hdl>>();
  m_pSocketData->GetSocketList(plstHdl);

  LOG_TRACE << "Start Push Alarm Msg To All User...";
  // 给每个Socket发送
  for (auto iter = plstHdl->begin(); iter != plstHdl->end(); iter++)
  {
    // 获取SID
    m_pSocketData->GetSID(*iter, strSID);
    // 组包
    if (CNetPackage::Inst()->BuildPushAlarm(strSID, EErrCode::EC_SUCCESS, lstInfo, strDst))
    {
      try
      {
        LOG_TRACE << "Start Push Alarm Msg To User...";
        // 发送失败时会抛出异常
        m_wsserver.send(*iter, CUnitConvert::Inst()->AnsiToUTF8(strDst),
          websocketpp::frame::opcode::value::text); // 推送数据
        LOG_TRACE << strDst;
        LOG_TRACE << "Start Push Alarm Msg To User Success";
      }
      catch (std::exception &e)
      {
        LOG_ERROR << e.what();
        // 不对异常进行处理
        std::ignore = e;  // 不会出现unused的警告
      }
    }
  }
}


// -- CPushThread --

//------------------------------------------------------------------------------
CPushThread::CPushThread(shared_ptr<CDataCache> pCache,
  shared_ptr<CSocketRelativeData> pSocketData, server &wsserver)
{
  m_pTagThread = make_shared<CTagPushThread>(pCache->GetTagMap(), pSocketData, wsserver);
  m_pEvtThread = make_shared<CEventPushThread>(pCache->GetEvtQueue(), pSocketData, wsserver);
  m_pAlmThread = make_shared<CAlarmPushThread>(pCache->GetAlmQueue(), pSocketData, wsserver);
}

//------------------------------------------------------------------------------
CPushThread::~CPushThread()
{
}
