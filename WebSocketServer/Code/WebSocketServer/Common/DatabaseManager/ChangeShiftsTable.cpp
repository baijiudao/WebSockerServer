#include "stdafx.h"
#include "ChangeShiftsTable.h"

//------------------------------------------------------------------------------
CChangeShiftsTable::CChangeShiftsTable()
{
}

//------------------------------------------------------------------------------
CChangeShiftsTable::~CChangeShiftsTable()
{
}

//------------------------------------------------------------------------------
bool CChangeShiftsTable::Initialise(const otl_connect &dbconnect)
{
  std::string strSQLTable = R"(
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[tb_ChgShiftsAbstract]') AND type in (N'U'))
BEGIN
CREATE TABLE [dbo].[tb_ChgShiftsAbstract](
	[ID] [int] IDENTITY(1,1) NOT NULL,
	[UserName] [varchar](max) NOT NULL,
	[SuccessionTime] [datetime] NOT NULL,
	[ShiftTime] [datetime] NOT NULL,
 CONSTRAINT [PK_tb_ChangeShiftsAbstract] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
END

IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[tb_ChgShiftsDetailed]') AND type in (N'U'))
BEGIN
CREATE TABLE [dbo].[tb_ChgShiftsDetailed](
	[ChangeShiftsID] [int] NOT NULL,
	[AlarmProcessing] [varchar](max) NOT NULL,
	[CommunicationMonitoring] [varchar](max) NOT NULL,
	[LogText] [varchar](max) NOT NULL,
 CONSTRAINT [PK_tb_ChgShiftsDetailed] PRIMARY KEY CLUSTERED 
(
	[ChangeShiftsID] ASC
)WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
END
)";

  return OnInitialise(dbconnect, strSQLTable);
}

//------------------------------------------------------------------------------
bool CChangeShiftsTable::Add(const otl_connect &dbconnect,
  const std::list<std::shared_ptr<KTBChgShifts>> &lstInfo,
  std::list<uint32_t> &lstID)
{
  // 如果没有要添加的数据，则返回成功
  if (lstInfo.empty()) return true;
  otl_connect &db = const_cast<otl_connect &>(dbconnect);
  otl_nocommit_stream strm;

  string strSQL = R"(
INSERT INTO tb_ChgShiftsAbstract
           (UserName,
            SuccessionTime,
            ShiftTime)
      OUTPUT INSERTED.ID
      VALUES
           (:v_UserName<char[256]>,
            :v_SuccessionTime<timestamp>,
            :v_ShiftTime<timestamp>);

INSERT INTO tb_ChgShiftsDetailed
      (ChangeShiftsID,
      AlarmProcessing,
      CommunicationMonitoring,
      LogText)
      VALUES
      (SCOPE_IDENTITY(),
      :v_AlarmProcessing<char[256]>,
      :v_CommunicationMonitoring<char[256]>,
      :v_LogText<char[256]>)
)";

  try
  {
    otl_datetime  SuccessionTime;
    otl_datetime  ShiftTime;
    int nID = 0;
    uint64_t  ullM = 0;  // 秒
    struct tm *ptmp1 = nullptr;
    struct tm *ptmp2 = nullptr;

    strm.open(strSQL.size(), strSQL.c_str(), db, otl_implicit_select);

    // 添加所有数据
    for (auto &itemInfo : lstInfo)
    {
      ullM = itemInfo->ullSuccessionTime.first / 1000;
      ptmp1 = localtime((const time_t*)&ullM);
      ullM = itemInfo->ullShiftTime.first / 1000;
      ptmp2 = localtime((const time_t*)&ullM);

      // tb_ChgShiftsAbstract
      strm << itemInfo->strUserName.first;
      strm << otl_datetime(ptmp1->tm_year + 1900,
        ptmp1->tm_mon + 1,
        ptmp1->tm_mday,
        ptmp1->tm_hour,
        ptmp1->tm_min,
        ptmp1->tm_sec,
        itemInfo->ullSuccessionTime.first % 1000,
        OTL_CUSTOM_TIME_PRECISION);
      strm << otl_datetime(ptmp2->tm_year + 1900,
        ptmp2->tm_mon + 1,
        ptmp2->tm_mday,
        ptmp2->tm_hour,
        ptmp2->tm_min,
        ptmp2->tm_sec,
        itemInfo->ullSuccessionTime.first % 1000,
        OTL_CUSTOM_TIME_PRECISION);

      // tb_ChgShiftsDetailed
      strm << itemInfo->strAlarmProcessing.first;
      strm << itemInfo->strCommunicationMonitoring.first;
      strm << itemInfo->strLogText.first;

      // 没有返回ID则抛出异常
      if (strm.eof()) throw otl_exception(OTL_CUSTOM_EXCEPTION_IDENTITY_INVALID, 0);

      // 获取插入的ID
      strm >> nID;
      lstID.push_back(nID);
    }

    strm.flush();
    strm.close();
    // 提交事务
    db.commit();
  }
  catch (otl_exception &e)
  {
    strm.close();
    // 回滚事务
    db.rollback();
    LOG_ERROR << e.code << "：" << e.msg << " " << e.stm_text << " " << e.var_info;
    return false;
  }

  return true;
}
