#include "stdafx.h"
#include "AlarmAssistantTable.h"

using namespace std;

//------------------------------------------------------------------------------
CAlarmAssistantTable::CAlarmAssistantTable()
{
}

//------------------------------------------------------------------------------
CAlarmAssistantTable::~CAlarmAssistantTable()
{
}

//------------------------------------------------------------------------------
bool CAlarmAssistantTable::Initialise(const otl_connect &dbconnect)
{
  std::string strSQL = R"(
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[tb_AlarmAssistant]') AND type in (N'U'))
BEGIN
CREATE TABLE [dbo].[tb_AlarmAssistant](
	[ID] [int] IDENTITY(1,1) NOT NULL,
	[GlobalID] [int] NOT NULL,
	[DeviceName] [varchar](max) NOT NULL,
	[CameraIP] [int] NOT NULL,
	[CameraUserName] [varchar](max) NOT NULL,
	[CameraPassword] [varchar](max) NOT NULL,
	[EmergencyPlanFileName] [varchar](max) NOT NULL,
	[EmergencyPlanStorageAddress] [varchar](max) NOT NULL,
	[LinkageSchemeName] [varchar](max) NOT NULL,
	[LinkageSchemeDescription] [varchar](max) NOT NULL,
 CONSTRAINT [PK_tb_AlarmAssistant] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
END
)";

  return OnInitialise(dbconnect, strSQL);
}

//------------------------------------------------------------------------------
bool CAlarmAssistantTable::Add(const otl_connect &dbconnect,
  const std::list<std::shared_ptr<KTBAlarmAssistant>> &lstInfo,
  std::list<uint32_t> &lstID)
{
  // ���û��Ҫ��ӵ����ݣ��򷵻سɹ�
  if (lstInfo.empty()) return true;
  otl_connect &db = const_cast<otl_connect &>(dbconnect);
  otl_nocommit_stream strm;

  string strSQL = R"(
INSERT INTO tb_AlarmAssistant
           (GlobalID,
            DeviceName,
            CameraIP,
            CameraUserName,
            CameraPassword,
            EmergencyPlanFileName,
            EmergencyPlanStorageAddress,
            LinkageSchemeName,
            LinkageSchemeDescription)
      OUTPUT INSERTED.ID
      VALUES
           (:v_GlobalID<int>,
            :v_DeviceName<char[256]>,
            :v_CameraIP<int>,
            :v_CameraUserName<char[256]>,
            :v_CameraPassword<char[256]>,
            :v_EmergencyPlanFileName<char[256]>,
            :v_EmergencyPlanStorageAddress<char[256]>,
            :v_LinkageSchemeName<char[256]>,
            :v_LinkageSchemeDescription<char[256]>)
)";

  try
  {
    int nID = 0;
    strm.open(strSQL.size(), strSQL.c_str(), db, otl_implicit_select);

    // �����������
    for (auto &itemInfo : lstInfo)
    {
      strm << static_cast<int>(itemInfo->lGlobalID.first);
      strm << itemInfo->strDeviceName.first;
      strm << static_cast<int>(itemInfo->lCameraIP.first);
      strm << itemInfo->strCameraUserName.first;
      strm << itemInfo->strCameraPassword.first;
      strm << itemInfo->strEmergencyPlanFileName.first;
      strm << itemInfo->strEmergencyPlanStorageAddress.first;
      strm << itemInfo->strLinkageSchemeName.first;
      strm << itemInfo->strLinkageSchemeDescription.first;

      // û�з���ID���׳��쳣
      if (strm.eof()) throw otl_exception(OTL_CUSTOM_EXCEPTION_IDENTITY_INVALID, 0);

      // ��ȡ�����ID
      strm >> nID;
      lstID.push_back(nID);
    }

    strm.flush();
    strm.close();
    // �ύ����
    db.commit();
  }
  catch (otl_exception &e)
  {
    strm.close();
    // �ع�����
    db.rollback();
    LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
    return false;
  }

  return true;
}

