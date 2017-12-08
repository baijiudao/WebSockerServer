#pragma once
#include <Windows.h>
#include <tchar.h>
#include <DbgHelp.h>
#pragma comment(lib, "dbghelp.lib")

#include <FileVersion/FileVersion.h>

#ifdef UNICODE
#define TSprintf	wsprintf
#else
#define TSprintf	sprintf
#endif

void DisableSetUnhandledExceptionFilter()
{
  void* addr = (void*)(GetProcAddress(LoadLibrary(_T("kernel32.dll")),
    "SetUnhandledExceptionFilter"));

  if (addr)
  {
    unsigned char code[16];
    int size = 0;

    code[size++] = 0x33;
    code[size++] = 0xC0;
    code[size++] = 0xC2;
    code[size++] = 0x04;
    code[size++] = 0x00;

    DWORD dwOldFlag, dwTempFlag;
    VirtualProtect(addr, size, PAGE_READWRITE, &dwOldFlag);
    WriteProcessMemory(GetCurrentProcess(), addr, code, size, NULL);
    VirtualProtect(addr, size, dwOldFlag, &dwTempFlag);
  }
}

void CreateDumpFile(LPCTSTR strPath, EXCEPTION_POINTERS *pException)
{
  // ����Dump�ļ�;
  HANDLE hDumpFile = CreateFile(strPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  // Dump��Ϣ;
  MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
  dumpInfo.ExceptionPointers = pException;
  dumpInfo.ThreadId = GetCurrentThreadId();
  dumpInfo.ClientPointers = TRUE;

  // д��Dump�ļ�����;
  MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);
  CloseHandle(hDumpFile);
}

LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException)
{
  TCHAR szDumpDir[MAX_PATH] = { 0 };
  TCHAR szDumpFile[MAX_PATH] = { 0 };
  TCHAR szMsg[MAX_PATH] = { 0 };
  SYSTEMTIME	stTime = { 0 };
  // ����dump�ļ�·��;
  GetLocalTime(&stTime);
  ::GetCurrentDirectory(MAX_PATH, szDumpDir);
  TSprintf(szDumpFile, _T("%s\\%s_%04d%02d%02d_%02d%02d%02d.dmp"), szDumpDir, GetSelfProductVersion().c_str(),
    stTime.wYear, stTime.wMonth, stTime.wDay,
    stTime.wHour, stTime.wMinute, stTime.wSecond);
  // ����dump�ļ�;
  CreateDumpFile(szDumpFile, pException);

  // ����һ������Ի��������ʾ�ϴ��� ���˳�����;
  TSprintf(szMsg, _T("Program Crashed.\r\ndump file : %s"), szDumpFile);
  FatalAppExit(-1, szMsg);

  return EXCEPTION_EXECUTE_HANDLER;
}

// �������ʱ�Ƿ������Զ�����dump�ļ�;
// ֻ��Ҫ��main������ʼ�����øú�������;
void RunCrashHandler()
{
  SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
  DisableSetUnhandledExceptionFilter();
}

