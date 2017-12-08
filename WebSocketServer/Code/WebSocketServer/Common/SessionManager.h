//------------------------------------------------------------------------------
// �������ڣ�2017-06-19 15:50:51
// �ļ���������������Session����
//------------------------------------------------------------------------------
#ifndef _SESSION_MANAGER_H_
#define _SESSION_MANAGER_H_

#include <winsock.h>
#include <sstream>
#include <memory>
#include "Session.h"

typedef map<string, shared_ptr<CSessionData>>  MapSession; // <SID, Session>

//------------------------------------------------------------------------------
// �������ڣ�2017-08-10 09:20:29 
// �� �� ����Session������
//          ֻ����SESSION��SID֮��Ĺ�ϵ
//------------------------------------------------------------------------------
class CSessionManager
{
private:
  recursive_mutex m_mutexRc;    // ʹ��mapʱ����Դ��
  MapSession    m_mapSession; // ��������ÿ���û���Session��Ϣ
protected:
  string  OnCreSID();
public:
  CSessionManager();
  ~CSessionManager();
  shared_ptr<CSessionData> CreSession();
  shared_ptr<CSessionData> GetSession(const string & strSID);
  void  DelSession(const string & strSID);
};


//------------------------------------------------------------------------------
// �������ڣ�2017-08-15 09:14:25 
// �� �� ����ʹ������Ϣ����ģ��
//------------------------------------------------------------------------------
class CAuthorManager
{
public:
  CAuthorManager();
  ~CAuthorManager();
};

#endif
