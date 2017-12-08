//------------------------------------------------------------------------------
// �������ڣ�2017-09-06 13:09:39
// �ļ��������澯������ذ��ı����
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "AlarmAssistantPackage.h"

using namespace std;

//------------------------------------------------------------------------------
CAlarmAssistantPackage::CAlarmAssistantPackage()
{
}

//------------------------------------------------------------------------------
CAlarmAssistantPackage::~CAlarmAssistantPackage()
{
}

//------------------------------------------------------------------------------
bool CAlarmAssistantPackage::ParseAdd(const std::string &strRecv,
  std::string &strSID, std::list<shared_ptr<KTBAlarmAssistant>> &lstInfo)
{
  if (strRecv.empty()) return false;

  Json::ArrayIndex  jIndex = 0;
  Json::Reader  jReader;
  Json::Value   jRoot;
  Json::Value   jArrData;
  uint32_t      ulGID = 0;

  // ��������
  if (!jReader.parse(strRecv, jRoot)) return false;
  // ������ͷ
  if (!OnParseHeader(jRoot, strSID)) return false;

  // ��ȡData��
  jArrData = jRoot[JSON_KEY_DATA];
  if (!jArrData.isArray()) return false;
  jIndex = jArrData.size();

  // ��ȡÿ��������
  shared_ptr<KTBAlarmAssistant> ptInfo = nullptr;
  for (Json::ArrayIndex i = 0; i < jIndex; i++)
  {
    // isValidIndex �ж������Ƿ���Ч
    if (!jArrData.isValidIndex(i)) return false;
    // ʵ��������
    ptInfo = make_shared<KTBAlarmAssistant>();

    // �ж�������Ч�Բ���ȡ����
    // �澯������ϢID
    ptInfo->lID = make_pair(false, false);

    // ȫ��ID
    if (!OnParseGlobalID(jArrData[i][JSON_KEY_GLOBAL_ID], ulGID)) return false;
    ptInfo->lGlobalID = make_pair(ulGID, true);

    // �豸����
    if (!jArrData[i][JSON_KEY_DEVICE_NAME].isString()) return false;
    ptInfo->strDeviceName = make_pair(jArrData[i][JSON_KEY_DEVICE_NAME].asString(), true);

    // ��������ͷIP��IPV4��
    if (!jArrData[i][JSON_KEY_CAMERA_IP].isUInt()) return false;
    ptInfo->lCameraIP = make_pair(jArrData[i][JSON_KEY_CAMERA_IP].asUInt(), true);

    // ����ͷ�����û���
    if (!jArrData[i][JSON_KEY_CAMERA_USER_NAME].isString()) return false;
    ptInfo->strCameraUserName = make_pair(jArrData[i][JSON_KEY_CAMERA_USER_NAME].asString(), true);

    // ����ͷ��������
    if (!jArrData[i][JSON_KEY_CAMERA_PASSWORD].isString()) return false;
    ptInfo->strCameraPassword = make_pair(jArrData[i][JSON_KEY_CAMERA_PASSWORD].asString(), true);

    // Ӧ��Ԥ���ļ���
    if (!jArrData[i][JSON_KEY_EMERGENCY_PLAN_FILE_NAME].isString()) return false;
    ptInfo->strEmergencyPlanFileName = make_pair(jArrData[i][JSON_KEY_EMERGENCY_PLAN_FILE_NAME].asString(), true);

    // Ӧ��Ԥ����ŵ�ַ
    if (!jArrData[i][JSON_KEY_EMERGENCY_PLAN_STORAGE_ADDRESS].isString()) return false;
    ptInfo->strEmergencyPlanStorageAddress = make_pair(jArrData[i][JSON_KEY_EMERGENCY_PLAN_STORAGE_ADDRESS].asString(), true);

    // ������������
    if (!jArrData[i][JSON_KEY_LINKAGE_SCHEME_NAME].isString()) return false;
    ptInfo->strLinkageSchemeName = make_pair(jArrData[i][JSON_KEY_LINKAGE_SCHEME_NAME].asString(), true);

    // ��������˵��
    if (!jArrData[i][JSON_KEY_LINKAGE_SCHEME_DESCRIPTION].isString()) return false;
    ptInfo->strLinkageSchemeDescription = make_pair(jArrData[i][JSON_KEY_LINKAGE_SCHEME_DESCRIPTION].asString(), true);

    // ��ӵ�������
    lstInfo.push_back(ptInfo);
  }

  return true;
}

