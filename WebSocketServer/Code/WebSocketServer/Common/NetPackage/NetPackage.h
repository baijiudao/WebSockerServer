//------------------------------------------------------------------------------
// �������ڣ�2017-06-12 15:07:17
// �ļ����������ļ��������/������ݰ�
//------------------------------------------------------------------------------
#ifndef _NET_PACKAGE_H_
#define _NET_PACKAGE_H_

#include <Singleton.h>
#include "LoginPackage.h"
#include "TagPackage.h"
#include "EventPackage.h"
#include "AlarmPackage.h"
#include "AlarmAssistantPackage.h"
#include "ChgShiftsPackage.h"
#include "CheckStatusPackage.h"
#include "BMSInfoPackage.h"
#include "CameraControlPackage.h"
#include "LinkagePackage.h"
#include "TimetableDispatchPackage.h"

//------------------------------------------------------------------------------
// �������ڣ�2017-09-06 16:27:24 
// �� �� ���������ں�ʹ��jsoncpp��������˽������ݱ�����ANSI��ʽ���������������Ҳ��ANSI��
//------------------------------------------------------------------------------
class CNetPackage : public ISingleton<CNetPackage>
{
public:
  CNetPackage();
  ~CNetPackage();
  // ��ȡCommand
  ECommand  GetCommand(const std::string &strRecv);
  // Ĭ�ϰ�
  bool  BuildDefault(const std::string &strSID, ECommand eCmd, EErrCode eEC, std::string &strSend);
  // �����������ݰ�
  bool  ParseLogin(const std::string &strRecv, std::string &strUserName, std::string &strPassword);
  bool  ParseLogout(const std::string &strRecv, std::string &strSID);
  bool  ParseReadTag(const std::string &strRecv, std::string &strSID, std::list<shared_ptr<KTagInfo>> &lstInfo);
  bool  ParseWriteTag(const std::string &strRecv, std::string &strSID, std::list<shared_ptr<KTagInfo>> &lstInfo);
  bool  ParseWriteEvent(const std::string &strRecv, std::string &strSID, std::list<shared_ptr<KEventInfo>> &lstInfo);
  bool  ParseReadHistoryEvent(const std::string &strRecv, std::string &strSID, uint64_t &ullStartTime, uint64_t &ullEndTime);
  bool  ParseReadHistoryAlarm(const std::string &strRecv, std::string &strSID, uint64_t &ullStartTime, uint64_t &ullEndTime);
  // �澯��������
  bool  ParseAdd_AlarmAssistant(const std::string &strRecv, std::string &strSID, std::list<shared_ptr<KTBAlarmAssistant>> &lstInfo);
  bool  ParseDelete_AlarmAssistant(const std::string &strRecv, std::string &strSID, std::list<uint32_t> &lstID);
  bool  ParseModify_AlarmAssistant(const std::string &strRecv, std::string &strSID, std::list<std::shared_ptr<KTBAlarmAssistant>> &lstInfo);
  bool  ParseSearch_AlarmAssistant(const std::string &strRecv, std::string &strSID, uint32_t &ulGlobalID);
  bool  ParseSearch_Num_AlarmAssistant(const std::string &strRecv, std::string &strSID, std::shared_ptr<KTBAlarmAssistantSearchCondition_Num> ptCondition);
  bool  ParseSearchPaging_AlarmAssistant(const std::string &strRecv, std::string &strSID, std::shared_ptr<KTBAlarmAssistantSearchCondition> ptCondition);
  // ���Ӱ����
  bool  ParseAdd_ChangeShifts(const std::string &strRecv, std::string &strSID, std::list<shared_ptr<KTBChgShifts>> &lstInfo);
  // ״̬������
  bool  ParseCheckDBStatus(const std::string &strRecv, std::string &strSID);
  bool  ParseCheckOPCStatus(const std::string &strRecv, std::string &strSID);
  // �ӱ���id��ѯ�豸�����Ϣ����
  bool ParseSearchID(const std::string &strRecv, std::string &strSID, uint32_t &lstInfo);
  
