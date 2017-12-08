//------------------------------------------------------------------------------
// 创建日期：2017-08-18 13:52:45
// 文件描述：用来优化系统的类
//------------------------------------------------------------------------------
#ifndef _OPTIMIZE_H_
#define _OPTIMIZE_H_

#include <stdint.h>
#include <set>
#include <mutex>

// 随机数类型
typedef uint64_t  random_type;

//------------------------------------------------------------------------------
// 创建日期：2017-08-18 14:44:09 
// 类 描 述：主要是解决RDBSInterface每个对象都会推送数据的问题，会造成数据重复
//          这个类只让一个RDBSInterface接收推送
//------------------------------------------------------------------------------
class COptimize
{
private:
  std::recursive_mutex m_mutex;
  bool  m_bUsed;  // 标识RDBSInterface对象已经有一个对象进行消息推送了
  random_type m_ullUsedMark;     // 用来进行消息推送的RDBSInterface的标志
  std::set<random_type>  m_setMark;  // 所有RDBSInterface的标志（每个RDBSInterface中有个唯一的随机码）
public:
  COptimize();
  ~COptimize();
  random_type CreMark();
  random_type GetUsedMark();
  void      DelMark(random_type ullMark);
};

#endif // !_OPTIMIZE_H_
