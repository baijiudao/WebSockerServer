#ifndef _DEFINE_H_
#define _DEFINE_H_

#include <queue>
#include <memory>
#include <typeindex>

#include <boost/any.hpp>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/client.hpp>

#include "../../../../RDBS/RDBSExtern/RDBSExtern/RDBSExternDefine.h"

typedef websocketpp::server<websocketpp::config::asio> server;
typedef websocketpp::client<websocketpp::config::asio> client;

using websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

using namespace std;

// Socket监听端口
#define SERVER_PORT   32767

// 服务器自己使用并判断错误类型的错误码
typedef enum class _eErrCode : uint32_t {
  EC_SUCCESS = 0,                             // 0  成功
  EC_UNKNOW,                                  // 1  未知错误
  EC_FAILD,                                   // 2  错误
  EC_SESSION_UNINVALID,                       // 3  Session失效
  EC_LOGIN_PASSWORD_ERROR,                    // 4  密码错误
  EC_LOGIN_ACCOUNT_LOCKED,                    // 5  账号锁定
  EC_LOGIN_ACCOUNT_UNINVALID,                 // 6  账号无效
  EC_LOGGED_OUT,                              // 7  账号已经登出
  EC_UNAUTHORIZED_ACCESS,                     // 8  无权访问
  EC_READ_TAG_FAILD,                          // 9  读取tag变量失败
  EC_READ_HISTROY_EVENT_FAILD,                // 10  读取历史事件失败
  EC_READ_HISTROY_ALARM_FAILD,                // 11  读取历史报警失败
  EC_NOT_LOGIN,                               // 12  未登录
  EC_LOGOUT_FAILD,                            // 13  退出登录失败
  EC_NOT_SESSION,                             // 14  没有找到SESSION（生成SESSION失败或者非法访问，需要重新连接）
  EC_CREATE_SESSION_FAILD,                    // 15  创建Session失败
  EC_SHORT_TIMEREPEAT_OPERATION,              // 16  在短时间同一个连接多次发送同一个消息
  EC_LOGGED_IN,                               // 17  账号已经登陆(当前SOCKET已经有账号被记录)
                                                   
  // 解包或者打包失败                               
  EC_PACKAGE_PARSE_FAILD,                     // 18  解析网络包失败
  EC_PACKAGE_BUILD_FAILD,                     // 19  创建网络包失败
                                                   
  EC_PACKAGE_PARSE_LOGIN_FAILD,               // 20  解析登录包失败
                                                   
  // 内部的一些详细错误                            
  EC_BMS_CREATE_INSTANCE_FAILD,               // 21  创建资源失败
  EC_BMS_QUERY_INTERFACE_FAILD,               // 22  获取接口指针失败
  EC_BMS_VALUE_INVALID,                       // 23  获取到的数据无效
  EC_BMS_CREATE_ARR_FAILD,                    // 24  SafeArrayCreate执行失败
                                                   
  EC_BMS_SERVER_INIT_FAILD,                   // 25  Server对象Initialize方法失败SyncReadEvents
  EC_BMS_SERVER_LOGIN_FAILD,                  // 26  Server对象login方法失败
  EC_BMS_SERVER_LOGOUT_FAILD,                 // 27  Server对象logout方法失败
  EC_BMS_SERVER_GET_ACCESS_OBJ_FAILD,         // 28  Server对象GetAccessObject方法失败
  EC_BMS_RCX_WRITE_EVENT_FAILD,               // 29  Server对象WriteEvent方法失败
  EC_BMS_SERVER_UNINIT_FAILD,                 // 30  Server对象Uninitialize方法失败
  EC_BMS_RCX_INIT_FAILD,                      // 31  RcxData对象Initialize方法失败
  EC_BMS_RCX_GET_ALL_TAG_FAILD,               // 32  RcxData对象GetConfigTags方法失败
  EC_BMS_RCX_WRITE_TAGS_FAILD,                // 33  RcxData对象WriteTags方法失败
  EC_BMS_RCX_SET_SINK_PTR_FAILD,              // 34  RcxData对象SetSinkPointers方法失败
  EC_BMS_RCX_UNINIT_FAILD,                    // 35  RcxData对象Uninitialize方法失败
  EC_BMS_HAC_SYNC_READ_EVT_FAILD,             // 36  Hac对象SyncReadEvents方法失败
  EC_BMS_HAC_SYNC_READ_ALM_FAILD,             // 37  Hac对象SyncReadAlarms方法失败

  EC_BMS_SERVER_OBJ_UNINVALID,                // 38  Server对象无效
  EC_BMS_RCX_OBJ_UNINVALID,                   // 39  RcxData对象无效
  EC_BMS_SHAC_OBJ_UNINVALID,                  // 40  SHac对象无效
  EC_GID_SET_DATA_FAILD,                      // 41  GID对象SetData失败
  EC_PARAM_UNINVALID,                         // 42  参数无效
  EC_RT_QUE_UNINVALID,                        // 43  实时队列对象无效
  EC_INTERFACE_OBJ_UNINVALID,                 // 44  接口对象无效（为NULL）
  EC_INTERFACE_OBJ_INIT_FAILD,                // 45  接口对象初始化失败（现阶段需要重新连接Socket才会再次初始化）

  EC_BMS_GET_FUNC_AUTHORITY,                  // 46  Server对象GetFunctionAuthority方法失败
  EC_DATABASE_FAIL,                           // 47  数据库操作失败

  EC_NO_PERMISSION,                           // 48  用户没有相关权限
  EC_BAD_ANY_CAST,                            // 49  any_cast转换出错
  EC_TAG_INVAILD,                             // 50  tag变量无效

  EC_CHECK_OPC_STATUS_FAILD,                  // 51  检测opc状态失败
  EC_COMMAND_FAILD,                           // 52  请求的通信协议命令类型不存在

  EC_DATABASE_NODATA = 100,                   // 100 没有请求到相关数据
  EC_DATABASE_ALREADYEXISTS,                  // 101 要插入的数据已经存在
  
  // 联动信息相关错误 200 - 299
  EC_BMS_LINKAGE_WATCH_OBJ_UNINVALID = 200,   // 200  LinkageWatch对象无效
  EC_BMS_READ_LINKAGE_WATCH_FAILD,            // 201  linkage watch对象的获取联动监视信息接口失败
  EC_READ_LINKAGE_WATCH_INFO_FAILD,           // 202 读取联动监视信息失败
  EC_LINKAGE_MANUAL_CONFIRMATION_FAILD,       // 203 人工确认失败
  EC_LINKAGE_TERMINATE_TASK_FAILD,            // 204 终止任务失败

  // 时间调度信息相关错误 300 - 399
  EC_BMS_TIMETABLE_DISPATCH_WATCH_OBJ_UNINVALID = 300,  // 300  TimetableDispatchWatch对象无效
  EC_BMS_READ_TIMETABLE_DISPATCH_WATCH_FAILD,           // 301  TimetableDispatchWatch对象的获取时间表调度监视信息接口失败
  EC_TIMETABLE_DISPATCH_TERMINATE_TASK_FAILD,           // 302  终止时间调度任务失败

  EC_LAST
}EErrCode;