//------------------------------------------------------------------------------
bool CAlarmAssistantPackage::ParseDelete(const std::string &strRecv,
  std::string &strSID, std::list<uint32_t> &lstID)
{
  if (strRecv.empty()) return false;

  Json::ArrayIndex  jIndex = 0;
  Json::Reader  jReader;
  Json::Value   jRoot;
  Json::Value   jArrData;

  // ��������
  if (!jReader.parse(strRecv, jRoot)) return false;
  // ������ͷ
  if (!OnParseHeader(jRoot, strSID)) return false;

  // ��ȡData��
  jArrData = jRoot[JSON_KEY_DATA];
  if (!jArrData.isArray()) return false;
  jIndex = jArrData.size();

  // ��ȡÿ��������
  for (Json::ArrayIndex i = 0; i < jIndex; i++)
  {
    // isValidIndex �ж������Ƿ���Ч
    if (!jArrData.isValidIndex(i)) return false;

    // �ж�������Ч��
    if (!jArrData[i][JSON_KEY_ID].isUInt()) return false;

    // ��ȡ����
    lstID.push_back(jArrData[i][JSON_KEY_ID].asUInt());
  }

  return true;
}

//------------------------------------------------------------------------------
bool CAlarmAssistantPackage::ParseModify(const std::string &strRecv,
  std::string &strSID, std::list<shared_ptr<KTBAlarmAssistant>> &lstInfo)
{
  if (strRecv.empty()) return false;

  Json::ArrayIndex  jIndex = 0;
  Json::Reader  jReader;
  Json::Value   jRoot;
  Json::Value   jArrData;

  // ��������
  if (!jReader.parse(strRecv, jRoot)) return false;
  // ������ͷ
  if (!OnParseHeader(jRoot, strSID)) return false;

  // ��ȡData��
  jArrData = jRoot[JSON_KEY_DATA];
  if (!jArrData.isArray()) return false;
  jIndex = jArrData.size();

  // ��ȡÿ��������
  shared_ptr<KTBAlarmAssistant> ptInfo = nullptr;
  for (Json::ArrayIndex i = 0; i < jIndex; i++)
  {
    // isValidIndex �ж������Ƿ���Ч
    if (!jArrData.isValidIndex(i)) return false;
    // ʵ��������
    ptInfo = make_shared<KTBAlarmAssistant>();

    // �ж�������Ч�Բ���ȡ����
    // �澯������ϢID
    if (!jArrData[i][JSON_KEY_ID].isUInt()) return false;
    ptInfo->lID = make_pair(jArrData[i][JSON_KEY_ID].asUInt(), true);

    // ȫ��ID
    // null

    // �豸����
    if (!jArrData[i][JSON_KEY_DEVICE_NAME].isString()) return false;
    ptInfo->strDeviceName = make_pair(jArrData[i][JSON_KEY_DEVICE_NAME].asString(), true);

    // ��������ͷIP��IPV4��
    if (!jArrData[i][JSON_KEY_CAMERA_IP].isUInt()) return false;
    ptInfo->lCameraIP = make_pair(jArrData[i][JSON_KEY_CAMERA_IP].asUInt(), true);

    // ����ͷ�����û���
    if (!jArrData[i][JSON_KEY_CAMERA_USER_NAME].isString()) return false;
    ptInfo->strCameraUserName = make_pair(jArrData[i][JSON_KEY_CAMERA_USER_NAME].asString(), true);

    // ����ͷ��������
    if (!jArrData[i][JSON_KEY_CAMERA_PASSWORD].isString()) return false;
    ptInfo->strCameraPassword = make_pair(jArrData[i][JSON_KEY_CAMERA_PASSWORD].asString(), true);

    // Ӧ��Ԥ���ļ���
    if (!jArrData[i][JSON_KEY_EMERGENCY_PLAN_FILE_NAME].isString()) return false;
    ptInfo->strEmergencyPlanFileName = make_pair(jArrData[i][JSON_KEY_EMERGENCY_PLAN_FILE_NAME].asString(), true);

    // Ӧ��Ԥ����ŵ�ַ
    if (!jArrData[i][JSON_KEY_EMERGENCY_PLAN_STORAGE_ADDRESS].isString()) return false;
    ptInfo->strEmergencyPlanStorageAddress = make_pair(jArrData[i][JSON_KEY_EMERGENCY_PLAN_STORAGE_ADDRESS].asString(), true);

    // ������������
    if (!jArrData[i][JSON_KEY_LINKAGE_SCHEME_NAME].isString()) return false;
    ptInfo->strLinkageSchemeName = make_pair(jArrData[i][JSON_KEY_LINKAGE_SCHEME_NAME].asString(), true);

    // ��������˵��
    if (!jArrData[i][JSON_KEY_LINKAGE_SCHEME_DESCRIPTION].isString()) return false;
    ptInfo->strLinkageSchemeDescription = make_pair(jArrData[i][JSON_KEY_LINKAGE_SCHEME_DESCRIPTION].asString(), true);

    // ��ӵ�������
    lstInfo.push_back(ptInfo);
  }

  return true;
}

