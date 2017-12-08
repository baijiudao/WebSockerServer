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

// Socket�����˿�
#define SERVER_PORT   32767

// �������Լ�ʹ�ò��жϴ������͵Ĵ�����
typedef enum class _eErrCode : uint32_t {
  EC_SUCCESS = 0,                             // 0  �ɹ�
  EC_UNKNOW,                                  // 1  δ֪����
  EC_FAILD,                                   // 2  ����
  EC_SESSION_UNINVALID,                       // 3  SessionʧЧ
  EC_LOGIN_PASSWORD_ERROR,                    // 4  �������
  EC_LOGIN_ACCOUNT_LOCKED,                    // 5  �˺�����
  EC_LOGIN_ACCOUNT_UNINVALID,                 // 6  �˺���Ч
  EC_LOGGED_OUT,                              // 7  �˺��Ѿ��ǳ�
  EC_UNAUTHORIZED_ACCESS,                     // 8  ��Ȩ����
  EC_READ_TAG_FAILD,                          // 9  ��ȡtag����ʧ��
  EC_READ_HISTROY_EVENT_FAILD,                // 10  ��ȡ��ʷ�¼�ʧ��
  EC_READ_HISTROY_ALARM_FAILD,                // 11  ��ȡ��ʷ����ʧ��
  EC_NOT_LOGIN,                               // 12  δ��¼
  EC_LOGOUT_FAILD,                            // 13  �˳���¼ʧ��
  EC_NOT_SESSION,                             // 14  û���ҵ�SESSION������SESSIONʧ�ܻ��߷Ƿ����ʣ���Ҫ�������ӣ�
  EC_CREATE_SESSION_FAILD,                    // 15  ����Sessionʧ��
  EC_SHORT_TIMEREPEAT_OPERATION,              // 16  �ڶ�ʱ��ͬһ�����Ӷ�η���ͬһ����Ϣ
  EC_LOGGED_IN,                               // 17  �˺��Ѿ���½(��ǰSOCKET�Ѿ����˺ű���¼)
                                                   
  // ������ߴ��ʧ��                               
  EC_PACKAGE_PARSE_FAILD,                     // 18  ���������ʧ��
  EC_PACKAGE_BUILD_FAILD,                     // 19  ���������ʧ��
                                                   
  EC_PACKAGE_PARSE_LOGIN_FAILD,               // 20  ������¼��ʧ��
                                                   
  // �ڲ���һЩ��ϸ����                            
  EC_BMS_CREATE_INSTANCE_FAILD,               // 21  ������Դʧ��
  EC_BMS_QUERY_INTERFACE_FAILD,               // 22  ��ȡ�ӿ�ָ��ʧ��
  EC_BMS_VALUE_INVALID,                       // 23  ��ȡ����������Ч
  EC_BMS_CREATE_ARR_FAILD,                    // 24  SafeArrayCreateִ��ʧ��
                                                   
  EC_BMS_SERVER_INIT_FAILD,                   // 25  Server����Initialize����ʧ��SyncReadEvents
  EC_BMS_SERVER_LOGIN_FAILD,                  // 26  Server����login����ʧ��
  EC_BMS_SERVER_LOGOUT_FAILD,                 // 27  Server����logout����ʧ��
  EC_BMS_SERVER_GET_ACCESS_OBJ_FAILD,         // 28  Server����GetAccessObject����ʧ��
  EC_BMS_RCX_WRITE_EVENT_FAILD,               // 29  Server����WriteEvent����ʧ��
  EC_BMS_SERVER_UNINIT_FAILD,                 // 30  Server����Uninitialize����ʧ��
  EC_BMS_RCX_INIT_FAILD,                      // 31  RcxData����Initialize����ʧ��
  EC_BMS_RCX_GET_ALL_TAG_FAILD,               // 32  RcxData����GetConfigTags����ʧ��
  EC_BMS_RCX_WRITE_TAGS_FAILD,                // 33  RcxData����WriteTags����ʧ��
  EC_BMS_RCX_SET_SINK_PTR_FAILD,              // 34  RcxData����SetSinkPointers����ʧ��
  EC_BMS_RCX_UNINIT_FAILD,                    // 35  RcxData����Uninitialize����ʧ��
  EC_BMS_HAC_SYNC_READ_EVT_FAILD,             // 36  Hac����SyncReadEvents����ʧ��
  EC_BMS_HAC_SYNC_READ_ALM_FAILD,             // 37  Hac����SyncReadAlarms����ʧ��

  EC_BMS_SERVER_OBJ_UNINVALID,                // 38  Server������Ч
  EC_BMS_RCX_OBJ_UNINVALID,                   // 39  RcxData������Ч
  EC_BMS_SHAC_OBJ_UNINVALID,                  // 40  SHac������Ч
  EC_GID_SET_DATA_FAILD,                      // 41  GID����SetDataʧ��
  EC_PARAM_UNINVALID,                         // 42  ������Ч
  EC_RT_QUE_UNINVALID,                        // 43  ʵʱ���ж�����Ч
  EC_INTERFACE_OBJ_UNINVALID,                 // 44  �ӿڶ�����Ч��ΪNULL��
  EC_INTERFACE_OBJ_INIT_FAILD,                // 45  �ӿڶ����ʼ��ʧ�ܣ��ֽ׶���Ҫ��������Socket�Ż��ٴγ�ʼ����

  EC_BMS_GET_FUNC_AUTHORITY,                  // 46  Server����GetFunctionAuthority����ʧ��
  EC_DATABASE_FAIL,                           // 47  ���ݿ����ʧ��

  EC_NO_PERMISSION,                           // 48  �û�û�����Ȩ��
  EC_BAD_ANY_CAST,                            // 49  any_castת������
  EC_TAG_INVAILD,                             // 50  tag������Ч

  EC_CHECK_OPC_STATUS_FAILD,                  // 51  ���opc״̬ʧ��
  EC_COMMAND_FAILD,                           // 52  �����ͨ��Э���������Ͳ�����

  EC_DATABASE_NODATA = 100,                   // 100 û�������������
  EC_DATABASE_ALREADYEXISTS,                  // 101 Ҫ����������Ѿ�����
  
  // ������Ϣ��ش��� 200 - 299
  EC_BMS_LINKAGE_WATCH_OBJ_UNINVALID = 200,   // 200  LinkageWatch������Ч
  EC_BMS_READ_LINKAGE_WATCH_FAILD,            // 201  linkage watch����Ļ�ȡ����������Ϣ�ӿ�ʧ��
  EC_READ_LINKAGE_WATCH_INFO_FAILD,           // 202 ��ȡ����������Ϣʧ��
  EC_LINKAGE_MANUAL_CONFIRMATION_FAILD,       // 203 �˹�ȷ��ʧ��
  EC_LINKAGE_TERMINATE_TASK_FAILD,            // 204 ��ֹ����ʧ��

  // ʱ�������Ϣ��ش��� 300 - 399
  EC_BMS_TIMETABLE_DISPATCH_WATCH_OBJ_UNINVALID = 300,  // 300  TimetableDispatchWatch������Ч
  EC_BMS_READ_TIMETABLE_DISPATCH_WATCH_FAILD,           // 301  TimetableDispatchWatch����Ļ�ȡʱ�����ȼ�����Ϣ�ӿ�ʧ��
  EC_TIMETABLE_DISPATCH_TERMINATE_TASK_FAILD,           // 302  ��ֹʱ���������ʧ��

  EC_LAST
}EErrCode;

