// RDBSInterface.h : Declaration of the CRDBSInterface

#pragma once
#include "resource.h"       // main symbols



#include "WebSocketServer_i.h"
#include "_IRDBSInterfaceEvents_CP.h"

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "Define.h"
#include "DataCache.h"
#include "PushDataCache.h"
#include "Optimize.h"
#include "CommonUnit.h"
#include <DatabaseManager/DatabaseManager.h>

#include "../../../../RDBS/RDBS4.1/Rcx.h"

#include "Logger.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;
using namespace std;

class CSocketRelativeData;

// CRDBSInterface

//------------------------------------------------------------------------------
// 创建日期：2017-07-18 10:12:25 
// 类 描 述：多线程不安全
//------------------------------------------------------------------------------
class ATL_NO_VTABLE CRDBSInterface :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<CRDBSInterface, &CLSID_RDBSInterface>,
  public IConnectionPointContainerImpl<CRDBSInterface>,
  public CProxy_IRDBSInterfaceEvents<CRDBSInterface>,
  public IDispatchImpl<IRDBSInterface, &IID_IRDBSInterface, &LIBID_WebSocketServerLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
  public IDispatchImpl<IBMSCallBack, &IID_IBMSCallBack, &LIBID_WebSocketServerLib>,                //IDispatch回调接口
  public IDispatchImpl<IBMSShutdown, &IID_IBMSShutdown, &LIBID_WebSocketServerLib>,                //IDispatch回调接口
  public IDispatchImpl < IBMSConnectionVerify, &IID_IBMSConnectionVerify, &LIBID_WebSocketServerLib > //IDispatch回调接口
{
public:
  CRDBSInterface()
  {
  }

  DECLARE_REGISTRY_RESOURCEID(IDR_RDBSINTERFACE)


  BEGIN_COM_MAP(CRDBSInterface)
    COM_INTERFACE_ENTRY(IRDBSInterface)
    //COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY2(IDispatch, IRDBSInterface)   //支持多个IDispatch接口
    COM_INTERFACE_ENTRY(IBMSCallBack)         //IDispatch回调接口
    COM_INTERFACE_ENTRY(IBMSShutdown)         //IDispatch回调接口
    COM_INTERFACE_ENTRY(IBMSConnectionVerify) //IDispatch回调接口
    COM_INTERFACE_ENTRY(IConnectionPointContainer)
  END_COM_MAP()

  BEGIN_CONNECTION_POINT_MAP(CRDBSInterface)
    CONNECTION_POINT_ENTRY(__uuidof(_IRDBSInterfaceEvents))
  END_CONNECTION_POINT_MAP()


  DECLARE_PROTECT_FINAL_CONSTRUCT()

  HRESULT FinalConstruct();
  void FinalRelease();

private:
  shared_ptr<CDataCache>  m_pDataCache; // 要推送的实时数据缓存对象
  IDispatch*  m_pIBMSServerDispatch;  // BMSServer对象
  IDispatch*  m_pIBMSRcxDataDispatch; // BMSRcxData对象
  IDispatch*  m_pIBMSHacDispatch;     // BMSHac对象（历史数据服务）
  IDispatch*  m_pILinkageWatch;       // 联动监视框对象
  IDispatch*  m_pDispatchWatch;       // 时间调度监视框对象

  std::atomic_bool  m_bInit;
  std::atomic_bool  m_bLogin;

  random_type m_ullMark;  // 一个随机标志位（用来判断是否是被选中的用来推送的对象）
  shared_ptr<COptimize> m_pOptimize;  // 保存所有RDBSInterface标志位的类
protected:
  void  Test();
  EErrCode  OnLogin(const string &strAccount, const string &strPassword);
  EErrCode  OnLogout();
  EErrCode  OnServerInit(const string &strHostName);
  EErrCode  OnRcxDataInit();
  EErrCode  OnHacInit();
  EErrCode  OnLinkageWatchInit();
  EErrCode  OnTimetableDispatchWatchInit();
  EErrCode  OnServerUninit();
  EErrCode  OnRcxDataUninit();
  EErrCode  OnHacUninit();
  EErrCode  OnSetSinkPointer();
  EErrCode  OnGetNull();
  EErrCode  OnGetAllTag(map<DWORD, shared_ptr<KTagInfo>> &mapTag);
  EErrCode  OnGetFunctionAuthority(int32_t &lOnlineConfig, int32_t &lTaskSchedule, int32_t &lWarningEvent);
public:
  EErrCode  Init(const string &strHostName, shared_ptr<CDataCache> pDataCache, shared_ptr<COptimize> pOptimize);
  EErrCode  Uninit();
  EErrCode  Login(const string &strAccount, const string &strPassword, EUserType &eType);
  EErrCode  Logout();
  EErrCode  ReadTag(list<shared_ptr<KTagInfo>> &lstInfo);
  EErrCode  WriteTag(const list<shared_ptr<KTagInfo>> &lstInfo);
  EErrCode  WriteEvent(const list<shared_ptr<KEventInfo>> &lstInfo);
  EErrCode  ReadHistroyEvent(uint64_t ullStart, uint64_t ullEnd, std::list<shared_ptr<KEventInfo>> &lstInfo);
  EErrCode  ReadHistroyAlarm(uint64_t ullStart, uint64_t ullEnd, std::list<shared_ptr<KAlarmInfo>> &lstInfo);
  EErrCode  ReadLinkageWatchInfo(list<shared_ptr<KLinkageWatchInfo>> &lstInfo);
  EErrCode  LinkageManualConfirmation(uint32_t ulItemID);
  EErrCode  LinkageTerminateTask(uint32_t ulItemID);
  EErrCode  ReadTimetableDispatchWatchInfo(list<shared_ptr<KTimetableDispatchWatchInfo>> &lstInfo);
  EErrCode  TimetableDispatchTerminateTask(uint32_t ulItemID);

  // 暂时用来监测RDBS相关数据库的状态
  bool  CheckDBStatus(std::list<std::shared_ptr<KDBStatusInfo>> &lstInfo);

  //IBMSCallback 的接口函数
  STDMETHOD(OnDataChange)(
    /*[in]*/ LONG NumTags,
    /*[in]*/ SAFEARRAY* pTagIDs,
    /*[in]*/ SAFEARRAY* pTimes,
    /*[in]*/ SAFEARRAY* pValues
    );

  STDMETHOD(OnEventsChange)(
    /*[in]*/ LONG EvtCount,
    /*[in]*/ SAFEARRAY* pEvtTypes,
    /*[in]*/ SAFEARRAY* pEvtIDs,
    /*[in]*/ SAFEARRAY* pUserNames,
    /*[in]*/ SAFEARRAY* pAccessLevels,
    /*[in]*/ SAFEARRAY* pEvtTimes,
    /*[in]*/ SAFEARRAY* pEvtSrc
    );

  STDMETHOD(OnAlarmsChange)(
    /*[in]*/ LONG AlmCount,
    /*[in]*/ SAFEARRAY* pAlmTypes,
    /*[in]*/ SAFEARRAY* pAlmIDs,
    /*[in]*/ SAFEARRAY* pTagIDs,
    /*[in]*/ SAFEARRAY* pTagName,
    /*[in]*/ SAFEARRAY* pTagValues,
    /*[in]*/ SAFEARRAY* pAlmTimes,
    /*[in]*/ SAFEARRAY* pAlmPriorities,
    /*[in]*/ SAFEARRAY* pAck,
    /*[in]*/ SAFEARRAY* pLimits,
    /*[in]*/ SAFEARRAY* pStatus,
    /*[in]*/ SAFEARRAY* pSources,
    /*[in]*/ SAFEARRAY* pReserved
    );

  //IBMSShutdown 的接口函数
  STDMETHOD(ShutdownRequest)(
    /*[in]*/BSTR bstrReason
    );

  //IBMSConnectionVerify 的接口函数 
  STDMETHOD(IsActive)(
    /*[out, retval]*/VARIANT_BOOL* bActive
    );
};

OBJECT_ENTRY_AUTO(__uuidof(RDBSInterface), CRDBSInterface)