//------------------------------------------------------------------------------
bool CAlarmAssistantTable::Delete(const otl_connect &dbconnect,
  const std::list<uint32_t> &lstID)
{
  // ���û��Ҫɾ�������ݣ��򷵻سɹ�
  if (lstID.empty()) return true;
  otl_connect &db = const_cast<otl_connect &>(dbconnect);
  otl_nocommit_stream strm;

  string strSQL = R"(
DELETE FROM tb_AlarmAssistant
WHERE ID = :v_ID<int>
)";

  try
  {
    strm.open(strSQL.size(), strSQL.c_str(), db);

    // �����������
    for (auto &itemID : lstID)
      strm << static_cast<int>(itemID);

    strm.flush();
    // �ύ����
    db.commit();
    strm.close();
  }
  catch (otl_exception &e)
  {
    // �ع�����
    db.rollback();
    strm.close();
    LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
    return false;
  }

  return true;
}

//------------------------------------------------------------------------------
bool CAlarmAssistantTable::Modify(const otl_connect &dbconnect,
  const std::list<std::shared_ptr<KTBAlarmAssistant>> &lstInfo)
{
  // ���û��Ҫ��ӵ����ݣ��򷵻سɹ�
  if (lstInfo.empty()) return true;
  otl_connect &db = const_cast<otl_connect &>(dbconnect);
  otl_nocommit_stream strm;

  string strSQL = R"(
  UPDATE tb_AlarmAssistant
  SET
            DeviceName = :v_DeviceName<char[256]>,
            CameraIP = :v_CameraIP<int>,
            CameraUserName = :v_CameraUserName<char[256]>,
            CameraPassword = :v_CameraPassword<char[256]>,
            EmergencyPlanFileName = :v_EmergencyPlanFileName<char[256]>,
            EmergencyPlanStorageAddress = :v_EmergencyPlanStorageAddress<char[256]>,
            LinkageSchemeName = :v_LinkageSchemeName<char[256]>,
            LinkageSchemeDescription = :v_LinkageSchemeDescription<char[256]>
  WHERE ID = :v_ID<int>
)";

  try
  {
    strm.open(strSQL.size(), strSQL.c_str(), db);

    // �����������
    for (auto &itemInfo : lstInfo)
    {
      // GlobalID �����޸�
      strm << itemInfo->strDeviceName.first;
      strm << static_cast<int>(itemInfo->lCameraIP.first);
      strm << itemInfo->strCameraUserName.first;
      strm << itemInfo->strCameraPassword.first;
      strm << itemInfo->strEmergencyPlanFileName.first;
      strm << itemInfo->strEmergencyPlanStorageAddress.first;
      strm << itemInfo->strLinkageSchemeName.first;
      strm << itemInfo->strLinkageSchemeDescription.first;
      strm << static_cast<int>(itemInfo->lID.first);
    }

    strm.flush();
    // �ύ����
    db.commit();
    strm.close();
  }
  catch (otl_exception &e)
  {
    // �ع�����
    db.rollback();
    strm.close();
    LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
    return false;
  }

  return true;
}

//------------------------------------------------------------------------------
// �����豸ID��ѯ���
//------------------------------------------------------------------------------
bool CAlarmAssistantTable::Search(const otl_connect &dbconnect,
  uint32_t ulGlobalID, std::list<std::shared_ptr<KTBAlarmAssistant>> &lstInfo)
{
  otl_connect &db = const_cast<otl_connect &>(dbconnect);
  otl_nocommit_stream strm;

  // �齨��ѯ�����е�SQL WHERE����
  string  strCondition = " WHERE GlobalID = :v_GlobalID<int> ";

  string strSQL = R"(
SELECT * FROM tb_AlarmAssistant 
)" + strCondition + R"( ORDER BY ID DESC
)";

  try
  {
    strm.open(strSQL.size(), strSQL.c_str(), db);
    strm.set_flush(false);

    // �����������
    strm << static_cast<int>(ulGlobalID);

    // �ж��Ƿ��н��
    for (; 0 == strm.eof();)
    {
      shared_ptr<KTBAlarmAssistant>  ptTBInfo = make_shared<KTBAlarmAssistant>();
      strm >> ptTBInfo->lID.first;
      strm >> ptTBInfo->lGlobalID.first;
      strm >> ptTBInfo->strDeviceName.first;
      strm >> ptTBInfo->lCameraIP.first;
      strm >> ptTBInfo->strCameraUserName.first;
      strm >> ptTBInfo->strCameraPassword.first;
      strm >> ptTBInfo->strEmergencyPlanFileName.first;
      strm >> ptTBInfo->strEmergencyPlanStorageAddress.first;
      strm >> ptTBInfo->strLinkageSchemeName.first;
      strm >> ptTBInfo->strLinkageSchemeDescription.first;
      lstInfo.push_back(ptTBInfo);
    }

    strm.close();
  }
  catch (otl_exception &e)
  {
    // �ع�����
    db.rollback();
    strm.close();
    LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
    return false;
  }

  return true;
}