// 请求类型
typedef enum class _eCommand : uint32_t{
	CMD_LOGIN = 0,                      // 0   登录
	CMD_LOGOUT,                         // 1   登出
	CMD_READ_TAG,                       // 2   读若干个变量
	CMD_READ_HISTORY_EVENT,             // 3   读历史事件
	CMD_READ_HISTORY_ALARM,             // 4   读历史告警
	CMD_WRITE_TAG,                      // 5   写变量
	CMD_WRITE_EVENT,                    // 6   写事件
	CMD_PUSH_TAG,                       // 7   推送变量
	CMD_PUSH_ALARM,                     // 8   推送告警
	CMD_PUSH_EVENT,                     // 9   推送事件
	CMD_ADD_ALARM_ASSISTANT,            // 10  添加告警辅助信息
	CMD_DEL_ALARM_ASSISTANT,            // 11  删除告警辅助信息
	CMD_MODIFY_ALARM_ASSISTANT,         // 12  修改告警辅助信息
	CMD_SEARCH_NUM_ALARM_ASSISTANT,     // 13  查询告警辅助信息（按数量返回）
	CMD_SEARCH_PAGING_ALARM_ASSISTANT,  // 14  查询告警辅助信息（按页返回）
	CMD_SEARCH_ALARM_ASSISTANT,         // 15  查询告警辅助信息（返回所有查询结果）
	//begin add by gaoxin 2017.10.12
	CMD_READ_HISTORY_EVENT_EX,               // 16  读历史事件
	CMD_READ_HISTORY_ALARM_EX,               // 17  读历史告警

	CMD_ADD_SUPPLIER = 100,                   // 100 添加供应商信息
	CMD_DELETE_SUPPLIER = 101,                // 101 删除供应商信息
	CMD_MODIFY_SUPPLIER = 102,                // 102 修改供应商信息
	CMD_SEARCH_SUPPLIER = 103,                // 103 查询供应商信息
	CMD_SEARCH_SUPPLIERINF_FORTAGID = 104,    // 104 使用绑定的变量id查询设备供应商信息

	CMD_ADD_METERTYPE = 110,                  // 110 添加计量表类型信息
	CMD_DELETE_METERTYPE = 111,               // 111 删除计量表类型信息
	CMD_MODIFY_METERTYPE = 112,               // 112 修改计量表类型信息
	CMD_SEARCH_METERTYPE = 113,               // 113 查询计量表类型信息

	CMD_ADD_ENERGYCATE = 120,                 // 120 添加耗能类别信息
	CMD_DELETE_ENERGYCATE = 121,              // 121 删除耗能类别信息
	CMD_MODIFY_ENERGYCATE = 122,              // 122 修改耗能类别信息
	CMD_SEARCH_ENERGYCATE = 123,              // 123 查询耗能类别信息

	CMD_ADD_FAULTCATE = 130,                  // 130 添加故障类别信息
	CMD_DELETE_FAULTCATE = 131,               // 131 删除故障类别信息
	CMD_MODIFY_FAULTCATE = 132,               // 132 修改故障类别信息
	CMD_SEARCH_FAULTCATE = 133,               // 133 查询故障类别信息

	CMD_ADD_DEVICEINF = 140,                  // 140 增加设备档案信息
	CMD_DELETE_DEVICEINF = 141,               // 141 增加设备档案信息
	CMD_MODIFY_DEVICEINF = 142,               // 142 增加设备档案信息
	CMD_SEARCH_DEVICEINF = 143,               // 143 增加设备档案信息
	CMD_SEARCH_DEVICEINF_FORTAGID = 144,      // 144 使用绑定的变量id查询设备档案信息

	CMD_ADD_METERINF = 150,                   // 150 添加计量表信息
	CMD_DELETE_METERINF = 151,                // 151 删除计量表信息
	CMD_MODIFY_METERINF = 152,                // 152 修改计量表信息
	CMD_SEARCH_METERINF = 153,                // 153 查询计量表信息

	CMD_ADD_DEVICEPARAMETER = 160,            // 160 添加设备参数
	CMD_DELETE_DEVICEPARAMETER = 161,         // 161 删除设备参数
	CMD_SEARCH_DEVICEPARAMETER = 162,         // 162 查询设备参数

	CMD_ADD_METERPARAMETER = 170,            // 170 添加能源采集参数
	CMD_DELETE_METERPARAMETER = 171,         // 171 删除能源采集参数
	CMD_SEARCH_METERPARAMETER = 172,         // 172 查询能源采集参数

	CMD_ADD_DEVICEREPAIRINF = 180,            // 180 添加设备维修信息
	CMD_DELETE_DEVICEREPAIRINF = 181,         // 181 删除设备维修信息
	CMD_MODIFY_DEVICEREPAIRINF = 182,         // 182 修改设备维修信息
	CMD_SEARCH_DEVICEREPAIRINF = 183,         // 183 查询设备维修信息
	CMD_SEARCH_REPAIRINF_FORDEVICE = 184,     // 184 使用设备查询设备维修档案
	CMD_SEARCH_REPAIRINF_FORTAGID = 185,      // 185 使用绑定的变量id查询设备维修档案

	CMD_ADD_RUNINF = 190,                     // 190 添加设备运行信息
	CMD_DELETE_RUNINF = 191,                  // 191 删除设备运行信息
	CMD_MODIFY_RUNINF = 192,                  // 192 修改设备运行信息
	CMD_SEARCH_RUNINF = 193,                  // 193 查询设备运行信息
	CMD_SEARCH_RUNINF_FORDEVICE = 194,        // 194 使用设备查询设备运行档案
	CMD_SEARCH_RUNINF_FORTAGID = 195,         // 195 使用绑定的变量id查询设备运行档案

	CMD_SEARCH_METERVALUE = 200,              // 200 查询计量表读数

	CMD_SEARCH_DEVICEVALUE = 210,             // 210 按设备查询变量值

	CMD_SEARCH_ENERGYSTATISTICS = 220,        // 220 查询能源统计结果

	CMD_SEARCH_TAGVALUE = 230,                // 230 查询变量值

	CMD_SEARCH_SYSTEM = 240,                  // 240 查询点表子系统
	CMD_SEARCH_DEVICE = 250,                  // 250 查询点表设备
	CMD_SEARCH_TAG = 260,                     // 260 查询点表变量
	CMD_SEARCH_TAG_WITHOUTPARAMETER = 261,    // 261 查询点表变量除过已经配置给设备和计量表的

	CMD_SEARCH_OVERVIEW = 270,               //270 查询总览信息
	CMD_SEARCH_SUBSYSTEM_ALARM_COUNT = 271,  //271 系统下的报警数据
  //end add by gaoxin 2017.10.12
  CMD_ADD_CHANGE_SHIFTS = 290,        // 290 添加交接班信息
  CMD_CHECK_DB_STATUS = 300,          // 300 监测数据库状态
  CMD_CHECK_OPC_STATUS,               // 301 监测OPC状态

  CMD_CAMERA_SCREENSHOT = 1000,       // 1000 视频截图
  CMD_SEARCH_CAMERA_CONFIG = 1001,    // 1001 查询摄像头配置

  CMD_READ_LINKAGE_WATCH_INFO = 1100, // 1100 读取联动监视信息
  CMD_LINKAGE_MANUAL_CONFIRMATION,    // 1101 人工处理
  CMD_LINKAGE_TERMINATE_TASK,         // 1102 终止任务

  CMD_READ_TIMETABLE_DISPATCH_WATCH_INFO = 1200,  // 1200 读取时间表调度监视信息
  CMD_TIMETABLE_DISPATCH_TERMINATE_TASK,          // 1201 终止任务
  CMD_LAST                // 未知类型
}ECommand;

