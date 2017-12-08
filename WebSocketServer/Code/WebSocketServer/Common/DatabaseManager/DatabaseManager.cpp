#include "stdafx.h"
#include "DatabaseManager.h"

using namespace std;

//------------------------------------------------------------------------------
CDatabaseManager::CDatabaseManager()
{
  // 创建连接池，默认最小8个连接，最多32个连接，不自动事务提交
  m_pDBConnectPool = make_shared<COTLConnectPool>();
}

//------------------------------------------------------------------------------
// 断开数据连接
//------------------------------------------------------------------------------
CDatabaseManager::~CDatabaseManager()
{
    if (m_pThread)
    {
        m_pThread->join();
    }

  try
  {
    if (m_pDBConnectPool->is_open()) m_pDBConnectPool->close();
  }
  catch (otl_exception &e)
  {
    LOG_ERROR << e.code << "：" << e.msg << " " << e.stm_text << " " << e.var_info;
    std::ignore = e;
  }
}

//------------------------------------------------------------------------------
// 函数功能：连接数据库（如果已经连接，会返回连接失败）
// 参数列表：
//      [IN]：strUID：数据库用户名
//          ：strPWD：数据库密码
//          ：strDSN：数据来源名称
// 返 回 值：true :表示成功；  false :表示失败；
// 其他说明：
//------------------------------------------------------------------------------
bool CDatabaseManager::Connect(const std::string &strUID,
  const std::string &strPWD, const std::string &strDSN)
{
  lock_guard<recursive_mutex> alock(m_mutex);

  if (m_pDBConnectPool->is_open())
  {
    LOG_WARN << "The database is already connected";
    return false;
  }

  // 连接字符串
  string  strConnect = strUID + "/" + strPWD + "@" + strDSN;

  // 初始化连接
  try
  {
    otl_connect::otl_initialize(1);
    m_pDBConnectPool->open(strConnect.c_str());

    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (pDBConnect)
    {
      // 设置超时时间
      pDBConnect->set_timeout(3);

      // 申请空间
      m_pAlarmAssistantTable = make_shared<CAlarmAssistantTable>();
      m_pChangeShiftsTable = make_shared<CChangeShiftsTable>();
      m_pBMSInfoManager = make_shared<BMSInfoManager>();

      // 初始化所有表
      m_pAlarmAssistantTable->Initialise(const_cast<otl_connect &>(*pDBConnect));
      m_pChangeShiftsTable->Initialise(const_cast<otl_connect &>(*pDBConnect));

      // 将连接归还给连接池
      m_pDBConnectPool->put(std::move(pDBConnect));
    }
  }
  catch (otl_exception &e)
  {
    LOG_ERROR << e.code << "：" << e.msg << " " << e.stm_text << " " << e.var_info;
    return false;
  }

  //为报表的生成与老化数据转储启动检测线程
  //std::function<void(void)> thFunc = std::bind(&CDatabaseManager::OnThreadPrc, this);
  //m_pThread = make_shared<std::thread>(thFunc);

  return true;
}

//------------------------------------------------------------------------------
// 用来监测数据库当前状态
//------------------------------------------------------------------------------
bool CDatabaseManager::CheckDBStatus(std::list<std::shared_ptr<KDBStatusInfo>> &lstInfo)
{
  shared_ptr<KDBStatusInfo> ptInfo = make_shared<KDBStatusInfo>();
  ptInfo->eStatus = (m_pDBConnectPool->is_open()) ? EDBStatus::OK : EDBStatus::UNCONNECT;
  ptInfo->strDescribe = "WSServer Database";
  lstInfo.push_back(ptInfo);

  return true;
}

//------------------------------------------------------------------------------
bool CDatabaseManager::AddAlarmAssistant(const std::list<std::shared_ptr<KTBAlarmAssistant>> &lstInfo,
  std::list<uint32_t> &lstID)
{
  // 获取一个连接
  COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
  if (!pDBConnect) return false;

  // 处理连接
  bool bRet = m_pAlarmAssistantTable->Add(const_cast<otl_connect &>(*pDBConnect), lstInfo, lstID);

  // 将连接归还给连接池
  m_pDBConnectPool->put(std::move(pDBConnect));
  return bRet;
}

//------------------------------------------------------------------------------
bool CDatabaseManager::DeleteAlarmAssistant(const std::list<uint32_t> &lstID)
{
  // 获取一个连接
  COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
  if (!pDBConnect) return false;

  // 处理连接
  bool bRet = m_pAlarmAssistantTable->Delete(const_cast<otl_connect &>(*pDBConnect), lstID);

  // 将连接归还给连接池
  m_pDBConnectPool->put(std::move(pDBConnect));
  return bRet;
}

