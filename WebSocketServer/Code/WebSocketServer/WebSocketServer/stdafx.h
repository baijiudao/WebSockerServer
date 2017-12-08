// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef STRICT
#define STRICT
#endif

#include "targetver.h"

#define _ATL_APARTMENT_THREADED

#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit


#define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW

#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
#include <windows.h>

#define _XP_ENVIRONMENT_  // 在XP环境下运行
#define _CALL_BACK_   // 启动服务器回调线程
//#define _WEB_SOCKET_DEBUG_PRINT_  // 开启websocket模块中的打印信息
//#define _DEBUG_CODE_  // 调试代码（用来简化调试过程）

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if(p) { delete (p); (p)=nullptr; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p); (p)=nullptr; } }
#define SAFE_FREE(p)         { if(p) { free(p); (p)=nullptr; } }
#endif
