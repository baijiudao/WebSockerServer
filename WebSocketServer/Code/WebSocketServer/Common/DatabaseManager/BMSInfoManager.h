//------------------------------------------------------------------------------
// �������ڣ�2017-10-11 14:00:45
// �ļ�������������д���ݿ����Ϣ
//------------------------------------------------------------------------------
#ifndef _BMS_INFO_MANAGER_H_
#define _BMS_INFO_MANAGER_H_

#include "TableBase.h"

// �豸������Ϣ
typedef struct _SupplierInfo
{
    string sSupplierCode;   //��Ӧ�̴���
    string sSupplierName;   //��Ӧ������
    string sConPerson;      //��ϵ��
    string sConDepartment;  //��ϵ����
    string sConTelephone;   //��ϵ�绰
    string sFax;            //����
    string sAddress;        //��ַ
    string sZipCode;        //��������
    string sE_Mail;         //��������
    string sURL;            //��ַ
    string sRemarks;        //��ע
    _SupplierInfo()
    {
        sSupplierCode = "";
        sSupplierName = "";
        sConPerson = "";
        sConDepartment = "";
        sConTelephone = "";
        sFax = "";
        sAddress = "";
        sZipCode = "";
        sE_Mail = "";
        sURL = "";
        sRemarks = "";
    }
}SupplierInfo;

typedef struct _SupplierInfoAndID
{
    uint32_t ulGID;
    SupplierInfo supplierInfo;
}SupplierInfoAndID;

// �豸������Ϣ
typedef struct _DeviceInfo
{
    string DeviceNum;               //�豸���
    string DeviceName;              //�豸����
    string InstallLocation;         //��װλ��
    string UseDate;                 //ʹ������
    string UsageDepart;             //ʹ�ò���
    string DeviceStatus;            //�豸״��
    string ProductionDate;          //��������
    string DeviceSupplier;          //�豸��Ӧ��
    double AlarmUpLimit;            //��������
    double AlarmDownLimit;          //��������
    double RunCtrlUpLimit;          //���п�������
    double RunCtrlDownLimit;        //���п�������
    _DeviceInfo()
    {
        DeviceNum = "";
        DeviceName = "";
        InstallLocation = "";
        UseDate = "";
        UsageDepart = "";
        DeviceStatus = "";
        ProductionDate = "";
        DeviceSupplier = "";
        AlarmUpLimit = 0;
        AlarmDownLimit = 0;
        RunCtrlUpLimit = 0;
        RunCtrlDownLimit = 0;
    }
}DeviceInfo;

typedef struct _DeviceInfoAndID
{
    uint32_t ulGID;
    DeviceInfo deviceInfo;
}DeviceInfoAndID;

// �豸ά�޼�¼
typedef struct _RepairInfo
{
    int recordingID;             //��¼id
    string DeviceName;              //�豸����
    string RepairContent;           //ά������
    string RepairTime;              //ά��ʱ��
    string FaultReason;             //����ԭ��
    string RepairResults;           //ά�޽��
    string RepairManager;           //ά�޾�����
    string RepairUnit;              //ά�޵�λ
    string Remarks;                 //��ע
    _RepairInfo()
    {
        recordingID = 0;
        DeviceName = "";
        RepairContent = "";
        RepairTime = "";
        FaultReason = "";
        RepairResults = "";
        RepairManager = "";
        RepairUnit = "";
        Remarks = "";
    }
}RepairInfo;

typedef struct _RepairInfoAndID
{
    uint32_t ulGID;
    RepairInfo repairInfo;
}RepairInfoAndID;

// �豸������Ϣ
typedef struct _DeviceRunInfo
{
    int recordingID;             //��¼id
    string DeviceName;              //�豸����
    string StartTime;               //��ʼʱ��
    string EndTime;                 //����ʱ��
    string Operation;               //�������
    string Recorder;                //��¼Ա
    string Remarks;                 //��ע
    _DeviceRunInfo()
    {
        recordingID = 0;
        DeviceName = "";
        StartTime = "";
        EndTime = "";
        Operation = "";
        Recorder = "";
        Remarks = "";
    }
}DeviceRunInfo;

