//------------------------------------------------------------------------------
// �������ڣ�2017-09-18 10:20:56
// �ļ�������ֵ��̨�Ľ��Ӱ���־��Ĺ���
//------------------------------------------------------------------------------
#ifndef _CHANGE_SHIFTS_TABLE_H_
#define _CHANGE_SHIFTS_TABLE_H_

#include "TableBase.h"

//------------------------------------------------------------------------------
// �������ڣ�2017-09-18 10:32:02 
// ��    ���������ݿ��е�ChangeShifts��ֵ��̨���Ӱ���־�����Ӧ
//------------------------------------------------------------------------------

// ���Ӱ�������Ϣ
typedef struct _TBChgShifts
{
  std::pair<uint32_t, bool>     lID;                        // ID
  std::pair<std::string, bool>  strUserName;                // ֵ���ˣ��û�����
  std::pair<uint64_t, bool>     ullSuccessionTime;          // �Ӱ�ʱ�䣨���룩
  std::pair<uint64_t, bool>     ullShiftTime;               // ����ʱ�䣨���룩

  std::pair<std::string, bool>  strAlarmProcessing;         // �澯�������(��Ҫ����δ����ĸ澯)
  std::pair<std::string, bool>  strCommunicationMonitoring; // ͨ�ż��
  std::pair<std::string, bool>  strLogText;                 // ��־
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

// ���Ӱ��Ҫ��Ϣ
typedef struct _TBChgShiftsAbstract
{
  std::pair<uint32_t, bool>     lID;                        // ID
  std::pair<std::string, bool>  strUserName;                // ֵ���ˣ��û�����
  std::pair<uint64_t, bool>     ullSuccessionTime;          // �Ӱ�ʱ�䣨���룩
  std::pair<uint64_t, bool>     ullShiftTime;               // ����ʱ�䣨���룩

  _TBChgShiftsAbstract()
  {
    lID = { 0, false };
    strUserName = { "", false };
    ullSuccessionTime = { 0, false };
    ullShiftTime = { 0, false };
  }
}KTBChgShiftsAbstract;

// ���Ӱ���ϸ��Ϣ����Ҫ�Ǵ��ı����ݣ�
typedef struct _TBChgShiftsDetailed
{
  std::pair<uint32_t, bool>     lChgShiftsID;               // �뽻�Ӱ���־��Ϣ���Ӧ��ID
  std::pair<std::string, bool>  strAlarmProcessing;         // �澯�������(��Ҫ����δ����ĸ澯)
  std::pair<std::string, bool>  strCommunicationMonitoring; // ͨ�ż��
  std::pair<std::string, bool>  strLogText;                 // ��־
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
