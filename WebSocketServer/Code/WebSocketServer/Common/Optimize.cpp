#include "stdafx.h"
#include "Optimize.h"
#include <random>

using namespace std;


// -- COptimize --

//------------------------------------------------------------------------------
COptimize::COptimize()
{
  m_bUsed = false;
  m_ullUsedMark = -1;
}

//------------------------------------------------------------------------------
COptimize::~COptimize()
{
}

//------------------------------------------------------------------------------
// ����һ��Mark�����ж��Ƿ��б�ʹ�õ�Mark�����û�У������ѡ��һ��
//------------------------------------------------------------------------------
uint64_t COptimize::CreMark()
{
  lock_guard<recursive_mutex> alock(m_mutex);
  random_type ullMark = 0;

  // �����������һ������
  std::random_device  rd;
  std::default_random_engine  eng(rd());
  std::uniform_int_distribution<random_type>  dis;

  // �����������������ڣ�������һ��
  ullMark = dis(eng);
  if (m_setMark.find(ullMark) == m_setMark.end())
    ullMark = dis(eng);

  // �����������
  m_setMark.insert(ullMark);

  // �ж��Ƿ���Mark��ʹ��
  if (m_bUsed) ullMark;

  // û�о�ѡ��һ����Ϊ��ʹ�õ�Mark
  m_ullUsedMark = ullMark;
  m_bUsed = true;
  return ullMark;
}

//------------------------------------------------------------------------------
// ��ȡ��ѡ�е�mark
//------------------------------------------------------------------------------
random_type COptimize::GetUsedMark()
{
  lock_guard<recursive_mutex> alock(m_mutex);
  return m_ullUsedMark;
}

//------------------------------------------------------------------------------
// ɾ��һ��Mark��������Mark�Ǳ�ʹ�õģ��������ѡ��һ��
//------------------------------------------------------------------------------
void COptimize::DelMark(random_type ullMark)
{
  lock_guard<recursive_mutex> alock(m_mutex);

  // ɾ�����Mark
  m_setMark.erase(ullMark);
  // �ж��Ƿ��Ѿ�û��Mark
  if (m_setMark.empty())
  {
    m_bUsed = false;
    m_ullUsedMark = 0;
    return;
  }

  // ����Ǳ�ʹ�õ��Ǹ�������ѡ��һ��
  if (ullMark == m_ullUsedMark)
  {
    auto iter = m_setMark.begin();
    m_ullUsedMark = *iter;
  }
}
