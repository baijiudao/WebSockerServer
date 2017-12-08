// RDBSInterface.cpp : Implementation of CRDBSInterface

#include "stdafx.h"
#include "RDBSInterface.h"
#include <atlstr.h>
#include <iostream>
#include <time.h>
#include "UnitConvert.h"
#include "Session.h"
#include <comdef.h>

#include "../../../../RDBS/RDBS4.1/RDBS.h"
#include "../../../../RDBS/RDBS4.1/RDBS_i.c"
#include "../../../../RDBS/RDBS4.1/Hac.h"
#include "../../../../RDBS/RDBS4.1/Hac_i.c"
#include "../../../../RDBS/RDBS4.1/Rcx.h"
#include "../../../../RDBS/RDBS4.1/Rcx_i.c"
#include "../../../../RDBS/RDBSExtern/RDBSExtern/RDBSExtern_i.h"
#include "../../../../RDBS/RDBSExtern/RDBSExtern/RDBSExtern_i.c"

#define RDBS_ACCESS_RCX     10
#define RDBS_ACCESS_LINKAGE_WATCH   0x00F1
#define RDBS_ACCESS_TIMETABLE_DISPATCH_WATCH  0x00F2

// CRDBSInterface

//------------------------------------------------------------------------------
HRESULT CRDBSInterface::FinalConstruct()
{
  m_pDataCache = nullptr;

  m_pIBMSServerDispatch = nullptr;
  m_pIBMSRcxDataDispatch = nullptr;
  m_pIBMSHacDispatch = nullptr;
  m_pILinkageWatch = nullptr;

  m_bInit = false;
  m_bLogin = false;

  m_ullMark = 0;
  m_pOptimize = nullptr;
  return S_OK;
}

//------------------------------------------------------------------------------
void CRDBSInterface::FinalRelease()
{
  LOG_DEBUG << ">>" << 1;
  if (EErrCode::EC_SUCCESS == Uninit())
    LOG_DEBUG << "Uninit Success";
  else
    LOG_DEBUG << "Uninit Fail";
  LOG_DEBUG << ">>" << 2;

  m_bInit = false;
  m_bLogin = false;

  // 删除Mark
  if (m_pOptimize) m_pOptimize->DelMark(m_ullMark);
}

//#define _TEST_INTERFACE_
#ifdef _TEST_INTERFACE_
#else
#define _PARAM_CODE_
#endif // _TEST_INTERFACE_

//------------------------------------------------------------------------------
// 内部测试函数
//------------------------------------------------------------------------------
void CRDBSInterface::Test()
{
#ifdef _TEST_INTERFACE_
  DISPPARAMS dispparms;
  VARIANTARG* pArg;
  HRESULT hr;

  //参数初始化
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 2;  //2个参数

  pArg = new VARIANTARG[dispparms.cArgs];
  dispparms.rgvarg = pArg;
  memset(pArg, 0, sizeof(VARIANT) * dispparms.cArgs);

  long nNumTags = 0;
  SAFEARRAY* pIDs = NULL;

  //参数赋值
  dispparms.rgvarg[1].vt = VT_BYREF | VT_I4;                // [out] long* pNumTags
  dispparms.rgvarg[1].plVal = &nNumTags;
  dispparms.rgvarg[0].vt = VT_ARRAY | VT_BYREF | VT_I4;       // [out] SAFEARRAY(long)* ppTagIDs
  dispparms.rgvarg[0].pparray = &pIDs;

  // Invoke the method 
  hr = m_pILinkageWatch->Invoke(0x04, // TestInterface
    IID_NULL,
    LOCALE_SYSTEM_DEFAULT,
    DISPATCH_METHOD,
    &dispparms,
    NULL,  //无返回值
    0,
    NULL);

  if (FAILED(hr))
  {
    cout << "fail..." << endl;
  }

  //将接收到的数据放到集合中
//   long lVal = 0, lErr = 0;
//   VARIANT v;
//   DATE date;
//   //SHORT quality;
//   USES_CONVERSION;
// 
//   for (long i = 0; i < nNumTags; i++)
//   {
//     VariantInit(&v);
//     SafeArrayGetElement(pTagIDs, (long*)&i, &lVal);
//     SafeArrayGetElement(pValues, (long*)&i, &v);
//     SafeArrayGetElement(pTimes, (long*)&i, &date);
//     //SafeArrayGetElement(pQualities, (long*)&i, &quality);
//     SafeArrayGetElement(pErr, (long*)&i, &lErr);
//   }
// 
//   //内存清理
//   SafeArrayDestroy(pTagIDs);
//   SafeArrayDestroy(pValues);
//   SafeArrayDestroy(pTimes);
//   SafeArrayDestroy(pQualities);
//   SafeArrayDestroy(pErr);
  delete[] pArg;
  //VariantClear(&v); //清除内存
#else
  
#endif // _TEST_INTERFACE_

}

