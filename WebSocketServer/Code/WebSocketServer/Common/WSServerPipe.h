//------------------------------------------------------------------------------
// �������ڣ�2017-10-17 17:32:03
// �ļ�������WSServer��RDBS֮�佨����ͨ��
// �˳���������Ҫʹ�õĶ�������Ϊrdbs��Ҫwsserver�����˳���rdbs��������������rdbs��bug��
//------------------------------------------------------------------------------
#ifndef _WSSERVER_PIPE_H_
#define _WSSERVER_PIPE_H_

#include <Singleton.h>

//------------------------------------------------------------------------------
// �������ڣ�2017-10-17 17:32:25 
// �� �� ������RDBS����֮ǰ���������е�DCOM���ӣ�����RDBS����
//          �����������������RDBS�����޷��˳���BUG��
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
