

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


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


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IRDBSInterface,0xE99F4401,0x1E5F,0x44AC,0x85,0xFC,0x9D,0xA3,0xE0,0x17,0xAF,0x4B);


MIDL_DEFINE_GUID(IID, LIBID_WebSocketServerLib,0x75D83472,0xD4C4,0x440D,0x82,0x6F,0x67,0x05,0xC7,0x8F,0x3E,0xE5);


MIDL_DEFINE_GUID(IID, DIID__IRDBSInterfaceEvents,0xD6FE25CB,0x08C1,0x41C5,0xA7,0x0C,0xC9,0x26,0xB9,0xCF,0xB0,0x6E);


MIDL_DEFINE_GUID(CLSID, CLSID_RDBSInterface,0x23CE38BC,0xF7B7,0x42B1,0x8B,0x37,0xEA,0xEA,0x7A,0xA9,0x4D,0xA9);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