typedef struct _DeviceRunInfoAndID
{
    uint32_t ulGID;
    DeviceRunInfo deviceRunInfo;
}DeviceRunInfoAndID;

// ������������Ϣ
typedef struct _MeterType
{
    string strMeterType;       //����������
    string TypeDes;        //��������
    _MeterType()
    {
        strMeterType = "";
        TypeDes = "";
    }
}MeterType;

//���������Ϣ
typedef struct _EnergyCate
{
    string strEnergyCate;       //�������
    string TypeDes;        //��������
    _EnergyCate()
    {
        strEnergyCate = "";
        TypeDes = "";
    }
}EnergyCate;

//�豸���������Ϣ
typedef struct _DevFaultCate
{
    string FaultCate;          //�������
    string CategoryDes;        //���˵��
    string FaultFeature;       //��������
    _DevFaultCate()
    {
        FaultCate = "";
        CategoryDes = "";
        FaultFeature = "";
    }
}DevFaultCate;

// �豸������Ϣ
typedef struct _MeterInfo
{
    string MeterNumber;               //��������
    string strMeterType;              //����������
    string strEnergyCate;             //�������
    string MeterModel;                //���ͺ�
    string UseUnits;                  //ʹ�õ�λ
    double RateStandard;              //���ʱ�׼
    string StandardUnit;              //��׼������λ
    string InstallDate;               //��װ����
    string InstallLocation;           //��װλ��
    string ProductionFactory;         //���ɳ���
    short MeterMagnification;         //����
    double TranMeterBase;             //ת�����
    double MeterInitValue;            //��ĳ�ʼ����
    double TranMeterValue;            //ת�����
    string Remarks;                   //��ע
    string OldMeterNumber;            //�ɱ���
    uint32_t tagID;                   //��Դ����id

    _MeterInfo()
    {
        MeterNumber = "";
        strMeterType = "";
        strEnergyCate = "";
        MeterModel = "";
        UseUnits = "";
        RateStandard = 0;
        StandardUnit = "";
        InstallDate = "";
        InstallLocation = "";
        ProductionFactory = "";
        MeterMagnification = 0;
        TranMeterBase = 0;
        MeterInitValue = 0;
        TranMeterValue = 0;
        Remarks = "";
        tagID = 0;
        OldMeterNumber = "";
    }
}MeterInfo;

// �豸����
typedef struct _DeviceParameter
{
    string DeviceName;               //�豸����
    map<uint32_t,string> lstGID;     //����id�б�
    _DeviceParameter()
    {
        DeviceName = "";
    }
}DeviceParameter;

// ���������
typedef struct _MeterParameter
{
    string MeterNum;                //��������
    uint32_t ulGID;                 //����id�б�
    _MeterParameter()
    {
        MeterNum = "";
        ulGID = 0;
    }
}MeterParameter;

//�����ϵͳ
typedef struct _SubSysInfo
{
    map<string, string> lstSubSys;     //��ϵͳ�б�id��name��
}SubSysInfo;

//����豸
typedef struct _DeviceVarInfo
{
    string SysID;
    map<string, string> lstDevice;     //�豸�б�id��name��
    _DeviceVarInfo()
    {
        SysID = "";
    }
}DeviceVarInfo;

//������
typedef struct _TagVarInfo
{
    string DevID;
    map<string, string> lstTag;     //�����б�id��name��
    _TagVarInfo()
    {
        DevID = "";
    }
}TagVarInfo;

//��ҳ��ѯ�й�������
typedef struct _PublicRequestInfo
{
    uint64_t ullStartTime;
    uint64_t ullEndTime;
    int pageSize;
    int pageNum;
    _PublicRequestInfo()
    {
        ullStartTime = 0;
        ullEndTime = 0;
        pageSize = 0;
        pageNum = 0;
    }
}PublicRequestInfo;

//������/�豸/��������ṹ��
typedef struct _ValueRequestInfo
{
    PublicRequestInfo publicInfo;
    string strName;
    _ValueRequestInfo()
    {
        strName = "";
    }
}ValueRequestInfo;

