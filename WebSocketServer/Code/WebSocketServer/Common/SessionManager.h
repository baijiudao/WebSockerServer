//------------------------------------------------------------------------------
// 创建日期：2017-06-19 15:50:51
// 文件描述：用来管理Session的类
//------------------------------------------------------------------------------
#ifndef _SESSION_MANAGER_H_
#define _SESSION_MANAGER_H_

#include <winsock.h>
#include <sstream>
#include <memory>
#include "Session.h"

typedef map<string, shared_ptr<CSessionData>>  MapSession; // <SID, Session>

//------------------------------------------------------------------------------
// 创建日期：2017-08-10 09:20:29 
// 类 描 述：Session管理器
//          只保存SESSION和SID之间的关系
//------------------------------------------------------------------------------
class CSessionManager
{
private:
  recursive_mutex m_mutexRc;    // 使用map时的资源锁
  MapSession    m_mapSession; // 用来保存每个用户的Session信息
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
// 创建日期：2017-08-15 09:14:25 
// 类 描 述：使用者信息管理模块
//------------------------------------------------------------------------------
class CAuthorManager
{
public:
  CAuthorManager();
  ~CAuthorManager();
};

#endif
