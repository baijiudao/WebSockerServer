//------------------------------------------------------------------------------
// 创建日期：2017-09-18 10:20:56
// 文件描述：值班台的交接班日志表的管理
//------------------------------------------------------------------------------
#ifndef _CHANGE_SHIFTS_TABLE_H_
#define _CHANGE_SHIFTS_TABLE_H_

#include "TableBase.h"

//------------------------------------------------------------------------------
// 创建日期：2017-09-18 10:32:02 
// 描    述：与数据库中的ChangeShifts（值班台交接班日志）表对应
//------------------------------------------------------------------------------

// 交接班所有信息
typedef struct _TBChgShifts
{
  std::pair<uint32_t, bool>     lID;                        // ID
  std::pair<std::string, bool>  strUserName;                // 值班人（用户名）
  std::pair<uint64_t, bool>     ullSuccessionTime;          // 接班时间（毫秒）
  std::pair<uint64_t, bool>     ullShiftTime;               // 交班时间（毫秒）

  std::pair<std::string, bool>  strAlarmProcessing;         // 告警处理情况(主要包括未处理的告警)
  std::pair<std::string, bool>  strCommunicationMonitoring; // 通信监测
  std::pair<std::string, bool>  strLogText;                 // 日志
  _TBChgShifts()
  {
    lID = { 0, false };
    strUserName = { "", false };
    ullSuccessionTime = { 0, false };
    ullShiftTime = { 0, false };

    strAlarmProcessing = { "", false };
    strCommunicationMonitoring = { "", false };
    strLogText = { "", false };
  }
}KTBChgShifts;

// 交接班简要信息
typedef struct _TBChgShiftsAbstract
{
  std::pair<uint32_t, bool>     lID;                        // ID
  std::pair<std::string, bool>  strUserName;                // 值班人（用户名）
  std::pair<uint64_t, bool>     ullSuccessionTime;          // 接班时间（毫秒）
  std::pair<uint64_t, bool>     ullShiftTime;               // 交班时间（毫秒）

  _TBChgShiftsAbstract()
  {
    lID = { 0, false };
    strUserName = { "", false };
    ullSuccessionTime = { 0, false };
    ullShiftTime = { 0, false };
  }
}KTBChgShiftsAbstract;

// 交接班详细信息（主要是大文本数据）
typedef struct _TBChgShiftsDetailed
{
  std::pair<uint32_t, bool>     lChgShiftsID;               // 与交接班日志信息相对应的ID
  std::pair<std::string, bool>  strAlarmProcessing;         // 告警处理情况(主要包括未处理的告警)
  std::pair<std::string, bool>  strCommunicationMonitoring; // 通信监测
  std::pair<std::string, bool>  strLogText;                 // 日志
  _TBChgShiftsDetailed()
  {
    strAlarmProcessing = { "", false };
    strCommunicationMonitoring = { "", false };
    strLogText = { "", false };
  }
}KTBChgShiftsDetailed;

class CChangeShiftsTable : public CTableBase
{
public:
  CChangeShiftsTable();
  ~CChangeShiftsTable();
  bool  Initialise(const otl_connect &dbconnect);
  bool  Add(const otl_connect &dbconnect, const std::list<std::shared_ptr<KTBChgShifts>> &lstInfo, std::list<uint32_t> &lstID);
};

#endif