// ��������
typedef enum class _eCommand : uint32_t{
	CMD_LOGIN = 0,                      // 0   ��¼
	CMD_LOGOUT,                         // 1   �ǳ�
	CMD_READ_TAG,                       // 2   �����ɸ�����
	CMD_READ_HISTORY_EVENT,             // 3   ����ʷ�¼�
	CMD_READ_HISTORY_ALARM,             // 4   ����ʷ�澯
	CMD_WRITE_TAG,                      // 5   д����
	CMD_WRITE_EVENT,                    // 6   д�¼�
	CMD_PUSH_TAG,                       // 7   ���ͱ���
	CMD_PUSH_ALARM,                     // 8   ���͸澯
	CMD_PUSH_EVENT,                     // 9   �����¼�
	CMD_ADD_ALARM_ASSISTANT,            // 10  ��Ӹ澯������Ϣ
	CMD_DEL_ALARM_ASSISTANT,            // 11  ɾ���澯������Ϣ
	CMD_MODIFY_ALARM_ASSISTANT,         // 12  �޸ĸ澯������Ϣ
	CMD_SEARCH_NUM_ALARM_ASSISTANT,     // 13  ��ѯ�澯������Ϣ�����������أ�
	CMD_SEARCH_PAGING_ALARM_ASSISTANT,  // 14  ��ѯ�澯������Ϣ����ҳ���أ�
	CMD_SEARCH_ALARM_ASSISTANT,         // 15  ��ѯ�澯������Ϣ���������в�ѯ�����
	//begin add by gaoxin 2017.10.12
	CMD_READ_HISTORY_EVENT_EX,               // 16  ����ʷ�¼�
	CMD_READ_HISTORY_ALARM_EX,               // 17  ����ʷ�澯

	CMD_ADD_SUPPLIER = 100,                   // 100 ��ӹ�Ӧ����Ϣ
	CMD_DELETE_SUPPLIER = 101,                // 101 ɾ����Ӧ����Ϣ
	CMD_MODIFY_SUPPLIER = 102,                // 102 �޸Ĺ�Ӧ����Ϣ
	CMD_SEARCH_SUPPLIER = 103,                // 103 ��ѯ��Ӧ����Ϣ
	CMD_SEARCH_SUPPLIERINF_FORTAGID = 104,    // 104 ʹ�ð󶨵ı���id��ѯ�豸��Ӧ����Ϣ

	CMD_ADD_METERTYPE = 110,                  // 110 ��Ӽ�����������Ϣ
	CMD_DELETE_METERTYPE = 111,               // 111 ɾ��������������Ϣ
	CMD_MODIFY_METERTYPE = 112,               // 112 �޸ļ�����������Ϣ
	CMD_SEARCH_METERTYPE = 113,               // 113 ��ѯ������������Ϣ

	CMD_ADD_ENERGYCATE = 120,                 // 120 ��Ӻ��������Ϣ
	CMD_DELETE_ENERGYCATE = 121,              // 121 ɾ�����������Ϣ
	CMD_MODIFY_ENERGYCATE = 122,              // 122 �޸ĺ��������Ϣ
	CMD_SEARCH_ENERGYCATE = 123,              // 123 ��ѯ���������Ϣ

	CMD_ADD_FAULTCATE = 130,                  // 130 ��ӹ��������Ϣ
	CMD_DELETE_FAULTCATE = 131,               // 131 ɾ�����������Ϣ
	CMD_MODIFY_FAULTCATE = 132,               // 132 �޸Ĺ��������Ϣ
	CMD_SEARCH_FAULTCATE = 133,               // 133 ��ѯ���������Ϣ

	CMD_ADD_DEVICEINF = 140,                  // 140 �����豸������Ϣ
	CMD_DELETE_DEVICEINF = 141,               // 141 �����豸������Ϣ
	CMD_MODIFY_DEVICEINF = 142,               // 142 �����豸������Ϣ
	CMD_SEARCH_DEVICEINF = 143,               // 143 �����豸������Ϣ
	CMD_SEARCH_DEVICEINF_FORTAGID = 144,      // 144 ʹ�ð󶨵ı���id��ѯ�豸������Ϣ

	CMD_ADD_METERINF = 150,                   // 150 ��Ӽ�������Ϣ
	CMD_DELETE_METERINF = 151,                // 151 ɾ����������Ϣ
	CMD_MODIFY_METERINF = 152,                // 152 �޸ļ�������Ϣ
	CMD_SEARCH_METERINF = 153,                // 153 ��ѯ��������Ϣ

	CMD_ADD_DEVICEPARAMETER = 160,            // 160 ����豸����
	CMD_DELETE_DEVICEPARAMETER = 161,         // 161 ɾ���豸����
	CMD_SEARCH_DEVICEPARAMETER = 162,         // 162 ��ѯ�豸����

	CMD_ADD_METERPARAMETER = 170,            // 170 �����Դ�ɼ�����
	CMD_DELETE_METERPARAMETER = 171,         // 171 ɾ����Դ�ɼ�����
	CMD_SEARCH_METERPARAMETER = 172,         // 172 ��ѯ��Դ�ɼ�����

	CMD_ADD_DEVICEREPAIRINF = 180,            // 180 ����豸ά����Ϣ
	CMD_DELETE_DEVICEREPAIRINF = 181,         // 181 ɾ���豸ά����Ϣ
	CMD_MODIFY_DEVICEREPAIRINF = 182,         // 182 �޸��豸ά����Ϣ
	CMD_SEARCH_DEVICEREPAIRINF = 183,         // 183 ��ѯ�豸ά����Ϣ
	CMD_SEARCH_REPAIRINF_FORDEVICE = 184,     // 184 ʹ���豸��ѯ�豸ά�޵���
	CMD_SEARCH_REPAIRINF_FORTAGID = 185,      // 185 ʹ�ð󶨵ı���id��ѯ�豸ά�޵���

	CMD_ADD_RUNINF = 190,                     // 190 ����豸������Ϣ
	CMD_DELETE_RUNINF = 191,                  // 191 ɾ���豸������Ϣ
	CMD_MODIFY_RUNINF = 192,                  // 192 �޸��豸������Ϣ
	CMD_SEARCH_RUNINF = 193,                  // 193 ��ѯ�豸������Ϣ
	CMD_SEARCH_RUNINF_FORDEVICE = 194,        // 194 ʹ���豸��ѯ�豸���е���
	CMD_SEARCH_RUNINF_FORTAGID = 195,         // 195 ʹ�ð󶨵ı���id��ѯ�豸���е���

	CMD_SEARCH_METERVALUE = 200,              // 200 ��ѯ���������

	CMD_SEARCH_DEVICEVALUE = 210,             // 210 ���豸��ѯ����ֵ

	CMD_SEARCH_ENERGYSTATISTICS = 220,        // 220 ��ѯ��Դͳ�ƽ��

	CMD_SEARCH_TAGVALUE = 230,                // 230 ��ѯ����ֵ

	CMD_SEARCH_SYSTEM = 240,                  // 240 ��ѯ�����ϵͳ
	CMD_SEARCH_DEVICE = 250,                  // 250 ��ѯ����豸
	CMD_SEARCH_TAG = 260,                     // 260 ��ѯ������
	CMD_SEARCH_TAG_WITHOUTPARAMETER = 261,    // 261 ��ѯ�����������Ѿ����ø��豸�ͼ������

	CMD_SEARCH_OVERVIEW = 270,               //270 ��ѯ������Ϣ
	CMD_SEARCH_SUBSYSTEM_ALARM_COUNT = 271,  //271 ϵͳ�µı�������
  //end add by gaoxin 2017.10.12
  CMD_ADD_CHANGE_SHIFTS = 290,        // 290 ��ӽ��Ӱ���Ϣ
  CMD_CHECK_DB_STATUS = 300,          // 300 ������ݿ�״̬
  CMD_CHECK_OPC_STATUS,               // 301 ���OPC״̬

  CMD_CAMERA_SCREENSHOT = 1000,       // 1000 ��Ƶ��ͼ
  CMD_SEARCH_CAMERA_CONFIG = 1001,    // 1001 ��ѯ����ͷ����

  CMD_READ_LINKAGE_WATCH_INFO = 1100, // 1100 ��ȡ����������Ϣ
  CMD_LINKAGE_MANUAL_CONFIRMATION,    // 1101 �˹�����
  CMD_LINKAGE_TERMINATE_TASK,         // 1102 ��ֹ����

  CMD_READ_TIMETABLE_DISPATCH_WATCH_INFO = 1200,  // 1200 ��ȡʱ�����ȼ�����Ϣ
  CMD_TIMETABLE_DISPATCH_TERMINATE_TASK,          // 1201 ��ֹ����
  CMD_LAST                // δ֪����
}ECommand;