//------------------------------------------------------------------------------
bool CAlarmAssistantPackage::ParseSearch(const std::string &strRecv,
  std::string &strSID, uint32_t &ulGlobalID)
{
  if (strRecv.empty()) return false;

  Json::Reader  jReader;
  Json::Value   jRoot;

  // ��������
  if (!jReader.parse(strRecv, jRoot)) return false;
  // ������ͷ
  if (!OnParseHeader(jRoot, strSID)) return false;

  // �ж�������Ч�Բ���ȡ����
  // ȫ��ID
  if (!OnParseGlobalID(jRoot[JSON_KEY_GLOBAL_ID], ulGlobalID)) return false;

  return true;
}

//------------------------------------------------------------------------------
bool CAlarmAssistantPackage::ParseSearch_Num(const std::string &strRecv,
  std::string &strSID, std::shared_ptr<KTBAlarmAssistantSearchCondition_Num> ptCondition)
{
  if (strRecv.empty() || !ptCondition) return false;

  Json::ArrayIndex  jIndex = 0;
  Json::Reader  jReader;
  Json::Value   jRoot;
  Json::Value   jOptional; // ���������ѡֵ
  uint32_t      ulGID = 0;

  // ��������
  if (!jReader.parse(strRecv, jRoot)) return false;
  // ������ͷ
  if (!OnParseHeader(jRoot, strSID)) return false;

  // �ж�������Ч�Բ���ȡ����
  // ����Ҫ��ȡ������
  if (!jRoot[JSON_KEY_COUNT].isUInt()) return false;
  ptCondition->lCount = jRoot[JSON_KEY_COUNT].asUInt();

  // ����������ʼ����
  if (!jRoot[JSON_KEY_START_NUM].isUInt()) return false;
  ptCondition->lStartNum = jRoot[JSON_KEY_START_NUM].asUInt();

  // ȫ��ID����ѡ��
  if (OnParseGlobalID(jRoot[JSON_KEY_GLOBAL_ID], ulGID))
    ptCondition->tParameter.lGlobalID = make_pair(ulGID, true);

  // �豸������ѡ��
  jOptional = jRoot[JSON_KEY_DEVICE_NAME];
  if (jOptional.isString()) ptCondition->tParameter.strDeviceName = make_pair(jOptional.asString(), true);

  // �����������ơ���ѡ��
  jOptional = jRoot[JSON_KEY_LINKAGE_SCHEME_NAME];
  if (jOptional.isString()) ptCondition->tParameter.strLinkageSchemeName = make_pair(jOptional.asString(), true);

  return true;
}

