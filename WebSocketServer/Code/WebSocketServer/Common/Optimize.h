//------------------------------------------------------------------------------
// �������ڣ�2017-08-18 13:52:45
// �ļ������������Ż�ϵͳ����
//------------------------------------------------------------------------------
#ifndef _OPTIMIZE_H_
#define _OPTIMIZE_H_

#include <stdint.h>
#include <set>
#include <mutex>

// ���������
typedef uint64_t  random_type;

//------------------------------------------------------------------------------
// �������ڣ�2017-08-18 14:44:09 
// �� �� ������Ҫ�ǽ��RDBSInterfaceÿ�����󶼻��������ݵ����⣬����������ظ�
//          �����ֻ��һ��RDBSInterface��������
//------------------------------------------------------------------------------
class COptimize
{
private:
  std::recursive_mutex m_mutex;
  bool  m_bUsed;  // ��ʶRDBSInterface�����Ѿ���һ�����������Ϣ������
  random_type m_ullUsedMark;     // ����������Ϣ���͵�RDBSInterface�ı�־
  std::set<random_type>  m_setMark;  // ����RDBSInterface�ı�־��ÿ��RDBSInterface���и�Ψһ������룩
public:
  COptimize();
  ~COptimize();
  random_type CreMark();
  random_type GetUsedMark();
  void      DelMark(random_type ullMark);
};

#endif // !_OPTIMIZE_H_
