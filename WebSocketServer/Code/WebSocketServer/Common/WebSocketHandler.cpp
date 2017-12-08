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

  // 设置成功/失败日志输出等级（none为不输出，all为都输出）
  m_server.set_access_channels(websocketpp::log::alevel::none);
  m_server.set_error_channels(websocketpp::log::elevel::none);

  // 启动推送线程
  m_pPushThread = make_shared<CPushThread>(pParameter->GetDataCache(), pParameter->GetSocketData(), m_server);
}

//------------------------------------------------------------------------------
CWebSocketHandler::~CWebSocketHandler()
{
  // 停止WebSocket
  m_server.stop();
  if (m_pThread) m_pThread->join();
}

//------------------------------------------------------------------------------
void CWebSocketHandler::OnThreadPrc(uint16_t sPort)
{
  // listen on specified port
  // 这里必须使用v4（IPV4），否则XP系统无法使用

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

  LOG_DEBUG << "启动WSServer";
  // Start the ASIO io_service run loop
  try {
    m_server.run();
  }
  catch (const std::exception & e) {
    LOG_ERROR << e.what();
  }
}

//------------------------------------------------------------------------------
// WebSocket链接时的回调
//------------------------------------------------------------------------------
void CWebSocketHandler::OnConnect(connection_hdl hdl)
{
  LOG_DEBUG << "WebSocket Connect : " << m_server.get_con_from_hdl(hdl)->get_remote_endpoint();
  _OnConnect(hdl, m_pParameter);
}

//------------------------------------------------------------------------------
// WebSocket断开时的回调
//------------------------------------------------------------------------------
void CWebSocketHandler::OnDisconnect(connection_hdl hdl)
{
  LOG_DEBUG << "WebSocket Disconnect : " << m_server.get_con_from_hdl(hdl)->get_remote_endpoint();
  _OnDisconnect(hdl, m_pParameter);
}

//------------------------------------------------------------------------------
// WebSocket接收到消息时的回调
//------------------------------------------------------------------------------
void CWebSocketHandler::OnMessage(connection_hdl hdl, server::message_ptr msg)
{
  _OnMessage(hdl, msg, m_pParameter);
}

//------------------------------------------------------------------------------
// WebSocket出错时的回调
//------------------------------------------------------------------------------
void CWebSocketHandler::OnError(connection_hdl hdl)
{
  LOG_ERROR << "WebSocket Error：";
  std::error_code ec = m_server.get_con_from_hdl(hdl)->get_ec();
  LOG_ERROR << ec.value() << " " << ec.message();
}
// 
// //------------------------------------------------------------------------------
// // WebSocket出错时的回调（带外部传入的自定义参数）
// //------------------------------------------------------------------------------
// void CWebSocketHandler::_OnConnect(connection_hdl hdl,
//   shared_ptr<CParameter> pParameter)
// {
// 
// }
// 
// //------------------------------------------------------------------------------
// // WebSocket出错时的回调（带外部传入的自定义参数）
// //------------------------------------------------------------------------------
// void CWebSocketHandler::_OnDisconnect(connection_hdl hdl,
//   shared_ptr<CParameter> pParameter)
// {
// 
// }
// 
// //------------------------------------------------------------------------------
// // WebSocket出错时的回调（带外部传入的自定义参数）
// //------------------------------------------------------------------------------
// void CWebSocketHandler::_OnMessage(connection_hdl hdl,
//   server::message_ptr msg, shared_ptr<CParameter> pParameter)
// {
// 
// }

//------------------------------------------------------------------------------
// 运行WebSocket
//------------------------------------------------------------------------------
void CWebSocketHandler::Run(uint16_t sPort) {
  std::function<void(uint16_t)> thFunc = std::bind(&CWebSocketHandler::OnThreadPrc, this, std::placeholders::_1);
  m_pThread = make_shared<std::thread>(thFunc, sPort);
}