//------------------------------------------------------------------------------
bool CAlarmAssistantPackage::ParseSearchPaging(const std::string &strRecv,
  std::string &strSID, std::shared_ptr<KTBAlarmAssistantSearchCondition> ptCondition)
{
  if (strRecv.empty() || !ptCondition) return false;

  Json::ArrayIndex  jIndex = 0;
  Json::Reader  jReader;
  Json::Value   jRoot;
  Json::Value   jOptional; // ���������ѡֵ
  uint32_t      ulGID = 0;

  // ��������
  if (!jReader.parse(strRecv, jRoot)) return false;
  // ������ͷ
  if (!OnParseHeader(jRoot, strSID)) return false;

  // �ж�������Ч�Բ���ȡ����
  // ҳ��С������������
  if (!jRoot[JSON_KEY_PAGE_SIZE].isUInt()) return false;
  ptCondition->lPageSize = jRoot[JSON_KEY_PAGE_SIZE].asUInt();

  // ����ڼ�ҳ
  if (!jRoot[JSON_KEY_PAGE_NUM].isUInt()) return false;
  ptCondition->lPageNum = jRoot[JSON_KEY_PAGE_NUM].asUInt();

  // ȫ��ID����ѡ��
  if (OnParseGlobalID(jRoot[JSON_KEY_GLOBAL_ID], ulGID))
    ptCondition->tParameter.lGlobalID = make_pair(ulGID, true);

  // �豸������ѡ��
  jOptional = jRoot[JSON_KEY_DEVICE_NAME];
  if (jOptional.isString()) ptCondition->tParameter.strDeviceName = make_pair(jOptional.asString(), true);

  // �����������ơ���ѡ��
  jOptional = jRoot[JSON_KEY_LINKAGE_SCHEME_NAME];
  if (jOptional.isString()) ptCondition->tParameter.strLinkageSchemeName = make_pair(jOptional.asString(), true);

  return true;
}

//------------------------------------------------------------------------------
bool CAlarmAssistantPackage::BuildAdd(const std::string &strSID, EErrCode eEC,
  const std::list<uint32_t> &lstID, std::string &strSend)
{
  if (strSID.empty()) return false;

  Json::FastWriter  jWriter;
  Json::Value       jRoot;
  Json::Value       jArrData;
  Json::Value       jValue;

  // ������ͷ
  if (!OnBuildHeader(strSID, ECommand::CMD_ADD_ALARM_ASSISTANT, eEC, jRoot)) return false;

  // ����ID��
  for (auto &item : lstID)
  {
    jValue.clear();
    jValue[JSON_KEY_ID] = item;
    jArrData.append(jValue);
  }

  // ���data��
  jRoot[JSON_KEY_DATA] = jArrData;

  // ת��Ϊ�ַ���
  strSend = jWriter.write(jRoot);
  return true;
}

//------------------------------------------------------------------------------
bool CAlarmAssistantPackage::BuildDelete(const std::string &strSID, EErrCode eEC,
  const std::list<uint32_t> &lstID, std::string &strSend)
{
  if (strSID.empty()) return false;

  Json::FastWriter  jWriter;
  Json::Value       jRoot;
  Json::Value       jArrData;
  Json::Value       jValue;

  // ������ͷ
  if (!OnBuildHeader(strSID, ECommand::CMD_DEL_ALARM_ASSISTANT, eEC, jRoot)) return false;

  // ����ID��
  for (auto &item : lstID)
  {
    jValue.clear();
    jValue[JSON_KEY_ID] = item;
    jArrData.append(jValue);
  }

  // ���data��
  jRoot[JSON_KEY_DATA] = jArrData;

  // ת��Ϊ�ַ���
  strSend = jWriter.write(jRoot);
  return true;
}