//����ֵ
typedef struct _ValueInfo
{
    uint32_t ulId;
    string tagName;
    string tagValue;
    string collectionTime;
    _ValueInfo()
    {
        ulId = 0;
        tagName = "";
        tagValue = "";
        collectionTime = "";
    }
}ValueInfo;

//����������ṹ��
typedef struct _MeterValueInfo
{
    string meterNumber;
    string meterType;
    ValueInfo valueInfo;
    _MeterValueInfo()
    {
        meterNumber = "";
        meterType = "";
    }
}MeterValueInfo;

//�������������
typedef struct _ResponseMeterValueInfo
{
    int pageSize;
    int pageNum;
    int pageTotal;
    list<MeterValueInfo> lstValueInfo;
    _ResponseMeterValueInfo()
    {
        pageSize = 0;
        pageNum = 0;
        pageTotal = 0;
    }
}ResponseMeterValueInfo;

//�豸���������
typedef struct _DeviceValueInfo
{
    int pageSize;
    int pageNum;
    int pageTotal;
    list<ValueInfo> lstValueInfo;
    _DeviceValueInfo()
    {
        pageSize = 0;
        pageNum = 0;
        pageTotal = 0;
    }
}DeviceValueInfo;

//��ʷ�¼�����ṹ��
typedef struct _HistoryEventRequestInfo
{
    PublicRequestInfo publicInfo;
    string strUserName;
    EEventType enventType;
    
    _HistoryEventRequestInfo()
    {
        strUserName = "";
        enventType = 0;
    }
}HistoryEventRequestInfo;

//��ʷ�澯����ṹ��
typedef struct _HistoryAlarmRequestInfo
{
    PublicRequestInfo publicInfo;
    string strDeviceName;
    uint32_t    ulPriority;
    string strArea;
    _HistoryAlarmRequestInfo()
    {
        strDeviceName = "";
        ulPriority = 0;
        strArea = "";
    }
}HistoryAlarmRequestInfo;

typedef struct _HistoryEventInfo
{
    int pageSize;
    int pageNum;
    int pageTotal;
    list<KEventInfo> lstValueInfo;
    _HistoryEventInfo()
    {
        pageSize = 0;
        pageNum = 0;
        pageTotal = 0;
    }
}HistoryEventInfo;

// Alarm
typedef struct _AlarmInfoEx
{
    uint32_t    ulGID;      // ȫ�ֱ���
    uint64_t    ullTimeMs;  // ʱ�䣨���룩
    uint32_t    ulID;       // �澯id
    uint32_t    ulCategory;
    uint32_t    ulType;
    uint32_t    ulPriority;
    string      strSource;
    string      strValue;
    string      strMessage;
    string      strLimit;
    bool        bAck;

    _AlarmInfoEx()
    {
        ulGID = 0;
        ullTimeMs = 0;
        ulID = 0;
        ulCategory = 0;
        ulType = 0;
        ulPriority = 0;
        strSource = "";
        strValue = "";
        strMessage = "";
        strLimit = "";
        bAck = false;
    }
}KAlarmInfoEx;

typedef struct _HistoryAlarmInfo
{
    int pageSize;
    int pageNum;
    int pageTotal;
    list<KAlarmInfoEx> lstValueInfo;
    _HistoryAlarmInfo()
    {
        pageSize = 0;
        pageNum = 0;
        pageTotal = 0;
    }
}HistoryAlarmInfo;

typedef struct _CommonData
{
	string subsystemId;
	int deviceCount;
	int tagCount;
	//map<shared_ptr<DeviceVarInfo>, int> commonData;
}CommonData;

typedef struct _OverViewInfo
{
	list<shared_ptr<CommonData>> overData;
}OverViewInfo;

typedef struct _Subsystem_Alarm
{
	map<string, int> alarmInfo;
}Subsystem_Alarm;

class BMSInfoManager : public CTableBase
{
public:
    BMSInfoManager();
    ~BMSInfoManager();
    EErrCode SearchSupplierForTagID(const otl_connect &dbconnect, const uint32_t &lstID, std::shared_ptr<SupplierInfoAndID> &ptResult);
    EErrCode SearchDeviceInfForTagID(const otl_connect &dbconnect, const uint32_t &lstID, std::shared_ptr<DeviceInfoAndID> &ptResult);
    EErrCode SearchRepairInfForTagID(const otl_connect &dbconnect, const uint32_t &lstID, std::shared_ptr<RepairInfoAndID> &ptResult);
    EErrCode SearchRunInfForTagID(const otl_connect &dbconnect, const uint32_t &lstID, std::shared_ptr<DeviceRunInfoAndID> &ptResult);

