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
// debugģʽ��������̨�������
#include "Console.h"
#endif // _DEBUG

#define _USE_DUMP_    // �Ƿ�ʹ������dump��ģ��
//#define _MEMORY_LEAK_ // ����ע���ڴ�й©ģ��

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
//���Զ�λ�������ڴ�й¶ ���ڵ��ļ��;�����һ�У����ڼ�� malloc ������ڴ�
#define _CRTDBG_MAP_ALLOC 
#include <stdlib.h>
#include <crtdbg.h>

//�ѷ����ڴ����Ϣ�������������Զ�λ����һ�з������ڴ�й¶�����ڼ�� new ������ڴ�
#ifdef _DEBUG
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

//����
inline void EnableMemLeakCheck()
{
  //������ڳ����˳�ʱ�Զ����� _CrtDumpMemoryLeaks(),���ڶ���˳����ڵ����.
  //���ֻ��һ���˳�λ�ã������ڳ����˳�֮ǰ���� _CrtDumpMemoryLeaks()
  _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
}
#endif // _MEMORY_LEAK_

// ������������
bool OnStart()
{
  // �������������Ϊ��������ı�������������������棬����������ݸ������ͷ�
  {
    // ��ȡһЩ������Ϣ
    shared_ptr<KBasicInfo>  ptBasicInfo = make_shared<KBasicInfo>();
    if (!CBasicInformation::Inst()->Generate(ptBasicInfo))
    {
      LOG_WARN << "��ȡ������Ϣʧ��...";
      return false;
    }

    // �汾��Ϣ
    LOG_INFO << "��ǰ�汾��" << ptBasicInfo->strProductVersion;
    // ������
    LOG_INFO << "HostName : " << ptBasicInfo->strHostName;

    CoInitializeEx(NULL, COINIT_MULTITHREADED);

    // ����������Ϣ
    shared_ptr<KConf> ptConf = make_shared<KConf>();

    // ��������ͷ��Ϣ
    if (!CCameraConfig::Inst()->Load(ptBasicInfo->strCamConfPathName, ptConf->lstCamConf))
    {
      LOG_ERROR << "����ͷ�����ļ���ȡʧ��...";
      return false;
    }

    // ��������ͷ����
    std::shared_ptr<CCameraOperate> pCamOpera = make_shared<CCameraOperate>();
    if (!pCamOpera->Start(ptConf->lstCamConf, ptBasicInfo->strNodePathName,
      ptBasicInfo->strJSmpegPathName, ptBasicInfo->strFFmpegPathName))
    {
      LOG_ERROR << "����ͷ����ģ������ʧ��...";
      return false;
    }

    // ��ȡWSServer�����ļ�
    if (!CWSServerConfig::Inst()->Load(ptBasicInfo->strWSSConfPathName, ptConf->ptWSSConf))
    {
      LOG_ERROR << "���������ļ�WSServerConfig.jsonʧ��...";
      return false;
    }

    // �����洢tag,event��alarm����Ҫ���͵����ݵĶ���
    shared_ptr<CPushDataCache>  pPushDataCache = make_shared<CPushDataCache>();
    // �洢tag��event��alarm�����ݵĶ���
    shared_ptr<CDataCache>  pDataCache = make_shared<CDataCache>();
    // Session������
    shared_ptr<CSessionManager> pSManager = make_shared<CSessionManager>();
    // Socket��֮��ص���Ϣ�Ĳ���
    shared_ptr<CSocketRelativeData> pSocketData = make_shared<CSocketRelativeData>();
    // �������沢�Ż��������ݵ���
    shared_ptr<COptimize> pOptimize = make_shared<COptimize>();
    // ���ݿ����������
    shared_ptr<CDatabaseManager>  pDBManager = make_shared<CDatabaseManager>();
    if (!pDBManager->Connect(ptConf->ptWSSConf->ptDBConf->strUserName,
      ptConf->ptWSSConf->ptDBConf->strPassword, ptConf->ptWSSConf->ptDBConf->strDSN))
    {
      LOG_WARN << "The database connection failed";
      return false;
    }

    // ���ݵĲ���
    shared_ptr<CParameter>  pParameter = make_shared<CParameter>(pSManager,
      pSocketData,
      pPushDataCache,
      pDataCache,
      pOptimize,
      pDBManager,
      ptBasicInfo,
      ptConf);

    // �����������
    shared_ptr<CWebSocketHandler> pWSHandler = make_shared<CWebSocketHandler>(pParameter);
    pWSHandler->Run(SERVER_PORT);

    // �ȴ�RDBS��ͨ��������
    if (!CWSServerPipe::Inst()->WaitConnect())
    {
      LOG_ERROR << "PIPE WaitConnect Fail...";
      return false;
    }

    // �ֶ��ͷ�������Դ
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

  // ��RDBS����֪ͨ������RDBS��WSServer�Ѿ�������DCOM���ӶϿ�
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
  //�����쳣����ص�����
  RunCrashHandler();
#endif // _USE_DUMP_

#ifdef _MEMORY_LEAK_
  EnableMemLeakCheck();
  //_CrtSetBreakAlloc(19250);
#endif // _MEMORY_LEAK_

  // �����־��ʼ��
  std::shared_ptr<CLogger>  pLogger = make_shared<CLogger>(logging::trivial::debug);

  // �������Ƿ���������
  shared_ptr<CForbidMultiProcess> pForbidMultiProc = make_shared<CForbidMultiProcess>();
  if (pForbidMultiProc->AllowStart())
  {
    try
    {
      if (!OnStart()) LOG_WARN << "��������ʧ��...";
    }
    catch (std::exception &e)
    {
      LOG_ERROR << e.what();
    }
  }

  LOG_DEBUG << "3 ����Զ��˳�...";

  std::this_thread::sleep_for(std::chrono::milliseconds(3000));

  return 0;
  return _AtlModule.WinMain(nShowCmd);
}

