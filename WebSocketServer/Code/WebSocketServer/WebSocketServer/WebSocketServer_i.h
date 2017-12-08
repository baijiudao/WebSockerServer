

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Fri Dec 01 10:27:08 2017
 */
/* Compiler settings for WebSocketServer.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0603 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __WebSocketServer_i_h__
#define __WebSocketServer_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IRDBSInterface_FWD_DEFINED__
#define __IRDBSInterface_FWD_DEFINED__
typedef interface IRDBSInterface IRDBSInterface;

#endif 	/* __IRDBSInterface_FWD_DEFINED__ */


#ifndef ___IRDBSInterfaceEvents_FWD_DEFINED__
#define ___IRDBSInterfaceEvents_FWD_DEFINED__
typedef interface _IRDBSInterfaceEvents _IRDBSInterfaceEvents;

#endif 	/* ___IRDBSInterfaceEvents_FWD_DEFINED__ */


#ifndef __RDBSInterface_FWD_DEFINED__
#define __RDBSInterface_FWD_DEFINED__

#ifdef __cplusplus
typedef class RDBSInterface RDBSInterface;
#else
typedef struct RDBSInterface RDBSInterface;
#endif /* __cplusplus */

#endif 	/* __RDBSInterface_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IRDBSInterface_INTERFACE_DEFINED__
#define __IRDBSInterface_INTERFACE_DEFINED__

/* interface IRDBSInterface */
/* [unique][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IRDBSInterface;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E99F4401-1E5F-44AC-85FC-9DA3E017AF4B")
    IRDBSInterface : public IDispatch
    {
    public:
    };
    
    
#else 	/* C style interface */

    typedef struct IRDBSInterfaceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IRDBSInterface * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IRDBSInterface * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IRDBSInterface * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IRDBSInterface * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IRDBSInterface * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IRDBSInterface * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IRDBSInterface * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        END_INTERFACE
    } IRDBSInterfaceVtbl;

    interface IRDBSInterface
    {
        CONST_VTBL struct IRDBSInterfaceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRDBSInterface_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IRDBSInterface_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IRDBSInterface_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IRDBSInterface_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IRDBSInterface_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IRDBSInterface_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IRDBSInterface_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IRDBSInterface_INTERFACE_DEFINED__ */



#ifndef __WebSocketServerLib_LIBRARY_DEFINED__
#define __WebSocketServerLib_LIBRARY_DEFINED__

/* library WebSocketServerLib */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_WebSocketServerLib;

#ifndef ___IRDBSInterfaceEvents_DISPINTERFACE_DEFINED__
#define ___IRDBSInterfaceEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IRDBSInterfaceEvents */
/* [uuid] */ 


EXTERN_C const IID DIID__IRDBSInterfaceEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("D6FE25CB-08C1-41C5-A70C-C926B9CFB06E")
    _IRDBSInterfaceEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IRDBSInterfaceEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IRDBSInterfaceEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IRDBSInterfaceEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IRDBSInterfaceEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IRDBSInterfaceEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IRDBSInterfaceEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IRDBSInterfaceEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IRDBSInterfaceEvents * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        END_INTERFACE
    } _IRDBSInterfaceEventsVtbl;

    interface _IRDBSInterfaceEvents
    {
        CONST_VTBL struct _IRDBSInterfaceEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IRDBSInterfaceEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _IRDBSInterfaceEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _IRDBSInterfaceEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _IRDBSInterfaceEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _IRDBSInterfaceEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _IRDBSInterfaceEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _IRDBSInterfaceEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IRDBSInterfaceEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_RDBSInterface;

#ifdef __cplusplus

class DECLSPEC_UUID("23CE38BC-F7B7-42B1-8B37-EAEA7AA94DA9")
RDBSInterface;
#endif
#endif /* __WebSocketServerLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