  //��Ϣ����Ӧ��
  bool ParseAddOrModSupplier(const std::string &strRecv, std::string &strSID, shared_ptr<SupplierInfo>&lstInfo);
  bool ParseDelOrSelSupplier(const std::string &strRecv, std::string &strSID, string &lstInfo);
  //��Ϣ�������������
  bool ParseAddOrModMeterType(const std::string &strRecv, std::string &strSID, shared_ptr<MeterType>&lstInfo);
  bool ParseDelOrSelMeterType(const std::string &strRecv, std::string &strSID, string &lstInfo);
  //��Ϣ����������
  bool ParseAddOrModEnergyCate(const std::string &strRecv, std::string &strSID, shared_ptr<EnergyCate>&lstInfo);
  bool ParseDelOrSelEnergyCate(const std::string &strRecv, std::string &strSID, string &lstInfo);
  //��Ϣ�����豸�������
  bool ParseAddOrModDevFaultCate(const std::string &strRecv, std::string &strSID, shared_ptr<DevFaultCate>&lstInfo);
  bool ParseDelOrSelDevFaultCate(const std::string &strRecv, std::string &strSID, string &lstInfo);
  //��Ϣ�����豸������Ϣ
  bool ParseAddOrModDeviceInfo(const std::string &strRecv, std::string &strSID, shared_ptr<DeviceInfo>&lstInfo);
  bool ParseDelOrSelDeviceInfo(const std::string &strRecv, std::string &strSID, string &lstInfo);
  //��Ϣ�������������Ϣ
  bool ParseAddOrModMeterInfo(const std::string &strRecv, std::string &strSID, shared_ptr<MeterInfo>&lstInfo);
  bool ParseDelOrSelMeterInfo(const std::string &strRecv, std::string &strSID, string &lstInfo);
  //��Ϣ�����豸����
  bool ParseAddDeviceParameter(const std::string &strRecv, std::string &strSID, shared_ptr<DeviceParameter>&lstInfo);
  bool ParseDelDeviceParameter(const std::string &strRecv, std::string &strSID, shared_ptr<DeviceParameter>&lstInfo);
  bool ParseSearchDeviceParameter(const std::string &strRecv, std::string &strSID, string &lstInfo);
  //��Ϣ������������
  bool ParseAddMeterParameter(const std::string &strRecv, std::string &strSID, shared_ptr<MeterParameter>&lstInfo);
  bool ParseDelMeterParameter(const std::string &strRecv, std::string &strSID, shared_ptr<MeterParameter>&lstInfo);
  bool ParseSearchMeterParameter(const std::string &strRecv, std::string &strSID, string &lstInfo);
  //��Ϣ�����豸ά�޼�¼
  bool ParseAddOrModDeviceRepair(const std::string &strRecv, std::string &strSID, shared_ptr<RepairInfo>&lstInfo);
  bool ParseDelOrSelDeviceRepair(const std::string &strRecv, std::string &strSID, int &lstInfo);
  bool ParseSearchDeviceRepair(const std::string &strRecv, std::string &strSID, string &lstInfo);
  //��Ϣ�����豸���м�¼
  bool ParseAddOrModDeviceRun(const std::string &strRecv, std::string &strSID, shared_ptr<DeviceRunInfo>&lstInfo);
  bool ParseDelOrSelDeviceRun(const std::string &strRecv, std::string &strSID, int &lstInfo);
  bool ParseSearchDeviceRun(const std::string &strRecv, std::string &strSID, string &lstInfo);
  
  //��Ϣ��������Ϣ
  bool ParseSearchSubSysVariableInfo(const std::string &strRecv, std::string &strSID);
  bool ParseSearchDeviceVariableInfo(const std::string &strRecv, std::string &strSID, std::string &lstInfo);
  bool ParseSearchTagVariableInfo(const std::string &strRecv, std::string &strSID, string &lstInfo);

  //����
  bool ParseSearchOverViewInfo(const std::string &strRecv, std::string &strSID, string &lstInfo);
  
  //��Ϣ���������/�豸/��������
  bool ParseSearchMeterValueInfo(const std::string &strRecv, std::string &strSID, shared_ptr<ValueRequestInfo> &lstInfo);
  bool ParseSearchDeviceValueInfo(const std::string &strRecv, std::string &strSID, shared_ptr<ValueRequestInfo> &lstInfo);
  bool ParseSearchVariableValueInfo(const std::string &strRecv, std::string &strSID, shared_ptr<ValueRequestInfo> &lstInfo);
 
  //��Ϣ������ʷ�¼�/�澯
  bool ParseSearchHistoryEventExInfo(const std::string &strRecv, std::string &strSID, shared_ptr<HistoryEventRequestInfo> &lstInfo);
  bool ParseSearchHistoryAlarmExInfo(const std::string &strRecv, std::string &strSID, shared_ptr<HistoryAlarmRequestInfo> &lstInfo);
  // ����ͷ��ͼ����
  bool  ParseCameraScreenshot(const std::string &strRecv, std::string &strSID, std::shared_ptr<KCamScreenshotInfo> ptInfo);
  bool  ParseSearchCameraConfig(const std::string &strRecv, std::string &strSID);
  // ������Ϣ
  bool  ParseReadLinkageWatchInfo(const std::string &strRecv, std::string &strSID);
  bool  ParseLinkageManualConfirmation(const std::string &strRecv, std::string &strSID, uint32_t &ulItemID);
  bool  ParseLinkageTerminateTask(const std::string &strRecv, std::string &strSID, uint32_t &ulItemID);
  // ʱ��������Ϣ
  bool  ParseReadTimetableDispatchWatchInfo(const std::string &strRecv, std::string &strSID);
  bool  ParseTimetableDispatchTerminateTask(const std::string &strRecv, std::string &strSID, uint32_t &ulItemID);

