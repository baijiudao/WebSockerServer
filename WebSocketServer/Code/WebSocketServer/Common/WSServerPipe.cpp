#include "stdafx.h"
#include "WSServerPipe.h"

//------------------------------------------------------------------------------
CWSServerPipe::CWSServerPipe()
{
  LPTSTR csName = _T("\\\\.\\Pipe\\WSServer");
  m_hPipe = CreateNamedPipe(csName,
    PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE, 1, 0, 0, 1, NULL);
}

//------------------------------------------------------------------------------
CWSServerPipe::~CWSServerPipe()
{
  CloseHandle(m_hPipe);
}

//------------------------------------------------------------------------------
// 等待RDBS的命名通道的连接
//------------------------------------------------------------------------------
bool CWSServerPipe::WaitConnect()
{
  if (INVALID_HANDLE_VALUE == m_hPipe) return false;
  return (TRUE == ConnectNamedPipe(m_hPipe, NULL)) ? true : false;
}

//------------------------------------------------------------------------------
// 像RDBS的命令通道发送通知
//------------------------------------------------------------------------------
bool CWSServerPipe::SendNotification()
{
  char Message[1024] = "c";
  DWORD WriteNum; // 发送的是数据长度

  if (INVALID_HANDLE_VALUE == m_hPipe) return false;
  return (TRUE == WriteFile(m_hPipe, Message,
    strlen(Message), &WriteNum, NULL)) ? true : false;
}