//------------------------------------------------------------------------------
bool CAlarmAssistantPackage::BuildModify(const std::string &strSID, EErrCode eEC,
  const std::list<uint32_t> &lstID, std::string &strSend)
{
  if (strSID.empty()) return false;

  Json::FastWriter  jWriter;
  Json::Value       jRoot;
  Json::Value       jArrData;
  Json::Value       jValue;

  // ������ͷ
  if (!OnBuildHeader(strSID, ECommand::CMD_MODIFY_ALARM_ASSISTANT, eEC, jRoot)) return false;

  // ����ID��
  for (auto &item : lstID)
  {
    jValue.clear();
    jValue[JSON_KEY_ID] = item;
    jArrData.append(jValue);
  }

  // ���data��
  jRoot[JSON_KEY_DATA] = jArrData;

  // ת��Ϊ�ַ���
  strSend = jWriter.write(jRoot);
  return true;
}

//------------------------------------------------------------------------------
bool CAlarmAssistantPackage::BuildSearch(const std::string &strSID,
  EErrCode eEC, const std::list<std::shared_ptr<KTBAlarmAssistant>> &lstInfo,
  std::string &strSend)
{
  if (strSID.empty()) return false;

  Json::FastWriter  jWriter;
  Json::Value       jRoot;
  Json::Value       jArrData;
  Json::Value       jValue;

  // ������ͷ
  if (!OnBuildHeader(strSID, ECommand::CMD_SEARCH_ALARM_ASSISTANT, eEC, jRoot)) return false;

  // ��Ӳ�ѯ��������
  for (auto &item : lstInfo)
  {
    jValue.clear();
    jValue[JSON_KEY_ID] = item->lID.first;
    OnBuildGlobalID(item->lGlobalID.first, jValue[JSON_KEY_GLOBAL_ID]);
    jValue[JSON_KEY_DEVICE_NAME] = item->strDeviceName.first;
    jValue[JSON_KEY_CAMERA_IP] = item->lCameraIP.first;
    jValue[JSON_KEY_CAMERA_USER_NAME] = item->strCameraUserName.first;
    jValue[JSON_KEY_CAMERA_PASSWORD] = item->strCameraPassword.first;
    jValue[JSON_KEY_EMERGENCY_PLAN_FILE_NAME] = item->strEmergencyPlanFileName.first;
    jValue[JSON_KEY_EMERGENCY_PLAN_STORAGE_ADDRESS] = item->strEmergencyPlanStorageAddress.first;
    jValue[JSON_KEY_LINKAGE_SCHEME_NAME] = item->strLinkageSchemeName.first;
    jValue[JSON_KEY_LINKAGE_SCHEME_DESCRIPTION] = item->strLinkageSchemeDescription.first;

    jArrData.append(jValue);
  }

  // ���data��
  jRoot[JSON_KEY_DATA] = jArrData;

  // ת��Ϊ�ַ���
  strSend = jWriter.write(jRoot);
  return true;
}

