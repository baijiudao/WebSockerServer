//------------------------------------------------------------------------------
// �������ڣ�2017-10-19 15:59:43
// �ļ��������������ƽ��̣��ý����޷��࿪
//------------------------------------------------------------------------------
#ifndef _FORBID_MULTI_PROCESS_H_
#define _FORBID_MULTI_PROCESS_H_

class CForbidMultiProcess
{
private:
  HANDLE m_hProcMutex;
public:
  CForbidMultiProcess();
  ~CForbidMultiProcess();
  bool  AllowStart();
};

#endif // !_FORBID_MULTI_PROCESS_H_
