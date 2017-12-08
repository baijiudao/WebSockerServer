#include "stdafx.h"
#include "WebSocketHandler.h"
#include <iostream>

using namespace std;

// -- CWebSocketHandler --

//------------------------------------------------------------------------------
CWebSocketHandler::CWebSocketHandler(shared_ptr<CParameter> pParameter)
{
  assert(pParameter);
  m_pParameter = pParameter;

  // Initialize Asio Transport
  m_server.init_asio();

  // Register handler callbacks
  m_server.set_open_handler(bind(&CWebSocketHandler::OnConnect, this, ::_1));
  m_server.set_close_handler(bind(&CWebSocketHandler::OnDisconnect, this, ::_1));
  m_server.set_message_handler(bind(&CWebSocketHandler::OnMessage, this, ::_1, ::_2));
  m_server.set_fail_handler(bind(&CWebSocketHandler::OnError, this, ::_1));

  // ���óɹ�/ʧ����־����ȼ���noneΪ�������allΪ�������
  m_server.set_access_channels(websocketpp::log::alevel::none);
  m_server.set_error_channels(websocketpp::log::elevel::none);

  // ���������߳�
  m_pPushThread = make_shared<CPushThread>(pParameter->GetDataCache(), pParameter->GetSocketData(), m_server);
}

//------------------------------------------------------------------------------
CWebSocketHandler::~CWebSocketHandler()
{
  // ֹͣWebSocket
  m_server.stop();
  if (m_pThread) m_pThread->join();
}

//------------------------------------------------------------------------------
void CWebSocketHandler::OnThreadPrc(uint16_t sPort)
{
  // listen on specified port
  // �������ʹ��v4��IPV4��������XPϵͳ�޷�ʹ��

  try
  {
    m_server.listen(websocketpp::lib::asio::ip::tcp::v4(), sPort);

    // Start the server accept loop
    m_server.start_accept();
  }
  catch (const std::exception & e)
  {
    LOG_ERROR << e.what();
  }

  LOG_DEBUG << "����WSServer";
  // Start the ASIO io_service run loop
  try {
    m_server.run();
  }
  catch (const std::exception & e) {
    LOG_ERROR << e.what();
  }
}

//------------------------------------------------------------------------------
// WebSocket����ʱ�Ļص�
//------------------------------------------------------------------------------
void CWebSocketHandler::OnConnect(connection_hdl hdl)
{
  LOG_DEBUG << "WebSocket Connect : " << m_server.get_con_from_hdl(hdl)->get_remote_endpoint();
  _OnConnect(hdl, m_pParameter);
}

//------------------------------------------------------------------------------
// WebSocket�Ͽ�ʱ�Ļص�
//------------------------------------------------------------------------------
void CWebSocketHandler::OnDisconnect(connection_hdl hdl)
{
  LOG_DEBUG << "WebSocket Disconnect : " << m_server.get_con_from_hdl(hdl)->get_remote_endpoint();
  _OnDisconnect(hdl, m_pParameter);
}

//------------------------------------------------------------------------------
// WebSocket���յ���Ϣʱ�Ļص�
//------------------------------------------------------------------------------
void CWebSocketHandler::OnMessage(connection_hdl hdl, server::message_ptr msg)
{
  _OnMessage(hdl, msg, m_pParameter);
}

//------------------------------------------------------------------------------
// WebSocket����ʱ�Ļص�
//------------------------------------------------------------------------------
void CWebSocketHandler::OnError(connection_hdl hdl)
{
  LOG_ERROR << "WebSocket Error��";
  std::error_code ec = m_server.get_con_from_hdl(hdl)->get_ec();
  LOG_ERROR << ec.value() << " " << ec.message();
}
// 
// //------------------------------------------------------------------------------
// // WebSocket����ʱ�Ļص������ⲿ������Զ��������
// //------------------------------------------------------------------------------
// void CWebSocketHandler::_OnConnect(connection_hdl hdl,
//   shared_ptr<CParameter> pParameter)
// {
// 
// }
// 
// //------------------------------------------------------------------------------
// // WebSocket����ʱ�Ļص������ⲿ������Զ��������
// //------------------------------------------------------------------------------
// void CWebSocketHandler::_OnDisconnect(connection_hdl hdl,
//   shared_ptr<CParameter> pParameter)
// {
// 
// }
// 
// //------------------------------------------------------------------------------
// // WebSocket����ʱ�Ļص������ⲿ������Զ��������
// //------------------------------------------------------------------------------
// void CWebSocketHandler::_OnMessage(connection_hdl hdl,
//   server::message_ptr msg, shared_ptr<CParameter> pParameter)
// {
// 
// }

//------------------------------------------------------------------------------
// ����WebSocket
//------------------------------------------------------------------------------
void CWebSocketHandler::Run(uint16_t sPort) {
  std::function<void(uint16_t)> thFunc = std::bind(&CWebSocketHandler::OnThreadPrc, this, std::placeholders::_1);
  m_pThread = make_shared<std::thread>(thFunc, sPort);
}
