//------------------------------------------------------------------------------
// �������ڣ�2017-10-07 23:07:28
// �ļ�����������һ������ģʽ�Ļ��࣬�̳������Ϳ��Գ�Ϊһ��������
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

// ��ʼ����̬��Ա����
template <typename T>
std::shared_ptr<T> ISingleton<T>::m_pInstance = nullptr;
template <typename T>
std::recursive_mutex ISingleton<T>::m_mutex;

#endif // !_SINGLETON_H_