// �û�����
typedef enum class _eUserType : uint32_t
{
  GENERAL = 0,            // 0 ��ͨ�û�
  ADMINISTRATORS,         // 1 ����Ա�û�

  LAST
}EUserType;

// ȫ��Cookie�б����COM��������
typedef enum
{
  COOKIE_SERVER = 0,
  COOKIE_RCX_DATA,
  COOKIE_HAC,
  COOKIE_LAST
}ECookie;

// Tag��������
typedef enum
{
  TYPE_BOOL = 0,
  TYPE_INTEGER,
  TYPE_FLOAT,
  TYPE_STRING,
  TYPE_LAST
}ETagType;

// ��ʱ�������δ����¼����ͱ���
#define _EVENT_TYPE_TMP_
#ifdef _EVENT_TYPE_TMP_
typedef uint32_t EEventType;
#else
// �¼�����
// Event Type
typedef enum {
  EVENT_TYPE_0 = 0,
  EVENT_TYPE_1,
  EVENT_TYPE_LAST
}EEventType;
#endif // _EVENT_TYPE_TMP_

// Tag�ṹ��
typedef struct _TagInfo
{
  uint32_t    ulGID;      // ȫ�ֱ���
  boost::any  anyValue;   // ����ֵ
  uint16_t    sQuality;   //����
  uint64_t    ullTimeMs;  // ʱ�䣨���룩
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
  uint64_t    ullTimeMs;  // ʱ�䣨���룩
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
  uint32_t    ulGID;      // ȫ�ֱ���
  uint64_t    ullTimeMs;  // ʱ�䣨���룩
  uint32_t    ulID;
  std::string strUserName;
  uint32_t    ulCategory;
  std::string strDescribe;
  uint32_t    ulPriority;
  bool        bAck;

  // û�ҵ���ֵ����
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

// OPC״̬��
typedef enum class _eOPCStatus : uint32_t
{
  OK = 0,     // 0  ����
  UNCONNECT,  // 1  ��������
  LAST
}EOPCStatus;

// OPC״̬��Ϣ
typedef struct _OPCStatusInfo
{
  uint32_t    ulID;     // OPC��ϵͳID
  std::string strName;  // ��ϵͳ����
  EOPCStatus  eStatus;  // ��ϵͳ״̬
}KOPCStatusInfo;

#endif // !_DEFINE_H_
