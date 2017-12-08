//------------------------------------------------------------------------------
// 创建日期：2017-08-16 16:18:43
// 文件描述：WebSocket的回调函数
//------------------------------------------------------------------------------
#ifndef _WEB_SOCKET_HANDLER_H_
#define _WEB_SOCKET_HANDLER_H_

#include "Define.h"
#include "Parameter.h"
#include "PushThread.h"

//------------------------------------------------------------------------------
// 创建日期：2017-08-16 16:19:47 
// 类 描 述：
//------------------------------------------------------------------------------
class CWebSocketHandler
{
private:
  server m_server;
  shared_ptr<std::thread> m_pThread;
  shared_ptr<CParameter>  m_pParameter;
  shared_ptr<CPushThread> m_pPushThread;
protected:
  void OnThreadPrc(uint16_t sPort);
  void OnConnect(connection_hdl hdl);
  void OnDisconnect(connection_hdl hdl);
  void OnMessage(connection_hdl hdl, server::message_ptr msg);
  void OnError(connection_hdl hdl);
  void _OnConnect(connection_hdl hdl, shared_ptr<CParameter> pParameter);
  void _OnDisconnect(connection_hdl hdl, shared_ptr<CParameter> pParameter);
  void _OnMessage(connection_hdl hdl, server::message_ptr msg, shared_ptr<CParameter> pParameter);
public:
  CWebSocketHandler(shared_ptr<CParameter> pParameter);
  ~CWebSocketHandler();
  void Run(uint16_t sPort);
};

#endif // !_WEB_SOCKET_HANDLER_H_
