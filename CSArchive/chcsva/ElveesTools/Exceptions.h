#ifndef ELVEES_EXCEPTIONS__
#define ELVEES_EXCEPTIONS__

#if defined (_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

#ifndef _WINDOWS_
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif

#define MAX_ERRORMSG_LEN  256
#define MAX_ERRORWRH_LEN  256

///////////////////////////////////////////////////////////////////////////////
// Namespace: Elvees
///////////////////////////////////////////////////////////////////////////////

namespace Elvees {

///////////////////////////////////////////////////////////////////////////////
// CException
///////////////////////////////////////////////////////////////////////////////

class CException
{
public: 
	explicit CException(LPCTSTR lpWhere, LPCTSTR lpMessage);
	virtual ~CException();

	virtual LPCTSTR GetWhere() const;
	virtual LPCTSTR GetMessage() const;

	void Report(HWND hWnd = NULL) const;

protected:
	TCHAR m_stWhere[MAX_ERRORWRH_LEN];
	TCHAR m_stMessage[MAX_ERRORMSG_LEN];
};

///////////////////////////////////////////////////////////////////////////////
// CWin32Exception
///////////////////////////////////////////////////////////////////////////////

class CWin32Exception : public CException
{
public:
	explicit CWin32Exception(LPCTSTR lpWhere, DWORD dwError);

	DWORD GetError() const;

protected:
	DWORD m_dwError;
};

///////////////////////////////////////////////////////////////////////////////
// CStructuredException
///////////////////////////////////////////////////////////////////////////////

class CStructuredException : public CException
{
public:
	static void MapSEtoCE();
	operator DWORD() { return m_er.ExceptionCode; }

	virtual LPCTSTR GetWhere() const;
	virtual LPCTSTR GetMessage() const;

private:
	explicit CStructuredException(PEXCEPTION_POINTERS pep);

	static void __cdecl TranslateSEtoCE(UINT dwEC,
		PEXCEPTION_POINTERS pep);

	LPTSTR lstrrchr(LPCTSTR string, int ch) const;
	LPTSTR GetFilePart(LPCTSTR source) const;
	LPCTSTR GetExceptionDescription(DWORD dwExceptionCode) const;

private:
	EXCEPTION_RECORD m_er;
	CONTEXT          m_context;

	TCHAR m_stStMessage[512];
};

///////////////////////////////////////////////////////////////////////////////
// Namespace: Elvees
///////////////////////////////////////////////////////////////////////////////

} // End of namespace Elvees

#endif // ELVEES_EXCEPTIONS__
