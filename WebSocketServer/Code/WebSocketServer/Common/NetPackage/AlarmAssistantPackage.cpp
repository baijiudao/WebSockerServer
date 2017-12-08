//------------------------------------------------------------------------------
// 创建日期：2017-09-06 13:09:39
// 文件描述：告警辅助相关包的编解码
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

  // 解析数据
  if (!jReader.parse(strRecv, jRoot)) return false;
  // 解析包头
  if (!OnParseHeader(jRoot, strSID)) return false;

  // 获取Data组
  jArrData = jRoot[JSON_KEY_DATA];
  if (!jArrData.isArray()) return false;
  jIndex = jArrData.size();

  // 获取每个的内容
  shared_ptr<KTBAlarmAssistant> ptInfo = nullptr;
  for (Json::ArrayIndex i = 0; i < jIndex; i++)
  {
    // isValidIndex 判断索引是否有效
    if (!jArrData.isValidIndex(i)) return false;
    // 实例化对象
    ptInfo = make_shared<KTBAlarmAssistant>();

    // 判断数据有效性并获取数据
    // 告警辅助信息ID
    ptInfo->lID = make_pair(false, false);

    // 全局ID
    if (!OnParseGlobalID(jArrData[i][JSON_KEY_GLOBAL_ID], ulGID)) return false;
    ptInfo->lGlobalID = make_pair(ulGID, true);

    // 设备名称
    if (!jArrData[i][JSON_KEY_DEVICE_NAME].isString()) return false;
    ptInfo->strDeviceName = make_pair(jArrData[i][JSON_KEY_DEVICE_NAME].asString(), true);

    // 关联摄像头IP（IPV4）
    if (!jArrData[i][JSON_KEY_CAMERA_IP].isUInt()) return false;
    ptInfo->lCameraIP = make_pair(jArrData[i][JSON_KEY_CAMERA_IP].asUInt(), true);

    // 摄像头访问用户名
    if (!jArrData[i][JSON_KEY_CAMERA_USER_NAME].isString()) return false;
    ptInfo->strCameraUserName = make_pair(jArrData[i][JSON_KEY_CAMERA_USER_NAME].asString(), true);

    // 摄像头访问密码
    if (!jArrData[i][JSON_KEY_CAMERA_PASSWORD].isString()) return false;
    ptInfo->strCameraPassword = make_pair(jArrData[i][JSON_KEY_CAMERA_PASSWORD].asString(), true);

    // 应急预案文件名
    if (!jArrData[i][JSON_KEY_EMERGENCY_PLAN_FILE_NAME].isString()) return false;
    ptInfo->strEmergencyPlanFileName = make_pair(jArrData[i][JSON_KEY_EMERGENCY_PLAN_FILE_NAME].asString(), true);

    // 应急预案存放地址
    if (!jArrData[i][JSON_KEY_EMERGENCY_PLAN_STORAGE_ADDRESS].isString()) return false;
    ptInfo->strEmergencyPlanStorageAddress = make_pair(jArrData[i][JSON_KEY_EMERGENCY_PLAN_STORAGE_ADDRESS].asString(), true);

    // 联动方案名称
    if (!jArrData[i][JSON_KEY_LINKAGE_SCHEME_NAME].isString()) return false;
    ptInfo->strLinkageSchemeName = make_pair(jArrData[i][JSON_KEY_LINKAGE_SCHEME_NAME].asString(), true);

    // 联动方案说明
    if (!jArrData[i][JSON_KEY_LINKAGE_SCHEME_DESCRIPTION].isString()) return false;
    ptInfo->strLinkageSchemeDescription = make_pair(jArrData[i][JSON_KEY_LINKAGE_SCHEME_DESCRIPTION].asString(), true);

    // 添加到链表中
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

  // 解析数据
  if (!jReader.parse(strRecv, jRoot)) return false;
  // 解析包头
  if (!OnParseHeader(jRoot, strSID)) return false;

  // 获取Data组
  jArrData = jRoot[JSON_KEY_DATA];
  if (!jArrData.isArray()) return false;
  jIndex = jArrData.size();

  // 获取每个的内容
  for (Json::ArrayIndex i = 0; i < jIndex; i++)
  {
    // isValidIndex 判断索引是否有效
    if (!jArrData.isValidIndex(i)) return false;

    // 判断数据有效性
    if (!jArrData[i][JSON_KEY_ID].isUInt()) return false;

    // 获取数据
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

  // 解析数据
  if (!jReader.parse(strRecv, jRoot)) return false;
  // 解析包头
  if (!OnParseHeader(jRoot, strSID)) return false;

  // 获取Data组
  jArrData = jRoot[JSON_KEY_DATA];
  if (!jArrData.isArray()) return false;
  jIndex = jArrData.size();

  // 获取每个的内容
  shared_ptr<KTBAlarmAssistant> ptInfo = nullptr;
  for (Json::ArrayIndex i = 0; i < jIndex; i++)
  {
    // isValidIndex 判断索引是否有效
    if (!jArrData.isValidIndex(i)) return false;
    // 实例化对象
    ptInfo = make_shared<KTBAlarmAssistant>();

    // 判断数据有效性并获取数据
    // 告警辅助信息ID
    if (!jArrData[i][JSON_KEY_ID].isUInt()) return false;
    ptInfo->lID = make_pair(jArrData[i][JSON_KEY_ID].asUInt(), true);

    // 全局ID
    // null

    // 设备名称
    if (!jArrData[i][JSON_KEY_DEVICE_NAME].isString()) return false;
    ptInfo->strDeviceName = make_pair(jArrData[i][JSON_KEY_DEVICE_NAME].asString(), true);

    // 关联摄像头IP（IPV4）
    if (!jArrData[i][JSON_KEY_CAMERA_IP].isUInt()) return false;
    ptInfo->lCameraIP = make_pair(jArrData[i][JSON_KEY_CAMERA_IP].asUInt(), true);

    // 摄像头访问用户名
    if (!jArrData[i][JSON_KEY_CAMERA_USER_NAME].isString()) return false;
    ptInfo->strCameraUserName = make_pair(jArrData[i][JSON_KEY_CAMERA_USER_NAME].asString(), true);

    // 摄像头访问密码
    if (!jArrData[i][JSON_KEY_CAMERA_PASSWORD].isString()) return false;
    ptInfo->strCameraPassword = make_pair(jArrData[i][JSON_KEY_CAMERA_PASSWORD].asString(), true);

    // 应急预案文件名
    if (!jArrData[i][JSON_KEY_EMERGENCY_PLAN_FILE_NAME].isString()) return false;
    ptInfo->strEmergencyPlanFileName = make_pair(jArrData[i][JSON_KEY_EMERGENCY_PLAN_FILE_NAME].asString(), true);

    // 应急预案存放地址
    if (!jArrData[i][JSON_KEY_EMERGENCY_PLAN_STORAGE_ADDRESS].isString()) return false;
    ptInfo->strEmergencyPlanStorageAddress = make_pair(jArrData[i][JSON_KEY_EMERGENCY_PLAN_STORAGE_ADDRESS].asString(), true);

    // 联动方案名称
    if (!jArrData[i][JSON_KEY_LINKAGE_SCHEME_NAME].isString()) return false;
    ptInfo->strLinkageSchemeName = make_pair(jArrData[i][JSON_KEY_LINKAGE_SCHEME_NAME].asString(), true);

    // 联动方案说明
    if (!jArrData[i][JSON_KEY_LINKAGE_SCHEME_DESCRIPTION].isString()) return false;
    ptInfo->strLinkageSchemeDescription = make_pair(jArrData[i][JSON_KEY_LINKAGE_SCHEME_DESCRIPTION].asString(), true);

    // 添加到链表中
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

  // 解析数据
  if (!jReader.parse(strRecv, jRoot)) return false;
  // 解析包头
  if (!OnParseHeader(jRoot, strSID)) return false;

  // 判断数据有效性并获取数据
  // 全局ID
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
  Json::Value   jOptional; // 用来保存可选值
  uint32_t      ulGID = 0;

  // 解析数据
  if (!jReader.parse(strRecv, jRoot)) return false;
  // 解析包头
  if (!OnParseHeader(jRoot, strSID)) return false;

  // 判断数据有效性并获取数据
  // 请求要获取的数量
  if (!jRoot[JSON_KEY_COUNT].isUInt()) return false;
  ptCondition->lCount = jRoot[JSON_KEY_COUNT].asUInt();

  // 请求数据起始索引
  if (!jRoot[JSON_KEY_START_NUM].isUInt()) return false;
  ptCondition->lStartNum = jRoot[JSON_KEY_START_NUM].asUInt();

  // 全局ID【可选】
  if (OnParseGlobalID(jRoot[JSON_KEY_GLOBAL_ID], ulGID))
    ptCondition->tParameter.lGlobalID = make_pair(ulGID, true);

  // 设备名【可选】
  jOptional = jRoot[JSON_KEY_DEVICE_NAME];
  if (jOptional.isString()) ptCondition->tParameter.strDeviceName = make_pair(jOptional.asString(), true);

  // 联动方案名称【可选】
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
  Json::Value   jOptional; // 用来保存可选值
  uint32_t      ulGID = 0;

  // 解析数据
  if (!jReader.parse(strRecv, jRoot)) return false;
  // 解析包头
  if (!OnParseHeader(jRoot, strSID)) return false;

  // 判断数据有效性并获取数据
  // 页大小（请求数量）
  if (!jRoot[JSON_KEY_PAGE_SIZE].isUInt()) return false;
  ptCondition->lPageSize = jRoot[JSON_KEY_PAGE_SIZE].asUInt();

  // 请求第几页
  if (!jRoot[JSON_KEY_PAGE_NUM].isUInt()) return false;
  ptCondition->lPageNum = jRoot[JSON_KEY_PAGE_NUM].asUInt();

  // 全局ID【可选】
  if (OnParseGlobalID(jRoot[JSON_KEY_GLOBAL_ID], ulGID))
    ptCondition->tParameter.lGlobalID = make_pair(ulGID, true);

  // 设备名【可选】
  jOptional = jRoot[JSON_KEY_DEVICE_NAME];
  if (jOptional.isString()) ptCondition->tParameter.strDeviceName = make_pair(jOptional.asString(), true);

  // 联动方案名称【可选】
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

  // 构建包头
  if (!OnBuildHeader(strSID, ECommand::CMD_ADD_ALARM_ASSISTANT, eEC, jRoot)) return false;

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

//------------------------------------------------------------------------------
bool CAlarmAssistantPackage::BuildDelete(const std::string &strSID, EErrCode eEC,
  const std::list<uint32_t> &lstID, std::string &strSend)
{
  if (strSID.empty()) return false;

  Json::FastWriter  jWriter;
  Json::Value       jRoot;
  Json::Value       jArrData;
  Json::Value       jValue;

  // 构建包头
  if (!OnBuildHeader(strSID, ECommand::CMD_DEL_ALARM_ASSISTANT, eEC, jRoot)) return false;

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

//------------------------------------------------------------------------------
bool CAlarmAssistantPackage::BuildModify(const std::string &strSID, EErrCode eEC,
  const std::list<uint32_t> &lstID, std::string &strSend)
{
  if (strSID.empty()) return false;

  Json::FastWriter  jWriter;
  Json::Value       jRoot;
  Json::Value       jArrData;
  Json::Value       jValue;

  // 构建包头
  if (!OnBuildHeader(strSID, ECommand::CMD_MODIFY_ALARM_ASSISTANT, eEC, jRoot)) return false;

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

  // 构建包头
  if (!OnBuildHeader(strSID, ECommand::CMD_SEARCH_ALARM_ASSISTANT, eEC, jRoot)) return false;

  // 添加查询到的数据
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

  // 添加data组
  jRoot[JSON_KEY_DATA] = jArrData;

  // 转换为字符串
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

  // 构建包头
  if (!OnBuildHeader(strSID, ECommand::CMD_SEARCH_NUM_ALARM_ASSISTANT, eEC, jRoot)) return false;

  // 添加当前起始位置和总数
  jRoot[JSON_KEY_START_NUM] = ptResult->lStartNum;
  jRoot[JSON_KEY_TOTAL] = ptResult->lTotal;

  // 添加查询到的数据
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

  // 添加data组
  jRoot[JSON_KEY_DATA] = jArrData;

  // 转换为字符串
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

  // 构建包头
  if (!OnBuildHeader(strSID, ECommand::CMD_SEARCH_PAGING_ALARM_ASSISTANT, eEC, jRoot)) return false;

  // 添加当前页和总页数
  jRoot[JSON_KEY_PAGE_NUM] = ptResult->lPageNum;
  jRoot[JSON_KEY_TOTAL_PAGES] = ptResult->lTotalPages;

  // 添加查询到的数据
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

  // 添加data组
  jRoot[JSON_KEY_DATA] = jArrData;

  // 转换为字符串
  strSend = jWriter.write(jRoot);
  return true;
}
