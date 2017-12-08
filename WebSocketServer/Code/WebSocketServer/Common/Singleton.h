//------------------------------------------------------------------------------
// 创建日期：2017-10-07 23:07:28
// 文件描述：定义一个单例模式的基类，继承这个类就可以成为一个单例类
//------------------------------------------------------------------------------
#ifndef _SINGLETON_H_
#define _SINGLETON_H_

#include <memory>
#include <mutex>
#include <boost/noncopyable.hpp>

template <typename T>
class ISingleton : public boost::noncopyable
{
private:
  static std::shared_ptr<T> m_pInstance;
  static std::recursive_mutex m_mutex;
public:
  static std::shared_ptr<T> Inst()
  {
    if (!m_pInstance)
    {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      if (!m_pInstance) m_pInstance = std::make_shared<T>();
    }
    return m_pInstance;
  }
};

// 初始化静态成员变量
template <typename T>
std::shared_ptr<T> ISingleton<T>::m_pInstance = nullptr;
template <typename T>
std::recursive_mutex ISingleton<T>::m_mutex;

#endif // !_SINGLETON_H_