//------------------------------------------------------------------------------
bool CDatabaseManager::ModifyAlarmAssistant(const std::list<std::shared_ptr<KTBAlarmAssistant>> &lstInfo)
{
  // 获取一个连接
  COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
  if (!pDBConnect) return false;

  // 处理连接
  bool bRet = m_pAlarmAssistantTable->Modify(const_cast<otl_connect &>(*pDBConnect), lstInfo);

  // 将连接归还给连接池
  m_pDBConnectPool->put(std::move(pDBConnect));
  return bRet;
}

//------------------------------------------------------------------------------
bool CDatabaseManager::SearchAlarmAssistant(uint32_t ulGlobalID,
  std::list<std::shared_ptr<KTBAlarmAssistant>> &lstInfo)
{
  // 获取一个连接
  COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
  if (!pDBConnect) return false;

  // 处理连接
  bool bRet = m_pAlarmAssistantTable->Search(const_cast<otl_connect &>(*pDBConnect), ulGlobalID, lstInfo);

  // 将连接归还给连接池
  m_pDBConnectPool->put(std::move(pDBConnect));
  return bRet;
}

//------------------------------------------------------------------------------
bool CDatabaseManager::SearchAlarmAssistant_Num(
  const std::shared_ptr<KTBAlarmAssistantSearchCondition_Num> ptCondition,
  std::shared_ptr<KTBAlarmAssistantSearchResult_Num> ptResult)
{
  // 获取一个连接
  COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
  if (!pDBConnect) return false;

  // 处理连接
  bool bRet = m_pAlarmAssistantTable->Search_Num(const_cast<otl_connect &>(*pDBConnect), ptCondition, ptResult);

  // 将连接归还给连接池
  m_pDBConnectPool->put(std::move(pDBConnect));
  return bRet;
}

//------------------------------------------------------------------------------
bool CDatabaseManager::SearchPagingAlarmAssistant(
  const std::shared_ptr<KTBAlarmAssistantSearchCondition> ptCondition,
  std::shared_ptr<KTBAlarmAssistantSearchResult> ptResult)
{
  // 获取一个连接
  COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
  if (!pDBConnect) return false;

  // 处理连接
  bool bRet = m_pAlarmAssistantTable->SearchPaging(const_cast<otl_connect &>(*pDBConnect), ptCondition, ptResult);

  // 将连接归还给连接池
  m_pDBConnectPool->put(std::move(pDBConnect));
  return bRet;
}

//------------------------------------------------------------------------------
bool CDatabaseManager::AddChangeShifts(
  const std::list<std::shared_ptr<KTBChgShifts>> &lstInfo,
  std::list<uint32_t> &lstID)
{
  // 获取一个连接
  COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
  if (!pDBConnect) return false;

  // 处理连接
  bool bRet = m_pChangeShiftsTable->Add(const_cast<otl_connect &>(*pDBConnect), lstInfo, lstID);

  // 将连接归还给连接池
  m_pDBConnectPool->put(std::move(pDBConnect));
  return bRet;
}

