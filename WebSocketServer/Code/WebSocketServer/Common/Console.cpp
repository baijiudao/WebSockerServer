//------------------------------------------------------------------------------
// ��������: 2014-08-18 14:00:40
// �ļ�����: �����򿪿���̨���ڣ��Ա����������Ϣ���Դ�ӡ������̨;
//            ֻ��Ҫ�������ļ���ͷ�ļ����ɣ����趨�����
//------------------------------------------------------------------------------
#include "StdAfx.h"
#include "Console.h"

namespace Custom
{
  CConsole g_StartConsole;

  CConsole::CConsole(void)
  {
    AllocConsole(); // �򿪿���̨��Դ
    OpenCnsl();
  }


  CConsole::~CConsole(void)
  {
    FreeConsole();  // �ͷſ���̨��Դ
  }

  void CConsole::OpenCnsl()
  {
    freopen( "CONOUT$", "w+t", stdout );// ����д
    freopen( "CONIN$", "r+t", stdin ); // �����
  }
}