// 用户类型
typedef enum class _eUserType : uint32_t
{
  GENERAL = 0,            // 0 普通用户
  ADMINISTRATORS,         // 1 管理员用户

  LAST
}EUserType;

// 全局Cookie中保存的COM对象索引
typedef enum
{
  COOKIE_SERVER = 0,
  COOKIE_RCX_DATA,
  COOKIE_HAC,
  COOKIE_LAST
}ECookie;

// Tag数据类型
typedef enum
{
  TYPE_BOOL = 0,
  TYPE_INTEGER,
  TYPE_FLOAT,
  TYPE_STRING,
  TYPE_LAST
}ETagType;

// 暂时先用整形代替事件类型变量
#define _EVENT_TYPE_TMP_
#ifdef _EVENT_TYPE_TMP_
typedef uint32_t EEventType;
#else
// 事件类型
// Event Type
typedef enum {
  EVENT_TYPE_0 = 0,
  EVENT_TYPE_1,
  EVENT_TYPE_LAST
}EEventType;
#endif // _EVENT_TYPE_TMP_

// Tag结构体
typedef struct _TagInfo
{
  uint32_t    ulGID;      // 全局变量
  boost::any  anyValue;   // 变量值
  uint16_t    sQuality;   //质量
  uint64_t    ullTimeMs;  // 时间（毫秒）
  _TagInfo()
  {
    ulGID = 0;
    // any empty
    sQuality = 0;
    ullTimeMs = 0;
  }
}KTagInfo;