//------------------------------------------------------------------------------
bool CAlarmAssistantPackage::BuildSearch_Num(const std::string &strSID,
  EErrCode eEC, const std::shared_ptr<KTBAlarmAssistantSearchResult_Num> ptResult,
  std::string &strSend)
{
  if (strSID.empty() || !ptResult) return false;

  Json::FastWriter  jWriter;
  Json::Value       jRoot;
  Json::Value       jArrData;
  Json::Value       jValue;

  // ������ͷ
  if (!OnBuildHeader(strSID, ECommand::CMD_SEARCH_NUM_ALARM_ASSISTANT, eEC, jRoot)) return false;

  // ��ӵ�ǰ��ʼλ�ú�����
  jRoot[JSON_KEY_START_NUM] = ptResult->lStartNum;
  jRoot[JSON_KEY_TOTAL] = ptResult->lTotal;

  // ��Ӳ�ѯ��������
  for (auto &item : ptResult->lstData)
  {
    jValue.clear();
    jValue[JSON_KEY_ID] = item->lID.first;
    OnBuildGlobalID(item->lGlobalID.first, jValue[JSON_KEY_GLOBAL_ID]);
    jValue[JSON_KEY_DEVICE_NAME] = item->strDeviceName.first;
    jValue[JSON_KEY_CAMERA_IP] = item->lCameraIP.first;
    jValue[JSON_KEY_CAMERA_USER_NAME] = item->strCameraUserName.first;
    jValue[JSON_KEY_CAMERA_PASSWORD] = item->strCameraPassword.first;
    jValue[JSON_KEY_EMERGENCY_PLAN_FILE_NAME] = item->strEmergencyPlanFileName.first;
    jValue[JSON_KEY_EMERGENCY_PLAN_STORAGE_ADDRESS] = item->strEmergencyPlanStorageAddress.first;
    jValue[JSON_KEY_LINKAGE_SCHEME_NAME] = item->strLinkageSchemeName.first;
    jValue[JSON_KEY_LINKAGE_SCHEME_DESCRIPTION] = item->strLinkageSchemeDescription.first;

    jArrData.append(jValue);
  }

  // ���data��
  jRoot[JSON_KEY_DATA] = jArrData;

  // ת��Ϊ�ַ���
  strSend = jWriter.write(jRoot);
  return true;
}

//------------------------------------------------------------------------------
bool CAlarmAssistantPackage::BuildSearchPaging(const std::string &strSID,
  EErrCode eEC, const std::shared_ptr<KTBAlarmAssistantSearchResult> ptResult,
  std::string &strSend)
{
  if (strSID.empty() || !ptResult) return false;

  Json::FastWriter  jWriter;
  Json::Value       jRoot;
  Json::Value       jArrData;
  Json::Value       jValue;

  // ������ͷ
  if (!OnBuildHeader(strSID, ECommand::CMD_SEARCH_PAGING_ALARM_ASSISTANT, eEC, jRoot)) return false;

  // ��ӵ�ǰҳ����ҳ��
  jRoot[JSON_KEY_PAGE_NUM] = ptResult->lPageNum;
  jRoot[JSON_KEY_TOTAL_PAGES] = ptResult->lTotalPages;

  // ��Ӳ�ѯ��������
  for (auto &item : ptResult->lstData)
  {
    jValue.clear();
    jValue[JSON_KEY_ID] = item->lID.first;
    OnBuildGlobalID(item->lGlobalID.first, jValue[JSON_KEY_GLOBAL_ID]);
    jValue[JSON_KEY_DEVICE_NAME] = item->strDeviceName.first;
    jValue[JSON_KEY_CAMERA_IP] = item->lCameraIP.first;
    jValue[JSON_KEY_CAMERA_USER_NAME] = item->strCameraUserName.first;
    jValue[JSON_KEY_CAMERA_PASSWORD] = item->strCameraPassword.first;
    jValue[JSON_KEY_EMERGENCY_PLAN_FILE_NAME] = item->strEmergencyPlanFileName.first;
    jValue[JSON_KEY_EMERGENCY_PLAN_STORAGE_ADDRESS] = item->strEmergencyPlanStorageAddress.first;
    jValue[JSON_KEY_LINKAGE_SCHEME_NAME] = item->strLinkageSchemeName.first;
    jValue[JSON_KEY_LINKAGE_SCHEME_DESCRIPTION] = item->strLinkageSchemeDescription.first;

    jArrData.append(jValue);
  }

  // ���data��
  jRoot[JSON_KEY_DATA] = jArrData;

  // ת��Ϊ�ַ���
  strSend = jWriter.write(jRoot);
  return true;
}
