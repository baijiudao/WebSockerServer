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
// 创建一个Mark，并判断是否有被使用的Mark，如果没有，就随机选择一个
//------------------------------------------------------------------------------
uint64_t COptimize::CreMark()
{
  lock_guard<recursive_mutex> alock(m_mutex);
  random_type ullMark = 0;

  // 这里随机生成一个数字
  std::random_device  rd;
  std::default_random_engine  eng(rd());
  std::uniform_int_distribution<random_type>  dis;

  // 生成随机数，如果存在，再生成一次
  ullMark = dis(eng);
  if (m_setMark.find(ullMark) == m_setMark.end())
    ullMark = dis(eng);

  // 将随机数保存
  m_setMark.insert(ullMark);

  // 判断是否有Mark被使用
  if (m_bUsed) ullMark;

  // 没有就选择一个作为被使用的Mark
  m_ullUsedMark = ullMark;
  m_bUsed = true;
  return ullMark;
}

//------------------------------------------------------------------------------
// 获取被选中的mark
//------------------------------------------------------------------------------
random_type COptimize::GetUsedMark()
{
  lock_guard<recursive_mutex> alock(m_mutex);
  return m_ullUsedMark;
}

//------------------------------------------------------------------------------
// 删除一个Mark，如果这个Mark是被使用的，则随机再选择一个
//------------------------------------------------------------------------------
void COptimize::DelMark(random_type ullMark)
{
  lock_guard<recursive_mutex> alock(m_mutex);

  // 删除这个Mark
  m_setMark.erase(ullMark);
  // 判断是否已经没有Mark
  if (m_setMark.empty())
  {
    m_bUsed = false;
    m_ullUsedMark = 0;
    return;
  }

  // 如果是被使用的那个，则再选择一个
  if (ullMark == m_ullUsedMark)
  {
    auto iter = m_setMark.begin();
    m_ullUsedMark = *iter;
  }
}
