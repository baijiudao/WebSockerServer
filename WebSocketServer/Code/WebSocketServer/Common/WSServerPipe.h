//------------------------------------------------------------------------------
// 创建日期：2017-10-17 17:32:03
// 文件描述：WSServer与RDBS之间建立的通道
// 退出进程所需要使用的东西，因为rdbs需要wsserver正常退出，rdbs才能正常结束（rdbs的bug）
//------------------------------------------------------------------------------
#ifndef _WSSERVER_PIPE_H_
#define _WSSERVER_PIPE_H_

#include <Singleton.h>

//------------------------------------------------------------------------------
// 创建日期：2017-10-17 17:32:25 
// 类 描 述：在RDBS结束之前结束掉所有的DCOM连接，否则RDBS不能
//          主动结束掉连接造成RDBS进程无法退出的BUG；
//------------------------------------------------------------------------------
class CWSServerPipe : public ISingleton<CWSServerPipe>
{
private:
  HANDLE m_hPipe;
public:
  CWSServerPipe();
  ~CWSServerPipe();
  bool  WaitConnect();
  bool  SendNotification();
};

#endif // !_WSSERVER_PIPE_H_
