#include "Exceptions.h"
#include "TODO.h"

#include <tchar.h>
#include <eh.h>

#define countof(x) (sizeof(x)/sizeof((x)[0]))

///////////////////////////////////////////////////////////////////////////////
// Namespace: Elvees
///////////////////////////////////////////////////////////////////////////////

namespace Elvees {

///////////////////////////////////////////////////////////////////////////////
// CException
///////////////////////////////////////////////////////////////////////////////

CException::CException(LPCTSTR lpWhere, LPCTSTR lpMessage)
{
	if(lpWhere)
		lstrcpy(m_stWhere, lpWhere);
	else
		lstrcpy(m_stWhere, TEXT("Enexpected"));

	if(lpMessage)
		lstrcpy(m_stMessage, lpMessage);
	else
		lstrcpy(m_stMessage, TEXT("Unknown error."));
}

CException::~CException()
{
}

LPCTSTR CException::GetWhere() const
{
	return const_cast<LPCTSTR>(m_stWhere);
}

LPCTSTR CException::GetMessage() const
{
	return const_cast<LPCTSTR>(m_stMessage);
}

void CException::Report(HWND hWnd /* = NULL */) const
{
	::MessageBox(hWnd, m_stMessage, m_stWhere, MB_ICONSTOP);
}

///////////////////////////////////////////////////////////////////////////////
// CWin32Exception
///////////////////////////////////////////////////////////////////////////////

CWin32Exception::CWin32Exception(LPCTSTR lpWhere, DWORD dwError)
	: CException(lpWhere, NULL)
	, m_dwError(dwError)
{
	if(!FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
		0,
		dwError,
		0,
		m_stMessage,
		MAX_ERRORMSG_LEN - 1,
		NULL))
	{
		lstrcpy(m_stMessage, TEXT("FormatMessage failed."));
	}
}

DWORD CWin32Exception::GetError() const
{
	return m_dwError;
}

///////////////////////////////////////////////////////////////////////////////
// CStructuredException
///////////////////////////////////////////////////////////////////////////////

CStructuredException::CStructuredException(PEXCEPTION_POINTERS pep)
	: CException(NULL, NULL)
{
	m_er      = *pep->ExceptionRecord;
	m_context = *pep->ContextRecord;
}

void CStructuredException::MapSEtoCE()
{
	_set_se_translator(TranslateSEtoCE);
}

void __cdecl CStructuredException::TranslateSEtoCE(UINT /*dwEC*/, PEXCEPTION_POINTERS pep)
{
	throw CStructuredException(pep);
}

LPCTSTR CStructuredException::GetWhere() const
{
	return TEXT("CSE");
}

LPCTSTR CStructuredException::GetMessage() const
{
	// Create a filename to record the error information to.
	// Storing it in the executable directory works well.

	TCHAR szModuleName[MAX_PATH*2];
	ZeroMemory(szModuleName, sizeof(szModuleName));
	if(GetModuleFileName(0, szModuleName, countof(szModuleName)-2) <= 0)
		lstrcpy(szModuleName, TEXT("Unknown"));

	TCHAR *pszFilePart = GetFilePart(szModuleName);

	// Extract the file name portion and remove it's file extension
	TCHAR *lastperiod = lstrrchr(pszFilePart, TEXT('.'));
	if(lastperiod)
		lastperiod[0] = 0;

	// VirtualQuery can be used to get the allocation base associated with a
	// code address, which is the same as the ModuleHandle. This can be used
	// to get the filename of the module that the crash happened in.

	MEMORY_BASIC_INFORMATION MemInfo;

	TCHAR szCrashModulePathName[MAX_PATH*2];
	ZeroMemory(szCrashModulePathName, sizeof(szCrashModulePathName));

	TCHAR *pszCrashModuleFileName = TEXT("Unknown");

	if(VirtualQuery((void*)m_context.Eip, &MemInfo, sizeof(MemInfo)) &&
		(GetModuleFileName((HINSTANCE)MemInfo.AllocationBase,
		szCrashModulePathName,
		sizeof(szCrashModulePathName)-2) > 0))
	{
		pszCrashModuleFileName = GetFilePart(szCrashModulePathName);
	}

	wsprintf(const_cast<LPTSTR>(m_stStMessage),
		TEXT("%s caused %s(0x%08X) in module %s at %04x:%08X"),
		pszFilePart, GetExceptionDescription(m_er.ExceptionCode),
		m_er.ExceptionCode,
		pszCrashModuleFileName, m_context.SegCs, m_context.Eip);

	return m_stStMessage;
}

LPTSTR CStructuredException::lstrrchr(LPCTSTR string, int ch) const
{
	LPTSTR start = (LPTSTR)string;

	while(*string++);                       // find end of string
	// search towards front
	while(--string != start && *string != (TCHAR)ch);

	if(*string == (TCHAR)ch)				// char found ?
		return(LPTSTR)string;

	return NULL;
}

LPTSTR CStructuredException::GetFilePart(LPCTSTR source) const
{
	LPTSTR result = lstrrchr(source, TEXT('\\'));
	
	if(result)
		result++;
	else
		result = (LPTSTR)source;

	return result;
}

LPCTSTR CStructuredException::GetExceptionDescription(DWORD dwExceptionCode) const
{
	struct ExceptionNames
	{
		DWORD	dwCode;
		LPCTSTR	stName;
	};

	ExceptionNames ExceptionMap[] =
	{
		{0x40010005, TEXT("a Control-C")},
		{0x40010008, TEXT("a Control-Break")},
		{0x80000002, TEXT("a Datatype Misalignment")},
		{0x80000003, TEXT("a Breakpoint")},
		{0xc0000005, TEXT("an Access Violation")},
		{0xc0000006, TEXT("an In Page Error")},
		{0xc0000017, TEXT("a No Memory")},
		{0xc000001d, TEXT("an Illegal Instruction")},
		{0xc0000025, TEXT("a Noncontinuable Exception")},
		{0xc0000026, TEXT("an Invalid Disposition")},
		{0xc000008c, TEXT("a Array Bounds Exceeded")},
		{0xc000008d, TEXT("a Float Denormal Operand")},
		{0xc000008e, TEXT("a Float Divide by Zero")},
		{0xc000008f, TEXT("a Float Inexact Result")},
		{0xc0000090, TEXT("a Float Invalid Operation")},
		{0xc0000091, TEXT("a Float Overflow")},
		{0xc0000092, TEXT("a Float Stack Check")},
		{0xc0000093, TEXT("a Float Underflow")},
		{0xc0000094, TEXT("an Integer Divide by Zero")},
		{0xc0000095, TEXT("an Integer Overflow")},
		{0xc0000096, TEXT("a Privileged Instruction")},
		{0xc00000fD, TEXT("a Stack Overflow")},
		{0xc0000142, TEXT("a DLL Initialization Failed")},
		{0xe06d7363, TEXT("a Microsoft C++ Exception")},
	};

	for(int i = 0; i < countof(ExceptionMap); i++)
		if(dwExceptionCode == ExceptionMap[i].dwCode)
			return ExceptionMap[i].stName;

	return TEXT("an Unknown exception type");
}

///////////////////////////////////////////////////////////////////////////////
// Namespace: Elvees
///////////////////////////////////////////////////////////////////////////////

} // End of namespace Elvees