//------------------------------------------------------------------------------
bool CAlarmAssistantTable::Search_Num(const otl_connect &dbconnect,
  const std::shared_ptr<KTBAlarmAssistantSearchCondition_Num> ptCondition,
  std::shared_ptr<KTBAlarmAssistantSearchResult_Num> ptResult)
{
  if (!ptCondition || !ptResult) return false;
  otl_connect &db = const_cast<otl_connect &>(dbconnect);
  otl_nocommit_stream strm;

  // �齨��ѯ�����е�SQL WHERE����
  string  strCondition = "";
  if (ptCondition->tParameter.lGlobalID.second)
  {
    if (!strCondition.empty()) strCondition += R"( AND )";
    strCondition += " GlobalID = :v_GlobalID<int> ";
  }

  if (ptCondition->tParameter.strDeviceName.second)
  {
    if (!strCondition.empty()) strCondition += R"( AND )";
    strCondition += " DeviceName = :v_DeviceName<char[256]> ";
  }

  if (ptCondition->tParameter.strLinkageSchemeName.second)
  {
    if (!strCondition.empty()) strCondition += R"( AND )";
    strCondition += " LinkageSchemeName = :v_LinkageSchemeName<char[256]> ";
  }

  if (!strCondition.empty()) strCondition = " WHERE " + strCondition;

  string strSQL = R"(
SELECT * FROM tb_AlarmAssistant w1 
WHERE ID in 
(
  SELECT top
)" + to_string(ptCondition->lCount) + R"(
ID FROM
  (
    SELECT top
)" + to_string(ptCondition->lStartNum + ptCondition->lCount) + R"(
ID  FROM tb_AlarmAssistant
)" + strCondition + R"( ORDER BY ID DESC
  ) w 
  ORDER BY w.ID ASC
) 
ORDER BY w1.ID DESC
)";

  string strSQLCount = R"(
SELECT COUNT(0) FROM tb_AlarmAssistant 
)" + strCondition;

  // ��Ӳ�ѯ����
  auto  funcAddCondition = [&strm, &ptCondition](){
    if (ptCondition->tParameter.lGlobalID.second) strm << static_cast<int>(ptCondition->tParameter.lGlobalID.first);
    if (ptCondition->tParameter.strDeviceName.second) strm << ptCondition->tParameter.strDeviceName.first;
    if (ptCondition->tParameter.strLinkageSchemeName.second) strm << ptCondition->tParameter.strLinkageSchemeName.first;
  };

  try
  {
    strm.open(strSQL.size(), strSQL.c_str(), db);
    strm.set_flush(false);

    // �����������
    funcAddCondition();
    
    // �ж��Ƿ��н��
    ptResult->lStartNum = ptCondition->lStartNum;
    for (; 0 == strm.eof();)
    {
      shared_ptr<KTBAlarmAssistant>  ptTBInfo = make_shared<KTBAlarmAssistant>();
      strm >> ptTBInfo->lID.first;
      strm >> ptTBInfo->lGlobalID.first;
      strm >> ptTBInfo->strDeviceName.first;
      strm >> ptTBInfo->lCameraIP.first;
      strm >> ptTBInfo->strCameraUserName.first;
      strm >> ptTBInfo->strCameraPassword.first;
      strm >> ptTBInfo->strEmergencyPlanFileName.first;
      strm >> ptTBInfo->strEmergencyPlanStorageAddress.first;
      strm >> ptTBInfo->strLinkageSchemeName.first;
      strm >> ptTBInfo->strLinkageSchemeDescription.first;
      ptResult->lstData.push_back(ptTBInfo);
    }

    strm.close();

    // ��ȡ����
    strm.open(strSQLCount.size(), strSQLCount.c_str(), db);

    // ��Ӳ�ѯ����
    funcAddCondition();

    // �ж��Ƿ��н��
    if (!strm.eof())
      strm >> ptResult->lTotal;

    strm.close();
  }
  catch (otl_exception &e)
  {
    // �ع�����
    db.rollback();
    strm.close();
    LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
    return false;
  }

  return true;
}