    EErrCode AddSuplier(const otl_connect &dbconnect, const std::shared_ptr<SupplierInfo> &ptResult);
    EErrCode DeleteSupplier(const otl_connect &dbconnect, const string supplierName);
    EErrCode ModifySuplier(const otl_connect &dbconnect, const std::shared_ptr<SupplierInfo> &ptResult);
    EErrCode SearchSuplier(const otl_connect &dbconnect, const string supplierName, list<std::shared_ptr<SupplierInfo>> &ptResult);

    EErrCode AddMeterType(const otl_connect &dbconnect, const std::shared_ptr<MeterType> &ptResult);
    EErrCode DeleteMeterType(const otl_connect &dbconnect, const string meterTypeName);
    EErrCode ModifyMeterType(const otl_connect &dbconnect, const std::shared_ptr<MeterType> &ptResult);
    EErrCode SearchMeterType(const otl_connect &dbconnect, const string meterTypeName, list<std::shared_ptr<MeterType>> &ptResult);

    EErrCode AddEnergyCate(const otl_connect &dbconnect, const std::shared_ptr<EnergyCate> &ptResult);
    EErrCode DeleteEnergyCate(const otl_connect &dbconnect, const string name);
    EErrCode ModifyEnergyCate(const otl_connect &dbconnect, const std::shared_ptr<EnergyCate> &ptResult);
    EErrCode SearchEnergyCate(const otl_connect &dbconnect, const string name, list<std::shared_ptr<EnergyCate>> &ptResult);

    EErrCode AddDevFaultCate(const otl_connect &dbconnect, const std::shared_ptr<DevFaultCate> &ptResult);
    EErrCode DeleteDevFaultCate(const otl_connect &dbconnect, const string name);
    EErrCode ModifyDevFaultCate(const otl_connect &dbconnect, const std::shared_ptr<DevFaultCate> &ptResult);
    EErrCode SearchDevFaultCate(const otl_connect &dbconnect, const string name, list<std::shared_ptr<DevFaultCate>> &ptResult); 

    EErrCode AddDeviceInfo(const otl_connect &dbconnect, const std::shared_ptr<DeviceInfo> &ptResult);
    EErrCode DeleteDeviceInfo(const otl_connect &dbconnect, const string name);
    EErrCode ModifyDeviceInfo(const otl_connect &dbconnect, const std::shared_ptr<DeviceInfo> &ptResult);
    EErrCode SearchDeviceInfo(const otl_connect &dbconnect, const string name, list<std::shared_ptr<DeviceInfo>> &ptResult);

    EErrCode AddMeterInfo(const otl_connect &dbconnect, const std::shared_ptr<MeterInfo> &ptResult);
    EErrCode DeleteMeterInfo(const otl_connect &dbconnect, const string name);
    EErrCode ModifyMeterInfo(const otl_connect &dbconnect, const std::shared_ptr<MeterInfo> &ptResult);
    EErrCode SearchMeterInfo(const otl_connect &dbconnect, const string name, list<std::shared_ptr<MeterInfo>> &ptResult);

    EErrCode AddDeviceParameter(const otl_connect &dbconnect, const std::shared_ptr<DeviceParameter> &ptResult);
    EErrCode DeleteDeviceParameter(const otl_connect &dbconnect, const std::shared_ptr<DeviceParameter> &ptResult);
    EErrCode SearchDeviceParameter(const otl_connect &dbconnect, const string name, std::shared_ptr<DeviceParameter> &ptResult);

    EErrCode AddMeterParameter(const otl_connect &dbconnect, const std::shared_ptr<MeterParameter> &ptResult);
    EErrCode DeleteMeterParameter(const otl_connect &dbconnect, const std::shared_ptr<MeterParameter> &ptResult);
    EErrCode SearchMeterParameter(const otl_connect &dbconnect, const string name, std::shared_ptr<MeterParameter> &ptResult);
    