//------------------------------------------------------------------------------
EErrCode CDatabaseManager::SearchSupplierForTagID(const uint32_t &lstID, std::shared_ptr<SupplierInfoAndID> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->SearchSupplierForTagID(const_cast<otl_connect &>(*pDBConnect), lstID, ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}
//------------------------------------------------------------------------------
EErrCode CDatabaseManager::SearchDeviceInfForTagID(const uint32_t &lstID, std::shared_ptr<DeviceInfoAndID> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->SearchDeviceInfForTagID(const_cast<otl_connect &>(*pDBConnect), lstID, ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

//------------------------------------------------------------------------------
EErrCode CDatabaseManager::SearchRepairInfForTagID(const uint32_t &lstID, std::shared_ptr<RepairInfoAndID> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->SearchRepairInfForTagID(const_cast<otl_connect &>(*pDBConnect), lstID, ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

//------------------------------------------------------------------------------
EErrCode CDatabaseManager::SearchRunInfForTagID(const uint32_t &lstID, std::shared_ptr<DeviceRunInfoAndID> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->SearchRunInfForTagID(const_cast<otl_connect &>(*pDBConnect), lstID, ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

//------------------------------------------------------------------------------
EErrCode CDatabaseManager::AddSupplier(const std::shared_ptr<SupplierInfo> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->AddSuplier(const_cast<otl_connect &>(*pDBConnect), ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

//------------------------------------------------------------------------------
EErrCode CDatabaseManager::DeleteSupplier(const string supplierName)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->DeleteSupplier(const_cast<otl_connect &>(*pDBConnect), supplierName);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::ModifySuplier(const std::shared_ptr<SupplierInfo> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->ModifySuplier(const_cast<otl_connect &>(*pDBConnect), ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::SearchSuplier(const string supplierName, list<std::shared_ptr<SupplierInfo>> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->SearchSuplier(const_cast<otl_connect &>(*pDBConnect), supplierName, ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::AddMeterType(const std::shared_ptr<MeterType> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->AddMeterType(const_cast<otl_connect &>(*pDBConnect), ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::DeleteMeterType(const string meterTypeName)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->DeleteMeterType(const_cast<otl_connect &>(*pDBConnect), meterTypeName);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::ModifyMeterType(const std::shared_ptr<MeterType> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->ModifyMeterType(const_cast<otl_connect &>(*pDBConnect), ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::SearchMeterType(const string meterTypeName, list<std::shared_ptr<MeterType>> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->SearchMeterType(const_cast<otl_connect &>(*pDBConnect), meterTypeName, ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::AddEnergyCate(const std::shared_ptr<EnergyCate> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->AddEnergyCate(const_cast<otl_connect &>(*pDBConnect), ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::DeleteEnergyCate(const string name)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->DeleteEnergyCate(const_cast<otl_connect &>(*pDBConnect), name);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::ModifyEnergyCate(const std::shared_ptr<EnergyCate> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->ModifyEnergyCate(const_cast<otl_connect &>(*pDBConnect), ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::SearchEnergyCate(const string name, list<std::shared_ptr<EnergyCate>> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->SearchEnergyCate(const_cast<otl_connect &>(*pDBConnect), name, ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::AddDevFaultCate(const std::shared_ptr<DevFaultCate> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->AddDevFaultCate(const_cast<otl_connect &>(*pDBConnect), ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::DeleteDevFaultCate(const string name)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->DeleteDevFaultCate(const_cast<otl_connect &>(*pDBConnect), name);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::ModifyDevFaultCate(const std::shared_ptr<DevFaultCate> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->ModifyDevFaultCate(const_cast<otl_connect &>(*pDBConnect), ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::SearchDevFaultCate(const string name, list<std::shared_ptr<DevFaultCate>> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->SearchDevFaultCate(const_cast<otl_connect &>(*pDBConnect), name, ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::AddDeviceInfo(const std::shared_ptr<DeviceInfo> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->AddDeviceInfo(const_cast<otl_connect &>(*pDBConnect), ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::DeleteDeviceInfo(const string name)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->DeleteDeviceInfo(const_cast<otl_connect &>(*pDBConnect), name);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::ModifyDeviceInfo(const std::shared_ptr<DeviceInfo> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->ModifyDeviceInfo(const_cast<otl_connect &>(*pDBConnect), ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::SearchDeviceInfo(const string name, list<std::shared_ptr<DeviceInfo>> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->SearchDeviceInfo(const_cast<otl_connect &>(*pDBConnect), name, ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::AddMeterInfo(const std::shared_ptr<MeterInfo> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->AddMeterInfo(const_cast<otl_connect &>(*pDBConnect), ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::DeleteMeterInfo(const string name)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->DeleteMeterInfo(const_cast<otl_connect &>(*pDBConnect), name);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::ModifyMeterInfo(const std::shared_ptr<MeterInfo> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->ModifyMeterInfo(const_cast<otl_connect &>(*pDBConnect), ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::SearchMeterInfo(const string name, list<std::shared_ptr<MeterInfo>> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->SearchMeterInfo(const_cast<otl_connect &>(*pDBConnect), name, ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::AddDeviceParameter(const std::shared_ptr<DeviceParameter> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->AddDeviceParameter(const_cast<otl_connect &>(*pDBConnect), ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::DeleteDeviceParameter(const std::shared_ptr<DeviceParameter> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->DeleteDeviceParameter(const_cast<otl_connect &>(*pDBConnect), ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::SearchDeviceParameter(const string name, std::shared_ptr<DeviceParameter> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->SearchDeviceParameter(const_cast<otl_connect &>(*pDBConnect), name, ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::AddMeterParameter(const std::shared_ptr<MeterParameter> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->AddMeterParameter(const_cast<otl_connect &>(*pDBConnect), ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::DeleteMeterParameter(const std::shared_ptr<MeterParameter> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->DeleteMeterParameter(const_cast<otl_connect &>(*pDBConnect), ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::SearchMeterParameter(const string name, std::shared_ptr<MeterParameter> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->SearchMeterParameter(const_cast<otl_connect &>(*pDBConnect), name, ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::AddDeviceRepair(const std::shared_ptr<RepairInfo> &ptResult, uint32_t & ulRecordingID)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->AddDeviceRepair(const_cast<otl_connect &>(*pDBConnect), ptResult, ulRecordingID);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::DeleteDeviceRepair(const int name)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->DeleteDeviceRepair(const_cast<otl_connect &>(*pDBConnect), name);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::ModifyDeviceRepair(const std::shared_ptr<RepairInfo> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->ModifyDeviceRepair(const_cast<otl_connect &>(*pDBConnect), ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::SearchDeviceRepair(const int name, list<shared_ptr<RepairInfo>> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->SearchDeviceRepair(const_cast<otl_connect &>(*pDBConnect), name, ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::SearchDeviceRepair(const string name, list<shared_ptr<RepairInfo>> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->SearchDeviceRepair(const_cast<otl_connect &>(*pDBConnect), name, ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::AddDeviceRun(const std::shared_ptr<DeviceRunInfo> &ptResult, uint32_t & ulRecordingID)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->AddDeviceRun(const_cast<otl_connect &>(*pDBConnect), ptResult, ulRecordingID);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::DeleteDeviceRun(const int name)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->DeleteDeviceRun(const_cast<otl_connect &>(*pDBConnect), name);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::ModifyDeviceRun(const std::shared_ptr<DeviceRunInfo> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->ModifyDeviceRun(const_cast<otl_connect &>(*pDBConnect), ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::SearchDeviceRun(const int name, list<shared_ptr<DeviceRunInfo>> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->SearchDeviceRun(const_cast<otl_connect &>(*pDBConnect), name, ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::SearchDeviceRun(const string name, list<shared_ptr<DeviceRunInfo>> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->SearchDeviceRun(const_cast<otl_connect &>(*pDBConnect), name, ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::SearchSubSysVariableInfo(shared_ptr<SubSysInfo> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->SearchSubSysVariableInfo(const_cast<otl_connect &>(*pDBConnect), ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::SearchDeviceVariableInfo(const string name, shared_ptr<DeviceVarInfo> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->SearchDeviceVariableInfo(const_cast<otl_connect &>(*pDBConnect), name, ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::SearchTagVariableInfo(const string name, shared_ptr<TagVarInfo> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->SearchTagVariableInfo(const_cast<otl_connect &>(*pDBConnect), name, ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::SearchOverViewInfo(shared_ptr<OverViewInfo> &ptResult)
{
	// 获取一个连接
	COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
	if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

	// 处理连接
	EErrCode bRet = m_pBMSInfoManager->SearchOverViewInfo(const_cast<otl_connect &>(*pDBConnect), ptResult);

	// 将连接归还给连接池
	m_pDBConnectPool->put(std::move(pDBConnect));
	return bRet;
}

EErrCode CDatabaseManager::SearchAlarmCountInfo(shared_ptr<Subsystem_Alarm> &ptResult)
{
	// 获取一个连接
	COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
	if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

	// 处理连接
	EErrCode bRet = m_pBMSInfoManager->SearchSystemAlarmCountInfo(const_cast<otl_connect &>(*pDBConnect), ptResult);

	// 将连接归还给连接池
	m_pDBConnectPool->put(std::move(pDBConnect));
	return bRet;
}

EErrCode CDatabaseManager::SearchTagVarWithoutPar(const string name, shared_ptr<TagVarInfo> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->SearchTagVarWithoutPar(const_cast<otl_connect &>(*pDBConnect), name, ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::SearchMeterValueInfo(const shared_ptr<ValueRequestInfo>  ptRequestInfo, shared_ptr<ResponseMeterValueInfo> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->SearchMeterValueInfo(const_cast<otl_connect &>(*pDBConnect), ptRequestInfo, ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::SearchDeviceValueInfo(const shared_ptr<ValueRequestInfo>  ptRequestInfo, shared_ptr<DeviceValueInfo> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->SearchDeviceValueInfo(const_cast<otl_connect &>(*pDBConnect), ptRequestInfo, ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::SearchVariableValueInfo(const shared_ptr<ValueRequestInfo>  ptRequestInfo, shared_ptr<DeviceValueInfo> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->SearchVariableValueInfo(const_cast<otl_connect &>(*pDBConnect), ptRequestInfo, ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::SearchHistoryEventExInfo(const shared_ptr<HistoryEventRequestInfo>  ptRequestInfo, shared_ptr<HistoryEventInfo> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->SearchHistoryEventExInfo(const_cast<otl_connect &>(*pDBConnect), ptRequestInfo, ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

EErrCode CDatabaseManager::SearchHistoryAlarmExInfo(const shared_ptr<HistoryAlarmRequestInfo>  ptRequestInfo, shared_ptr<HistoryAlarmInfo> &ptResult)
{
    // 获取一个连接
    COTLConnectPool::connect_ptr  pDBConnect = m_pDBConnectPool->get();
    if (!pDBConnect) return EErrCode::EC_DATABASE_FAIL;

    // 处理连接
    EErrCode bRet = m_pBMSInfoManager->SearchHistoryAlarmExInfo(const_cast<otl_connect &>(*pDBConnect), ptRequestInfo, ptResult);

    // 将连接归还给连接池
    m_pDBConnectPool->put(std::move(pDBConnect));
    return bRet;
}

void CDatabaseManager::OnThreadPrc()
{
    //m_pDatabaseRepportForms->startCheck(m_pDBConnectPool);
}
