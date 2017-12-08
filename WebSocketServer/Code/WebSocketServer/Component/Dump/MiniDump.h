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
  // 创建Dump文件;
  HANDLE hDumpFile = CreateFile(strPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  // Dump信息;
  MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
  dumpInfo.ExceptionPointers = pException;
  dumpInfo.ThreadId = GetCurrentThreadId();
  dumpInfo.ClientPointers = TRUE;

  // 写入Dump文件内容;
  MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);
  CloseHandle(hDumpFile);
}

LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException)
{
  TCHAR szDumpDir[MAX_PATH] = { 0 };
  TCHAR szDumpFile[MAX_PATH] = { 0 };
  TCHAR szMsg[MAX_PATH] = { 0 };
  SYSTEMTIME	stTime = { 0 };
  // 构建dump文件路径;
  GetLocalTime(&stTime);
  ::GetCurrentDirectory(MAX_PATH, szDumpDir);
  TSprintf(szDumpFile, _T("%s\\%s_%04d%02d%02d_%02d%02d%02d.dmp"), szDumpDir, GetSelfProductVersion().c_str(),
    stTime.wYear, stTime.wMonth, stTime.wDay,
    stTime.wHour, stTime.wMinute, stTime.wSecond);
  // 创建dump文件;
  CreateDumpFile(szDumpFile, pException);

  // 弹出一个错误对话框或者提示上传， 并退出程序;
  TSprintf(szMsg, _T("Program Crashed.\r\ndump file : %s"), szDumpFile);
  FatalAppExit(-1, szMsg);

  return EXCEPTION_EXECUTE_HANDLER;
}

// 程序崩溃时是否启动自动生成dump文件;
// 只需要在main函数开始处调用该函数即可;
void RunCrashHandler()
{
  SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
  DisableSetUnhandledExceptionFilter();
}

