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

  // 解析数据
  if (!jReader.parse(strRecv, jRoot)) return false;
  // 解析包头
  if (!OnParseHeader(jRoot, strSID)) return false;

  // 获取Data组
  jArrData = jRoot[JSON_KEY_DATA];
  if (!jArrData.isArray()) return false;
  jIndex = jArrData.size();

  // 获取每个的内容
  shared_ptr<KTBChgShifts> ptInfo = nullptr;
  for (Json::ArrayIndex i = 0; i < jIndex; i++)
  {
    // isValidIndex 判断索引是否有效
    if (!jArrData.isValidIndex(i)) return false;
    // 实例化对象
    ptInfo = make_shared<KTBChgShifts>();

    // 判断数据有效性并获取数据
    // 交接班信息ID
    ptInfo->lID = make_pair(false, false);

    // 用户名
    if (!jArrData[i][JSON_KEY_USER_NAME].isString()) return false;
    ptInfo->strUserName = make_pair(jArrData[i][JSON_KEY_USER_NAME].asString(), true);

    // 接班时间
    if (!OnParseDateTime(jArrData[i][JSON_KEY_SUCCESSION_TIME], ptInfo->ullSuccessionTime.first)) return false;

    // 交班时间
    if (!OnParseDateTime(jArrData[i][JSON_KEY_SHIFT_TIME], ptInfo->ullShiftTime.first)) return false;

    // 告警处理情况
    if (!jArrData[i][JSON_KEY_ALARM_PROCESSING].isString()) return false;
    ptInfo->strAlarmProcessing = make_pair(jArrData[i][JSON_KEY_ALARM_PROCESSING].asString(), true);

    // 通信监测
    if (!jArrData[i][JSON_KEY_COMMUNICATION_MONITORING].isString()) return false;
    ptInfo->strCommunicationMonitoring = make_pair(jArrData[i][JSON_KEY_COMMUNICATION_MONITORING].asString(), true);

    // 日志文本
    if (!jArrData[i][JSON_KEY_LOG_TEXT].isString()) return false;
    ptInfo->strLogText = make_pair(jArrData[i][JSON_KEY_LOG_TEXT].asString(), true);

    // 添加到链表中
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

  // 构建包头
  if (!OnBuildHeader(strSID, ECommand::CMD_ADD_CHANGE_SHIFTS, eEC, jRoot)) return false;

  // 构建ID组
  for (auto &item : lstID)
  {
    jValue.clear();
    jValue[JSON_KEY_ID] = item;
    jArrData.append(jValue);
  }

  // 添加data组
  jRoot[JSON_KEY_DATA] = jArrData;

  // 转换为字符串
  strSend = jWriter.write(jRoot);
  return true;
}