    EErrCode AddDeviceRepair(const otl_connect &dbconnect, const std::shared_ptr<RepairInfo> &ptResult, uint32_t & ulRecordingID);
    EErrCode DeleteDeviceRepair(const otl_connect &dbconnect, const int name);
    EErrCode ModifyDeviceRepair(const otl_connect &dbconnect, const std::shared_ptr<RepairInfo> &ptResult);
    EErrCode SearchDeviceRepair(const otl_connect &dbconnect, const int name, list<shared_ptr<RepairInfo>> &ptResult);
    EErrCode SearchDeviceRepair(const otl_connect &dbconnect, const string name, list<shared_ptr<RepairInfo>> &ptResult);

    EErrCode AddDeviceRun(const otl_connect &dbconnect, const std::shared_ptr<DeviceRunInfo> &ptResult, uint32_t & ulRecordingID);
    EErrCode DeleteDeviceRun(const otl_connect &dbconnect, const int name);
    EErrCode ModifyDeviceRun(const otl_connect &dbconnect, const std::shared_ptr<DeviceRunInfo> &ptResult);
    EErrCode SearchDeviceRun(const otl_connect &dbconnect, const int name, list<shared_ptr<DeviceRunInfo>> &ptResult);
    EErrCode SearchDeviceRun(const otl_connect &dbconnect, const string name, list<shared_ptr<DeviceRunInfo>> &ptResult);

    EErrCode SearchSubSysVariableInfo(const otl_connect &dbconnect, shared_ptr<SubSysInfo> &ptResult);
    EErrCode SearchDeviceVariableInfo(const otl_connect &dbconnect, const string name, shared_ptr<DeviceVarInfo> &ptResult);
    EErrCode SearchTagVariableInfo(const otl_connect &dbconnect, const string name, shared_ptr<TagVarInfo> &ptResult);
 
	//����
	EErrCode SearchOverViewInfo(const otl_connect &dbconnect, shared_ptr<OverViewInfo> &ptResult);

	EErrCode SearchTagVarWithoutPar(const otl_connect &dbconnect, const string name, shared_ptr<TagVarInfo> &ptResult);
    
    EErrCode SearchMeterValueInfo(const otl_connect &dbconnect, const shared_ptr<ValueRequestInfo>  ptRequestInfo, shared_ptr<ResponseMeterValueInfo> &ptResult);
    EErrCode SearchDeviceValueInfo(const otl_connect &dbconnect, const shared_ptr<ValueRequestInfo>  ptRequestInfo, shared_ptr<DeviceValueInfo> &ptResult);
    EErrCode SearchVariableValueInfo(const otl_connect &dbconnect, const shared_ptr<ValueRequestInfo>  ptRequestInfo, shared_ptr<DeviceValueInfo> &ptResult);

    EErrCode SearchHistoryEventExInfo(const otl_connect &dbconnect, const shared_ptr<HistoryEventRequestInfo>  ptRequestInfo, shared_ptr<HistoryEventInfo> &ptResult);
    EErrCode SearchHistoryAlarmExInfo(const otl_connect &dbconnect, const shared_ptr<HistoryAlarmRequestInfo>  ptRequestInfo, shared_ptr<HistoryAlarmInfo> &ptResult);
	EErrCode SearchSystemAlarmCountInfo(const otl_connect &dbconnect, shared_ptr<Subsystem_Alarm> &ptResult);
private:
    bool SearchDeviceFromID(const otl_connect &dbconnect, const uint32_t &lstID, string &devResult);
    bool NameExists(const otl_connect &dbconnect, const string tableName, const string keyName, const string &keyValue);
    bool IdExists(const otl_connect &dbconnect, const string tableName, const string keyIdName, const int &keyIdValue = 0);
    otl_datetime TransformTimeString(const string &strTime);
    string TransformTimeOtlTime(const otl_datetime &otlTime);
    string TransformTimeUnint64(uint64_t ulltime);

    otl_datetime m_otlTime;
};

#endif // !_BMS_INFO_MANAGER_H_