//------------------------------------------------------------------------------
// ��ѯ��ҳʽ����
//------------------------------------------------------------------------------
bool CAlarmAssistantTable::SearchPaging(const otl_connect &dbconnect,
  const std::shared_ptr<KTBAlarmAssistantSearchCondition> ptCondition,
  std::shared_ptr<KTBAlarmAssistantSearchResult> ptResult)
{
  if (!ptCondition || !ptResult) return false;
  otl_connect &db = const_cast<otl_connect &>(dbconnect);
  otl_nocommit_stream strm;

  // �齨��ѯ�����е�SQL WHERE����
  string  strCondition = "";
  if (ptCondition->tParameter.lGlobalID.second)
  {
    if (!strCondition.empty()) strCondition += R"( AND )";
    strCondition += " GlobalID = :v_GlobalID<int> ";
  }

  if (ptCondition->tParameter.strDeviceName.second)
  {
    if (!strCondition.empty()) strCondition += R"( AND )";
    strCondition += " DeviceName = :v_DeviceName<char[256]> ";
  }

  if (ptCondition->tParameter.strLinkageSchemeName.second)
  {
    if (!strCondition.empty()) strCondition += R"( AND )";
    strCondition += " LinkageSchemeName = :v_LinkageSchemeName<char[256]> ";
  }

  if (!strCondition.empty()) strCondition = " WHERE " + strCondition;

  string strSQL = R"(
SELECT * FROM tb_AlarmAssistant w1 
WHERE ID in 
(
  SELECT top 
)" + to_string(ptCondition->lPageSize) + R"(
ID FROM
  (
    SELECT top
)" + to_string(ptCondition->lPageNum * ptCondition->lPageSize) + R"(
 ID  FROM tb_AlarmAssistant
)" + strCondition + R"( ORDER BY ID DESC
  ) w 
  ORDER BY w.ID ASC
) 
ORDER BY w1.ID DESC
)";

  string strSQLCount = R"(
SELECT COUNT(0) FROM tb_AlarmAssistant 
)" + strCondition;

  // ��Ӳ�ѯ����
  auto  funcAddCondition = [&strm, &ptCondition](){
    if (ptCondition->tParameter.lGlobalID.second) strm << static_cast<int>(ptCondition->tParameter.lGlobalID.first);
    if (ptCondition->tParameter.strDeviceName.second) strm << ptCondition->tParameter.strDeviceName.first;
    if (ptCondition->tParameter.strLinkageSchemeName.second) strm << ptCondition->tParameter.strLinkageSchemeName.first;
  };

  try
  {
    strm.open(strSQL.size(), strSQL.c_str(), db);

    // �����������
    strm << static_cast<int>(ptCondition->lPageSize);
    strm << static_cast<int>(ptCondition->lPageNum * ptCondition->lPageSize);
    funcAddCondition();

    // �ж��Ƿ��н��
    if (!strm.eof())
    {
      ptResult->lPageNum = ptCondition->lPageNum;

      for (; 0 == strm.eof();)
      {
        shared_ptr<KTBAlarmAssistant>  ptTBInfo = make_shared<KTBAlarmAssistant>();
        strm >> ptTBInfo->lID.first;
        strm >> ptTBInfo->lGlobalID.first;
        strm >> ptTBInfo->strDeviceName.first;
        strm >> ptTBInfo->lCameraIP.first;
        strm >> ptTBInfo->strCameraUserName.first;
        strm >> ptTBInfo->strCameraPassword.first;
        strm >> ptTBInfo->strEmergencyPlanFileName.first;
        strm >> ptTBInfo->strEmergencyPlanStorageAddress.first;
        strm >> ptTBInfo->strLinkageSchemeName.first;
        strm >> ptTBInfo->strLinkageSchemeDescription.first;
        ptResult->lstData.push_back(ptTBInfo);
      }
    }

    strm.close();

    // ��ȡ����
    strm.open(strSQLCount.size(), strSQLCount.c_str(), db);

    // ��Ӳ�ѯ����
    funcAddCondition();

    // �ж��Ƿ��н��
    if (!strm.eof())
    {
      int nTotal = 0;
      strm >> nTotal;

      ptResult->lTotalPages = nTotal / ptCondition->lPageSize;
      if (0 != (nTotal % ptCondition->lPageSize))
        ptResult->lTotalPages += 1;
    }

    strm.close();
  }
  catch (otl_exception &e)
  {
    // �ع�����
    db.rollback();
    strm.close();
    LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
    return false;
  }

  return true;
}
