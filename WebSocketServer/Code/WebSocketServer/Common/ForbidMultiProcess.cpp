#include "stdafx.h"
#include "ForbidMultiProcess.h"
#include <string>
#include <Logger.h>

using namespace std;

//------------------------------------------------------------------------------
CForbidMultiProcess::CForbidMultiProcess()
{
  m_hProcMutex = nullptr;
}

//------------------------------------------------------------------------------
CForbidMultiProcess::~CForbidMultiProcess()
{
  // 关闭互斥锁句柄
  CloseHandle(m_hProcMutex);
}

//------------------------------------------------------------------------------
// 用来检验是否允许启动进程
//------------------------------------------------------------------------------
bool CForbidMultiProcess::AllowStart()
{
  // 添加进程间互斥锁，为了让程序只启动一次
  TCHAR szMutexName[] = _T("WebSocketServerMutex");

  m_hProcMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, szMutexName);
  if (m_hProcMutex)
  {
    CloseHandle(m_hProcMutex);
    m_hProcMutex = nullptr;
    LOG_WARN << "WebSocketServer 程序重复启动...";
    return false;
  }

  // 如果没有启动过程序（则没有创建该互斥锁）则创建一个互斥锁
  m_hProcMutex = CreateMutex(NULL, FALSE, szMutexName);
  if (!m_hProcMutex)
  {
    LOG_WARN << "Create Mutex 失败， ErrorCode : " << GetLastError() << "...";
    return false;
  }

  return true;
}