// Event
typedef struct _EventInfo
{
  uint64_t    ullTimeMs;  // 时间（毫秒）
  uint32_t    ulID;
  std::string strUserName;
  std::string strRecord;
  uint32_t    ulAccessLevel;
  EEventType  eType;
  _EventInfo()
  {
    ullTimeMs = 0;
    ulID = 0;
    strUserName = "";
    strRecord = "";
    ulAccessLevel = 0;
    eType = 0;
  }
}KEventInfo;

// Alarm
typedef struct _AlarmInfo
{
  uint32_t    ulGID;      // 全局变量
  uint64_t    ullTimeMs;  // 时间（毫秒）
  uint32_t    ulID;
  std::string strUserName;
  uint32_t    ulCategory;
  std::string strDescribe;
  uint32_t    ulPriority;
  bool        bAck;

  // 没找到赋值对象
  std::string strArea;
  std::string strTag;
  std::string strValue;
  std::string strAlarmEvent;
  _AlarmInfo()
  {
    ulGID = 0;
    ullTimeMs = 0;
    ulID = 0;
    strUserName = "";
    ulCategory = 0;
    strDescribe = "";
    ulPriority = 0;
    bAck = false;

    strArea = "";
    strTag = "";
    strValue = "";
    strAlarmEvent = "";
  }
}KAlarmInfo;

// OPC状态码
typedef enum class _eOPCStatus : uint32_t
{
  OK = 0,     // 0  正常
  UNCONNECT,  // 1  不能连接
  LAST
}EOPCStatus;

// OPC状态信息
typedef struct _OPCStatusInfo
{
  uint32_t    ulID;     // OPC子系统ID
  std::string strName;  // 子系统名字
  EOPCStatus  eStatus;  // 子系统状态
}KOPCStatusInfo;

#endif // !_DEFINE_H_
