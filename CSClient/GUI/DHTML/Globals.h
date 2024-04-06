// Globals.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// Global Helper Functions & Macros
/////////////////////////////////////////////////////////////////////////////

#ifndef __GLOBALS_H__
#define __GLOBALS_H__

/////////////////////////////////////////////////////////////////////////////
// Global Helper Functions


/////////////////////////////////////////////////////////////////////////////
// Helper for reliable and small Release calls
// Taken from OLEIMPL2.H & OLEUNK.CPP
//////////////////////////////////////////////////////////////////////

#ifndef RELEASE
	#ifndef _DEBUG
		// Helper for reliable and small Release calls
		DWORD AFXAPI _AfxRelease(LPUNKNOWN* plpUnknown);
		// Generate smaller code in release build
		#define RELEASE(lpUnk) _AfxRelease((LPUNKNOWN*)&lpUnk)
	#else
		// Generate larger but typesafe code in debug build
		#define RELEASE(lpUnk) do \
			{ if ((lpUnk) != NULL) { (lpUnk)->Release(); (lpUnk) = NULL; } } while (0)
	#endif
#endif

/////////////////////////////////////////////////////////////////////////////
// Diagnostic helper to discover what ambient properties MSHTML 
// asks of the host 
//////////////////////////////////////////////////////////////////////

#ifdef _TRACE_AMB_PROPRERIES
CString StringDISPID(DISPID dispidMember);
#endif // _TRACE_AMB_PROPRERIES

//////////////////////////////////////////////////////////////////////
// UNICODE <-> ANSI Helper functions
//////////////////////////////////////////////////////////////////////

#ifdef UNICODE 
	#define FROM_OLE_STRING(str) str 
	#define TO_OLE_STRING(str) str  
#else 
	#define FROM_OLE_STRING(str) ConvertToAnsi(str) 
	char* ConvertToAnsi(OLECHAR FAR* szW);   
	#define TO_OLE_STRING(str) ConvertToUnicode(str) 
	OLECHAR* ConvertToUnicode(char FAR* szA);    
	// Maximum length of string that can be converted
	// between Ansi & Unicode 
	#define STRCONVERT_MAXLEN 300          
#endif

//////////////////////////////////////////////////////////////////////
// Helper macros for setting up DISPPARAMS structures
//////////////////////////////////////////////////////////////////////

#define SETDISPPARAMS(dp, numArgs, pvArgs, numNamed, pNamed) \
	{\
	(dp).cArgs=numArgs;\
	(dp).rgvarg=pvArgs;\
	(dp).cNamedArgs=numNamed;\
	(dp).rgdispidNamedArgs=pNamed;\
	}
#define SETNOPARAMS(dp) SETDISPPARAMS(dp, 0, NULL, 0, NULL)

//////////////////////////////////////////////////////////////////////
// Helper macro to determine number of elements in 
// an array (not bytes)
//////////////////////////////////////////////////////////////////////
#ifndef _countof
	#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

//////////////////////////////////////////////////////////////////////
// IUnknown macros for implementing redirection to parent
// IUnknown implementation
//////////////////////////////////////////////////////////////////////

#ifndef IMPLEMENT_IUNKNOWN

#define T_NAME(x)	_T(#x)

#define IMPLEMENT_IUNKNOWN_ADDREF(ObjectClass, InterfaceClass) \
    STDMETHODIMP_(ULONG) ObjectClass::X##InterfaceClass::AddRef(void) \
    { \
				TRACE(_T("X") T_NAME(InterfaceClass) _T("::AddRef Called\n") ); \
				\
        METHOD_PROLOGUE(ObjectClass, InterfaceClass); \
        return pThis->ExternalAddRef(); \
    }

#define IMPLEMENT_IUNKNOWN_RELEASE(ObjectClass, InterfaceClass) \
    STDMETHODIMP_(ULONG) ObjectClass::X##InterfaceClass::Release(void) \
    { \
				TRACE(_T("X") T_NAME(InterfaceClass)  _T("::Release Called\n") ); \
				\
        METHOD_PROLOGUE(ObjectClass, InterfaceClass); \
        return pThis->ExternalRelease(); \
    }

#define IMPLEMENT_IUNKNOWN_QUERYINTERFACE(ObjectClass, InterfaceClass) \
    STDMETHODIMP ObjectClass::X##InterfaceClass::QueryInterface(REFIID riid, LPVOID *ppVoid) \
    { \
				TRACE(_T("X") T_NAME(InterfaceClass)  _T("::QueryInterface Called\n")); \
				TRACE1("QueryInterface REFIID = %s\n",IID_DbgName(riid)); \
				\
        METHOD_PROLOGUE(ObjectClass, InterfaceClass); \
        return (HRESULT)pThis->ExternalQueryInterface(&riid, ppVoid); \
    }

#define IMPLEMENT_IUNKNOWN(ObjectClass, InterfaceClass) \
    IMPLEMENT_IUNKNOWN_ADDREF(ObjectClass, InterfaceClass) \
    IMPLEMENT_IUNKNOWN_RELEASE(ObjectClass, InterfaceClass) \
    IMPLEMENT_IUNKNOWN_QUERYINTERFACE(ObjectClass, InterfaceClass)

#endif

CString IID_DbgName(REFIID iid);

/////////////////////////////////////////////////////////////////////////////


#endif // __GLOBALS_H__

