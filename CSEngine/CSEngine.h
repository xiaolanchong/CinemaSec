// CSEngine.h : main header file for the CSEngine DLL
//

#ifndef _CS_ENGINE_H_
#define _CS_ENGINE_H_

namespace csengine
{
//! ��� �������� CreateEngineInterface
enum 
{
	IFACE_OK = 0,	//! �����
	IFACE_FAILED	//! ������, ��������� �� ������
};
}

//! This function is automatically exported and allows you to access any interfaces exposed with the above macros.
//! if pReturnCode is set, it will return one of the following values
//! extend this for other error conditions/code
//! 
//! \param pName UID ��������� ��� ��� ���������� ���
//! \param ppInterface ��� �������� ����������
//! \return IFACE_OK ��� IFACE_FAILED
extern "C" DWORD WINAPI CreateEngineInterface(const wchar_t *pName, void **ppInterface);

#endif