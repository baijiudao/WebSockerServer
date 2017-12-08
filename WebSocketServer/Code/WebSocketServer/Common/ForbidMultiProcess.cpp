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
  // �رջ��������
  CloseHandle(m_hProcMutex);
}

//------------------------------------------------------------------------------
// ���������Ƿ�������������
//------------------------------------------------------------------------------
bool CForbidMultiProcess::AllowStart()
{
  // ��ӽ��̼以������Ϊ���ó���ֻ����һ��
  TCHAR szMutexName[] = _T("WebSocketServerMutex");

  m_hProcMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, szMutexName);
  if (m_hProcMutex)
  {
    CloseHandle(m_hProcMutex);
    m_hProcMutex = nullptr;
    LOG_WARN << "WebSocketServer �����ظ�����...";
    return false;
  }

  // ���û��������������û�д����û��������򴴽�һ��������
  m_hProcMutex = CreateMutex(NULL, FALSE, szMutexName);
  if (!m_hProcMutex)
  {
    LOG_WARN << "Create Mutex ʧ�ܣ� ErrorCode : " << GetLastError() << "...";
    return false;
  }

  return true;
}
