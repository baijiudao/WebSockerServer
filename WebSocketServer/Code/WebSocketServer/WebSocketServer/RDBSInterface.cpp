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

  // ɾ��Mark
  if (m_pOptimize) m_pOptimize->DelMark(m_ullMark);
}

//#define _TEST_INTERFACE_
#ifdef _TEST_INTERFACE_
#else
#define _PARAM_CODE_
#endif // _TEST_INTERFACE_

//------------------------------------------------------------------------------
// �ڲ����Ժ���
//------------------------------------------------------------------------------
void CRDBSInterface::Test()
{
#ifdef _TEST_INTERFACE_
  DISPPARAMS dispparms;
  VARIANTARG* pArg;
  HRESULT hr;

  //������ʼ��
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 2;  //2������

  pArg = new VARIANTARG[dispparms.cArgs];
  dispparms.rgvarg = pArg;
  memset(pArg, 0, sizeof(VARIANT) * dispparms.cArgs);

  long nNumTags = 0;
  SAFEARRAY* pIDs = NULL;

  //������ֵ
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
    NULL,  //�޷���ֵ
    0,
    NULL);

  if (FAILED(hr))
  {
    cout << "fail..." << endl;
  }

  //�����յ������ݷŵ�������
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
//   //�ڴ�����
//   SafeArrayDestroy(pTagIDs);
//   SafeArrayDestroy(pValues);
//   SafeArrayDestroy(pTimes);
//   SafeArrayDestroy(pQualities);
//   SafeArrayDestroy(pErr);
  delete[] pArg;
  //VariantClear(&v); //����ڴ�
#else
  
#endif // _TEST_INTERFACE_

}

