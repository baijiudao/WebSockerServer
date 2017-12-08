// WebSocketServer.cpp : Implementation of WinMain


#include "stdafx.h"
#include "resource.h"
#include "WebSocketServer_i.h"
#include "xdlldata.h"

#include "WebSocketHandler.h"
#include "PushThread.h"
#include <atlstr.h>

#include <iostream>
#include <io.h>
#include <memory>

#include "Logger.h"
#include <CameraOperate.h>
#include "Config/WSServerConfig.h"
#include "Config/Config.h"
#include "WSServerPipe.h"
#include "BasicInformation.h"
#include "ForbidMultiProcess.h"

#include <boost/filesystem.hpp>

#ifdef _DEBUG
// debug模式弹出控制台方便调试
#include "Console.h"
#endif // _DEBUG

#define _USE_DUMP_    // 是否使用生成dump的模块
//#define _MEMORY_LEAK_ // 用来注释内存泄漏模块

#ifdef _USE_DUMP_
#include "./Dump/MiniDump.h"
#endif // _USE_DUMP_

using namespace ATL;


class CWebSocketServerModule : public ATL::CAtlExeModuleT< CWebSocketServerModule >
{
public :
	DECLARE_LIBID(LIBID_WebSocketServerLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_WEBSOCKETSERVER, "{3F4E13AD-1324-4C2C-B8E5-D240A9721486}")
	};

CWebSocketServerModule _AtlModule;


#ifdef _MEMORY_LEAK_
//可以定位到发生内存泄露 所在的文件和具体那一行，用于检测 malloc 分配的内存
#define _CRTDBG_MAP_ALLOC 
#include <stdlib.h>
#include <crtdbg.h>

//把分配内存的信息保存下来，可以定位到那一行发生了内存泄露。用于检测 new 分配的内存
#ifdef _DEBUG
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

//有用
inline void EnableMemLeakCheck()
{
  //该语句在程序退出时自动调用 _CrtDumpMemoryLeaks(),用于多个退出出口的情况.
  //如果只有一个退出位置，可以在程序退出之前调用 _CrtDumpMemoryLeaks()
  _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
}
#endif // _MEMORY_LEAK_

// 启动的主任务
bool OnStart()
{
  // 加这个大括号是为了让里面的变量的作用域就在这里面，让里面的数据更容易释放
  {
    // 获取一些基础信息
    shared_ptr<KBasicInfo>  ptBasicInfo = make_shared<KBasicInfo>();
    if (!CBasicInformation::Inst()->Generate(ptBasicInfo))
    {
      LOG_WARN << "获取基础信息失败...";
      return false;
    }

    // 版本信息
    LOG_INFO << "当前版本：" << ptBasicInfo->strProductVersion;
    // 主机名
    LOG_INFO << "HostName : " << ptBasicInfo->strHostName;

    CoInitializeEx(NULL, COINIT_MULTITHREADED);

    // 所有配置信息
    shared_ptr<KConf> ptConf = make_shared<KConf>();

    // 加载摄像头信息
    if (!CCameraConfig::Inst()->Load(ptBasicInfo->strCamConfPathName, ptConf->lstCamConf))
    {
      LOG_ERROR << "摄像头配置文件读取失败...";
      return false;
    }

    // 启动摄像头推流
    std::shared_ptr<CCameraOperate> pCamOpera = make_shared<CCameraOperate>();
    if (!pCamOpera->Start(ptConf->lstCamConf, ptBasicInfo->strNodePathName,
      ptBasicInfo->strJSmpegPathName, ptBasicInfo->strFFmpegPathName))
    {
      LOG_ERROR << "摄像头推流模块启动失败...";
      return false;
    }

    // 读取WSServer配置文件
    if (!CWSServerConfig::Inst()->Load(ptBasicInfo->strWSSConfPathName, ptConf->ptWSSConf))
    {
      LOG_ERROR << "加载配置文件WSServerConfig.json失败...";
      return false;
    }

    // 用来存储tag,event和alarm的需要推送的数据的队列
    shared_ptr<CPushDataCache>  pPushDataCache = make_shared<CPushDataCache>();
    // 存储tag，event和alarm的数据的队列
    shared_ptr<CDataCache>  pDataCache = make_shared<CDataCache>();
    // Session管理类
    shared_ptr<CSessionManager> pSManager = make_shared<CSessionManager>();
    // Socket与之相关的信息的操作
    shared_ptr<CSocketRelativeData> pSocketData = make_shared<CSocketRelativeData>();
    // 用来保存并优化推送数据的类
    shared_ptr<COptimize> pOptimize = make_shared<COptimize>();
    // 数据库操作管理类
    shared_ptr<CDatabaseManager>  pDBManager = make_shared<CDatabaseManager>();
    if (!pDBManager->Connect(ptConf->ptWSSConf->ptDBConf->strUserName,
      ptConf->ptWSSConf->ptDBConf->strPassword, ptConf->ptWSSConf->ptDBConf->strDSN))
    {
      LOG_WARN << "The database connection failed";
      return false;
    }

    // 传递的参数
    shared_ptr<CParameter>  pParameter = make_shared<CParameter>(pSManager,
      pSocketData,
      pPushDataCache,
      pDataCache,
      pOptimize,
      pDBManager,
      ptBasicInfo,
      ptConf);

    // 启动网络监听
    shared_ptr<CWebSocketHandler> pWSHandler = make_shared<CWebSocketHandler>(pParameter);
    pWSHandler->Run(SERVER_PORT);

    // 等待RDBS的通道的连接
    if (!CWSServerPipe::Inst()->WaitConnect())
    {
      LOG_ERROR << "PIPE WaitConnect Fail...";
      return false;
    }

    // 手动释放所有资源
    pWSHandler = nullptr;
    pParameter = nullptr;
    pDBManager = nullptr;
    pOptimize = nullptr;
    pSocketData = nullptr;
    pSManager = nullptr;
    pDataCache = nullptr;
    pPushDataCache = nullptr;
    pCamOpera = nullptr;
    ptConf = nullptr;
    ptBasicInfo = nullptr;
  }

  // 向RDBS发送通知，告诉RDBS，WSServer已经将所有DCOM连接断开
  if (!CWSServerPipe::Inst()->SendNotification())
  {
    LOG_ERROR << "PIPE SendNotification Fail...";
    return false;
  }

  return true;
}

//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
								LPTSTR /*lpCmdLine*/, int nShowCmd)
{
#ifdef _USE_DUMP_
  //设置异常处理回调函数
  RunCrashHandler();
#endif // _USE_DUMP_

#ifdef _MEMORY_LEAK_
  EnableMemLeakCheck();
  //_CrtSetBreakAlloc(19250);
#endif // _MEMORY_LEAK_

  // 添加日志初始化
  std::shared_ptr<CLogger>  pLogger = make_shared<CLogger>(logging::trivial::debug);

  // 检查进程是否允许启动
  shared_ptr<CForbidMultiProcess> pForbidMultiProc = make_shared<CForbidMultiProcess>();
  if (pForbidMultiProc->AllowStart())
  {
    try
    {
      if (!OnStart()) LOG_WARN << "程序启动失败...";
    }
    catch (std::exception &e)
    {
      LOG_ERROR << e.what();
    }
  }

  LOG_DEBUG << "3 秒后自动退出...";

  std::this_thread::sleep_for(std::chrono::milliseconds(3000));

  return 0;
  return _AtlModule.WinMain(nShowCmd);
}