  // �齨�������ݰ�
  bool  BuildLogin(const std::string &strSID, EErrCode eEC, EUserType eType, std::string &strSend);
  bool  BuildLogout(EErrCode eEC, std::string &strSend);
  bool  BuildReadTag(const std::string &strSID, EErrCode eEC, const std::list<shared_ptr<KTagInfo>> &lstInfo, std::string &strSend);
  bool  BuildWriteTag(const std::string &strSID, EErrCode eEC, const std::list<shared_ptr<KTagInfo>> &lstInfo, std::string &strSend);
  bool  BuildWriteEvent(const std::string &strSID, EErrCode eEC, std::string &strSend);
  bool  BuildPushTag(const std::string &strSID, EErrCode eEC, const std::list<shared_ptr<KTagInfo>> &lstInfo, std::string &strSend);
  bool  BuildReadHistoryEvent(const std::string &strSID, EErrCode eEC, const std::list<shared_ptr<KEventInfo>> &lstInfo, std::string &strSend);
  bool  BuildPushEvent(const std::string &strSID, EErrCode eEC, const std::list<shared_ptr<KEventInfo>> &lstInfo, std::string &strSend);
  bool  BuildReadHistoryAlarm(const std::string &strSID, EErrCode eEC, const std::list<shared_ptr<KAlarmInfo>> &lstInfo, std::string &strSend);
  bool  BuildPushAlarm(const std::string &strSID, EErrCode eEC, const std::list<shared_ptr<KAlarmInfo>> &lstInfo, std::string &strSend);
  // �澯�����齨
  bool  BuildAdd_AlarmAssistant(const std::string &strSID, EErrCode eEC, const std::list<uint32_t> &lstID, std::string &strSend);
  bool  BuildDelete_AlarmAssistant(const std::string &strSID, EErrCode eEC, const std::list<uint32_t> &lstID, std::string &strSend);
  bool  BuildModify_AlarmAssistant(const std::string &strSID, EErrCode eEC, const std::list<uint32_t> &lstID, std::string &strSend);
  bool  BuildSearch_AlarmAssistant(const std::string &strSID, EErrCode eEC, const std::list<std::shared_ptr<KTBAlarmAssistant>> &lstInfo, std::string &strSend);
  bool  BuildSearch_Num_AlarmAssistant(const std::string &strSID, EErrCode eEC, const std::shared_ptr<KTBAlarmAssistantSearchResult_Num> ptResult, std::string &strSend);
  bool  BuildSearchPaging_AlarmAssistant(const std::string &strSID, EErrCode eEC, const std::shared_ptr<KTBAlarmAssistantSearchResult> ptResult, std::string &strSend);
  // ���Ӱ��齨
  bool  BuildAdd_ChangeShifts(const std::string &strSID, EErrCode eEC, const std::list<uint32_t> &lstID, std::string &strSend);
  // ״̬����齨
  bool  BuildCheckDBStatus(const std::string &strSID, EErrCode eEC, const std::list<std::shared_ptr<KDBStatusInfo>> &lstInfo, std::string &strSend);
  bool  BuildCheckOPCStatus(const std::string &strSID, EErrCode eEC, const std::list<std::shared_ptr<KOPCStatusInfo>> &lstInfo, std::string &strSend);

  // �ӱ���id��ѯ�豸�����Ϣ����
  bool BuildSearchSupplierForTagID(std::string &strSID, EErrCode eEC, const std::shared_ptr<SupplierInfoAndID> &lstInfo, std::string &strSend);
  bool BuildSearchDeviceInfForTagID(std::string &strSID, EErrCode eEC, const std::shared_ptr<DeviceInfoAndID> &lstInfo, std::string &strSend);
  bool BuildSearchRepairInfForTagID(std::string &strSID, EErrCode eEC, const std::shared_ptr<RepairInfoAndID> &lstInfo, std::string &strSend);
  bool BuildSearchRunInfForTagID(std::string &strSID, EErrCode eEC, const std::shared_ptr<DeviceRunInfoAndID> &lstInfo, std::string &strSend);
  
  bool BuildSearchSupplier(std::string &strSID, EErrCode eEC, const list<std::shared_ptr<SupplierInfo>> &lstInfo, std::string &strSend);
  bool BuildOnlyHead(std::string &strSID, EErrCode eEC, ECommand eCmd, std::string &strSend);
  bool BuildAddDeviceRecordInfo(std::string &strSID, EErrCode eEC, ECommand eCmd, const uint32_t &ulRecordingID, std::string &strSend);