//------------------------------------------------------------------------------
EErrCode CRDBSInterface::OnLogin(const string &strAccount, const string &strPassword)
{
  //ջ����ʱ����
  DISPPARAMS dispparms;
  VARIANTARG* pArg;      //�������ָ��
  VARIANTARG vaResult;   //Variant����ֵ
  HRESULT hr;

  CString csAcc(strAccount.c_str());
  CString csPwd(strPassword.c_str());

  // 1������IBMSServer��Login
  //������ʼ�� 
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 2;

  // �Բ��������ڴ�
  pArg = new VARIANTARG[dispparms.cArgs];
  dispparms.rgvarg = pArg;
  memset(pArg, 0, sizeof(VARIANT) * dispparms.cArgs);

  dispparms.rgvarg[0].vt = VT_BSTR;          //Password
  dispparms.rgvarg[0].bstrVal = csPwd.AllocSysString();
  dispparms.rgvarg[1].vt = VT_BSTR;          //UserName
  dispparms.rgvarg[1].bstrVal = csAcc.AllocSysString();

  //����ֵ��ʼ��
  VariantInit(&vaResult);

  //Invoke���� 
  hr = m_pIBMSServerDispatch->Invoke(0x03, // Login
    IID_NULL,
    LOCALE_SYSTEM_DEFAULT,
    DISPATCH_METHOD,
    &dispparms,
    &vaResult, //��������
    0,
    nullptr);

  //�������
  delete[] pArg;
  if (FAILED(hr)) return EErrCode::EC_BMS_SERVER_LOGIN_FAILD;

  //���ݷ���ֵ��д����״̬
  if (vaResult.vt == VT_I2)
  {
    switch (vaResult.iVal)
    {
    case 1: // ��¼�ɹ�,����ǰ�û�δע��
    case 0: break; // ��¼�ɹ�
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

  // ����IBMSServer��Logout
  if (m_pIBMSServerDispatch)
  {
    // ��������
    memset(&dispparms, 0, sizeof(DISPPARAMS));
    dispparms.cArgs = 0; //�޲���

    //Invoke���� 
    HRESULT hr = m_pIBMSServerDispatch->Invoke(0x04,
      IID_NULL,
      LOCALE_SYSTEM_DEFAULT,
      DISPATCH_METHOD,
      &dispparms,
      nullptr, //�޲�������
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
  VARIANTARG* pArg;      //�������ָ��
  VARIANTARG vaResult;   //Variant����ֵ

  // ������
  CString csHostName(strHostName.c_str());
  CString csHostNameRcx = csHostName + _T(".RCX");

  // ����BMSServer��ʵ��
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

  // ��ȡBMSServer��ʵ��
  hr = mqi[0].pItf->QueryInterface(IID_IDispatch, (void**)&m_pIBMSServerDispatch);
  if (FAILED(hr)) return EErrCode::EC_BMS_QUERY_INTERFACE_FAILD;
  if (!m_pIBMSServerDispatch) return EErrCode::EC_BMS_SERVER_OBJ_UNINVALID;
  mqi[0].pItf->Release();

  // ����IBMSServer��Initialize����
  //������ʼ��
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 1;

  //�Բ��������ڴ�
  pArg = new VARIANTARG[dispparms.cArgs];
  dispparms.rgvarg = pArg;
  memset(pArg, 0, sizeof(VARIANT) * dispparms.cArgs);

  //��ֵ
  dispparms.rgvarg[0].vt = VT_BSTR;
  dispparms.rgvarg[0].bstrVal = csHostNameRcx.AllocSysString();

  //����ֵ��ʼ�� 
  VariantInit(&vaResult);

  // Invoke ����
  hr = m_pIBMSServerDispatch->Invoke(0x01, //Initialize
    IID_NULL,
    LOCALE_SYSTEM_DEFAULT,
    DISPATCH_METHOD,
    &dispparms,
    &vaResult,
    0,
    nullptr);

  //�������
  delete[] pArg;
  if (FAILED(hr)) return EErrCode::EC_BMS_SERVER_INIT_FAILD;

  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode CRDBSInterface::OnRcxDataInit()
{
  DISPPARAMS dispparms;
  VARIANTARG* pArg;      //�������ָ��
  VARIANTARG vaResult;   //Variant����ֵ
  HRESULT hr;

  // ���� IBMSServer��GetAccessObject ������ȡIBMSRcxData����
  //������ʼ��
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 2;  //2������

  //����: �����ڴ�
  pArg = new VARIANTARG[dispparms.cArgs];
  dispparms.rgvarg = pArg;
  memset(pArg, 0, sizeof(VARIANT) * dispparms.cArgs);

  //����: ��ֵ
  IUnknown* pUnk = nullptr;
  dispparms.rgvarg[0].vt = VT_BYREF | VT_UNKNOWN;    // [out] IUnknown** ppUnk
  dispparms.rgvarg[0].ppunkVal = &pUnk;
  dispparms.rgvarg[1].vt = VT_I4;                    // [in] long objId 
  dispparms.rgvarg[1].lVal = RDBS_ACCESS_RCX;

  //���ز�����ʼ�� 
  VariantInit(&vaResult);

  // Invoke ����
  hr = m_pIBMSServerDispatch->Invoke(0x05,  //GetAccessObject
    IID_NULL,
    LOCALE_SYSTEM_DEFAULT,
    DISPATCH_METHOD,
    &dispparms,
    &vaResult,
    0,
    nullptr);

  //��������ڴ� 
  delete[] pArg;
  if (FAILED(hr) || pUnk == nullptr) return EErrCode::EC_BMS_SERVER_GET_ACCESS_OBJ_FAILD;

  //���ݷ���ֵ��д����״̬
  if (vaResult.vt == VT_I2)
  {
    switch (vaResult.iVal)
    {
    case 0: break; //��¼�ɹ�
    case -1: return EErrCode::EC_NOT_LOGIN;
    case -2: return EErrCode::EC_UNAUTHORIZED_ACCESS;
    default:return EErrCode::EC_UNKNOW;
    }
  }

  // ��ȡIBMRcxData����
  hr = pUnk->QueryInterface(IID_IDispatch, (void**)&m_pIBMSRcxDataDispatch);
  if (FAILED(hr)) return EErrCode::EC_BMS_QUERY_INTERFACE_FAILD;
  if (!m_pIBMSRcxDataDispatch) return EErrCode::EC_BMS_RCX_OBJ_UNINVALID;
  pUnk->Release();

  // ���� IBMSRcxData��Initialize ����
  // ������ʼ��
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 0; 	   //�޲���

  //����ֵ��ʼ��
  VariantInit(&vaResult);

  // Invoke ����
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
  VARIANTARG* pArg;      //�������ָ��
  VARIANTARG vaResult;   //Variant����ֵ
  HRESULT hr;

  // ���� IBMSServer��GetAccessObject������ȡIBMSHac����
  //������ʼ��
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 2;  //2������

  //����: �����ڴ�
  pArg = new VARIANTARG[dispparms.cArgs];
  dispparms.rgvarg = pArg;
  memset(pArg, 0, sizeof(VARIANT) * dispparms.cArgs);

  //����: ��ֵ
  IUnknown* pUnk = nullptr;
  dispparms.rgvarg[0].vt = VT_BYREF | VT_UNKNOWN;    // [out] IUnknown** ppUnk
  dispparms.rgvarg[0].ppunkVal = &pUnk;
  dispparms.rgvarg[1].vt = VT_I4;					 // [in] long objId 
  dispparms.rgvarg[1].lVal = RDBS_ACCESS_HAC;

  //���ز�����ʼ�� 
  VariantInit(&vaResult);

  // Invoke ����
  hr = m_pIBMSServerDispatch->Invoke(0x05,  //GetAccessObject
    IID_NULL,
    LOCALE_SYSTEM_DEFAULT,
    DISPATCH_METHOD,
    &dispparms,
    &vaResult,
    0,
    nullptr);

  //��������ڴ� 
  delete[] pArg;
  if (FAILED(hr) || pUnk == nullptr) return EErrCode::EC_BMS_SERVER_GET_ACCESS_OBJ_FAILD;

  //���ݷ���ֵ��д����״̬
  if (vaResult.vt == VT_I2)
  {
    switch (vaResult.iVal)
    {
    case 0: break; //��¼�ɹ�
    case -1: return EErrCode::EC_NOT_LOGIN;
    case -2: return EErrCode::EC_UNAUTHORIZED_ACCESS;
    default:return EErrCode::EC_UNKNOW;
    }
  }

  // ��ȡBMSHac����
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
  VARIANTARG* pArg;      //�������ָ��
  VARIANTARG vaResult;   //Variant����ֵ
  HRESULT hr;

  // ���� IBMSServer��GetAccessObject������ȡLinkageWatch����
  //������ʼ��
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 2;  //2������

  //����: �����ڴ�
  pArg = new VARIANTARG[dispparms.cArgs];
  dispparms.rgvarg = pArg;
  memset(pArg, 0, sizeof(VARIANT) * dispparms.cArgs);

  //����: ��ֵ
  IUnknown* pUnk = nullptr;
  dispparms.rgvarg[0].vt = VT_BYREF | VT_UNKNOWN;    // [out] IUnknown** ppUnk
  dispparms.rgvarg[0].ppunkVal = &pUnk;
  dispparms.rgvarg[1].vt = VT_I4;					 // [in] long objId 
  dispparms.rgvarg[1].lVal = RDBS_ACCESS_LINKAGE_WATCH;

  //���ز�����ʼ�� 
  VariantInit(&vaResult);

  // Invoke ����
  hr = m_pIBMSServerDispatch->Invoke(0x05,  //GetAccessObject
    IID_NULL,
    LOCALE_SYSTEM_DEFAULT,
    DISPATCH_METHOD,
    &dispparms,
    &vaResult,
    0,
    nullptr);

  //��������ڴ� 
  delete[] pArg;
  if (FAILED(hr) || pUnk == nullptr) return EErrCode::EC_BMS_SERVER_GET_ACCESS_OBJ_FAILD;

  //���ݷ���ֵ��д����״̬
  if (vaResult.vt == VT_I2)
  {
    switch (vaResult.iVal)
    {
    case 0: break; //��¼�ɹ�
    case -1: return EErrCode::EC_NOT_LOGIN;
    case -2: return EErrCode::EC_UNAUTHORIZED_ACCESS;
    default:return EErrCode::EC_UNKNOW;
    }
  }

  // ��ȡLinkageWatch����
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
  VARIANTARG* pArg;      //�������ָ��
  VARIANTARG vaResult;   //Variant����ֵ
  HRESULT hr;

  // ���� IBMSServer��GetAccessObject������ȡTimetableDispatchWatch����
  //������ʼ��
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 2;  //2������

  //����: �����ڴ�
  pArg = new VARIANTARG[dispparms.cArgs];
  dispparms.rgvarg = pArg;
  memset(pArg, 0, sizeof(VARIANT) * dispparms.cArgs);

  //����: ��ֵ
  IUnknown* pUnk = nullptr;
  dispparms.rgvarg[0].vt = VT_BYREF | VT_UNKNOWN;    // [out] IUnknown** ppUnk
  dispparms.rgvarg[0].ppunkVal = &pUnk;
  dispparms.rgvarg[1].vt = VT_I4;					 // [in] long objId 
  dispparms.rgvarg[1].lVal = RDBS_ACCESS_TIMETABLE_DISPATCH_WATCH;

  //���ز�����ʼ�� 
  VariantInit(&vaResult);

  // Invoke ����
  hr = m_pIBMSServerDispatch->Invoke(0x05,  //GetAccessObject
    IID_NULL,
    LOCALE_SYSTEM_DEFAULT,
    DISPATCH_METHOD,
    &dispparms,
    &vaResult,
    0,
    nullptr);

  //��������ڴ� 
  delete[] pArg;
  if (FAILED(hr) || pUnk == nullptr) return EErrCode::EC_BMS_SERVER_GET_ACCESS_OBJ_FAILD;

  //���ݷ���ֵ��д����״̬
  if (vaResult.vt == VT_I2)
  {
    switch (vaResult.iVal)
    {
    case 0: break; //��¼�ɹ�
    case -1: return EErrCode::EC_NOT_LOGIN;
    case -2: return EErrCode::EC_UNAUTHORIZED_ACCESS;
    default:return EErrCode::EC_UNKNOW;
    }
  }

  // ��ȡTimetableDispatchWatch����
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

  // ���� IBMSServer��Uninitialize ����
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

    //�ͷŽӿ�	
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

  // ���� IBMSRcxData��Uninitialize ����
  if (m_pIBMSRcxDataDispatch)
  {
    // ������ʼ��
    VariantInit(&vaResult);
    memset(&dispparms, 0, sizeof(DISPPARAMS));

    //�޲���

    // Invoke ����
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

    //�ͷŽӿ�
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
// �������Լ���ָ�봫�ݸ�����ˣ��÷���˵Ļص����е���
//------------------------------------------------------------------------------
EErrCode CRDBSInterface::OnSetSinkPointer()
{
  DISPPARAMS dispparms;
  VARIANTARG* pArg;
  HRESULT hr;

  IUnknown* pUnkIBMSCallBack = static_cast<IBMSCallBack*>(this);
  IUnknown* pUnkIBMSShutdown = static_cast<IBMSShutdown*>(this);
  IUnknown* pUnkIBMSConnectionVerify = static_cast<IBMSConnectionVerify*>(this);

  //������ʼ��
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 1;  //1������

  pArg = new VARIANTARG[dispparms.cArgs];
  dispparms.rgvarg = pArg;
  memset(pArg, 0, sizeof(VARIANT) * dispparms.cArgs);

  //������ȫ����,����ֵ	
  SAFEARRAY* psa;
  SAFEARRAYBOUND bound = { 3, 0 };
  psa = SafeArrayCreate(VT_UNKNOWN, 1, &bound);
  if (psa == nullptr) return EErrCode::EC_BMS_CREATE_ARR_FAILD;

  //��Ԫ�ؼӵ�������
  int i = 0;
  SafeArrayPutElement(psa, (long*)&i, pUnkIBMSCallBack);
  i++;
  SafeArrayPutElement(psa, (long*)&i, pUnkIBMSShutdown);
  i++;
  SafeArrayPutElement(psa, (long*)&i, pUnkIBMSConnectionVerify);

  //������ֵ
  dispparms.rgvarg[0].vt = VT_ARRAY | VT_UNKNOWN;// [in] SAFEARRAY(IUnknown*) pUnks
  dispparms.rgvarg[0].parray = psa;

  // Invoke ����
  hr = m_pIBMSRcxDataDispatch->Invoke(0x03,//SetSinkPointers
    IID_NULL,
    LOCALE_SYSTEM_DEFAULT,
    DISPATCH_METHOD,
    &dispparms,
    nullptr,  //�޷���ֵ
    0,
    nullptr);

  LOG_DEBUG << "==>" << 1;
  //�ڴ�����
  // �˴�ע�͵����������������Լ��ͷ��Լ����������⣬���޽������
  //SafeArrayDestroy(psa);
  delete[] pArg;

  LOG_DEBUG << "==>" << 2;
  if (FAILED(hr)) LOG_DEBUG << "SetSinkPointers Fail";
  if (FAILED(hr)) return EErrCode::EC_BMS_RCX_SET_SINK_PTR_FAILD;
  LOG_DEBUG << "SetSinkPointers Success";
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
// ʲô������ȡ��ֻ�ǵ����·���˵�GetAllTags�ӿڣ��Ա�ص��ں����ܳɹ�����
//------------------------------------------------------------------------------
EErrCode CRDBSInterface::OnGetNull()
{
  DISPPARAMS dispparms;
  VARIANTARG* pArg;
  HRESULT hr;

  //������ʼ��
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 6;  //2������

  pArg = new VARIANTARG[dispparms.cArgs];
  dispparms.rgvarg = pArg;
  memset(pArg, 0, sizeof(VARIANT) * dispparms.cArgs);

  long nNumTags = 0;
  SAFEARRAY* pTagIDs = NULL;
  SAFEARRAY* pValues = NULL;
  SAFEARRAY* pTimes = NULL;
  SAFEARRAY* pQualities = NULL;
  SAFEARRAY* pErr = NULL;

  //������ֵ
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
    NULL,  //�޷���ֵ
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

  //������ʼ��
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 6;  //2������

  pArg = new VARIANTARG[dispparms.cArgs];
  dispparms.rgvarg = pArg;
  memset(pArg, 0, sizeof(VARIANT) * dispparms.cArgs);

  long nNumTags = 0;
  SAFEARRAY* pTagIDs = NULL;
  SAFEARRAY* pValues = NULL;
  SAFEARRAY* pTimes = NULL;
  SAFEARRAY* pQualities = NULL;
  SAFEARRAY* pErr = NULL;

  //������ֵ
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
    NULL,  //�޷���ֵ
    0,
    NULL);

  if (FAILED(hr)) return EErrCode::EC_BMS_RCX_GET_ALL_TAG_FAILD;

  //�����յ������ݷŵ�������
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

    if (lErr == 0) //�ɹ�
    {
      switch (v.vt)
      {
      case VT_BOOL: //������
        ptTag->anyValue = (VARIANT_TRUE == v.boolVal) ? true : false;
        break;

      case VT_I4:   //������
        ptTag->anyValue = v.lVal;
        break;

      case VT_R4:   //������
        ptTag->anyValue = v.fltVal;
        break;

      case VT_BSTR: //�ַ���
      {
        if (v.bstrVal)
        {
          std::string str = W2A(v.bstrVal);
          // �����Ӹ�ֵany�б���Ĳ���string����
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

  //�ڴ�����
  SafeArrayDestroy(pTagIDs);
  SafeArrayDestroy(pValues);
  SafeArrayDestroy(pTimes);
  SafeArrayDestroy(pQualities);
  SafeArrayDestroy(pErr);
  delete[] pArg;
  //VariantClear(&v); //����ڴ�

  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
//              [Զ�̵�¼]  [���б�����֤] [RDBS��¼�����¼]   [�������]    [��ȡ��ʷ����]
//               �û���֤     ����Ȩ��    ONLINECONFIG        TASKSCHEDULE    WARNINGEVENT
//                 ERR        LEVEL         LEVEL              LEVEL          LEVEL
//����Ա   GLY      0          1             1                  1             1     AAA     TTT
//������� RWDD     0          0            -1                  1            -1     CAC     FTF
//ȫ�ֿ��� QJKZ     0          1            -1                 -1             0     CCB     FFT
//ȫ�ּ��� QJJS     0          0             0                  0             0     BBB     FFT
//��ʷ��¼ LSJL     0          0            -1                 -1            -1     CCC     FFF
//�����¼� BJSJ     0          0            -1                 -1             1     CCA     FFT
//                                      1��ʾ��Ȩ��¼     1��ʾ��Ȩ����  ������-1��ʾ��Ȩ 
//
// ������ȡ����ĳ��������Ȩ��
//------------------------------------------------------------------------------
EErrCode CRDBSInterface::OnGetFunctionAuthority(int32_t &lOnlineConfig,
  int32_t &lTaskSchedule, int32_t &lWarningEvent)
{
  DISPPARAMS dispparms;
  VARIANTARG* pArg;
  HRESULT hr;

  //������ʼ��
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 3;

  pArg = new VARIANTARG[dispparms.cArgs];
  dispparms.rgvarg = pArg;
  memset(pArg, 0, sizeof(VARIANT) * dispparms.cArgs);

  //������ֵ
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
    NULL,  //�޷���ֵ
    0,
    NULL);

  if (FAILED(hr)) return EErrCode::EC_BMS_GET_FUNC_AUTHORITY;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
// ��ʱ������IP����Ӧ���þ�������������
//------------------------------------------------------------------------------
EErrCode CRDBSInterface::Init(const string &strHostName,
  shared_ptr<CDataCache>  pDataCache, shared_ptr<COptimize> pOptimize)
{
  if (strHostName.empty()) return EErrCode::EC_PARAM_UNINVALID;
  if (!pDataCache || !pOptimize) return EErrCode::EC_RT_QUE_UNINVALID;

  // ����Ѿ���ʼ�����򷵻سɹ�
  if (m_bInit) return EErrCode::EC_SUCCESS;
  m_pDataCache = pDataCache;

  EErrCode eErr = EErrCode::EC_UNKNOW;
  eErr = OnServerInit(strHostName);
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  // ����һ��Mark
  m_pOptimize = pOptimize;
  m_ullMark = m_pOptimize->CreMark();

  // ��ʼ���ɹ�
  m_bInit = true;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode CRDBSInterface::Uninit()
{
  // ���δ��ʼ�����򷵻سɹ�
  if (!m_bInit) return EErrCode::EC_SUCCESS;
  EErrCode eErr = EErrCode::EC_UNKNOW;

  // ����Ѿ���¼�����˳���¼
  if (m_bLogin)
  {
    // �˳���¼
    eErr = Logout();
    if (EErrCode::EC_SUCCESS != eErr) return eErr;
    m_bLogin = false;
  }

  // BMSServer��uninit
  eErr = OnServerUninit();
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  // ����ʼ���ɹ�
  m_bInit = false;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode CRDBSInterface::Login(const string &strAccount,
  const string &strPassword, EUserType &eType)
{
  if (strAccount.empty() || strPassword.empty()) return EErrCode::EC_PARAM_UNINVALID;

  // ���û�г�ʼ�����򷵻�ʧ��
  if (!m_bInit) return EErrCode::EC_INTERFACE_OBJ_INIT_FAILD;
  // ����Ѿ��ɹ���¼���򷵻سɹ�
  if (m_bLogin) return EErrCode::EC_SUCCESS;

  EErrCode eErr = EErrCode::EC_UNKNOW;
  eErr = OnLogin(strAccount, strPassword);
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  eErr = OnRcxDataInit();
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  eErr = OnHacInit();
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  // ��ȡlinkage watch����
  eErr = OnLinkageWatchInit();
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  // ��ȡTimetable Dispatch watch����
  eErr = OnTimetableDispatchWatchInit();
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

#ifdef _CALL_BACK_
  // ���ͷ���ǰ�Ľӿڵ���
  eErr = OnGetNull();
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  // ������������Ҫ�����͵�ָ�뷢�͸�������
  eErr = OnSetSinkPointer();
  if (EErrCode::EC_SUCCESS != eErr) return eErr;
#endif // _CALL_BACK_

  // ��ȡ��һЩ�����Ĳ���Ȩ�ޣ������ж��û�����
  int32_t li, lj, lk;
  eErr = OnGetFunctionAuthority(li, lj, lk);
  if (EErrCode::EC_SUCCESS != eErr) return eErr;
  if ((1 == li) && (1 == lj) && (1 == lk))
    eType = EUserType::ADMINISTRATORS;
  else
    eType = EUserType::GENERAL;

  // ��¼�ɹ�
  m_bLogin = true;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode CRDBSInterface::Logout()
{
  // ���û�г�ʼ�����򷵻�ʧ��
  if (!m_bInit) return EErrCode::EC_INTERFACE_OBJ_INIT_FAILD;
  // ���û�е�¼���򷵻سɹ�
  if (!m_bLogin) return EErrCode::EC_SUCCESS;

  EErrCode eErr = EErrCode::EC_UNKNOW;
  eErr = OnRcxDataUninit();
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  eErr = OnHacUninit();
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  eErr = OnLogout();
  if (EErrCode::EC_SUCCESS != eErr) return eErr;

  // �ǳ��ɹ�
  m_bLogin = false;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode CRDBSInterface::ReadTag(list<shared_ptr<KTagInfo>> &lstInfo)
{
  // �ж��Ƿ��ʼ���ҵ�¼
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

    // �����ݸ��ƹ�ȥ
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
  // �ж��Ƿ��ʼ���ҵ�¼
  if (!m_bInit) return EErrCode::EC_INTERFACE_OBJ_INIT_FAILD;
  if (!m_bLogin) return EErrCode::EC_LOGGED_OUT;

  int nNum = lstInfo.size();
  if (nNum <= 0) return EErrCode::EC_SUCCESS;

  shared_ptr<CUnitConvert> pConv = CUnitConvert::Inst();
  DISPPARAMS dispparms;
  VARIANTARG* pArg;      //�������ָ��
  HRESULT hr;

  // Set up the parameters
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 4;  //����

  // allocate memory for parameters
  pArg = new VARIANTARG[dispparms.cArgs];
  dispparms.rgvarg = pArg;
  memset(pArg, 0, sizeof(VARIANT) * dispparms.cArgs);

  //����pValues��ȫ����,����ֵ	
  SAFEARRAY* psaErrs = NULL;  //����Errs
  SAFEARRAYBOUND rgsabound[1];
  rgsabound[0].cElements = nNum;
  rgsabound[0].lLbound = 0;

  // Creating a SafeArray with 1 element.
  SAFEARRAY* psaTagID = SafeArrayCreate(VT_I4, 1, rgsabound);
  SAFEARRAY* psaVal = SafeArrayCreate(VT_VARIANT, 1, rgsabound);
  if (psaTagID == NULL || psaVal == NULL) return EErrCode::EC_BMS_CREATE_ARR_FAILD;

  //��Ԫ�ؼӵ�������
  long i = 0;
  VARIANT v;
  DWORD dwGID = 0;

  for (auto iter = lstInfo.begin(); iter != lstInfo.end(); iter++, i++)
  {
    VariantInit(&v);
    //��Ԫ�ؼӵ�������
    SafeArrayPutElement(psaTagID, (long*)&i, &(*iter)->ulGID);

    //������ֵ
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
  dispparms.rgvarg[3].lVal = nNum;                      // ÿ��дnNum��Tag
  dispparms.rgvarg[2].vt = VT_ARRAY | VT_I4;           //[in] SAFEARRAY(long) pTagIDs, 
  dispparms.rgvarg[2].parray = psaTagID;
  dispparms.rgvarg[1].vt = VT_ARRAY | VT_VARIANT;      //[in] SAFEARRAY(VARIANT) pValues,
  dispparms.rgvarg[1].parray = psaVal;
  dispparms.rgvarg[0].vt = VT_ARRAY | VT_BYREF | VT_I4;  //[out] SAFEARRAY(long)* ppErrs
  dispparms.rgvarg[0].pparray = &psaErrs;

  // ���� IBMSRcxData��WriteTags ����
  hr = m_pIBMSRcxDataDispatch->Invoke(0x04, //WriteTags
    IID_NULL,
    LOCALE_SYSTEM_DEFAULT,
    DISPATCH_METHOD,
    &dispparms,
    NULL, //�޷��ز���
    0,
    NULL);


  //�����ڴ�
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
  // �ж��Ƿ��ʼ���ҵ�¼
  if (!m_bInit) return EErrCode::EC_INTERFACE_OBJ_INIT_FAILD;
  if (!m_bLogin) return EErrCode::EC_LOGGED_OUT;
  int nNum = lstInfo.size();
  if (nNum <= 0) return EErrCode::EC_SUCCESS;

  shared_ptr<CUnitConvert> pConv = CUnitConvert::Inst();
  DISPPARAMS dispparms;
  VARIANTARG* pArg;      //�������ָ��
  HRESULT hr;

  // Set up the parameters
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 10;  //����

  // allocate memory for parameters
  pArg = new VARIANTARG[dispparms.cArgs];
  dispparms.rgvarg = pArg;
  memset(pArg, 0, sizeof(VARIANT) * dispparms.cArgs);

  //����pValues��ȫ����,����ֵ	
  SAFEARRAYBOUND rgsabound[1];
  rgsabound[0].cElements = nNum;
  rgsabound[0].lLbound = 0;

  // ��ÿ���¼���д��
  for (auto iter = lstInfo.begin(); iter != lstInfo.end(); iter++)
  {
    BSTR  bstrRecord = _bstr_t((*iter)->strRecord.c_str()).copy();
    BSTR  bstrUserName = _bstr_t((*iter)->strUserName.c_str()).copy();

    SYSTEMTIME  stTime;
    DATE    daTime;
    GetLocalTime(&stTime);
    SystemTimeToVariantTime(&stTime, &daTime);

    // ����A2W��ʱ�ڴ��ͷ�
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

    // ���� IBMSRcxData��WriteTags ����
    hr = m_pIBMSRcxDataDispatch->Invoke(0x05, //WriteEvent
      IID_NULL,
      LOCALE_SYSTEM_DEFAULT,
      DISPATCH_METHOD,
      &dispparms,
      NULL, //�޷��ز���
      0,
      NULL);

    // �ͷŴ洢
    SysFreeString(bstrRecord);
    SysFreeString(bstrUserName);

    // ִ��ʧ�ܾ�����ѭ��
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
  // �ж��Ƿ��ʼ���ҵ�¼
  if (!m_bInit) return EErrCode::EC_INTERFACE_OBJ_INIT_FAILD;
  if (!m_bLogin) return EErrCode::EC_LOGGED_OUT;

  // ����û�Ȩ��
  int32_t lOnlineConfig, lTaskSchedule, lWarningEvent;
  EErrCode  eErr = OnGetFunctionAuthority(lOnlineConfig, lTaskSchedule, lWarningEvent);
  if (-1 == lWarningEvent) return EErrCode::EC_NO_PERMISSION;

  //ջ����ʱ����
  DISPPARAMS dispparms;
  VARIANTARG* pArg;     // �������ָ��
  VARIANTARG vaResult;
  HRESULT hr;

  // ת��������
  shared_ptr<CUnitConvert> pConv = CUnitConvert::Inst();

  //������ʼ��
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 4;  // 4������

  pArg = new VARIANTARG[dispparms.cArgs];
  dispparms.rgvarg = pArg;
  memset(pArg, 0, sizeof(VARIANT) * dispparms.cArgs);

  //ʱ��ת��
  DATE ftStartTime = pConv->UINT64ToVariantTime(ullStart);
  DATE ftEndTime = pConv->UINT64ToVariantTime(ullEnd);

  long  lCount = 0;
  SAFEARRAY*  psaValues = nullptr;

  //������ֵ
  dispparms.rgvarg[3].vt = VT_BYREF | VT_DATE;               // [in, out]	DATE* StartTime,
  dispparms.rgvarg[3].pdate = &ftStartTime;
  dispparms.rgvarg[2].vt = VT_BYREF | VT_DATE;               // [in, out]	DATE* EndTime,
  dispparms.rgvarg[2].pdate = &ftEndTime;
  dispparms.rgvarg[1].vt = VT_BYREF | VT_I4;                  // [out]
  dispparms.rgvarg[1].plVal = &lCount;
  dispparms.rgvarg[0].vt = VT_ARRAY | VT_BYREF | VT_VARIANT;   // [out] SAFEARRAY(VARIANT)*
  dispparms.rgvarg[0].pparray = &psaValues;

  // ��ȡ��ʷ�¼�����
  VariantInit(&vaResult);
  hr = m_pIBMSHacDispatch->Invoke(0x08,   // SyncReadEvents 
    IID_NULL,
    LOCALE_SYSTEM_DEFAULT,
    DISPATCH_METHOD,
    &dispparms,
    &vaResult,  //�޷���ֵ
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

      //������ʼ��
      VariantInit(&vaResultItem);
      memset(&dispparms, 0, sizeof(DISPPARAMS)); //�޲���

      // get_Source
      hr = v.pdispVal->Invoke(0x00,
        IID_NULL,
        LOCALE_SYSTEM_DEFAULT,
        DISPATCH_PROPERTYGET,
        &dispparms,
        &vaResultItem,  //����ֵ
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
        &vaResultItem,  //����ֵ
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
        &vaResultItem,  //����ֵ
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
        &vaResultItem,  //����ֵ
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
        &vaResultItem,  //����ֵ
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
        &vaResultItem,  //����ֵ
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
        &vaResultItem,  //����ֵ
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
  // �ж��Ƿ��ʼ���ҵ�¼
  if (!m_bInit) return EErrCode::EC_INTERFACE_OBJ_INIT_FAILD;
  if (!m_bLogin) return EErrCode::EC_LOGGED_OUT;

  //ջ����ʱ����
  DISPPARAMS dispparms;
  VARIANTARG* pArg;     // �������ָ��
  VARIANTARG vaResult;
  HRESULT hr;
  shared_ptr<CUnitConvert> pConv = CUnitConvert::Inst();

  //������ʼ��
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 4;  // 4������

  pArg = new VARIANTARG[dispparms.cArgs];
  dispparms.rgvarg = pArg;
  memset(pArg, 0, sizeof(VARIANT) * dispparms.cArgs);

  //ʱ��ת��
  DATE ftStartTime = pConv->UINT64ToVariantTime(ullStart);
  DATE ftEndTime = pConv->UINT64ToVariantTime(ullEnd);

  long  lCount = 0;
  SAFEARRAY*  psaValues = nullptr;

  //������ֵ
  dispparms.rgvarg[3].vt = VT_BYREF | VT_DATE;               // [in, out]	DATE* StartTime,
  dispparms.rgvarg[3].pdate = &ftStartTime;
  dispparms.rgvarg[2].vt = VT_BYREF | VT_DATE;               // [in, out]	DATE* EndTime,
  dispparms.rgvarg[2].pdate = &ftEndTime;
  dispparms.rgvarg[1].vt = VT_BYREF | VT_I4;                  // [out]
  dispparms.rgvarg[1].plVal = &lCount;
  dispparms.rgvarg[0].vt = VT_ARRAY | VT_BYREF | VT_VARIANT;   // [out] SAFEARRAY(VARIANT)*
  dispparms.rgvarg[0].pparray = &psaValues;

  // ��ȡ��ʷ��������
  VariantInit(&vaResult);
  hr = m_pIBMSHacDispatch->Invoke(0x07,   // SyncReadAlarms 
    IID_NULL,
    LOCALE_SYSTEM_DEFAULT,
    DISPATCH_METHOD,
    &dispparms,
    &vaResult,  //�޷���ֵ
    0,
    nullptr);

  if (FAILED(hr)) return EErrCode::EC_BMS_HAC_SYNC_READ_ALM_FAILD;

  LOG_TRACE << "Alarm History Count = " << lCount;

  VARIANTARG vaResultItem;
  VARIANT v;
  int t = 0;
  shared_ptr<KAlarmInfo>  ptAlarm = nullptr;
  string  strDef = "Ĭ���ַ���";
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

      //������ʼ��
      VariantInit(&vaResultItem);
      memset(&dispparms, 0, sizeof(DISPPARAMS)); //�޲���

      // get_TagId
      hr = v.pdispVal->Invoke(0x01,
        IID_NULL,
        LOCALE_SYSTEM_DEFAULT,
        DISPATCH_PROPERTYGET,
        &dispparms,
        &vaResultItem,  //����ֵ
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
        &vaResultItem,  //����ֵ
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
        &vaResultItem,  //����ֵ
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
        &vaResultItem,  //����ֵ
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
        &vaResultItem,  //����ֵ
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
        &vaResultItem,  //����ֵ
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
        &vaResultItem,  //����ֵ
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
        &vaResultItem,  //����ֵ
        0,
        nullptr);
      if (FAILED(hr)) continue;
      ptAlarm->bAck = (VARIANT_TRUE == vaResultItem.boolVal) ? true : false;

      // ���û�е�����
      ptAlarm->strArea = strDef;
      ptAlarm->strTag = strDef;
      ptAlarm->strValue = strDef;
      ptAlarm->strAlarmEvent = strDef;
    }
  }

  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
// ��ȡ����������Ϣ
//------------------------------------------------------------------------------
EErrCode CRDBSInterface::ReadLinkageWatchInfo(list<shared_ptr<KLinkageWatchInfo>> &lstInfo)
{
  DISPPARAMS dispparms;
  VARIANTARG* pArg;
  HRESULT hr;

  //������ʼ��
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 8;  //8������

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

  //������ֵ
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
    NULL,  //�޷���ֵ
    0,
    NULL);

  if (FAILED(hr)) return EErrCode::EC_BMS_READ_LINKAGE_WATCH_FAILD;

  //�����յ������ݷŵ�������
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

  //�ڴ�����
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
// �˹�ȷ��
//------------------------------------------------------------------------------
EErrCode CRDBSInterface::LinkageManualConfirmation(uint32_t ulItemID)
{
  //ջ����ʱ����
  DISPPARAMS dispparms;
  VARIANTARG* pArg;      //�������ָ��
  HRESULT hr;

  //������ʼ�� 
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 1;

  // �Բ��������ڴ�
  pArg = new VARIANTARG[dispparms.cArgs];
  dispparms.rgvarg = pArg;
  memset(pArg, 0, sizeof(VARIANT) * dispparms.cArgs);

  dispparms.rgvarg[0].vt = VT_I4;          //ItemID
  dispparms.rgvarg[0].lVal = ulItemID;

  //Invoke���� 
  hr = m_pILinkageWatch->Invoke(0x02, // ConfirmLSNotify
    IID_NULL,
    LOCALE_SYSTEM_DEFAULT,
    DISPATCH_METHOD,
    &dispparms,
    nullptr,
    0,
    nullptr);

  //�������
  delete[] pArg;
  if (FAILED(hr)) return EErrCode::EC_LINKAGE_MANUAL_CONFIRMATION_FAILD;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode CRDBSInterface::LinkageTerminateTask(uint32_t ulItemID)
{
  //ջ����ʱ����
  DISPPARAMS dispparms;
  VARIANTARG* pArg;      //�������ָ��
  HRESULT hr;

  //������ʼ�� 
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 1;

  // �Բ��������ڴ�
  pArg = new VARIANTARG[dispparms.cArgs];
  dispparms.rgvarg = pArg;
  memset(pArg, 0, sizeof(VARIANT) * dispparms.cArgs);

  dispparms.rgvarg[0].vt = VT_I4;          //ItemID
  dispparms.rgvarg[0].lVal = ulItemID;

  //Invoke���� 
  hr = m_pILinkageWatch->Invoke(0x01, // TerminateLSNotify
    IID_NULL,
    LOCALE_SYSTEM_DEFAULT,
    DISPATCH_METHOD,
    &dispparms,
    nullptr,
    0,
    nullptr);

  //�������
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

  //������ʼ��
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 7;  //7������

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

  //������ֵ
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
    NULL,  //�޷���ֵ
    0,
    NULL);

  if (FAILED(hr)) return EErrCode::EC_BMS_READ_TIMETABLE_DISPATCH_WATCH_FAILD;

  //�����յ������ݷŵ�������
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

  //�ڴ�����
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
  //ջ����ʱ����
  DISPPARAMS dispparms;
  VARIANTARG* pArg;      //�������ָ��
  HRESULT hr;

  //������ʼ�� 
  memset(&dispparms, 0, sizeof(DISPPARAMS));
  dispparms.cArgs = 1;

  // �Բ��������ڴ�
  pArg = new VARIANTARG[dispparms.cArgs];
  dispparms.rgvarg = pArg;
  memset(pArg, 0, sizeof(VARIANT) * dispparms.cArgs);

  dispparms.rgvarg[0].vt = VT_I4;          //ItemID
  dispparms.rgvarg[0].lVal = ulItemID;

  //Invoke���� 
  hr = m_pDispatchWatch->Invoke(0x01, // TermTask
    IID_NULL,
    LOCALE_SYSTEM_DEFAULT,
    DISPATCH_METHOD,
    &dispparms,
    nullptr,
    0,
    nullptr);

  //�������
  delete[] pArg;
  if (FAILED(hr)) return EErrCode::EC_TIMETABLE_DISPATCH_TERMINATE_TASK_FAILD;
  return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
bool CRDBSInterface::CheckDBStatus(std::list<std::shared_ptr<KDBStatusInfo>> &lstInfo)
{
  // 946656000 �� 2000/1/1 0:0:0
  uint64_t  ullStart = (uint64_t)946656000 * (uint64_t)1000;
  uint64_t  ullEnd = (uint64_t)946656001 * (uint64_t)1000;
  list<shared_ptr<KEventInfo>> lstEventInfo;
  EErrCode  eErr = ReadHistroyEvent(ullStart, ullEnd, lstEventInfo);
  
  // ����ȡ�������ݿ���Ϣд�뵽�б���
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
  // �жϱ������Ƿ���������
  if (m_ullMark != m_pOptimize->GetUsedMark()) return S_OK;

  //��Ч����
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
  //����IO��¼����ʱ������
  for (long i = 0; i < NumTags; i++)
  {
    //��ʼ��
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
      case VT_BOOL: //������
        ptTag->anyValue = (VARIANT_TRUE == v.boolVal) ? true : false;
        LOG_TRACE << boost::any_cast<bool>(ptTag->anyValue);
        break;

      case VT_I4:   //������
        ptTag->anyValue = static_cast<uint32_t>(v.lVal);
        LOG_TRACE << boost::any_cast<uint32_t>(ptTag->anyValue);
        break;

      case VT_R4:   //������
        ptTag->anyValue = static_cast<float>(v.fltVal);
        LOG_TRACE << boost::any_cast<float>(ptTag->anyValue);
        break;

      case VT_BSTR: //�ַ���
      {
        if (v.bstrVal)
        {
          string str = W2A(v.bstrVal);
          // ���������ӷ���any����string����
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

    // ת�����ݲ������ݷ��������
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
  // �жϱ������Ƿ���������
  if (m_ullMark != m_pOptimize->GetUsedMark()) return S_OK;

  //��Ч����
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
  //�����¼����ʱ������
  for (long i = 0; i < EvtCount; i++)
  {
    SafeArrayGetElement(pEvtTypes, (long*)&i, &sEvtType);
    SafeArrayGetElement(pEvtIDs, (long*)&i, &lEvtID);
    SafeArrayGetElement(pAccessLevels, (long*)&i, &lAccesslevel);
    SafeArrayGetElement(pEvtTimes, (long*)&i, &dateEvtTime);
    SafeArrayGetElement(pUserNames, (long*)&i, &bstrUserName);
    SafeArrayGetElement(pEvtSrc, (long*)&i, &bstrEvtSr);

    ptEvent = make_shared<KEventInfo>();

    ptEvent->eType = (EEventType)sEvtType;  //�¼�����
    ptEvent->ulID = lEvtID;                  //�¼���ID
    ptEvent->ulAccessLevel = lAccesslevel;   //����Ȩ��0-1000
    ptEvent->ullTimeMs = pConv->VariantTimeToUINT64(dateEvtTime); //�¼�ʱ��

    // Ϊ�˷����ͷ�W2A��ջ�ϵĿռ�
    {
      if (bstrUserName) ptEvent->strUserName = W2A(bstrUserName); //�û���
      if (bstrEvtSr) ptEvent->strRecord = W2A(bstrEvtSr);      //�¼���¼
    }

    //��ֹ�ڴ����� 2003.6
    SysFreeString(bstrUserName);
    SysFreeString(bstrEvtSr);

    // ת�����ݲ������ݷ��������
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
  // �жϱ������Ƿ���������
  if (m_ullMark != m_pOptimize->GetUsedMark()) return S_OK;

  //��Ч����
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
  string  strDef = "Ĭ���ַ���";

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
  //�����¼����ʱ������
  for (long i = 0; i < AlmCount; i++)
  {
    //��ʼ��
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

    ptAlarm->ulGID = lTagID;  // ������ȫ��ID
    ptAlarm->ullTimeMs = pConv->VariantTimeToUINT64(dateAlmTime); // �ɼ�ʱ��
    ptAlarm->ulID = lAlmID; // ������ID

    // Ϊ�˷����ͷ�W2A��ջ�ϵĿռ�
    {
      if (bstrTagName) ptAlarm->strUserName = W2A(bstrTagName); // ������
      if (bstrReserved) ptAlarm->strDescribe = W2A(bstrReserved); // ��������
    }

    ptAlarm->ulCategory = sAlmType; // ��������
    ptAlarm->ulPriority = lAlmPriorities; // �����ĵȼ�
    ptAlarm->bAck = (VARIANT_TRUE == bAck) ? true : false; // �Ƿ�Ӧ��  

    // ���û�е�����
    ptAlarm->strArea = strDef;
    ptAlarm->strTag = strDef;
    ptAlarm->strValue = strDef;
    ptAlarm->strAlarmEvent = strDef;

    //��ֹ�ڴ����� 2003.6
    SysFreeString(bstrTagName);
    SysFreeString(bstrSource);
    SysFreeString(bstrReserved);

    // ת�����ݲ������ݷ��������
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

