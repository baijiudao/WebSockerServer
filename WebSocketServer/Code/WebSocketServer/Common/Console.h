//------------------------------------------------------------------------------
// 创建日期: 2014-08-18 14:00:40
// 文件描述: 用来打开控制台窗口，以便后面的输出信息可以打印到控制台;
//            只需要包含该文件的头文件即可，无需定义对象
//------------------------------------------------------------------------------
#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include <stdio.h>

namespace Custom
{
  // 多线程：（√）
  // 多实例：（×）
  class CConsole
  {
  public:
    CConsole(void);
    ~CConsole(void);

    void OpenCnsl();
  };

  extern CConsole g_StartConsole;
}

#endif