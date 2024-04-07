// CSEngine.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "CSEngineApp.h"
#include "../CSChair/interfaceEx.h"
#include "CSEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CCSEngineApp

BEGIN_MESSAGE_MAP(CCSEngineApp, CWinApp)
END_MESSAGE_MAP()


// CCSEngineApp construction

CCSEngineApp::CCSEngineApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CCSEngineApp object

CCSEngineApp theApp;


// CCSEngineApp initialization

BOOL CCSEngineApp::InitInstance()
{
	CWinApp::InitInstance();
	return TRUE;
}

#if 0

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DllState.hModule = hInstance;
		// Extension DLL one-time initialization
//		AfxInitExtensionModule(DllState, hInstance);

		// Insert this DLL into the resource chain
//		new CDynLinkLibrary(DllState);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
	}
	return 1;   // ok
}

#endif

//FIXME : temp solutions

// ------------------------------------------------------------------------------------ //
// InterfaceReg.
// ------------------------------------------------------------------------------------ //
InterfaceReg *InterfaceReg::s_pInterfaceRegs = NULL;


InterfaceReg::InterfaceReg( InstantiateInterfaceFn fn, const wchar_t *pName ) :
m_pName(pName)
{
	m_CreateFn = fn;
	m_pNext = s_pInterfaceRegs;
	s_pInterfaceRegs = this;
}

/*extern "C" */DWORD WINAPI CreateEngineInterface( const wchar_t *pName, void **ppInterface )
{
	InterfaceReg *pCur;

	if( !ppInterface/* || !*ppInterface*/ ) return IFACE_FAILED;

	for(pCur=InterfaceReg::s_pInterfaceRegs; pCur; pCur=pCur->m_pNext)
	{
		if(wcscmp(pCur->m_pName, pName) == 0)
		{
			try
			{
				// mb throw exceptions
				*ppInterface = pCur->m_CreateFn();
				return IFACE_OK;
			}
			catch(std::exception&)
			{
				break;
			}
		}
	}

	*ppInterface = NULL;
	return IFACE_FAILED;	
}