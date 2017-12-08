//------------------------------------------------------------------------------
// 创建日期: 2014-08-18 14:00:40
// 文件描述: 用来打开控制台窗口，以便后面的输出信息可以打印到控制台;
//            只需要包含该文件的头文件即可，无需定义对象
//------------------------------------------------------------------------------
#include "StdAfx.h"
#include "Console.h"

namespace Custom
{
  CConsole g_StartConsole;

  CConsole::CConsole(void)
  {
    AllocConsole(); // 打开控制台资源
    OpenCnsl();
  }


  CConsole::~CConsole(void)
  {
    FreeConsole();  // 释放控制台资源
  }

  void CConsole::OpenCnsl()
  {
    freopen( "CONOUT$", "w+t", stdout );// 申请写
    freopen( "CONIN$", "r+t", stdin ); // 申请读
  }
}
