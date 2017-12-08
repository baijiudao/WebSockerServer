#include "stdafx.h"
#include "ChgShiftsPackage.h"

//------------------------------------------------------------------------------
CChgShiftsPackage::CChgShiftsPackage()
{
}

//------------------------------------------------------------------------------
CChgShiftsPackage::~CChgShiftsPackage()
{
}
//CMD_ADD_CHANGE_SHIFTS
//------------------------------------------------------------------------------
bool CChgShiftsPackage::ParseAdd(const std::string &strRecv, std::string &strSID, std::list<std::shared_ptr<KTBChgShifts>> &lstInfo)
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
  shared_ptr<KTBChgShifts> ptInfo = nullptr;
  for (Json::ArrayIndex i = 0; i < jIndex; i++)
  {
    // isValidIndex �ж������Ƿ���Ч
    if (!jArrData.isValidIndex(i)) return false;
    // ʵ��������
    ptInfo = make_shared<KTBChgShifts>();

    // �ж�������Ч�Բ���ȡ����
    // ���Ӱ���ϢID
    ptInfo->lID = make_pair(false, false);

    // �û���
    if (!jArrData[i][JSON_KEY_USER_NAME].isString()) return false;
    ptInfo->strUserName = make_pair(jArrData[i][JSON_KEY_USER_NAME].asString(), true);

    // �Ӱ�ʱ��
    if (!OnParseDateTime(jArrData[i][JSON_KEY_SUCCESSION_TIME], ptInfo->ullSuccessionTime.first)) return false;

    // ����ʱ��
    if (!OnParseDateTime(jArrData[i][JSON_KEY_SHIFT_TIME], ptInfo->ullShiftTime.first)) return false;

    // �澯�������
    if (!jArrData[i][JSON_KEY_ALARM_PROCESSING].isString()) return false;
    ptInfo->strAlarmProcessing = make_pair(jArrData[i][JSON_KEY_ALARM_PROCESSING].asString(), true);

    // ͨ�ż��
    if (!jArrData[i][JSON_KEY_COMMUNICATION_MONITORING].isString()) return false;
    ptInfo->strCommunicationMonitoring = make_pair(jArrData[i][JSON_KEY_COMMUNICATION_MONITORING].asString(), true);

    // ��־�ı�
    if (!jArrData[i][JSON_KEY_LOG_TEXT].isString()) return false;
    ptInfo->strLogText = make_pair(jArrData[i][JSON_KEY_LOG_TEXT].asString(), true);

    // ��ӵ�������
    lstInfo.push_back(ptInfo);
  }

  return true;
}

//------------------------------------------------------------------------------
bool CChgShiftsPackage::BuildAdd(const std::string &strSID, EErrCode eEC, const std::list<uint32_t> &lstID, std::string &strSend)
{
  if (strSID.empty()) return false;

  Json::FastWriter  jWriter;
  Json::Value       jRoot;
  Json::Value       jArrData;
  Json::Value       jValue;

  // ������ͷ
  if (!OnBuildHeader(strSID, ECommand::CMD_ADD_CHANGE_SHIFTS, eEC, jRoot)) return false;

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