  bool BuildSearchMeterType(std::string &strSID, EErrCode eEC, const list<std::shared_ptr<MeterType>> &lstInfo, std::string &strSend);
  bool BuildSearchEnergyCate(std::string &strSID, EErrCode eEC, const list<std::shared_ptr<EnergyCate>> &lstInfo, std::string &strSend);
  bool BuildSearchDevFaultCate(std::string &strSID, EErrCode eEC, const list<std::shared_ptr<DevFaultCate>> &lstInfo, std::string &strSend);
  bool BuildSearchDeviceInfo(std::string &strSID, EErrCode eEC, const list<std::shared_ptr<DeviceInfo>> &lstInfo, std::string &strSend);
  bool BuildSearchMeterInfo(std::string &strSID, EErrCode eEC, const list<std::shared_ptr<MeterInfo>> &lstInfo, std::string &strSend);
  bool BuildSearchDeviceParameter(std::string &strSID, EErrCode eEC, const std::shared_ptr<DeviceParameter> &lstInfo, std::string &strSend);
  bool BuildSearchMeterParameter(std::string &strSID, EErrCode eEC, const std::shared_ptr<MeterParameter> &lstInfo, std::string &strSend); 
  bool BuildSearchDeviceRepair(std::string &strSID, EErrCode eEC, ECommand eCmd, const list<shared_ptr<RepairInfo>> &lstInfo, std::string &strSend);
  bool BuildSearchDeviceRun(std::string &strSID, EErrCode eEC, ECommand eCmd, const list<shared_ptr<DeviceRunInfo>> &lstInfo, std::string &strSend);

  bool BuildSearchSubSysVariableInfo(std::string &strSID, EErrCode eEC, const shared_ptr<SubSysInfo> &lstInfo, std::string &strSend);
  bool BuildSearchDeviceVariableInfo(std::string &strSID, EErrCode eEC, const shared_ptr<DeviceVarInfo> &lstInfo, std::string &strSend);
  bool BuildSearchTagVariableInfo(std::string &strSID, EErrCode eEC, const shared_ptr<TagVarInfo> &lstInfo, std::string &strSend, ECommand command);

  //����
  bool BuildSearchOverViewInfo(std::string &strSID, EErrCode eEC, const shared_ptr<OverViewInfo> &lstInfo, std::string &strSend);
  bool BuildSearchSystemAlarmCountInfo(std::string &strSID, EErrCode eEC, const shared_ptr<Subsystem_Alarm> &lstInfo, std::string &strSend);
  
  bool BuildSearchMeterValueInfo(std::string &strSID, EErrCode eEC, const shared_ptr<ResponseMeterValueInfo> &lstInfo, std::string &strSend);
  bool BuildSearchDeviceValueInfo(std::string &strSID, EErrCode eEC, const shared_ptr<DeviceValueInfo> &lstInfo, std::string &strSend);
  bool BuildSearchVariableValueInfo(std::string &strSID, EErrCode eEC, const shared_ptr<DeviceValueInfo> &lstInfo, std::string &strSend);

  bool BuildSearchHistoryEventExInfo(std::string &strSID, EErrCode eEC, const shared_ptr<HistoryEventInfo> &lstInfo, std::string &strSend);
  bool BuildSearchHistoryAlarmExInfo(std::string &strSID, EErrCode eEC, const shared_ptr<HistoryAlarmInfo> &lstInfo, std::string &strSend);
  // ����ͷ��ͼ�齨
  bool  BuildCameraScreenshot(const std::string &strSID, EErrCode eEC, std::string &strSend);
  bool  BuildSearchCameraConfig(const std::string &strSID, EErrCode eEC,
    const std::list<std::shared_ptr<KCamConf>> &lstCamConf, std::string &strSend);
  // ������Ϣ
  bool  BuildReadLinkageWatchInfo(const std::string &strSID, EErrCode eEC,
    const std::list<shared_ptr<KLinkageWatchInfo>> &lstInfo, std::string &strSend);
  bool  BuildLinkageManualConfirmation(const std::string &strSID, EErrCode eEC, std::string &strSend);
  bool  BuildLinkageTerminateTask(const std::string &strSID, EErrCode eEC, std::string &strSend);
  // ʱ��������Ϣ
  bool  BuildReadTimetableDispatchInfo(const std::string &strSID, EErrCode eEC,
    const std::list<shared_ptr<KTimetableDispatchWatchInfo>> &lstInfo, std::string &strSend);
  bool  BuildTimetableDispatchTerminateTask(const std::string &strSID, EErrCode eEC, std::string &strSend);
};

#endif