//------------------------------------------------------------------------------
EErrCode CRDBSInterface::OnLogin(const string &strAccount, const string &strPassword)
{
  //栈内临时变量
  DISPPARAMS dispparms;
  VARIANTARG* pArg;      //参数存放指针
  VARIANTARG vaResult;   //Variant返回值
  HRESULT hr;

  CString csAcc(strAccount.c_str());
  CString csPwd(strPassword.c_str());

  // 1、调用IBMSServer的Login
  //参数初始化 
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 2;

  // 对参数分配内存
  pArg = new VARIANTARG[dispparms.cArgs];
  dispparms.rgvarg = pArg;
  memset(pArg, 0, sizeof(VARIANT) * dispparms.cArgs);

  dispparms.rgvarg[0].vt = VT_BSTR;          //Password
  dispparms.rgvarg[0].bstrVal = csPwd.AllocSysString();
  dispparms.rgvarg[1].vt = VT_BSTR;          //UserName
  dispparms.rgvarg[1].bstrVal = csAcc.AllocSysString();

  //返回值初始化
  VariantInit(&vaResult);

  //Invoke方法 
  hr = m_pIBMSServerDispatch->Invoke(0x03, // Login
    IID_NULL,
    LOCALE_SYSTEM_DEFAULT,
    DISPATCH_METHOD,
    &dispparms,
    &vaResult, //参数返回
    0,
    nullptr);

  //清除参数
  delete[] pArg;
  if (FAILED(hr)) return EErrCode::EC_BMS_SERVER_LOGIN_FAILD;

  //根据返回值填写返回状态
  if (vaResult.vt == VT_I2)
  {
    switch (vaResult.iVal)
    {
    case 1: // 登录成功,但先前用户未注销
    case 0: break; // 登录成功
    case -1:return EErrCode::EC_LOGIN_PASSWORD_ERROR;
    case -2:return EErrCode::EC_LOGIN_ACCOUNT_LOCKED;
    case -3:return EErrCode::EC_LOGIN_ACCOUNT_UNINVALID;
    default:return EErrCode::EC_UNKNOW;
    }
  }

  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode CRDBSInterface::OnLogout()
{
  DISPPARAMS dispparms;

  // 调用IBMSServer的Logout
  if (m_pIBMSServerDispatch)
  {
    // 参数设置
    memset(&dispparms, 0, sizeof(DISPPARAMS));
    dispparms.cArgs = 0; //无参数

    //Invoke方法 
    HRESULT hr = m_pIBMSServerDispatch->Invoke(0x04,
      IID_NULL,
      LOCALE_SYSTEM_DEFAULT,
      DISPATCH_METHOD,
      &dispparms,
      nullptr, //无参数返回
      0,
      nullptr);

    if (FAILED(hr)) return EErrCode::EC_BMS_SERVER_LOGOUT_FAILD;
  }

  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode CRDBSInterface::OnServerInit(const string &strHostName)
{
  DISPPARAMS dispparms;
  VARIANTARG* pArg;      //参数存放指针
  VARIANTARG vaResult;   //Variant返回值

  // 主机名
  CString csHostName(strHostName.c_str());
  CString csHostNameRcx = csHostName + _T(".RCX");

  // 创建BMSServer的实例
  COSERVERINFO ServerInfo;
  memset(&ServerInfo, 0, sizeof(COSERVERINFO));

  ServerInfo.pwszName = csHostName.AllocSysString();
  MULTI_QI mqi[1];
  mqi[0].pIID = &IID_IUnknown;
  mqi[0].pItf = nullptr;
  mqi[0].hr = S_FALSE;
  HRESULT hr = CoCreateInstanceEx(CLSID_BMSServer,
    nullptr,
    CLSCTX_ALL,
    &ServerInfo,
    1,
    mqi);

  if (FAILED(hr)) return EErrCode::EC_BMS_CREATE_INSTANCE_FAILD;

  // 获取BMSServer的实例
  hr = mqi[0].pItf->QueryInterface(IID_IDispatch, (void**)&m_pIBMSServerDispatch);
  if (FAILED(hr)) return EErrCode::EC_BMS_QUERY_INTERFACE_FAILD;
  if (!m_pIBMSServerDispatch) return EErrCode::EC_BMS_SERVER_OBJ_UNINVALID;
  mqi[0].pItf->Release();

  // 调用IBMSServer的Initialize方法
  //参数初始化
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 1;

  //对参数分配内存
  pArg = new VARIANTARG[dispparms.cArgs];
  dispparms.rgvarg = pArg;
  memset(pArg, 0, sizeof(VARIANT) * dispparms.cArgs);

  //赋值
  dispparms.rgvarg[0].vt = VT_BSTR;
  dispparms.rgvarg[0].bstrVal = csHostNameRcx.AllocSysString();

  //返回值初始化 
  VariantInit(&vaResult);

  // Invoke 方法
  hr = m_pIBMSServerDispatch->Invoke(0x01, //Initialize
    IID_NULL,
    LOCALE_SYSTEM_DEFAULT,
    DISPATCH_METHOD,
    &dispparms,
    &vaResult,
    0,
    nullptr);

  //清除参数
  delete[] pArg;
  if (FAILED(hr)) return EErrCode::EC_BMS_SERVER_INIT_FAILD;

  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode CRDBSInterface::OnRcxDataInit()
{
  DISPPARAMS dispparms;
  VARIANTARG* pArg;      //参数存放指针
  VARIANTARG vaResult;   //Variant返回值
  HRESULT hr;

  // 调用 IBMSServer的GetAccessObject 方法获取IBMSRcxData对象
  //参数初始化
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 2;  //2个参数

  //参数: 分配内存
  pArg = new VARIANTARG[dispparms.cArgs];
  dispparms.rgvarg = pArg;
  memset(pArg, 0, sizeof(VARIANT) * dispparms.cArgs);

  //参数: 赋值
  IUnknown* pUnk = nullptr;
  dispparms.rgvarg[0].vt = VT_BYREF | VT_UNKNOWN;    // [out] IUnknown** ppUnk
  dispparms.rgvarg[0].ppunkVal = &pUnk;
  dispparms.rgvarg[1].vt = VT_I4;                    // [in] long objId 
  dispparms.rgvarg[1].lVal = RDBS_ACCESS_RCX;

  //返回参数初始化 
  VariantInit(&vaResult);

  // Invoke 方法
  hr = m_pIBMSServerDispatch->Invoke(0x05,  //GetAccessObject
    IID_NULL,
    LOCALE_SYSTEM_DEFAULT,
    DISPATCH_METHOD,
    &dispparms,
    &vaResult,
    0,
    nullptr);

  //清除参数内存 
  delete[] pArg;
  if (FAILED(hr) || pUnk == nullptr) return EErrCode::EC_BMS_SERVER_GET_ACCESS_OBJ_FAILD;

  //根据返回值填写返回状态
  if (vaResult.vt == VT_I2)
  {
    switch (vaResult.iVal)
    {
    case 0: break; //登录成功
    case -1: return EErrCode::EC_NOT_LOGIN;
    case -2: return EErrCode::EC_UNAUTHORIZED_ACCESS;
    default:return EErrCode::EC_UNKNOW;
    }
  }

  // 获取IBMRcxData对象
  hr = pUnk->QueryInterface(IID_IDispatch, (void**)&m_pIBMSRcxDataDispatch);
  if (FAILED(hr)) return EErrCode::EC_BMS_QUERY_INTERFACE_FAILD;
  if (!m_pIBMSRcxDataDispatch) return EErrCode::EC_BMS_RCX_OBJ_UNINVALID;
  pUnk->Release();

  // 调用 IBMSRcxData的Initialize 方法
  // 参数初始化
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 0; 	   //无参数

  //返回值初始化
  VariantInit(&vaResult);

  // Invoke 方法
  hr = m_pIBMSRcxDataDispatch->Invoke(0x1, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD,
    &dispparms,
    &vaResult,
    0,
    nullptr);
  if (FAILED(hr)) return EErrCode::EC_BMS_RCX_INIT_FAILD;

  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode CRDBSInterface::OnHacInit()
{
  DISPPARAMS dispparms;
  VARIANTARG* pArg;      //参数存放指针
  VARIANTARG vaResult;   //Variant返回值
  HRESULT hr;

  // 调用 IBMSServer的GetAccessObject方法获取IBMSHac对象
  //参数初始化
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 2;  //2个参数

  //参数: 分配内存
  pArg = new VARIANTARG[dispparms.cArgs];
  dispparms.rgvarg = pArg;
  memset(pArg, 0, sizeof(VARIANT) * dispparms.cArgs);

  //参数: 赋值
  IUnknown* pUnk = nullptr;
  dispparms.rgvarg[0].vt = VT_BYREF | VT_UNKNOWN;    // [out] IUnknown** ppUnk
  dispparms.rgvarg[0].ppunkVal = &pUnk;
  dispparms.rgvarg[1].vt = VT_I4;					 // [in] long objId 
  dispparms.rgvarg[1].lVal = RDBS_ACCESS_HAC;

  //返回参数初始化 
  VariantInit(&vaResult);

  // Invoke 方法
  hr = m_pIBMSServerDispatch->Invoke(0x05,  //GetAccessObject
    IID_NULL,
    LOCALE_SYSTEM_DEFAULT,
    DISPATCH_METHOD,
    &dispparms,
    &vaResult,
    0,
    nullptr);

  //清除参数内存 
  delete[] pArg;
  if (FAILED(hr) || pUnk == nullptr) return EErrCode::EC_BMS_SERVER_GET_ACCESS_OBJ_FAILD;

  //根据返回值填写返回状态
  if (vaResult.vt == VT_I2)
  {
    switch (vaResult.iVal)
    {
    case 0: break; //登录成功
    case -1: return EErrCode::EC_NOT_LOGIN;
    case -2: return EErrCode::EC_UNAUTHORIZED_ACCESS;
    default:return EErrCode::EC_UNKNOW;
    }
  }

  // 获取BMSHac对象
  hr = pUnk->QueryInterface(IID_IDispatch, (void**)&m_pIBMSHacDispatch);
  if (FAILED(hr)) return EErrCode::EC_BMS_QUERY_INTERFACE_FAILD;
  if (!m_pIBMSHacDispatch) return EErrCode::EC_BMS_SHAC_OBJ_UNINVALID;
  pUnk->Release();

  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode CRDBSInterface::OnLinkageWatchInit()
{
  DISPPARAMS dispparms;
  VARIANTARG* pArg;      //参数存放指针
  VARIANTARG vaResult;   //Variant返回值
  HRESULT hr;

  // 调用 IBMSServer的GetAccessObject方法获取LinkageWatch对象
  //参数初始化
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 2;  //2个参数

  //参数: 分配内存
  pArg = new VARIANTARG[dispparms.cArgs];
  dispparms.rgvarg = pArg;
  memset(pArg, 0, sizeof(VARIANT) * dispparms.cArgs);

  //参数: 赋值
  IUnknown* pUnk = nullptr;
  dispparms.rgvarg[0].vt = VT_BYREF | VT_UNKNOWN;    // [out] IUnknown** ppUnk
  dispparms.rgvarg[0].ppunkVal = &pUnk;
  dispparms.rgvarg[1].vt = VT_I4;					 // [in] long objId 
  dispparms.rgvarg[1].lVal = RDBS_ACCESS_LINKAGE_WATCH;

  //返回参数初始化 
  VariantInit(&vaResult);

  // Invoke 方法
  hr = m_pIBMSServerDispatch->Invoke(0x05,  //GetAccessObject
    IID_NULL,
    LOCALE_SYSTEM_DEFAULT,
    DISPATCH_METHOD,
    &dispparms,
    &vaResult,
    0,
    nullptr);

  //清除参数内存 
  delete[] pArg;
  if (FAILED(hr) || pUnk == nullptr) return EErrCode::EC_BMS_SERVER_GET_ACCESS_OBJ_FAILD;

  //根据返回值填写返回状态
  if (vaResult.vt == VT_I2)
  {
    switch (vaResult.iVal)
    {
    case 0: break; //登录成功
    case -1: return EErrCode::EC_NOT_LOGIN;
    case -2: return EErrCode::EC_UNAUTHORIZED_ACCESS;
    default:return EErrCode::EC_UNKNOW;
    }
  }

  // 获取LinkageWatch对象
  hr = pUnk->QueryInterface(IID_IDispatch, (void**)&m_pILinkageWatch);
  if (FAILED(hr)) return EErrCode::EC_BMS_QUERY_INTERFACE_FAILD;
  if (!m_pILinkageWatch) return EErrCode::EC_BMS_LINKAGE_WATCH_OBJ_UNINVALID;
  pUnk->Release();

  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode CRDBSInterface::OnTimetableDispatchWatchInit()
{
  DISPPARAMS dispparms;
  VARIANTARG* pArg;      //参数存放指针
  VARIANTARG vaResult;   //Variant返回值
  HRESULT hr;

  // 调用 IBMSServer的GetAccessObject方法获取TimetableDispatchWatch对象
  //参数初始化
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 2;  //2个参数

  //参数: 分配内存
  pArg = new VARIANTARG[dispparms.cArgs];
  dispparms.rgvarg = pArg;
  memset(pArg, 0, sizeof(VARIANT) * dispparms.cArgs);

  //参数: 赋值
  IUnknown* pUnk = nullptr;
  dispparms.rgvarg[0].vt = VT_BYREF | VT_UNKNOWN;    // [out] IUnknown** ppUnk
  dispparms.rgvarg[0].ppunkVal = &pUnk;
  dispparms.rgvarg[1].vt = VT_I4;					 // [in] long objId 
  dispparms.rgvarg[1].lVal = RDBS_ACCESS_TIMETABLE_DISPATCH_WATCH;

  //返回参数初始化 
  VariantInit(&vaResult);

  // Invoke 方法
  hr = m_pIBMSServerDispatch->Invoke(0x05,  //GetAccessObject
    IID_NULL,
    LOCALE_SYSTEM_DEFAULT,
    DISPATCH_METHOD,
    &dispparms,
    &vaResult,
    0,
    nullptr);

  //清除参数内存 
  delete[] pArg;
  if (FAILED(hr) || pUnk == nullptr) return EErrCode::EC_BMS_SERVER_GET_ACCESS_OBJ_FAILD;

  //根据返回值填写返回状态
  if (vaResult.vt == VT_I2)
  {
    switch (vaResult.iVal)
    {
    case 0: break; //登录成功
    case -1: return EErrCode::EC_NOT_LOGIN;
    case -2: return EErrCode::EC_UNAUTHORIZED_ACCESS;
    default:return EErrCode::EC_UNKNOW;
    }
  }

  // 获取TimetableDispatchWatch对象
  hr = pUnk->QueryInterface(IID_IDispatch, (void**)&m_pDispatchWatch);
  if (FAILED(hr)) return EErrCode::EC_BMS_QUERY_INTERFACE_FAILD;
  if (!m_pILinkageWatch) return EErrCode::EC_BMS_TIMETABLE_DISPATCH_WATCH_OBJ_UNINVALID;
  pUnk->Release();

  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode CRDBSInterface::OnServerUninit()
{
  HRESULT hr;
  DISPPARAMS dispparms;

  // 调用 IBMSServer的Uninitialize 方法
  if (nullptr != m_pIBMSServerDispatch)
  {
    memset(&dispparms, 0, sizeof(DISPPARAMS));
    hr = m_pIBMSServerDispatch->Invoke(0x02, //Uninitialize
      IID_NULL,
      LOCALE_SYSTEM_DEFAULT,
      DISPATCH_METHOD,
      &dispparms,
      nullptr,
      0,
      nullptr);

    if (FAILED(hr)) return EErrCode::EC_BMS_SERVER_UNINIT_FAILD;

    //释放接口	
    m_pIBMSServerDispatch->Release();
    m_pIBMSServerDispatch = nullptr;
  }

  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode CRDBSInterface::OnRcxDataUninit()
{
  DISPPARAMS dispparms;
  VARIANTARG vaResult;
  HRESULT hr;

  // 调用 IBMSRcxData的Uninitialize 方法
  if (m_pIBMSRcxDataDispatch)
  {
    // 参数初始化
    VariantInit(&vaResult);
    memset(&dispparms, 0, sizeof(DISPPARAMS));

    //无参数

    // Invoke 方法
    hr = m_pIBMSRcxDataDispatch->Invoke(0x06, //Uninitialize
      IID_NULL,
      LOCALE_SYSTEM_DEFAULT,
      DISPATCH_METHOD,
      &dispparms,
      &vaResult,
      0,
      nullptr);
    if (FAILED(hr)) return EErrCode::EC_BMS_RCX_UNINIT_FAILD;

    if (vaResult.vt == VT_BOOL)
      if (!vaResult.boolVal) return EErrCode::EC_BMS_VALUE_INVALID;

    //释放接口
    m_pIBMSRcxDataDispatch->Release();
    m_pIBMSRcxDataDispatch = nullptr;
  }

  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode CRDBSInterface::OnHacUninit()
{
  if (m_pIBMSHacDispatch)
  {
    m_pIBMSHacDispatch->Release();
    m_pIBMSHacDispatch = nullptr;
  }

  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
// 将对象自己的指针传递给服务端，让服务端的回调进行调用
//------------------------------------------------------------------------------
EErrCode CRDBSInterface::OnSetSinkPointer()
{
  DISPPARAMS dispparms;
  VARIANTARG* pArg;
  HRESULT hr;

  IUnknown* pUnkIBMSCallBack = static_cast<IBMSCallBack*>(this);
  IUnknown* pUnkIBMSShutdown = static_cast<IBMSShutdown*>(this);
  IUnknown* pUnkIBMSConnectionVerify = static_cast<IBMSConnectionVerify*>(this);

  //参数初始化
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 1;  //1个参数

  pArg = new VARIANTARG[dispparms.cArgs];
  dispparms.rgvarg = pArg;
  memset(pArg, 0, sizeof(VARIANT) * dispparms.cArgs);

  //创建安全数组,并赋值	
  SAFEARRAY* psa;
  SAFEARRAYBOUND bound = { 3, 0 };
  psa = SafeArrayCreate(VT_UNKNOWN, 1, &bound);
  if (psa == nullptr) return EErrCode::EC_BMS_CREATE_ARR_FAILD;

  //将元素加到数组上
  int i = 0;
  SafeArrayPutElement(psa, (long*)&i, pUnkIBMSCallBack);
  i++;
  SafeArrayPutElement(psa, (long*)&i, pUnkIBMSShutdown);
  i++;
  SafeArrayPutElement(psa, (long*)&i, pUnkIBMSConnectionVerify);

  //参数复值
  dispparms.rgvarg[0].vt = VT_ARRAY | VT_UNKNOWN;// [in] SAFEARRAY(IUnknown*) pUnks
  dispparms.rgvarg[0].parray = psa;

  // Invoke 方法
  hr = m_pIBMSRcxDataDispatch->Invoke(0x03,//SetSinkPointers
    IID_NULL,
    LOCALE_SYSTEM_DEFAULT,
    DISPATCH_METHOD,
    &dispparms,
    nullptr,  //无返回值
    0,
    nullptr);

  LOG_DEBUG << "==>" << 1;
  //内存清理
  // 此处注释掉，否则会引起对象自己释放自己，引起问题，暂无解决方法
  //SafeArrayDestroy(psa);
  delete[] pArg;

  LOG_DEBUG << "==>" << 2;
  if (FAILED(hr)) LOG_DEBUG << "SetSinkPointers Fail";
  if (FAILED(hr)) return EErrCode::EC_BMS_RCX_SET_SINK_PTR_FAILD;
  LOG_DEBUG << "SetSinkPointers Success";
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
// 什么都不获取，只是调用下服务端的GetAllTags接口，以便回调在后面能成功开启
//------------------------------------------------------------------------------
EErrCode CRDBSInterface::OnGetNull()
{
  DISPPARAMS dispparms;
  VARIANTARG* pArg;
  HRESULT hr;

  //参数初始化
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 6;  //2个参数

  pArg = new VARIANTARG[dispparms.cArgs];
  dispparms.rgvarg = pArg;
  memset(pArg, 0, sizeof(VARIANT) * dispparms.cArgs);

  long nNumTags = 0;
  SAFEARRAY* pTagIDs = NULL;
  SAFEARRAY* pValues = NULL;
  SAFEARRAY* pTimes = NULL;
  SAFEARRAY* pQualities = NULL;
  SAFEARRAY* pErr = NULL;

  //参数赋值
  dispparms.rgvarg[5].vt = VT_BYREF | VT_I4;                // [out] long* pNumTags
  dispparms.rgvarg[5].plVal = &nNumTags;
  dispparms.rgvarg[4].vt = VT_ARRAY | VT_BYREF | VT_I4;       // [out] SAFEARRAY(long)* ppTagIDs
  dispparms.rgvarg[4].pparray = &pTagIDs;
  dispparms.rgvarg[3].vt = VT_ARRAY | VT_BYREF | VT_VARIANT;  // [out] SAFEARRAY(VARIANT)* ppValues
  dispparms.rgvarg[3].pparray = &pValues;
  dispparms.rgvarg[2].vt = VT_ARRAY | VT_BYREF | VT_DATE;     // [out] SAFEARRAY(DATE)* ppTimes,
  dispparms.rgvarg[2].pparray = &pTimes;
  dispparms.rgvarg[1].vt = VT_ARRAY | VT_BYREF | VT_I2;       // [out] SAFEARRAY(short)* ppQualities
  dispparms.rgvarg[1].pparray = &pQualities;
  dispparms.rgvarg[0].vt = VT_ARRAY | VT_BYREF | VT_I4;       // [out] SAFEARRAY(long)* pErr
  dispparms.rgvarg[0].pparray = &pErr;

  // Invoke the method 
  hr = m_pIBMSRcxDataDispatch->Invoke(0x02,//GetConfigTags
    IID_NULL,
    LOCALE_SYSTEM_DEFAULT,
    DISPATCH_METHOD,
    &dispparms,
    NULL,  //无返回值
    0,
    NULL);

  if (FAILED(hr)) return EErrCode::EC_BMS_RCX_GET_ALL_TAG_FAILD;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode CRDBSInterface::OnGetAllTag(map<DWORD, shared_ptr<KTagInfo>> &mapTag)
{
  shared_ptr<CUnitConvert> pConv = CUnitConvert::Inst();
  DISPPARAMS dispparms;
  VARIANTARG* pArg;
  HRESULT hr;

  //参数初始化
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 6;  //2个参数

  pArg = new VARIANTARG[dispparms.cArgs];
  dispparms.rgvarg = pArg;
  memset(pArg, 0, sizeof(VARIANT) * dispparms.cArgs);

  long nNumTags = 0;
  SAFEARRAY* pTagIDs = NULL;
  SAFEARRAY* pValues = NULL;
  SAFEARRAY* pTimes = NULL;
  SAFEARRAY* pQualities = NULL;
  SAFEARRAY* pErr = NULL;

  //参数赋值
  dispparms.rgvarg[5].vt = VT_BYREF | VT_I4;                // [out] long* pNumTags
  dispparms.rgvarg[5].plVal = &nNumTags;
  dispparms.rgvarg[4].vt = VT_ARRAY | VT_BYREF | VT_I4;       // [out] SAFEARRAY(long)* ppTagIDs
  dispparms.rgvarg[4].pparray = &pTagIDs;
  dispparms.rgvarg[3].vt = VT_ARRAY | VT_BYREF | VT_VARIANT;  // [out] SAFEARRAY(VARIANT)* ppValues
  dispparms.rgvarg[3].pparray = &pValues;
  dispparms.rgvarg[2].vt = VT_ARRAY | VT_BYREF | VT_DATE;     // [out] SAFEARRAY(DATE)* ppTimes,
  dispparms.rgvarg[2].pparray = &pTimes;
  dispparms.rgvarg[1].vt = VT_ARRAY | VT_BYREF | VT_I2;       // [out] SAFEARRAY(short)* ppQualities
  dispparms.rgvarg[1].pparray = &pQualities;
  dispparms.rgvarg[0].vt = VT_ARRAY | VT_BYREF | VT_I4;       // [out] SAFEARRAY(long)* pErr
  dispparms.rgvarg[0].pparray = &pErr;

  // Invoke the method 
  hr = m_pIBMSRcxDataDispatch->Invoke(0x02,//GetConfigTags
    IID_NULL,
    LOCALE_SYSTEM_DEFAULT,
    DISPATCH_METHOD,
    &dispparms,
    NULL,  //无返回值
    0,
    NULL);

  if (FAILED(hr)) return EErrCode::EC_BMS_RCX_GET_ALL_TAG_FAILD;

  //将接收到的数据放到集合中
  long lVal = 0, lErr = 0;
  VARIANT v;
  DATE date;
  //SHORT quality;
  USES_CONVERSION;

  for (long i = 0; i < nNumTags; i++)
  {
    VariantInit(&v);
    SafeArrayGetElement(pTagIDs, (long*)&i, &lVal);
    SafeArrayGetElement(pValues, (long*)&i, &v);
    SafeArrayGetElement(pTimes, (long*)&i, &date);
    //SafeArrayGetElement(pQualities, (long*)&i, &quality);
    SafeArrayGetElement(pErr, (long*)&i, &lErr);

    shared_ptr<KTagInfo>  ptTag = make_shared<KTagInfo>();

    if (lErr == 0) //成功
    {
      switch (v.vt)
      {
      case VT_BOOL: //开关量
        ptTag->anyValue = (VARIANT_TRUE == v.boolVal) ? true : false;
        break;

      case VT_I4:   //整形量
        ptTag->anyValue = v.lVal;
        break;

      case VT_R4:   //浮点量
        ptTag->anyValue = v.fltVal;
        break;

      case VT_BSTR: //字符串
      {
        if (v.bstrVal)
        {
          std::string str = W2A(v.bstrVal);
          // 这样子赋值any中保存的才是string类型
          ptTag->anyValue = str;
        }
      } break;
      }

      ptTag->ulGID = lVal;
      //ptTag->sQuality = quality;
      ptTag->ullTimeMs = pConv->VariantTimeToUINT64(date);
      mapTag.insert(make_pair(lVal, ptTag));
    }
  }

  //内存清理
  SafeArrayDestroy(pTagIDs);
  SafeArrayDestroy(pValues);
  SafeArrayDestroy(pTimes);
  SafeArrayDestroy(pQualities);
  SafeArrayDestroy(pErr);
  delete[] pArg;
  //VariantClear(&v); //清除内存

  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
//              [远程登录]  [所有变量验证] [RDBS登录界面登录]   [任务调度]    [获取历史数据]
//               用户验证     变量权限    ONLINECONFIG        TASKSCHEDULE    WARNINGEVENT
//                 ERR        LEVEL         LEVEL              LEVEL          LEVEL
//管理员   GLY      0          1             1                  1             1     AAA     TTT
//任务调度 RWDD     0          0            -1                  1            -1     CAC     FTF
//全局控制 QJKZ     0          1            -1                 -1             0     CCB     FFT
//全局监视 QJJS     0          0             0                  0             0     BBB     FFT
//历史记录 LSJL     0          0            -1                 -1            -1     CCC     FFF
//报警事件 BJSJ     0          0            -1                 -1             1     CCA     FFT
//                                      1表示有权登录     1表示有权调度  不等于-1表示有权 
//
// 用来获取操作某个方法的权限
//------------------------------------------------------------------------------
EErrCode CRDBSInterface::OnGetFunctionAuthority(int32_t &lOnlineConfig,
  int32_t &lTaskSchedule, int32_t &lWarningEvent)
{
  DISPPARAMS dispparms;
  VARIANTARG* pArg;
  HRESULT hr;

  //参数初始化
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 3;

  pArg = new VARIANTARG[dispparms.cArgs];
  dispparms.rgvarg = pArg;
  memset(pArg, 0, sizeof(VARIANT) * dispparms.cArgs);

  //参数赋值
  dispparms.rgvarg[2].vt = VT_BYREF | VT_I4;     // [out]
  dispparms.rgvarg[2].plVal = (LONG*)&lOnlineConfig;
  dispparms.rgvarg[1].vt = VT_BYREF | VT_I4;       // [out]
  dispparms.rgvarg[1].plVal = (LONG*)&lTaskSchedule;
  dispparms.rgvarg[0].vt = VT_BYREF | VT_I4;       // [out]
  dispparms.rgvarg[0].plVal = (LONG*)&lWarningEvent;

  // Invoke the method 
  hr = m_pIBMSServerDispatch->Invoke(0x0B,// 11 GetFunctionAuthority
    IID_NULL,
    LOCALE_SYSTEM_DEFAULT,
    DISPATCH_METHOD,
    &dispparms,
    NULL,  //无返回值
    0,
    NULL);

  if (FAILED(hr)) return EErrCode::EC_BMS_GET_FUNC_AUTHORITY;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
// 暂时不能用IP，而应该用局域网电脑名字
//------------------------------------------------------------------------------
EErrCode CRDBSInterface::Init(const string &strHostName,
  shared_ptr<CDataCache>  pDataCache, shared_ptr<COptimize> pOptimize)
{
  if (strHostName.empty()) return EErrCode::EC_PARAM_UNINVALID;
  if (!pDataCache || !pOptimize) return EErrCode::EC_RT_QUE_UNINVALID;

  // 如果已经初始化，则返回成功
  if (m_bInit) return EErrCode::EC_SUCCESS;
  m_pDataCache = pDataCache;

  EErrCode eErr = EErrCode::EC_UNKNOW;
  eErr = OnServerInit(strHostName);
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  // 创建一个Mark
  m_pOptimize = pOptimize;
  m_ullMark = m_pOptimize->CreMark();

  // 初始化成功
  m_bInit = true;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode CRDBSInterface::Uninit()
{
  // 如果未初始化，则返回成功
  if (!m_bInit) return EErrCode::EC_SUCCESS;
  EErrCode eErr = EErrCode::EC_UNKNOW;

  // 如果已经登录，则退出登录
  if (m_bLogin)
  {
    // 退出登录
    eErr = Logout();
    if (EErrCode::EC_SUCCESS != eErr) return eErr;
    m_bLogin = false;
  }

  // BMSServer的uninit
  eErr = OnServerUninit();
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  // 反初始化成功
  m_bInit = false;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode CRDBSInterface::Login(const string &strAccount,
  const string &strPassword, EUserType &eType)
{
  if (strAccount.empty() || strPassword.empty()) return EErrCode::EC_PARAM_UNINVALID;

  // 如果没有初始化，则返回失败
  if (!m_bInit) return EErrCode::EC_INTERFACE_OBJ_INIT_FAILD;
  // 如果已经成功登录，则返回成功
  if (m_bLogin) return EErrCode::EC_SUCCESS;

  EErrCode eErr = EErrCode::EC_UNKNOW;
  eErr = OnLogin(strAccount, strPassword);
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  eErr = OnRcxDataInit();
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  eErr = OnHacInit();
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  // 获取linkage watch对象
  eErr = OnLinkageWatchInit();
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  // 获取Timetable Dispatch watch对象
  eErr = OnTimetableDispatchWatchInit();
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

#ifdef _CALL_BACK_
  // 推送服务前的接口调用
  eErr = OnGetNull();
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  // 将服务器所需要的推送的指针发送给服务器
  eErr = OnSetSinkPointer();
  if (EErrCode::EC_SUCCESS != eErr) return eErr;
#endif // _CALL_BACK_

  // 获取对一些方法的操作权限，用来判断用户类型
  int32_t li, lj, lk;
  eErr = OnGetFunctionAuthority(li, lj, lk);
  if (EErrCode::EC_SUCCESS != eErr) return eErr;
  if ((1 == li) && (1 == lj) && (1 == lk))
    eType = EUserType::ADMINISTRATORS;
  else
    eType = EUserType::GENERAL;

  // 登录成功
  m_bLogin = true;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode CRDBSInterface::Logout()
{
  // 如果没有初始化，则返回失败
  if (!m_bInit) return EErrCode::EC_INTERFACE_OBJ_INIT_FAILD;
  // 如果没有登录，则返回成功
  if (!m_bLogin) return EErrCode::EC_SUCCESS;

  EErrCode eErr = EErrCode::EC_UNKNOW;
  eErr = OnRcxDataUninit();
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  eErr = OnHacUninit();
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  eErr = OnLogout();
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  // 登出成功
  m_bLogin = false;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode CRDBSInterface::ReadTag(list<shared_ptr<KTagInfo>> &lstInfo)
{
  // 判断是否初始化且登录
  if (!m_bInit) return EErrCode::EC_INTERFACE_OBJ_INIT_FAILD;
  if (!m_bLogin) return EErrCode::EC_LOGGED_OUT;

  EErrCode eErr = EErrCode::EC_UNKNOW;
  shared_ptr<CUnitConvert> pConv = CUnitConvert::Inst();
  map<DWORD, shared_ptr<KTagInfo>> mapAllTag;

  eErr = OnGetAllTag(mapAllTag);
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  for (auto iter = lstInfo.begin(); iter != lstInfo.end(); iter++)
  {
    auto mapIter = mapAllTag.find((*iter)->ulGID);
    if (mapIter == mapAllTag.end()) continue;

    // 将数据复制过去
    (*iter)->ulGID = mapIter->second->ulGID;
    (*iter)->sQuality = mapIter->second->sQuality;
    (*iter)->ullTimeMs = mapIter->second->ullTimeMs;
    (*iter)->anyValue = mapIter->second->anyValue;
  }

  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode CRDBSInterface::WriteTag(const list<shared_ptr<KTagInfo>> &lstInfo)
{
  // 判断是否初始化且登录
  if (!m_bInit) return EErrCode::EC_INTERFACE_OBJ_INIT_FAILD;
  if (!m_bLogin) return EErrCode::EC_LOGGED_OUT;

  int nNum = lstInfo.size();
  if (nNum <= 0) return EErrCode::EC_SUCCESS;

  shared_ptr<CUnitConvert> pConv = CUnitConvert::Inst();
  DISPPARAMS dispparms;
  VARIANTARG* pArg;      //参数存放指针
  HRESULT hr;

  // Set up the parameters
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 4;  //参数

  // allocate memory for parameters
  pArg = new VARIANTARG[dispparms.cArgs];
  dispparms.rgvarg = pArg;
  memset(pArg, 0, sizeof(VARIANT) * dispparms.cArgs);

  //创建pValues安全数组,并赋值	
  SAFEARRAY* psaErrs = NULL;  //变量Errs
  SAFEARRAYBOUND rgsabound[1];
  rgsabound[0].cElements = nNum;
  rgsabound[0].lLbound = 0;

  // Creating a SafeArray with 1 element.
  SAFEARRAY* psaTagID = SafeArrayCreate(VT_I4, 1, rgsabound);
  SAFEARRAY* psaVal = SafeArrayCreate(VT_VARIANT, 1, rgsabound);
  if (psaTagID == NULL || psaVal == NULL) return EErrCode::EC_BMS_CREATE_ARR_FAILD;

  //将元素加到数组上
  long i = 0;
  VARIANT v;
  DWORD dwGID = 0;

  for (auto iter = lstInfo.begin(); iter != lstInfo.end(); iter++, i++)
  {
    VariantInit(&v);
    //将元素加到数组上
    SafeArrayPutElement(psaTagID, (long*)&i, &(*iter)->ulGID);

    //赋变量值
    try
    {
      switch (CCommonUnit::Inst()->GetTagType((*iter)->anyValue))
      {
      case TYPE_BOOL:
      {
        v.vt = VT_BOOL;
        v.boolVal = boost::any_cast<bool>((*iter)->anyValue) ? VARIANT_TRUE : VARIANT_FALSE;
      } break;

      case TYPE_INTEGER:
      {
        v.vt = VT_I4;
        v.lVal = (LONG)boost::any_cast<uint32_t>((*iter)->anyValue);
      } break;
      

      case TYPE_FLOAT:
      {
        v.vt = VT_R4;
        v.fltVal = boost::any_cast<float>((*iter)->anyValue);
      } break;

      case TYPE_STRING:
      {
        USES_CONVERSION;
        string str = boost::any_cast<string>((*iter)->anyValue);
        v.vt = VT_BSTR;
        v.bstrVal = _bstr_t(str.c_str()).copy();
      } break;
      default: return EErrCode::EC_TAG_INVAILD;
      }
    }
    catch (boost::bad_any_cast &e)
    {
      LOG_ERROR << e.what();
      return EErrCode::EC_BAD_ANY_CAST;
    }

    SafeArrayPutElement(psaVal, (long*)&i, &v);
  }

  // The parameters are entered right to left
  USES_CONVERSION;
  dispparms.rgvarg[3].vt = VT_I4;                    // [in] long NumTags
  dispparms.rgvarg[3].lVal = nNum;                      // 每次写nNum个Tag
  dispparms.rgvarg[2].vt = VT_ARRAY | VT_I4;           //[in] SAFEARRAY(long) pTagIDs, 
  dispparms.rgvarg[2].parray = psaTagID;
  dispparms.rgvarg[1].vt = VT_ARRAY | VT_VARIANT;      //[in] SAFEARRAY(VARIANT) pValues,
  dispparms.rgvarg[1].parray = psaVal;
  dispparms.rgvarg[0].vt = VT_ARRAY | VT_BYREF | VT_I4;  //[out] SAFEARRAY(long)* ppErrs
  dispparms.rgvarg[0].pparray = &psaErrs;

  // 调用 IBMSRcxData的WriteTags 方法
  hr = m_pIBMSRcxDataDispatch->Invoke(0x04, //WriteTags
    IID_NULL,
    LOCALE_SYSTEM_DEFAULT,
    DISPATCH_METHOD,
    &dispparms,
    NULL, //无返回参数
    0,
    NULL);


  //清理内存
  SafeArrayDestroy(psaVal);
  SafeArrayDestroy(psaTagID);
  SafeArrayDestroy(psaErrs);
  delete[] pArg;

  if (FAILED(hr)) return EErrCode::EC_BMS_RCX_WRITE_TAGS_FAILD;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode CRDBSInterface::WriteEvent(const list<shared_ptr<KEventInfo>> &lstInfo)
{
  // 判断是否初始化且登录
  if (!m_bInit) return EErrCode::EC_INTERFACE_OBJ_INIT_FAILD;
  if (!m_bLogin) return EErrCode::EC_LOGGED_OUT;
  int nNum = lstInfo.size();
  if (nNum <= 0) return EErrCode::EC_SUCCESS;

  shared_ptr<CUnitConvert> pConv = CUnitConvert::Inst();
  DISPPARAMS dispparms;
  VARIANTARG* pArg;      //参数存放指针
  HRESULT hr;

  // Set up the parameters
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 10;  //参数

  // allocate memory for parameters
  pArg = new VARIANTARG[dispparms.cArgs];
  dispparms.rgvarg = pArg;
  memset(pArg, 0, sizeof(VARIANT) * dispparms.cArgs);

  //创建pValues安全数组,并赋值	
  SAFEARRAYBOUND rgsabound[1];
  rgsabound[0].cElements = nNum;
  rgsabound[0].lLbound = 0;

  // 将每个事件都写入
  for (auto iter = lstInfo.begin(); iter != lstInfo.end(); iter++)
  {
    BSTR  bstrRecord = _bstr_t((*iter)->strRecord.c_str()).copy();
    BSTR  bstrUserName = _bstr_t((*iter)->strUserName.c_str()).copy();

    SYSTEMTIME  stTime;
    DATE    daTime;
    GetLocalTime(&stTime);
    SystemTimeToVariantTime(&stTime, &daTime);

    // 方便A2W临时内存释放
    dispparms.rgvarg[9].vt = VT_BSTR;                   // [in] BSTR
    dispparms.rgvarg[9].bstrVal = bstrUserName;         // EvtSrc
    dispparms.rgvarg[8].vt = VT_DATE;                   // [in] DATE
    dispparms.rgvarg[8].date = daTime;                  // EvtTime
    dispparms.rgvarg[7].vt = VT_BSTR;                   // [in] BSTR
    dispparms.rgvarg[7].bstrVal = bstrRecord;           // EvtMsg
    dispparms.rgvarg[6].vt = VT_I4;                     // [in] long
    dispparms.rgvarg[6].lVal = (*iter)->ulAccessLevel;  // Severity
    dispparms.rgvarg[5].vt = VT_I4;                     // long
    dispparms.rgvarg[5].lVal = (*iter)->eType;          // EventType
    dispparms.rgvarg[4].vt = VT_I4;                     //[in] long
    dispparms.rgvarg[4].lVal = 0;                       // EventCategory
    dispparms.rgvarg[3].vt = VT_I4;                     // [in] long
    dispparms.rgvarg[3].lVal = 0;                       // AttrNum
    dispparms.rgvarg[2].vt = VT_ARRAY | VT_VARIANT;     // [in] VARIANT
    dispparms.rgvarg[2].parray = nullptr;               // pAttrs
    dispparms.rgvarg[1].vt = VT_BSTR;                   // [in] BSTR
    dispparms.rgvarg[1].bstrVal = bstrUserName;         // ActorID
    dispparms.rgvarg[0].vt = VT_I4;                     // [in] long
    dispparms.rgvarg[0].lVal = (*iter)->ulID;           // EvtID

    // 调用 IBMSRcxData的WriteTags 方法
    hr = m_pIBMSRcxDataDispatch->Invoke(0x05, //WriteEvent
      IID_NULL,
      LOCALE_SYSTEM_DEFAULT,
      DISPATCH_METHOD,
      &dispparms,
      NULL, //无返回参数
      0,
      NULL);

    // 释放存储
    SysFreeString(bstrRecord);
    SysFreeString(bstrUserName);

    // 执行失败就跳出循环
    if (FAILED(hr)) break;
  }

  delete[] pArg;

  if (FAILED(hr)) return EErrCode::EC_BMS_RCX_WRITE_EVENT_FAILD;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode CRDBSInterface::ReadHistroyEvent(uint64_t ullStart, uint64_t ullEnd,
  std::list<shared_ptr<KEventInfo>> &lstInfo)
{
  // 判断是否初始化且登录
  if (!m_bInit) return EErrCode::EC_INTERFACE_OBJ_INIT_FAILD;
  if (!m_bLogin) return EErrCode::EC_LOGGED_OUT;

  // 检查用户权限
  int32_t lOnlineConfig, lTaskSchedule, lWarningEvent;
  EErrCode  eErr = OnGetFunctionAuthority(lOnlineConfig, lTaskSchedule, lWarningEvent);
  if (-1 == lWarningEvent) return EErrCode::EC_NO_PERMISSION;

  //栈内临时变量
  DISPPARAMS dispparms;
  VARIANTARG* pArg;     // 参数存放指针
  VARIANTARG vaResult;
  HRESULT hr;

  // 转换功能类
  shared_ptr<CUnitConvert> pConv = CUnitConvert::Inst();

  //参数初始化
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 4;  // 4个参数

  pArg = new VARIANTARG[dispparms.cArgs];
  dispparms.rgvarg = pArg;
  memset(pArg, 0, sizeof(VARIANT) * dispparms.cArgs);

  //时间转换
  DATE ftStartTime = pConv->UINT64ToVariantTime(ullStart);
  DATE ftEndTime = pConv->UINT64ToVariantTime(ullEnd);

  long  lCount = 0;
  SAFEARRAY*  psaValues = nullptr;

  //参数赋值
  dispparms.rgvarg[3].vt = VT_BYREF | VT_DATE;               // [in, out]	DATE* StartTime,
  dispparms.rgvarg[3].pdate = &ftStartTime;
  dispparms.rgvarg[2].vt = VT_BYREF | VT_DATE;               // [in, out]	DATE* EndTime,
  dispparms.rgvarg[2].pdate = &ftEndTime;
  dispparms.rgvarg[1].vt = VT_BYREF | VT_I4;                  // [out]
  dispparms.rgvarg[1].plVal = &lCount;
  dispparms.rgvarg[0].vt = VT_ARRAY | VT_BYREF | VT_VARIANT;   // [out] SAFEARRAY(VARIANT)*
  dispparms.rgvarg[0].pparray = &psaValues;

  // 获取历史事件数据
  VariantInit(&vaResult);
  hr = m_pIBMSHacDispatch->Invoke(0x08,   // SyncReadEvents 
    IID_NULL,
    LOCALE_SYSTEM_DEFAULT,
    DISPATCH_METHOD,
    &dispparms,
    &vaResult,  //无返回值
    0,
    nullptr);

  if (FAILED(hr)) return EErrCode::EC_BMS_HAC_SYNC_READ_EVT_FAILD;

  LOG_TRACE << "Event History Count = " << lCount;

  VARIANTARG vaResultItem;
  VARIANT v;
  int t = 0;
  shared_ptr<KEventInfo>  ptEvent = nullptr;
  USES_CONVERSION;

  for (t = 0; t < lCount; t++)
  {
    VariantInit(&v);
    SafeArrayGetElement(psaValues, (long*)&t, &v);
    if (v.pdispVal == nullptr) continue;

    if (v.vt == VT_DISPATCH)
    {
      ptEvent = make_shared<KEventInfo>();
      lstInfo.push_back(ptEvent);

      //参数初始化
      VariantInit(&vaResultItem);
      memset(&dispparms, 0, sizeof(DISPPARAMS)); //无参数

      // get_Source
      hr = v.pdispVal->Invoke(0x00,
        IID_NULL,
        LOCALE_SYSTEM_DEFAULT,
        DISPATCH_PROPERTYGET,
        &dispparms,
        &vaResultItem,  //返回值
        0,
        nullptr);
      if (FAILED(hr)) continue;
      if (!vaResultItem.bstrVal) continue;
      ptEvent->strUserName = W2A(vaResultItem.bstrVal);

      // get_Time
      hr = v.pdispVal->Invoke(0x02,
        IID_NULL,
        LOCALE_SYSTEM_DEFAULT,
        DISPATCH_PROPERTYGET,
        &dispparms,
        &vaResultItem,  //返回值
        0,
        nullptr);
      if (FAILED(hr)) continue;
      ptEvent->ullTimeMs = pConv->VariantTimeToUINT64(vaResultItem.date);

      // get_Message
      hr = v.pdispVal->Invoke(0x01,
        IID_NULL,
        LOCALE_SYSTEM_DEFAULT,
        DISPATCH_PROPERTYGET,
        &dispparms,
        &vaResultItem,  //返回值
        0,
        nullptr);
      if (FAILED(hr)) continue;
      if (!vaResultItem.bstrVal) continue;
      ptEvent->strRecord = W2A(vaResultItem.bstrVal);

      // get_Severity
      hr = v.pdispVal->Invoke(0x03,
        IID_NULL,
        LOCALE_SYSTEM_DEFAULT,
        DISPATCH_PROPERTYGET,
        &dispparms,
        &vaResultItem,  //返回值
        0,
        nullptr);
      if (FAILED(hr)) continue;
      ptEvent->ulAccessLevel = vaResultItem.lVal;

      // get_Type
      hr = v.pdispVal->Invoke(0x04,
        IID_NULL,
        LOCALE_SYSTEM_DEFAULT,
        DISPATCH_PROPERTYGET,
        &dispparms,
        &vaResultItem,  //返回值
        0,
        nullptr);
      if (FAILED(hr)) continue;
      ptEvent->eType = (EEventType)vaResultItem.lVal;

      // get_ActorID
      hr = v.pdispVal->Invoke(0x06,
        IID_NULL,
        LOCALE_SYSTEM_DEFAULT,
        DISPATCH_PROPERTYGET,
        &dispparms,
        &vaResultItem,  //返回值
        0,
        nullptr);
      if (FAILED(hr)) continue;
      if (!vaResultItem.bstrVal) continue;
      ptEvent->strUserName = W2A(vaResultItem.bstrVal);

      // get_EventID
      hr = v.pdispVal->Invoke(0x07,
        IID_NULL,
        LOCALE_SYSTEM_DEFAULT,
        DISPATCH_PROPERTYGET,
        &dispparms,
        &vaResultItem,  //返回值
        0,
        nullptr);
      if (FAILED(hr)) continue;
      ptEvent->ulID = vaResultItem.lVal;
    }
  }

  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode CRDBSInterface::ReadHistroyAlarm(uint64_t ullStart, uint64_t ullEnd,
  std::list<shared_ptr<KAlarmInfo>> &lstInfo)
{
  // 判断是否初始化且登录
  if (!m_bInit) return EErrCode::EC_INTERFACE_OBJ_INIT_FAILD;
  if (!m_bLogin) return EErrCode::EC_LOGGED_OUT;

  //栈内临时变量
  DISPPARAMS dispparms;
  VARIANTARG* pArg;     // 参数存放指针
  VARIANTARG vaResult;
  HRESULT hr;
  shared_ptr<CUnitConvert> pConv = CUnitConvert::Inst();

  //参数初始化
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 4;  // 4个参数

  pArg = new VARIANTARG[dispparms.cArgs];
  dispparms.rgvarg = pArg;
  memset(pArg, 0, sizeof(VARIANT) * dispparms.cArgs);

  //时间转换
  DATE ftStartTime = pConv->UINT64ToVariantTime(ullStart);
  DATE ftEndTime = pConv->UINT64ToVariantTime(ullEnd);

  long  lCount = 0;
  SAFEARRAY*  psaValues = nullptr;

  //参数赋值
  dispparms.rgvarg[3].vt = VT_BYREF | VT_DATE;               // [in, out]	DATE* StartTime,
  dispparms.rgvarg[3].pdate = &ftStartTime;
  dispparms.rgvarg[2].vt = VT_BYREF | VT_DATE;               // [in, out]	DATE* EndTime,
  dispparms.rgvarg[2].pdate = &ftEndTime;
  dispparms.rgvarg[1].vt = VT_BYREF | VT_I4;                  // [out]
  dispparms.rgvarg[1].plVal = &lCount;
  dispparms.rgvarg[0].vt = VT_ARRAY | VT_BYREF | VT_VARIANT;   // [out] SAFEARRAY(VARIANT)*
  dispparms.rgvarg[0].pparray = &psaValues;

  // 获取历史报警数据
  VariantInit(&vaResult);
  hr = m_pIBMSHacDispatch->Invoke(0x07,   // SyncReadAlarms 
    IID_NULL,
    LOCALE_SYSTEM_DEFAULT,
    DISPATCH_METHOD,
    &dispparms,
    &vaResult,  //无返回值
    0,
    nullptr);

  if (FAILED(hr)) return EErrCode::EC_BMS_HAC_SYNC_READ_ALM_FAILD;

  LOG_TRACE << "Alarm History Count = " << lCount;

  VARIANTARG vaResultItem;
  VARIANT v;
  int t = 0;
  shared_ptr<KAlarmInfo>  ptAlarm = nullptr;
  string  strDef = "默认字符串";
  USES_CONVERSION;

  for (t = 0; t < lCount; t++)
  {
    VariantInit(&v);
    SafeArrayGetElement(psaValues, (long*)&t, &v);
    if (v.pdispVal == nullptr) continue;

    if (v.vt == VT_DISPATCH)
    {
      ptAlarm = make_shared<KAlarmInfo>();
      lstInfo.push_back(ptAlarm);

      //参数初始化
      VariantInit(&vaResultItem);
      memset(&dispparms, 0, sizeof(DISPPARAMS)); //无参数

      // get_TagId
      hr = v.pdispVal->Invoke(0x01,
        IID_NULL,
        LOCALE_SYSTEM_DEFAULT,
        DISPATCH_PROPERTYGET,
        &dispparms,
        &vaResultItem,  //返回值
        0,
        nullptr);
      if (FAILED(hr)) continue;
      ptAlarm->ulGID = vaResultItem.lVal;

      // get_Time
      hr = v.pdispVal->Invoke(0x03,
        IID_NULL,
        LOCALE_SYSTEM_DEFAULT,
        DISPATCH_PROPERTYGET,
        &dispparms,
        &vaResultItem,  //返回值
        0,
        nullptr);
      if (FAILED(hr)) continue;
      ptAlarm->ullTimeMs = pConv->VariantTimeToUINT64(vaResultItem.date);

      // get_EventID
      hr = v.pdispVal->Invoke(0x08,
        IID_NULL,
        LOCALE_SYSTEM_DEFAULT,
        DISPATCH_PROPERTYGET,
        &dispparms,
        &vaResultItem,  //返回值
        0,
        nullptr);
      if (FAILED(hr)) continue;
      ptAlarm->ulID = vaResultItem.lVal;

      // get_ActorID
      hr = v.pdispVal->Invoke(0x07,
        IID_NULL,
        LOCALE_SYSTEM_DEFAULT,
        DISPATCH_PROPERTYGET,
        &dispparms,
        &vaResultItem,  //返回值
        0,
        nullptr);
      if (FAILED(hr)) continue;
      if (!vaResultItem.bstrVal) continue;
      ptAlarm->strUserName = W2A(vaResultItem.bstrVal);

      // get_Category
      hr = v.pdispVal->Invoke(0x06,
        IID_NULL,
        LOCALE_SYSTEM_DEFAULT,
        DISPATCH_PROPERTYGET,
        &dispparms,
        &vaResultItem,  //返回值
        0,
        nullptr);
      if (FAILED(hr)) continue;
      ptAlarm->ulCategory = vaResultItem.lVal;

      // get_Message
      hr = v.pdispVal->Invoke(0x02,
        IID_NULL,
        LOCALE_SYSTEM_DEFAULT,
        DISPATCH_PROPERTYGET,
        &dispparms,
        &vaResultItem,  //返回值
        0,
        nullptr);
      if (FAILED(hr)) continue;
      if (!vaResultItem.bstrVal) continue;
      ptAlarm->strDescribe = W2A(vaResultItem.bstrVal);

      // get_Severity
      hr = v.pdispVal->Invoke(0x04,
        IID_NULL,
        LOCALE_SYSTEM_DEFAULT,
        DISPATCH_PROPERTYGET,
        &dispparms,
        &vaResultItem,  //返回值
        0,
        nullptr);
      if (FAILED(hr)) continue;
      ptAlarm->ulPriority = vaResultItem.lVal;

      // get_AckRequired
      hr = v.pdispVal->Invoke(0x0C,
        IID_NULL,
        LOCALE_SYSTEM_DEFAULT,
        DISPATCH_PROPERTYGET,
        &dispparms,
        &vaResultItem,  //返回值
        0,
        nullptr);
      if (FAILED(hr)) continue;
      ptAlarm->bAck = (VARIANT_TRUE == vaResultItem.boolVal) ? true : false;

      // 填充没有的数据
      ptAlarm->strArea = strDef;
      ptAlarm->strTag = strDef;
      ptAlarm->strValue = strDef;
      ptAlarm->strAlarmEvent = strDef;
    }
  }

  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
// 获取联动监视信息
//------------------------------------------------------------------------------
EErrCode CRDBSInterface::ReadLinkageWatchInfo(list<shared_ptr<KLinkageWatchInfo>> &lstInfo)
{
  DISPPARAMS dispparms;
  VARIANTARG* pArg;
  HRESULT hr;

  //参数初始化
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 8;  //8个参数

  pArg = new VARIANTARG[dispparms.cArgs];
  dispparms.rgvarg = pArg;
  memset(pArg, 0, sizeof(VARIANT) * dispparms.cArgs);

  long  lCount = 0;
  SAFEARRAY* parrulItemID = nullptr;
  SAFEARRAY* parrbstrLinkageName = nullptr;
  SAFEARRAY* parrulTaskStatus = nullptr;
  SAFEARRAY* parrulValidStartTime = nullptr;
  SAFEARRAY* parrulValidEndTime = nullptr;
  SAFEARRAY* parrbEnable = nullptr;
  SAFEARRAY* parrbManualConfirmation = nullptr;

  //参数赋值
  dispparms.rgvarg[7].vt = VT_BYREF | VT_I4;
  dispparms.rgvarg[7].plVal = &lCount;
  dispparms.rgvarg[6].vt = VT_ARRAY | VT_BYREF | VT_I4;
  dispparms.rgvarg[6].pparray = &parrulItemID;
  dispparms.rgvarg[5].vt = VT_ARRAY | VT_BYREF | VT_BSTR;
  dispparms.rgvarg[5].pparray = &parrbstrLinkageName;
  dispparms.rgvarg[4].vt = VT_ARRAY | VT_BYREF | VT_I4;
  dispparms.rgvarg[4].pparray = &parrulTaskStatus;
  dispparms.rgvarg[3].vt = VT_ARRAY | VT_BYREF | VT_I4;
  dispparms.rgvarg[3].pparray = &parrulValidStartTime;
  dispparms.rgvarg[2].vt = VT_ARRAY | VT_BYREF | VT_I4;
  dispparms.rgvarg[2].pparray = &parrulValidEndTime;
  dispparms.rgvarg[1].vt = VT_ARRAY | VT_BYREF | VT_BOOL;
  dispparms.rgvarg[1].pparray = &parrbEnable;
  dispparms.rgvarg[0].vt = VT_ARRAY | VT_BYREF | VT_BOOL;
  dispparms.rgvarg[0].pparray = &parrbManualConfirmation;

  // Invoke the method 
  hr = m_pILinkageWatch->Invoke(0x03, // ReadLinkageWatchInfo
    IID_NULL,
    LOCALE_SYSTEM_DEFAULT,
    DISPATCH_METHOD,
    &dispparms,
    NULL,  //无返回值
    0,
    NULL);

  if (FAILED(hr)) return EErrCode::EC_BMS_READ_LINKAGE_WATCH_FAILD;

  //将接收到的数据放到集合中
  long lItemID = 0;
  BSTR bstrLinkageName = nullptr;
  long lTaskStatus = 0;
  long lValidStartTime = 0;
  long lValidEndTime = 0;
  VARIANT_BOOL vbEnable = VARIANT_FALSE;
  VARIANT_BOOL vbManualConfirmation = VARIANT_FALSE;
  shared_ptr<KLinkageWatchInfo> ptInfo = nullptr;

  USES_CONVERSION;
  for (long i = 0; i < lCount; i++)
  {
    SafeArrayGetElement(parrulItemID, (long*)&i, &lItemID);
    SafeArrayGetElement(parrbstrLinkageName, (long*)&i, &bstrLinkageName);
    SafeArrayGetElement(parrulTaskStatus, (long*)&i, &lTaskStatus);
    SafeArrayGetElement(parrulValidStartTime, (long*)&i, &lValidStartTime);
    SafeArrayGetElement(parrulValidEndTime, (long*)&i, &lValidEndTime);
    SafeArrayGetElement(parrbEnable, (long*)&i, &vbEnable);
    SafeArrayGetElement(parrbManualConfirmation, (long*)&i, &vbManualConfirmation);

    ptInfo = make_shared<KLinkageWatchInfo>();
    ptInfo->ulItemID = lItemID;
    ptInfo->strName = W2A(bstrLinkageName);
    ptInfo->ulTaskStatus = lTaskStatus;
    ptInfo->ulValidStartTime = lValidStartTime;
    ptInfo->ulValidEndTime = lValidEndTime;
    ptInfo->bEnable = (VARIANT_TRUE == vbEnable) ? true : false;
    ptInfo->bManualConfirmation = (VARIANT_TRUE == vbManualConfirmation) ? true : false;

    lstInfo.push_back(ptInfo);
  }

  //内存清理
  SafeArrayDestroy(parrulItemID);
  SafeArrayDestroy(parrbstrLinkageName);
  SafeArrayDestroy(parrulTaskStatus);
  SafeArrayDestroy(parrulValidStartTime);
  SafeArrayDestroy(parrulValidEndTime);
  SafeArrayDestroy(parrbEnable);
  SafeArrayDestroy(parrbManualConfirmation);
  delete[] pArg;

  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
// 人工确认
//------------------------------------------------------------------------------
EErrCode CRDBSInterface::LinkageManualConfirmation(uint32_t ulItemID)
{
  //栈内临时变量
  DISPPARAMS dispparms;
  VARIANTARG* pArg;      //参数存放指针
  HRESULT hr;

  //参数初始化 
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 1;

  // 对参数分配内存
  pArg = new VARIANTARG[dispparms.cArgs];
  dispparms.rgvarg = pArg;
  memset(pArg, 0, sizeof(VARIANT) * dispparms.cArgs);

  dispparms.rgvarg[0].vt = VT_I4;          //ItemID
  dispparms.rgvarg[0].lVal = ulItemID;

  //Invoke方法 
  hr = m_pILinkageWatch->Invoke(0x02, // ConfirmLSNotify
    IID_NULL,
    LOCALE_SYSTEM_DEFAULT,
    DISPATCH_METHOD,
    &dispparms,
    nullptr,
    0,
    nullptr);

  //清除参数
  delete[] pArg;
  if (FAILED(hr)) return EErrCode::EC_LINKAGE_MANUAL_CONFIRMATION_FAILD;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode CRDBSInterface::LinkageTerminateTask(uint32_t ulItemID)
{
  //栈内临时变量
  DISPPARAMS dispparms;
  VARIANTARG* pArg;      //参数存放指针
  HRESULT hr;

  //参数初始化 
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 1;

  // 对参数分配内存
  pArg = new VARIANTARG[dispparms.cArgs];
  dispparms.rgvarg = pArg;
  memset(pArg, 0, sizeof(VARIANT) * dispparms.cArgs);

  dispparms.rgvarg[0].vt = VT_I4;          //ItemID
  dispparms.rgvarg[0].lVal = ulItemID;

  //Invoke方法 
  hr = m_pILinkageWatch->Invoke(0x01, // TerminateLSNotify
    IID_NULL,
    LOCALE_SYSTEM_DEFAULT,
    DISPATCH_METHOD,
    &dispparms,
    nullptr,
    0,
    nullptr);

  //清除参数
  delete[] pArg;
  if (FAILED(hr)) return EErrCode::EC_LINKAGE_TERMINATE_TASK_FAILD;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode CRDBSInterface::ReadTimetableDispatchWatchInfo(list<shared_ptr<KTimetableDispatchWatchInfo>> &lstInfo)
{
  DISPPARAMS dispparms;
  VARIANTARG* pArg;
  HRESULT hr;

  //参数初始化
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 7;  //7个参数

  pArg = new VARIANTARG[dispparms.cArgs];
  dispparms.rgvarg = pArg;
  memset(pArg, 0, sizeof(VARIANT) * dispparms.cArgs);

  long  lCount = 0;
  SAFEARRAY* parrulItemID = nullptr;
  SAFEARRAY* parrbstrName = nullptr;
  SAFEARRAY* parrdatePreRunTime = nullptr;
  SAFEARRAY* parrdateNextRunTime = nullptr;
  SAFEARRAY* parrbEnable = nullptr;
  SAFEARRAY* parrbTerminated = nullptr;

  //参数赋值
  dispparms.rgvarg[6].vt = VT_BYREF | VT_I4;
  dispparms.rgvarg[6].plVal = &lCount;
  dispparms.rgvarg[5].vt = VT_ARRAY | VT_BYREF | VT_I4;
  dispparms.rgvarg[5].pparray = &parrulItemID;
  dispparms.rgvarg[4].vt = VT_ARRAY | VT_BYREF | VT_BSTR;
  dispparms.rgvarg[4].pparray = &parrbstrName;
  dispparms.rgvarg[3].vt = VT_ARRAY | VT_BYREF | VT_DATE;
  dispparms.rgvarg[3].pparray = &parrdatePreRunTime;
  dispparms.rgvarg[2].vt = VT_ARRAY | VT_BYREF | VT_DATE;
  dispparms.rgvarg[2].pparray = &parrdateNextRunTime;
  dispparms.rgvarg[1].vt = VT_ARRAY | VT_BYREF | VT_BOOL;
  dispparms.rgvarg[1].pparray = &parrbEnable;
  dispparms.rgvarg[0].vt = VT_ARRAY | VT_BYREF | VT_BOOL;
  dispparms.rgvarg[0].pparray = &parrbTerminated;

  // Invoke the method 
  hr = m_pDispatchWatch->Invoke(0x02, // ReadTimetableDispatchWatchInfo
    IID_NULL,
    LOCALE_SYSTEM_DEFAULT,
    DISPATCH_METHOD,
    &dispparms,
    NULL,  //无返回值
    0,
    NULL);

  if (FAILED(hr)) return EErrCode::EC_BMS_READ_TIMETABLE_DISPATCH_WATCH_FAILD;

  //将接收到的数据放到集合中
  long lItemID = 0;
  BSTR bstrName = nullptr;
  DATE datePreRunTime = 0;
  DATE dateNextRunTime = 0;
  VARIANT_BOOL vbEnable = VARIANT_FALSE;
  VARIANT_BOOL vbTerminated = VARIANT_FALSE;
  shared_ptr<KTimetableDispatchWatchInfo> ptInfo = nullptr;

  USES_CONVERSION;
  for (long i = 0; i < lCount; i++)
  {
    SafeArrayGetElement(parrulItemID, (long*)&i, &lItemID);
    SafeArrayGetElement(parrbstrName, (long*)&i, &bstrName);
    SafeArrayGetElement(parrdatePreRunTime, (long*)&i, &datePreRunTime);
    SafeArrayGetElement(parrdateNextRunTime, (long*)&i, &dateNextRunTime);
    SafeArrayGetElement(parrbEnable, (long*)&i, &vbEnable);
    SafeArrayGetElement(parrbTerminated, (long*)&i, &vbTerminated);

    ptInfo = make_shared<KTimetableDispatchWatchInfo>();
    ptInfo->ulItemID = lItemID;
    ptInfo->strName = W2A(bstrName);
    ptInfo->ullPreRunTime = CUnitConvert::Inst()->VariantTimeToUINT64(datePreRunTime);
    ptInfo->ullNextRunTime = CUnitConvert::Inst()->VariantTimeToUINT64(dateNextRunTime);
    ptInfo->bEnable = (VARIANT_TRUE == vbEnable) ? true : false;
    ptInfo->bTerminated = (VARIANT_TRUE == vbTerminated) ? true : false;

    lstInfo.push_back(ptInfo);
  }

  //内存清理
  SafeArrayDestroy(parrulItemID);
  SafeArrayDestroy(parrbstrName);
  SafeArrayDestroy(parrdatePreRunTime);
  SafeArrayDestroy(parrdateNextRunTime);
  SafeArrayDestroy(parrbEnable);
  SafeArrayDestroy(parrbTerminated);
  delete[] pArg;

  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode CRDBSInterface::TimetableDispatchTerminateTask(uint32_t ulItemID)
{
  //栈内临时变量
  DISPPARAMS dispparms;
  VARIANTARG* pArg;      //参数存放指针
  HRESULT hr;

  //参数初始化 
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 1;

  // 对参数分配内存
  pArg = new VARIANTARG[dispparms.cArgs];
  dispparms.rgvarg = pArg;
  memset(pArg, 0, sizeof(VARIANT) * dispparms.cArgs);

  dispparms.rgvarg[0].vt = VT_I4;          //ItemID
  dispparms.rgvarg[0].lVal = ulItemID;

  //Invoke方法 
  hr = m_pDispatchWatch->Invoke(0x01, // TermTask
    IID_NULL,
    LOCALE_SYSTEM_DEFAULT,
    DISPATCH_METHOD,
    &dispparms,
    nullptr,
    0,
    nullptr);

  //清除参数
  delete[] pArg;
  if (FAILED(hr)) return EErrCode::EC_TIMETABLE_DISPATCH_TERMINATE_TASK_FAILD;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
bool CRDBSInterface::CheckDBStatus(std::list<std::shared_ptr<KDBStatusInfo>> &lstInfo)
{
  // 946656000 是 2000/1/1 0:0:0
  uint64_t  ullStart = (uint64_t)946656000 * (uint64_t)1000;
  uint64_t  ullEnd = (uint64_t)946656001 * (uint64_t)1000;
  list<shared_ptr<KEventInfo>> lstEventInfo;
  EErrCode  eErr = ReadHistroyEvent(ullStart, ullEnd, lstEventInfo);
  
  // 将获取到的数据库信息写入到列表中
  shared_ptr<KDBStatusInfo> ptInfo = make_shared<KDBStatusInfo>();
  ptInfo->eStatus = (EErrCode::EC_SUCCESS == eErr) ? EDBStatus::OK : EDBStatus::UNCONNECT;
  ptInfo->strDescribe = "RDBS Database";
  lstInfo.push_back(ptInfo);

  return true;
}

//------------------------------------------------------------------------------
STDMETHODIMP CRDBSInterface::OnDataChange(/*[in]*/ LONG NumTags,
  /*[in]*/ SAFEARRAY* pTagIDs,
  /*[in]*/ SAFEARRAY* pTimes,
  /*[in]*/ SAFEARRAY* pValues)
{
  LOG_TRACE << "DataChange";
  // 判断本对象是否允许推送
  if (m_ullMark != m_pOptimize->GetUsedMark()) return S_OK;

  //无效参数
  if (NumTags <= 0 || pTagIDs == NULL || pTimes == NULL || pValues == NULL)
    return E_INVALIDARG;

  shared_ptr<CUnitConvert> pConv = CUnitConvert::Inst();
  shared_ptr<CTagDataMap> pTagMap = m_pDataCache->GetTagMap();
  shared_ptr<KTagInfo>  ptTag = nullptr;

  long lVal = 0;
  VARIANT v;
  DATE date;


  LOG_TRACE << "DataChange Count:" << NumTags;
  USES_CONVERSION;
  //保存IO记录到临时缓冲区
  for (long i = 0; i < NumTags; i++)
  {
    //初始化
    VariantInit(&v);

    SafeArrayGetElement(pTagIDs, (long*)&i, &lVal);
    SafeArrayGetElement(pTimes, (long*)&i, &date);
    SafeArrayGetElement(pValues, (long*)&i, &v);

    ptTag = make_shared<KTagInfo>();

    ptTag->ulGID = lVal;
    ptTag->ullTimeMs = pConv->VariantTimeToUINT64(date);
    ptTag->sQuality = 0xC0; // BMS_QUALITY_GOOD

    LOG_TRACE << ((ptTag->ulGID >> 24) & 0x0FF) << ":" << ((ptTag->ulGID >> 16) & 0x0FF) << ":" << (ptTag->ulGID & 0x0FFFF);

    try
    {
      switch (v.vt)
      {
      case VT_BOOL: //开关量
        ptTag->anyValue = (VARIANT_TRUE == v.boolVal) ? true : false;
        LOG_TRACE << boost::any_cast<bool>(ptTag->anyValue);
        break;

      case VT_I4:   //整形量
        ptTag->anyValue = static_cast<uint32_t>(v.lVal);
        LOG_TRACE << boost::any_cast<uint32_t>(ptTag->anyValue);
        break;

      case VT_R4:   //浮点量
        ptTag->anyValue = static_cast<float>(v.fltVal);
        LOG_TRACE << boost::any_cast<float>(ptTag->anyValue);
        break;

      case VT_BSTR: //字符串
      {
        if (v.bstrVal)
        {
          string str = W2A(v.bstrVal);
          // 必须这样子否则any不是string类型
          ptTag->anyValue = str;
          LOG_TRACE << boost::any_cast<std::string>(ptTag->anyValue);
        }
        else
          LOG_TRACE << "nullptr";
      } break;
      default:break;
      }
    }
    catch (boost::bad_any_cast &e)
    {
      LOG_ERROR << "Value Type = " << v.vt << " : " << e.what();
    }

    // 转换数据并将数据放入队列中
    pTagMap->Insert(ptTag);
  }

  LOG_DEBUG << "OnDataChange" << time(nullptr);

  LOG_TRACE << "DataChange Success";
  return S_OK;
}

//------------------------------------------------------------------------------
STDMETHODIMP CRDBSInterface::OnEventsChange(/*[in]*/ LONG EvtCount,
  /*[in]*/ SAFEARRAY* pEvtTypes,
  /*[in]*/ SAFEARRAY* pEvtIDs,
  /*[in]*/ SAFEARRAY* pUserNames,
  /*[in]*/ SAFEARRAY* pAccessLevels,
  /*[in]*/ SAFEARRAY* pEvtTimes,
  /*[in]*/ SAFEARRAY* pEvtSrc)
{
  LOG_TRACE << "EventsChange";
  // 判断本对象是否允许推送
  if (m_ullMark != m_pOptimize->GetUsedMark()) return S_OK;

  //无效参数
  if (EvtCount <= 0 || pEvtTypes == NULL || pEvtIDs == NULL
    || pUserNames == NULL || pAccessLevels == NULL
    || pEvtTimes == NULL || pEvtSrc == NULL)
    return E_INVALIDARG;

  shared_ptr<CUnitConvert> pConv = CUnitConvert::Inst();
  shared_ptr<CEventDataQueue> pEvtQueue = m_pDataCache->GetEvtQueue();
  shared_ptr<KEventInfo>  ptEvent = nullptr;

  SHORT sEvtType = 0;
  long lEvtID = 0, lAccesslevel = 0;
  BSTR bstrUserName = NULL, bstrEvtSr = NULL;
  DATE dateEvtTime;

  LOG_TRACE << "EventsChange Count:" << EvtCount;
  USES_CONVERSION;
  //保存记录到临时缓冲区
  for (long i = 0; i < EvtCount; i++)
  {
    SafeArrayGetElement(pEvtTypes, (long*)&i, &sEvtType);
    SafeArrayGetElement(pEvtIDs, (long*)&i, &lEvtID);
    SafeArrayGetElement(pAccessLevels, (long*)&i, &lAccesslevel);
    SafeArrayGetElement(pEvtTimes, (long*)&i, &dateEvtTime);
    SafeArrayGetElement(pUserNames, (long*)&i, &bstrUserName);
    SafeArrayGetElement(pEvtSrc, (long*)&i, &bstrEvtSr);

    ptEvent = make_shared<KEventInfo>();

    ptEvent->eType = (EEventType)sEvtType;  //事件类型
    ptEvent->ulID = lEvtID;                  //事件的ID
    ptEvent->ulAccessLevel = lAccesslevel;   //访问权限0-1000
    ptEvent->ullTimeMs = pConv->VariantTimeToUINT64(dateEvtTime); //事件时间

    // 为了方便释放W2A在栈上的空间
    {
      if (bstrUserName) ptEvent->strUserName = W2A(bstrUserName); //用户名
      if (bstrEvtSr) ptEvent->strRecord = W2A(bstrEvtSr);      //事件记录
    }

    //防止内存增长 2003.6
    SysFreeString(bstrUserName);
    SysFreeString(bstrEvtSr);

    // 转换数据并将数据放入队列中
    pEvtQueue->PushInto(ptEvent);
  }

  LOG_TRACE << "EventsChange Success";
  return S_OK;
}

//------------------------------------------------------------------------------
STDMETHODIMP CRDBSInterface::OnAlarmsChange(/*[in]*/ LONG AlmCount,
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
  /*[in]*/ SAFEARRAY* pReserved)
{
  LOG_TRACE << "AlarmsChange";
  // 判断本对象是否允许推送
  if (m_ullMark != m_pOptimize->GetUsedMark()) return S_OK;

  //无效参数
  if (AlmCount <= 0 || pAlmTypes == NULL || pAlmIDs == NULL
    || pTagIDs == NULL || pTagName == NULL || pTagValues == NULL
    || pAlmTimes == NULL || pAlmPriorities == NULL || pAck == NULL
    || pLimits == NULL || pStatus == NULL || pSources == NULL
    || pReserved == NULL
    )
    return E_INVALIDARG;

  shared_ptr<CUnitConvert> pConv = CUnitConvert::Inst();
  shared_ptr<CAlarmDataQueue> pAlmQueue = m_pDataCache->GetAlmQueue();
  shared_ptr<KAlarmInfo>  ptAlarm = nullptr;
  string  strDef = "默认字符串";

  SHORT sAlmType = 0;
  long lAlmID = 0, lTagID = 0;
  BSTR bstrTagName = NULL;
  VARIANT vTagValue;
  DATE dateAlmTime;
  long lAlmPriorities = 0;
  VARIANT_BOOL bAck;
  float fLimit = 0;
  VARIANT_BOOL bStatus;
  BSTR bstrSource = NULL, bstrReserved = NULL;

  LOG_TRACE << "AlarmsChange Count:" << AlmCount;
  USES_CONVERSION;
  //保存记录到临时缓冲区
  for (long i = 0; i < AlmCount; i++)
  {
    //初始化
    VariantInit(&vTagValue);

    SafeArrayGetElement(pAlmTypes, (long*)&i, &sAlmType);
    SafeArrayGetElement(pAlmIDs, (long*)&i, &lAlmID);
    SafeArrayGetElement(pTagIDs, (long*)&i, &lTagID);
    SafeArrayGetElement(pTagName, (long*)&i, &bstrTagName);
    SafeArrayGetElement(pTagValues, (long*)&i, &vTagValue);
    SafeArrayGetElement(pAlmTimes, (long*)&i, &dateAlmTime);
    SafeArrayGetElement(pAlmPriorities, (long*)&i, &lAlmPriorities);
    SafeArrayGetElement(pAck, (long*)&i, &bAck);
    SafeArrayGetElement(pLimits, (long*)&i, &fLimit);
    SafeArrayGetElement(pStatus, (long*)&i, &bStatus);
    SafeArrayGetElement(pSources, (long*)&i, &bstrSource);
    SafeArrayGetElement(pReserved, (long*)&i, &bstrReserved);

    ptAlarm = make_shared<KAlarmInfo>();

    ptAlarm->ulGID = lTagID;  // 变量的全局ID
    ptAlarm->ullTimeMs = pConv->VariantTimeToUINT64(dateAlmTime); // 采集时间
    ptAlarm->ulID = lAlmID; // 报警的ID

    // 为了方便释放W2A在栈上的空间
    {
      if (bstrTagName) ptAlarm->strUserName = W2A(bstrTagName); // 变量名
      if (bstrReserved) ptAlarm->strDescribe = W2A(bstrReserved); // 备用描述
    }

    ptAlarm->ulCategory = sAlmType; // 报警类型
    ptAlarm->ulPriority = lAlmPriorities; // 报警的等级
    ptAlarm->bAck = (VARIANT_TRUE == bAck) ? true : false; // 是否应答  

    // 填充没有的数据
    ptAlarm->strArea = strDef;
    ptAlarm->strTag = strDef;
    ptAlarm->strValue = strDef;
    ptAlarm->strAlarmEvent = strDef;

    //防止内存增长 2003.6
    SysFreeString(bstrTagName);
    SysFreeString(bstrSource);
    SysFreeString(bstrReserved);

    // 转换数据并将数据放入队列中
    pAlmQueue->PushInto(ptAlarm);
  }

  LOG_TRACE << "AlarmsChange Success";
  return S_OK;
}

//------------------------------------------------------------------------------
STDMETHODIMP CRDBSInterface::ShutdownRequest(/*[in]*/BSTR bstrReason)
{
  LOG_DEBUG << "ShutdownRequest " << time(nullptr);
  LOG_DEBUG << CUnitConvert::Inst()->WStringToString(bstrReason);

  Uninit();
  return S_OK;
}

//------------------------------------------------------------------------------
STDMETHODIMP CRDBSInterface::IsActive(/*[out, retval]*/VARIANT_BOOL* bActive)
{
  LOG_TRACE << "IsActive";
  *bActive = true;
  cout << "IsActive " << time(nullptr) << endl;
  return S_OK;
}

